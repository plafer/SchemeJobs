#ifndef EVAL_H
#define EVAL_H

#include "inc/ast.h"
#include "inc/env.h"

// The Scheme evaluator. Evaluates `node` with respect to `env`, and returns the
// resulting astnode in `ret`.
// Possible errors:
// + EINVAL: An argument was NULL
// + EBADMSG: Invalid `node` type. This is very generic; we'll have to work on a
// better way to report errors.
// + ENOMEM: Out of memory.
int eval(struct astnode *node, struct astnode_env *env, struct astnode **ret);

// Apply `proc` to `args`. `proc` can either be a primitive or compound
// procedure. `args` must already have been evaluated.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: `proc` is not a primitive nor a compound procedure. Invalid
// arguments to the procedure.
int apply(struct astnode *proc, struct astnode_pair *args, struct astnode **ret);

#endif
