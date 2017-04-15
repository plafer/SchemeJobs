#ifndef PRMT_HANDLERS_H
#define PRMT_HANDLERS_H

#include "inc/ast.h"

// Scheme's cons.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: Wrong number or type of arguments.
// + ENOMEM: Failed to allocate an object.
int prmt_cons(struct astnode_pair *args, struct astnode **ret);

// Scheme's car.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: Wrong number or type of arguments.
int prmt_car(struct astnode_pair *args, struct astnode **ret);

// Scheme's cdr.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: Wrong number or type of arguments.
int prmt_cdr(struct astnode_pair *args, struct astnode **ret);

// `ret` is set to boolean #t if `node` is a pair. The empty list is not a pair.
// Possible errors:
// + EINVAL: Either argument is NULL.
// + EBADMSG: Wrong number or type of arguments.
int prmt_is_pair(struct astnode_pair *args, struct astnode **ret);

int prmt_plus(struct astnode_pair *args, struct astnode **ret);
int prmt_minus(struct astnode_pair *args, struct astnode **ret);
int prmt_mult(struct astnode_pair *args, struct astnode **ret);
int prmt_div(struct astnode_pair *args, struct astnode **ret);

#endif
