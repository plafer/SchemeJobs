#ifndef AST_H
#define AST_H

#include <stdint.h>

#define ASTNODE_BASE enum scm_type type;

// No strings for now.
// Only one type of number for now: 32 bit signed int.
enum scm_type {
  TYPE_SYM,
  TYPE_INT,
  TYPE_PAIR,
};

struct astnode {
  ASTNODE_BASE
};

//Note: empty pair () is going to be a symbol
struct astnode_sym {
  ASTNODE_BASE
  void *symi;
};

struct astnode_int {
  ASTNODE_BASE
  int32_t intval;
};

// (cons 1 '()) will output (1), so there's a check when printing: if cdr is
// of symbol type and is symbol (), then don't output () for symbol and end list
struct astnode_pair {
  ASTNODE_BASE
  struct astnode *car;
  struct astnode *cdr;
};

#endif
