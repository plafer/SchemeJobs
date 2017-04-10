#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "tests/CuTest.h"
#include "inc/ast.h"
#include "inc/env.h"
#include "inc/kw_handlers.h"
#include "inc/symbols.h"

// Note: We use the top_level_env object defined and initialized in envtests.c
extern struct astnode_env *top_level_env;

void TestDefine_NullArgs(CuTest *tc) {
  int err;

  err = kw_define(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestDefine_NormalBinding(CuTest *tc) {
  // (define z 3)
  const int VAL1 = 3;
  int err;
  char *sym;
  struct astnode_sym sym_node;
    struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_node,
    .cdr = (struct astnode *) &sec_pair
  };
  struct astnode_int *ret;

  sym = "z";
  sym_node.type = TYPE_SYM;
  err = putsym(sym, sym, &sym_node.symi);
  CuAssertIntEquals(tc, 0, err);

  err = kw_define(&first_pair, top_level_env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);

  // The return value of define is undefined; instead we lookup the environment
  err = lookup_env(top_level_env, &sym_node, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL1, ret->intval);
}

void TestDefine_Compproc(CuTest *tc) {
  // (define (fn a) a)
  int err;
  char *sym;
  struct astnode_sym sym_a_node;
  struct astnode_sym sym_fn_node;
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a_node,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair binding_pair_arg = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a_node,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair binding_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_fn_node,
    .cdr = (struct astnode *) &binding_pair_arg
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &binding_pair,
    .cdr = (struct astnode *) &sec_pair
  };
  struct astnode_compproc *ret;

  sym = "fn";
  sym_fn_node.type = TYPE_SYM;
  err = putsym(sym, sym+1, &sym_fn_node.symi);
  CuAssertIntEquals(tc, 0, err);

  sym = "a";
  sym_a_node.type = TYPE_SYM;
  err = putsym(sym, sym, &sym_a_node.symi);
  CuAssertIntEquals(tc, 0, err);

  err = kw_define(&first_pair, top_level_env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);

  // The return value of define is undefined; instead we lookup the environment
  err = lookup_env(top_level_env, &sym_fn_node, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_COMPPROC, ret->type);
}

CuSuite* KwGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, TestDefine_NullArgs);
  SUITE_ADD_TEST(suite, TestDefine_NormalBinding);
  SUITE_ADD_TEST(suite, TestDefine_Compproc);

  return suite;
}