#include <errno.h>
#include <string.h>

#include "inc/ast.h"
#include "inc/eval.h"
#include "inc/kw_handlers.h"
#include "inc/gc.h"
#include "inc/stdmacros.h"

int kw_define(struct astnode_pair *args, struct astnode_env *env,
	      struct astnode **ret)
{
  struct astnode_sym *sym;
  struct astnode *ret_temp;

  NULL_CHECK3(args, env, ret);

  ret_temp = NULL;
  if (args->car->type == TYPE_PAIR)
    {
      // We're defining a compound procedure: ((fn arg) (+ arg 3))
      struct astnode_compproc *proc;
      struct astnode_pair *bindings;

      RETONERR(alloc_astnode(TYPE_COMPPROC, &ret_temp));

      proc = (struct astnode_compproc *) ret_temp;
      bindings = (struct astnode_pair *) args->car;

      TYPE_CHECK(bindings->car, TYPE_SYM);
      sym = (struct astnode_sym *) bindings->car;

      proc->env = env;

      // We don't make sure cdr is a well-formed list; in the case where it
      // isn't, the program will blow up when the procedure is called.
      // That's how MIT-Scheme does it.
      proc->params = (struct astnode_pair *) bindings->cdr;

      TYPE_CHECK(args->cdr, TYPE_PAIR);
      if (is_empty_list(args->cdr))
	return EBADMSG;
      proc->body = (struct astnode_pair *) args->cdr;
    }
  else if (args->car->type == TYPE_SYM)
    {
      // We're defining a normal binding: (a 3)
      sym = (struct astnode_sym *) args->car;

      TYPE_CHECK(args->cdr, TYPE_PAIR);
      RETONERR(eval(((struct astnode_pair *) args->cdr)->car, env, &ret_temp));
    }
  else
    // Invalid syntax
    return EBADMSG;

  RETONERR(define_binding(env, sym, ret_temp));

  // Return value is undefined, so we just put the defined value
  *ret = ret_temp;

  return 0;
}

int kw_if(struct astnode_pair *args, struct astnode_env *env,
	  struct astnode **ret)
{
  struct astnode *cond;
  struct astnode *truepath;
  struct astnode *falsepath;
  struct astnode *evaled_cond;

  NULL_CHECK3(args, env, ret);

  if (is_empty_list((struct astnode *)args))
    return EBADMSG;

  cond = args->car;

  args = (struct astnode_pair *)args->cdr;
  TYPE_CHECK(args, TYPE_PAIR);
  truepath = args->car;

  args = (struct astnode_pair *)args->cdr;
  TYPE_CHECK(args, TYPE_PAIR);
  falsepath = args->car;

  if (!is_empty_list(args->cdr))
    return EBADMSG;

  // If we got here, arguments are valid
  RETONERR(eval(cond, env, &evaled_cond));

  // Only boolean false will have the false path evaled
  if (evaled_cond->type == TYPE_BOOLEAN &&
      ((struct astnode_boolean *)evaled_cond)->boolval == false)
    {
      RETONERR(eval(falsepath, env, ret));
    }
  else
    {
      RETONERR(eval(truepath, env, ret));
    }

  return 0;
}

int kw_quote(struct astnode_pair *args, struct astnode_env *env,
	     struct astnode **ret)
{
  NULL_CHECK3(args, env, ret);

  if (is_empty_list((struct astnode *)args) || !is_empty_list(args->cdr))
    return EBADMSG;

  *ret = args->car;

  return 0;
}

int kw_lambda(struct astnode_pair *args, struct astnode_env *env,
	      struct astnode **ret)
{
  struct astnode_pair *params;
  struct astnode_pair *body;

  NULL_CHECK3(args, env, ret);

  TYPE_CHECK(args->car, TYPE_PAIR);
  params = (struct astnode_pair *) args->car;

  TYPE_CHECK(args->cdr, TYPE_PAIR);
  if (is_empty_list(args->cdr))
    return EBADMSG;
  body = (struct astnode_pair *) args->cdr;

  RETONERR(alloc_astnode(TYPE_COMPPROC, ret));
  ((struct astnode_compproc *) *ret)->body = body;
  ((struct astnode_compproc *) *ret)->env = env;
  ((struct astnode_compproc *) *ret)->params = params;

  return 0;
}
