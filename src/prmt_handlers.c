#include "inc/ast.h"
#include "inc/gc.h"
#include "inc/prmt_handlers.h"
#include "inc/stdmacros.h"

// e.g. (cons 1 2)
// args: (1 2)
int prmt_cons(struct astnode_pair *args, struct astnode **ret)
{
  struct astnode *the_car;
  struct astnode *the_cdr;

  NULL_CHECK2(args, ret);
  TYPE_CHECK(args, TYPE_PAIR);
  if (is_empty_list((struct astnode *) args))
    return EBADMSG;

  the_car = args->car;

  TYPE_CHECK(args->cdr, TYPE_PAIR);
  the_cdr = ((struct astnode_pair *) args->cdr)->car;

  // If we got more than 2 arguments, fail.
  if (!is_empty_list(((struct astnode_pair *) args->cdr)->cdr))
    return EBADMSG;

  RETONERR(alloc_astnode(TYPE_PAIR, ret));
  ((struct astnode_pair *)*ret)->car = the_car;
  ((struct astnode_pair *)*ret)->cdr = the_cdr;

  return 0;
}

// e.g. (car obj)
// args: (obj)
int prmt_car(struct astnode_pair *args, struct astnode **ret)
{
  struct astnode_pair *obj;

  NULL_CHECK2(args, ret);

  TYPE_CHECK(args, TYPE_PAIR);
  if (is_empty_list((struct astnode *) args))
    return EBADMSG;

  TYPE_CHECK(args->car, TYPE_PAIR);
  obj = (struct astnode_pair *) args->car;

  // If we got more than 1 argument, fail
  if (!is_empty_list(args->cdr))
    return EBADMSG;

  *ret = obj->car;
  return 0;
}

// e.g. (cdr obj)
// args: (obj)
int prmt_cdr(struct astnode_pair *args, struct astnode **ret)
{
  struct astnode_pair *obj;

  NULL_CHECK2(args, ret);

  TYPE_CHECK(args, TYPE_PAIR);
  if (is_empty_list((struct astnode *) args))
    return EBADMSG;

  TYPE_CHECK(args->car, TYPE_PAIR);
  obj = (struct astnode_pair *) args->car;

  // If we got more than 1 argument, fail
  if (!is_empty_list(args->cdr))
    return EBADMSG;

  *ret = obj->cdr;
  return 0;
}

int prmt_is_pair(struct astnode_pair *args, struct astnode_boolean **ret)
{
  struct astnode *obj;

  NULL_CHECK2(args, ret);

  TYPE_CHECK(args, TYPE_PAIR);
  if (is_empty_list((struct astnode *) args))
    return EBADMSG;

  // If we got more than 1 argument, fail
  if (!is_empty_list(args->cdr))
    return EBADMSG;

  obj = args->car;

  // We is_empty_list first, without assuming that its type is also pair
  // (implementation might change later).
  if (is_empty_list(obj))
    *ret = BOOLEAN_FALSE;
  else if (obj->type == TYPE_PAIR)
    *ret = BOOLEAN_TRUE;
  else
    *ret = BOOLEAN_FALSE;

  return 0;
}
