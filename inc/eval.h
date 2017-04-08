#ifndef EVAL_H
#define EVAL_H

#include "inc/ast.h"
#include "inc/env.h"

// The Scheme eval. Evaluates `node` with respect to `env`, and returns the
// resulting astnode in `ret`.
// Possible errors:
// + EINVAL: An argument was NULL
// + EBADMSG: Invalid `node` type. This is very generic; we'll have to work on a
// better way to report errors.
// + ENOMEM: Out of memory.
int eval(struct astnode *node, struct astnode_env *env, struct astnode **ret);

#endif
