#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "inc/ast.h"
#include "inc/env.h"
#include "inc/eval.h"
#include "inc/stdmacros.h"

// Two possibilities: (define a 3) or (+ 1 2)
static int eval_pair(struct astnode_pair *node, struct astnode_env *env,
		     struct astnode **ret)
{
  struct astnode *evaled_car;
  int err;

  // If empty list, evaluate to itself
  if (is_empty_list((struct astnode *)node))
    {
      *ret = (struct astnode *) node;
      return 0;
    }

  err = eval(node->car, env, &evaled_car);
  if (err != 0)
    return err;

  if (evaled_car->type == TYPE_KEYWORD)
    {
      struct astnode_keyword *keyword = (struct astnode_keyword *) evaled_car;
      err = keyword->handler((struct astnode_pair *) node->cdr, env, ret);
    }
  else if (evaled_car->type == TYPE_PRMTPROC)
    {
      struct astnode_prmtproc *proc = (struct astnode_prmtproc *) evaled_car;
      err = proc->handler((struct astnode_pair *) node->cdr, ret);
    }
  else
    err = EBADMSG;

  return err;
}

// TODO: To support tail calls, we can add a flag `istailcall`, which will get
// passed down to apply (which will decide whether to extend the environment or
// reuse the same frame). `eval_list` (called by compound procedures) will set
// the flag only on the last call.
int eval(struct astnode *node, struct astnode_env *env, struct astnode **ret)
{
  int err;

  NULL_CHECK3(node, env, ret);

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
      // Primitive procedures are not expressions: it is an error to evaluate them
    case TYPE_PRMTPROC:
      *ret = node;
      err = 0;
      break;
    default:
      *ret = NULL;
      err = EBADMSG;
      break;
    }

  return err;
}
