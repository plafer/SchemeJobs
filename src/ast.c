#include <string.h>

#include "inc/ast.h"

struct astnode_boolean _boolean_true = {
  .type = TYPE_BOOLEAN,
  .boolval = true
};

struct astnode_boolean _boolean_false = {
  .type = TYPE_BOOLEAN,
  .boolval = false
};

struct astnode_pair _empty_list = {
  .type = TYPE_PAIR,
  .car = NULL,
  .cdr = NULL
};

bool is_empty_list(struct astnode *node)
{
  return node != NULL && node->type == TYPE_PAIR &&
    ((struct astnode_pair *)node)->car == NULL &&
    ((struct astnode_pair *)node)->cdr == NULL;
}
