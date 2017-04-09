#ifndef STD_MACROS_H
#define STD_MACROS_H

#include <errno.h>

#define RETONERR(exp) do {			\
  int err;					\
  err = (exp);					\
  if (err != 0) return err;			\
  } while (0)


#define NULL_CHECK1(arg) if ((arg) == NULL)	\
    return EINVAL

#define NULL_CHECK2(arg1, arg2)			\
  if ((arg1) == NULL || (arg2) == NULL)		\
    return EINVAL

#define NULL_CHECK3(arg1, arg2, arg3)				\
  if ((arg1) == NULL || (arg2) == NULL || (arg3) == NULL)	\
    return EINVAL

#define NULL_CHECK4(arg1, arg2, arg3, arg4)				\
  if ((arg1) == NULL || (arg2) == NULL ||				\
      (arg3) == NULL || (arg4) == NULL)				\
    return EINVAL

#define TYPE_CHECK(astnode, type_req)		\
  if ((astnode)->type != (type_req))		\
    return EBADMSG

#define TYPE_CHECK2(astnode, type1, type2)				\
  if ((astnode)->type != (type1) && (astnode)->type != (type2))		\
    return EBADMSG


#endif
