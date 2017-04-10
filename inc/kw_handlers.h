#ifndef KW_HANDLERS_H
#define KW_HANDLERS_H

#include "inc/ast.h"

// Define special form. Has two valid syntax:
// 1. (define a 3) creates a binding `a` -> 3
// 2. (define (fn a) (+ a 3)) creates a compound procedure
// `args` should NOT already be evaluated.
// + EINVAL: An argument was NULL.
// + EBADMSG: Wrong number or type of arguments.
// + ENOMEM: Failed to allocate an object.
int kw_define(struct astnode_pair *args, struct astnode_env *env,
	      struct astnode **ret);

#endif
