#ifndef AST_H
#define AST_H

#include <stdint.h>
#include <stdbool.h>

#define ASTNODE_BASE astnode_type type;

// No strings for now.
// Only one type of number for now: 32 bit signed int.
typedef enum {
  TYPE_SYM,
  TYPE_INT,
  TYPE_BOOLEAN,
  TYPE_PAIR,
  TYPE_ENV,
  TYPE_KEYWORD,
  TYPE_PRMTPROC,
  TYPE_MAX,
} astnode_type;

struct astnode {
  ASTNODE_BASE
};

struct astnode_sym {
  ASTNODE_BASE
  void *symi;
};

struct astnode_int {
  ASTNODE_BASE
  int32_t intval;
};

// 1. #t and #f are NOT symbols, and evaluate to themselves
// https://www.gnu.org/software/mit-scheme/documentation/mit-scheme-ref/Booleans.html
// 2. "In conditional tests, all values count as true except for #f, which counts
// as false"
// https://www.gnu.org/software/mit-scheme/documentation/mit-scheme-ref/True-and-False.html
struct astnode_boolean {
  ASTNODE_BASE
  bool boolval;
};

extern struct astnode_boolean _boolean_true;
extern struct astnode_boolean _boolean_false;

#define BOOLEAN_TRUE  ((struct astnode_boolean *) &_boolean_true)
#define BOOLEAN_FALSE ((struct astnode_boolean *) &_boolean_false)

// Functions should use is_empty_list(node) to check if a node is the empty list.
// Note: (pair? '()) must return false.
struct astnode_pair {
  ASTNODE_BASE
  struct astnode *car;
  struct astnode *cdr;
};

extern struct astnode_pair _empty_list;

#define EMPTY_LIST &_empty_list

// Example bindings: ((+ <proc>) (var <int>) ...)
struct astnode_env {
  ASTNODE_BASE
  struct astnode_env *parent;
  struct astnode_pair *bindings;
};

typedef int (*kw_handler)(struct astnode_pair *args, struct astnode_env *env,
			  struct astnode **ret);

// We create a keyword astnode type for keywords. The alternative would be to
// define keywords with special value, e.g.
// (list 'compound-procedure body params env)
// However, Scheme programmers could theoretically write a list with the same
// format to denote a keyword, e.g.
// ((list 'compound-procedure <some-body> <some-params> <some-env>) 1 2 3)) to
// simulate a procedure call. Using an astnode type other than list solves that
// problem (i.e. there is no way to define an astnode_keyword object in Scheme
// syntax).
// TODO: Verify this in SICP metacircular evaluator.
struct astnode_keyword {
  ASTNODE_BASE
  kw_handler handler;
};

// Must match signature of handlers in prmt_handlers.h
typedef int (*prmt_handler)(struct astnode_pair *args, struct astnode **ret);

struct astnode_prmtproc {
  ASTNODE_BASE
  prmt_handler handler;
};


bool is_empty_list(struct astnode *node);

#endif
