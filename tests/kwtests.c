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

void TestIf_NullArgs(CuTest *tc) {
  int err;

  err = kw_if(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestIf_NormalBindingTruePath(CuTest *tc)
{
  const int VAL1 = 42;
  const int VAL2 = 43;
  const int VAL3 = 44;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_int num2 = {
    .type = TYPE_INT,
    .intval = VAL2
  };
  struct astnode_int num3 = {
    .type = TYPE_INT,
    .intval = VAL3
  };
  struct astnode_pair third_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num3,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num2,
    .cdr = (struct astnode *) &third_pair
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = kw_if(&first_pair, top_level_env, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL2, ret->intval);
}

void TestIf_NormalBindingFalsePath(CuTest *tc)
{
  const int VAL1 = 42;
  const int VAL2 = 43;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_int num2 = {
    .type = TYPE_INT,
    .intval = VAL2
  };
  struct astnode_pair third_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num2,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  // We take a shortcut - we should have put the symbol "#f", but who has time
  // for that?
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &third_pair
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) &sec_pair
  };

  err = kw_if(&first_pair, top_level_env, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL2, ret->intval);
}

void TestIf_TooFewArgs(CuTest *tc)
{
  const int VAL1 = 42;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  // We take a shortcut - we should have put the symbol "#f", but who has time
  // for that?
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) &sec_pair
  };

  err = kw_if(&first_pair, top_level_env, (struct astnode **)&ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestIf_TooManyArgs(CuTest *tc)
{
  const int VAL1 = 42;
  const int VAL2 = 43;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_int num2 = {
    .type = TYPE_INT,
    .intval = VAL2
  };
  struct astnode_pair fourth_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num2,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair third_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &fourth_pair
  };
  // We take a shortcut - we should have put the symbol "#f", but who has time
  // for that?
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) &third_pair
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) &sec_pair
  };

  err = kw_if(&first_pair, top_level_env, (struct astnode **)&ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestQuote_NullArgs(CuTest *tc) {
  int err;

  err = kw_quote(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestQuote_ValidObj(CuTest *tc) {
  int err;
  struct astnode_sym sym_node;
  struct astnode_sym *ret;

  char *sym = "potato";
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_node,
    .cdr = (struct astnode *) EMPTY_LIST
  };

  sym_node.type = TYPE_SYM;
  err = putsym(sym, sym + strlen(sym) - 1, &sym_node.symi);
  CuAssertIntEquals(tc, 0, err);

  err = kw_quote(&first_pair, top_level_env, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_SYM, ret->type);
  CuAssertPtrEquals(tc, sym_node.symi, ret->symi);
}

void TestQuote_TooFewArgs(CuTest *tc)
{
  int err;
  struct astnode *ret;

  struct astnode_pair *first_pair = EMPTY_LIST;

  err = kw_quote(first_pair, top_level_env, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);

}

void TestQuote_TooManyArgs(CuTest *tc)
{
  int err;
  struct astnode *ret;

  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) &sec_pair
  };


  err = kw_quote(&first_pair, top_level_env, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestLambda_NullArgs(CuTest *tc) {
  int err;

  err = kw_if(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestLambda_ValidObj(CuTest *tc) {
  int err;
  char *sym;
  struct astnode_sym sym_a_node;
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a_node,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair binding_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a_node,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &binding_pair,
    .cdr = (struct astnode *) &sec_pair
  };
  struct astnode_compproc *ret;

  sym = "a";
  sym_a_node.type = TYPE_SYM;
  err = putsym(sym, sym, &sym_a_node.symi);
  CuAssertIntEquals(tc, 0, err);

  err = kw_lambda(&first_pair, top_level_env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_COMPPROC, ret->type);
}

void TestLambda_NoArgs(CuTest *tc) {
  int err;
  char *sym;
  struct astnode_sym sym_a_node;
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a_node,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) EMPTY_LIST,
    .cdr = (struct astnode *) &sec_pair
  };
  struct astnode_compproc *ret;

  sym = "a";
  sym_a_node.type = TYPE_SYM;
  err = putsym(sym, sym, &sym_a_node.symi);
  CuAssertIntEquals(tc, 0, err);

  err = kw_lambda(&first_pair, top_level_env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_COMPPROC, ret->type);
}

void TestLambda_TooFewArgs(CuTest *tc) {
  int err;
  char *sym;
  struct astnode_sym sym_a_node;
  struct astnode_pair binding_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a_node,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &binding_pair,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_compproc *ret;

  sym = "a";
  sym_a_node.type = TYPE_SYM;
  err = putsym(sym, sym, &sym_a_node.symi);
  CuAssertIntEquals(tc, 0, err);

  err = kw_lambda(&first_pair, top_level_env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

CuSuite* KwGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, TestDefine_NullArgs);
  SUITE_ADD_TEST(suite, TestDefine_NormalBinding);
  SUITE_ADD_TEST(suite, TestDefine_Compproc);
  SUITE_ADD_TEST(suite, TestIf_NullArgs);
  SUITE_ADD_TEST(suite, TestIf_NormalBindingTruePath);
  SUITE_ADD_TEST(suite, TestIf_NormalBindingFalsePath);
  SUITE_ADD_TEST(suite, TestIf_TooFewArgs);
  SUITE_ADD_TEST(suite, TestIf_TooManyArgs);
  SUITE_ADD_TEST(suite, TestQuote_NullArgs);
  SUITE_ADD_TEST(suite, TestQuote_ValidObj);
  SUITE_ADD_TEST(suite, TestQuote_TooFewArgs);
  SUITE_ADD_TEST(suite, TestQuote_TooManyArgs);
  SUITE_ADD_TEST(suite, TestLambda_NullArgs);
  SUITE_ADD_TEST(suite, TestLambda_ValidObj);
  SUITE_ADD_TEST(suite, TestLambda_NoArgs);
  SUITE_ADD_TEST(suite, TestLambda_TooFewArgs);

  return suite;
}
