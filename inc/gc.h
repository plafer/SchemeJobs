#ifndef GC_H
#define GC_H

#include <stddef.h>

#include "inc/ast.h"

// Allocates an astnode of type `type` and initializes the header. Places the
// allocated and initialized object in ret.
// Possible errors:
// + ENOMEM: Out of memory.
int alloc_astnode(astnode_type type, struct astnode **ret);

#endif
