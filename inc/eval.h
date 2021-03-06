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

// Evaluate each element in `unevaled_args` and placed a list of the evaluated
// arguments in `ret`.
// Possible errors:
// See eval.
int eval_list(struct astnode_pair *unevaled_args, struct astnode_env *env,
	      struct astnode_pair **ret);

// Convenience procedure that evaluates each element in `stmts` and returns the
// value of the last one. Return value is unspecified if `stmts` is the empty
// list.
// Possible errors:
// See eval.
int eval_many(struct astnode_pair *stmts, struct astnode_env *env,
	      struct astnode **ret);

// Apply `proc` to `args`. `proc` can either be a primitive or compound
// procedure. `args` must already have been evaluated.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: `proc` is not a primitive nor a compound procedure. Invalid
// arguments to the procedure.
int apply(struct astnode *proc, struct astnode_pair *args, struct astnode **ret);

#endif
