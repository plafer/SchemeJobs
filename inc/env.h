#ifndef ENV_H
#define ENV_H

#include "inc/ast.h"

// Makes top level environment, which includes primitive procedures' and keyword
// bindings.
// Possible errors:
// + EINVAL: An argument was NULL.
// + ENOMEM: Failed to allocate the new environment.
int make_top_level_env(struct astnode_env **ret);

// Adds a new frame to `env` and binds the formal parameters to the supplied
// arguments. The resulting environment is placed in `extended`.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: The number of arguments does not match the number of formal
// parameters.
// + ENOMEM: Failed to allocate the new frame.
int extend_env(struct astnode_env *env, struct astnode_pair *formal_params,
	       struct astnode_pair *args, struct astnode_env **extended);

// Looks up a binding for `sym` in `env`, placing the first one it finds in
// `ret`.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: No binding was found for `sym`
int lookup_env(struct astnode_env *env, struct astnode_sym *sym, struct astnode **ret);

// Adds a binding from `sym` to `val` in the first frame in `env`. If a binding
// already exists, the previous one is silently removed.
// Possible errors:
// + EINVAL: An argument was NULL.
// + EBADMSG: No binding was found for `sym`
int define_binding(struct astnode_env *env, struct astnode_sym *sym,
		      struct astnode *val);


#endif
