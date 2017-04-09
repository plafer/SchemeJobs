#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include "inc/ast.h"
#include "inc/gc.h"
#include "inc/stdmacros.h"

// This will be implemented as a mark and sweep GC. All astnodes will begin with
// an object signature, and contain a "live" bit and a size parameter. When an
// astnode is allocated and there is no more room in the heap, the following
// happens:
//
// MARK PHASE
// 1. For all roots, go to their location in memory.
// 2. Set the "live" bit.
// 3. Depending on the object's type (astnode.type), follow any link. (e.g. for
// a pair, follow car and cdr pointers). (Note: currently, empty_list is a
// variable in the data area i.e. outside the gc heap. Just something to keep in
// mind).
//
// SWEEP PHASE
// Start at the bottom of the heap. Begin a "free block". Scan until an object
// signature is found (this delimits the end of the free block). Jump over the
// object (using the size parameter in the header). Repeat, linking this new
// free block with the previous one.
//
// Note: the roots are all bindings in the top-level environment, as all
// pointers on the C stack (anything that looks like a pointer in the gc heap
// can be confirmed by making sure what it points to has a valid signature).
int alloc_astnode(astnode_type type, struct astnode **ret)
{
  struct astnode *new_node;

  NULL_CHECK1(ret);

  assert(type < TYPE_MAX);
  switch (type)
    {
    case TYPE_SYM:
      new_node = malloc(sizeof(struct astnode_sym));
      break;
    case TYPE_INT:
      new_node = malloc(sizeof(struct astnode_int));
      break;
    case TYPE_BOOLEAN:
      new_node = malloc(sizeof(struct astnode_boolean));
      break;
    case TYPE_PAIR:
      new_node = malloc(sizeof(struct astnode_pair));
      break;
    case TYPE_ENV:
      new_node = malloc(sizeof(struct astnode_env));
      break;
    case TYPE_KEYWORD:
      new_node = malloc(sizeof(struct astnode_keyword));
      break;
    case TYPE_PRMTPROC:
      new_node = malloc(sizeof(struct astnode_prmtproc));
      break;
    case TYPE_MAX:
      // Compiler happy
      return EINVAL;
    }

  if (new_node == NULL)
    return ENOMEM;

  new_node->type = type;
  *ret = new_node;
  return 0;
}
