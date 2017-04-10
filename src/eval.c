#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "inc/ast.h"
#include "inc/env.h"
#include "inc/eval.h"
#include "inc/gc.h"
#include "inc/stdmacros.h"

// Takes a list of objects to evaluate and returns a list of the corresponding
// objects evaluated.
int eval_list(struct astnode_pair *unevaled_args, struct astnode_env *env,
		     struct astnode_pair **ret)
{
  struct astnode_pair *evaled_args;
  struct astnode_pair *evaled_args_prev;
  struct astnode_pair *ret_temp;

  assert(unevaled_args != NULL && env != NULL && ret != NULL);

  if (is_empty_list((struct astnode *) unevaled_args))
    {
      *ret = unevaled_args;
      return 0;
    }

  for (evaled_args_prev = NULL;
       !is_empty_list((struct astnode *) unevaled_args);
       unevaled_args = (struct astnode_pair *) unevaled_args->cdr,
       evaled_args_prev = evaled_args)
    {
      TYPE_CHECK((struct astnode *) unevaled_args, TYPE_PAIR);

      // Allocate pair object and link to previous. Keep a reference to the
      // beginning of the list on the first iteration.
      RETONERR(alloc_astnode(TYPE_PAIR, (struct astnode **) &evaled_args));
      if (evaled_args_prev == NULL)
	ret_temp = evaled_args;
      else
	evaled_args_prev->cdr = (struct astnode *) evaled_args;

      // Eval current argument and store in `evaled_args` list
      RETONERR(eval((struct astnode *) unevaled_args->car, env, &evaled_args->car));
    }

  evaled_args_prev->cdr = (struct astnode *) EMPTY_LIST;
  *ret = ret_temp;

  return 0;
}

// Two possibilities: (define a 3) or (+ 1 2)
static int eval_pair(struct astnode_pair *node, struct astnode_env *env,
		     struct astnode **ret)
{
  struct astnode *evaled_car;

  // If empty list, evaluate to itself
  if (is_empty_list((struct astnode *)node))
    {
      *ret = (struct astnode *) node;
      return 0;
    }

  RETONERR(eval(node->car, env, &evaled_car));

  if (evaled_car->type == TYPE_KEYWORD)
    {
      struct astnode_pair *args;
      struct astnode_keyword *keyword;

      TYPE_CHECK(node->cdr, TYPE_PAIR);
      args = (struct astnode_pair *) node->cdr;
      keyword = (struct astnode_keyword *) evaled_car;

      RETONERR(keyword->handler(args, env, ret));
    }
  else
    {
      struct astnode_pair *evaled_args;

      RETONERR(eval_list((struct astnode_pair *) node->cdr, env, &evaled_args));
      RETONERR(apply(evaled_car, evaled_args, ret));
    }

  return 0;
}

// TODO: To support tail calls, we can add a flag `istailcall`, which will get
// passed down to apply (which will decide whether to extend the environment or
// reuse the same frame). `eval_list` (called by compound procedures) will set
// the flag only on the last call.
int eval(struct astnode *node, struct astnode_env *env, struct astnode **ret)
{
  int err;

  NULL_CHECK3(node, env, ret);

  assert(node->type < TYPE_MAX);
  switch (node->type)
    {
      // Symbols evaluate to their binding in the environment
    case TYPE_SYM:
      err = lookup_env(env, (struct astnode_sym *) node, ret);
      break;
      // Integers evaluate to themselves
    case TYPE_INT:
      *ret = node;
      err = 0;
      break;
      // Booleans evaluate to themselves
    case TYPE_BOOLEAN:
      *ret = node;
      err = 0;
      break;
      // Only Well-formed lists evaluate to a procedure application or a special
      // form handler.
      // The empty list evaluates to itself
    case TYPE_PAIR:
      err = eval_pair((struct astnode_pair *)node, env, ret);
      break;
      // An environment evaluates to itself
    case TYPE_ENV:
      *ret = node;
      err = 0;
      break;
      // Keywords are not expressions; it is an error to evaluate them
    case TYPE_KEYWORD:
      *ret = NULL;
      err = EBADMSG;
      break;
      // Primitive procedures evaluate to themselves
    case TYPE_PRMTPROC:
      *ret = node;
      err = 0;
      break;
    case TYPE_COMPPROC:
      *ret = node;
      err = 0;
      break;
      // To keep compiler happy
    case TYPE_MAX:
      err = EINVAL;
      break;
    }

  return err;
}

int eval_many(struct astnode_pair *stmts, struct astnode_env *env,
	      struct astnode **ret)
{
  struct astnode *ret_temp;
  NULL_CHECK3(stmts, env, ret);

  if (is_empty_list((struct astnode *) stmts))
    {
      *ret = (struct astnode *) EMPTY_LIST;
      return 0;
    }

  for ( ;
	!is_empty_list((struct astnode *) stmts);
	stmts = (struct astnode_pair *) stmts->cdr)
    {
      TYPE_CHECK(stmts, TYPE_PAIR);

      RETONERR(eval(stmts->car, env, &ret_temp));
    }

  *ret = ret_temp;

  return 0;
}

int apply(struct astnode *proc, struct astnode_pair *args, struct astnode **ret)
{
  NULL_CHECK2(proc, args);

  TYPE_CHECK2(proc, TYPE_PRMTPROC, TYPE_COMPPROC);
  if (proc->type == TYPE_PRMTPROC)
    {
      RETONERR(((struct astnode_prmtproc *) proc)->handler(args, ret));
    }
  else
    {
      struct astnode_compproc *compound_proc;
      struct astnode_env *extended_env;

      compound_proc = (struct astnode_compproc *) proc;
      RETONERR(extend_env(compound_proc->env, compound_proc->params, args,
			  &extended_env));
      RETONERR(eval_many(compound_proc->body, extended_env, ret));
    }

  return 0;
}
