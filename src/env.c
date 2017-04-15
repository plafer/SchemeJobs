#include <assert.h>
#include <errno.h>
#include <string.h>

#include "inc/ast.h"
#include "inc/gc.h"
#include "inc/env.h"
#include "inc/kw_handlers.h"
#include "inc/prmt_handlers.h"
#include "inc/stdmacros.h"
#include "inc/symbols.h"

// *******************************************************
// Top level environment bindings
// *******************************************************

// Convenience function that takes a symbol string and a prmt_handler and binds
// them in the environment.
static int bind_rawsym_prmt(struct astnode_env *env, char *rawsym, prmt_handler hdl)
{
  struct astnode_sym *symnode;
  struct astnode_prmtproc *hdlnode;

  // Add symbol to symbol table
  RETONERR(alloc_astnode(TYPE_SYM, (struct astnode **) &symnode));
  RETONERR(putsym(rawsym, rawsym + strlen(rawsym) - 1, &symnode->symi));

  // Wrap primitive handler in astnode
  RETONERR(alloc_astnode(TYPE_PRMTPROC, (struct astnode **) &hdlnode));
  hdlnode->handler = hdl;

  // Bind symbol in environment
  RETONERR(define_binding(env, symnode, (struct astnode *) hdlnode));

  return 0;
}

static int bind_rawsym_kw(struct astnode_env *env, char *rawsym, kw_handler hdl)
{
  struct astnode_sym *symnode;
  struct astnode_keyword *hdlnode;

  // Add symbol to symbol table
  RETONERR(alloc_astnode(TYPE_SYM, (struct astnode **) &symnode));
  RETONERR(putsym(rawsym, rawsym + strlen(rawsym) - 1, &symnode->symi));

  // Wrap primitive handler in astnode
  RETONERR(alloc_astnode(TYPE_KEYWORD, (struct astnode **) &hdlnode));
  hdlnode->handler = hdl;

  // Bind symbol in environment
  RETONERR(define_binding(env, symnode, (struct astnode *) hdlnode));

  return 0;
}

static int install_prmt_ops(struct astnode_env *env)
{
  RETONERR(bind_rawsym_prmt(env, "cons", prmt_cons));
  RETONERR(bind_rawsym_prmt(env, "car", prmt_car));
  RETONERR(bind_rawsym_prmt(env, "cdr", prmt_cdr));
  RETONERR(bind_rawsym_prmt(env, "pair?", prmt_is_pair));

  RETONERR(bind_rawsym_prmt(env, "+", prmt_plus));
  RETONERR(bind_rawsym_prmt(env, "-", prmt_minus));
  RETONERR(bind_rawsym_prmt(env, "*", prmt_mult));
  RETONERR(bind_rawsym_prmt(env, "/", prmt_div));

  return 0;
}

static int install_keywords(struct astnode_env *env)
{
  RETONERR(bind_rawsym_kw(env, "define", kw_define));
  RETONERR(bind_rawsym_kw(env, "if", kw_if));
  RETONERR(bind_rawsym_kw(env, "quote", kw_quote));
  RETONERR(bind_rawsym_kw(env, "lambda", kw_lambda));

  return 0;
}

// *******************************************************
// Environment manipulation
// *******************************************************

// Returns the binding of sym, or NULL if no such binding exists in local
// environment (i.e. the first frame).
static struct astnode_pair *find_local_binding(struct astnode_env *env,
					       struct astnode_sym *sym)
{
  struct astnode_pair *binding_scanner;

  assert(env != NULL);
  assert(sym->type == TYPE_SYM);

  if (is_empty_list((struct astnode *) env->bindings))
    return NULL;

  for (binding_scanner = env->bindings;
       !is_empty_list((struct astnode *) binding_scanner);
       binding_scanner = (struct astnode_pair *) binding_scanner->cdr)
    {
      struct astnode_pair *binding;

      assert(binding_scanner->car->type == TYPE_PAIR);
      binding = (struct astnode_pair *) binding_scanner->car;

      if (binding->car->type == TYPE_SYM &&
	  ((struct astnode_sym *)binding->car)->symi == sym->symi)
	{
	  return binding;
	}
    }

  return NULL;
}

static int make_empty_env(struct astnode_env **ret)
{
  struct astnode_env *env;

  NULL_CHECK1(ret);

  RETONERR(alloc_astnode(TYPE_ENV, (struct astnode **)&env));

  env->parent = NULL;

  RETONERR(alloc_astnode(TYPE_PAIR, (struct astnode **)&env->bindings));
  env->bindings->car = NULL;
  env->bindings->cdr = NULL;

  *ret = env;

  return 0;
}

int make_top_level_env(struct astnode_env **ret)
{
  RETONERR(make_empty_env(ret));
  RETONERR(install_prmt_ops(*ret));
  RETONERR(install_keywords(*ret));

  return 0;
}

int lookup_env(struct astnode_env *env, struct astnode_sym *sym, struct astnode **ret)
{
  struct astnode_pair *binding;

  NULL_CHECK3(sym, env, ret);

  while (env != NULL)
    {
      binding = find_local_binding(env, sym);
      if (binding != NULL)
	{
	  *ret = binding->cdr;
	  return 0;
	}
      env = env->parent;
    }

  return EBADMSG;
}

int extend_env(struct astnode_env *env, struct astnode_pair *formal_params,
	       struct astnode_pair *args, struct astnode_env **extended)
{
  struct astnode_env *extended_temp;
  struct astnode_pair *param_scanner;
  struct astnode_pair *arg_scanner;

  NULL_CHECK4(env, formal_params, args, extended);

  RETONERR(make_empty_env(&extended_temp));
  extended_temp->parent = env;

  // bind formal_params to args in extended_temp->bindings
  for (param_scanner = formal_params, arg_scanner = args;
       !is_empty_list((struct astnode *) param_scanner) &&
	 !is_empty_list((struct astnode *) arg_scanner);
       param_scanner = (struct astnode_pair *)param_scanner->cdr,
	 arg_scanner = (struct astnode_pair *) arg_scanner->cdr)
    {
      struct astnode_sym *param;
      struct astnode *arg;

      TYPE_CHECK(param_scanner, TYPE_PAIR);
      TYPE_CHECK(arg_scanner, TYPE_PAIR);

      param = (struct astnode_sym *) param_scanner->car;
      TYPE_CHECK(param, TYPE_SYM);
      arg = arg_scanner->car;

      RETONERR(define_binding(extended_temp, param, arg));
    }

  // If params and args were of different lengths, fail.
  // TODO: What about varargs?
  if (!(is_empty_list((struct astnode *) param_scanner) &&
	is_empty_list((struct astnode *) arg_scanner)))
    return EBADMSG;

  *extended = extended_temp;
  return 0;
}


int define_binding(struct astnode_env *env, struct astnode_sym *sym,
		      struct astnode *val)
{
  struct astnode_pair *binding;

  NULL_CHECK3(env, sym, val);

  binding = find_local_binding(env, sym);
  if (binding != NULL)
    {
      binding->cdr = val;
    }
  else
    {
      struct astnode_pair *binding_wrapper;

      RETONERR(alloc_astnode(TYPE_PAIR, (struct astnode **) &binding));
      binding->car = (struct astnode *)sym;
      binding->cdr = (struct astnode *)val;

      // TODO: Use the implementation of cons
      RETONERR(alloc_astnode(TYPE_PAIR, (struct astnode **) &binding_wrapper));
      binding_wrapper->car = (struct astnode *) binding;
      binding_wrapper->cdr = (struct astnode *) env->bindings;

      env->bindings = binding_wrapper;
    }

  return 0;
}
