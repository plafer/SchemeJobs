#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "tests/CuTest.h"
#include "inc/ast.h"
#include "inc/eval.h"
#include "inc/prmt_handlers.h"
#include "inc/symbols.h"

static struct astnode_env *env;

// Fake test that initialzes an environment for all the other tests
void TestEval_InitEnv(CuTest *tc) {
  int err;

  err = make_top_level_env(&env);
  CuAssertIntEquals(tc, 0, err);
}

void TestEval_NullArg(CuTest *tc) {
  int err;

  err = eval(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

// Test with cons, but any symbol installed in the environment would work just
// as well.
void TestEval_Sym(CuTest *tc) {
  int err;
  char *sym;
  struct astnode_sym sym_node;
  struct astnode_prmtproc *ret;

  sym = "cons";

  sym_node.type = TYPE_SYM;
  err = putsym(sym, sym + strlen(sym) - 1, &sym_node.symi);
  CuAssertIntEquals(tc, 0, err);

  err = eval((struct astnode *) &sym_node, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_PRMTPROC, ret->type);
  CuAssertPtrEquals(tc, prmt_cons, ret->handler);
}


void TestEval_Int(CuTest *tc) {
  const int NUMVAL = 3;
  int err;
  struct astnode_int num;
  struct astnode_int *ret;

  num.type = TYPE_INT;
  num.intval = NUMVAL;

  err = eval((struct astnode *) &num, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, NUMVAL, ret->intval);
}

void TestEval_Boolean(CuTest *tc) {
  int err;
  struct astnode_boolean *arg;
  struct astnode_boolean *ret;

  arg = BOOLEAN_TRUE;

  err = eval((struct astnode *) arg, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 1, (int) ret->boolval);
}

void TestEval_PairPrmt(CuTest *tc) {
  const int VAL1 = 55;
  const int VAL2 = 66;
  int err;
  char *sym;
  struct astnode_sym sym_cons;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair *ret;

  struct astnode_pair third_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num2,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &third_pair
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_cons,
    .cdr = (struct astnode *) &sec_pair
  };

  sym = "cons";
  sym_cons.type = TYPE_SYM;
  err = putsym(sym, sym + strlen(sym) - 1, &sym_cons.symi);
  CuAssertIntEquals(tc, 0, err);

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  err = eval((struct astnode *) &first_pair, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_PAIR, ret->type);
  CuAssertIntEquals(tc, TYPE_INT, ret->car->type);
  CuAssertIntEquals(tc, VAL1, ((struct astnode_int *) ret->car)->intval);
  CuAssertIntEquals(tc, TYPE_INT, ret->cdr->type);
  CuAssertIntEquals(tc, VAL2, ((struct astnode_int *) ret->cdr)->intval);
}

void TestEval_PairCompproc(CuTest *tc) {
  // (define (fn a) a)
  int err;
  char *sym;
  struct astnode_sym sym_define;
  struct astnode_sym sym_func;
  struct astnode_sym sym_a;
  struct astnode *ret;

  struct astnode_pair third_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair binding_arg = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_a,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair binding_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_func,
    .cdr = (struct astnode *) &binding_arg
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &binding_pair,
    .cdr = (struct astnode *) &third_pair
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &sym_define,
    .cdr = (struct astnode *) &sec_pair
  };

  sym = "define";
  sym_define.type = TYPE_SYM;
  err = putsym(sym, sym + strlen(sym) - 1, &sym_define.symi);
  CuAssertIntEquals(tc, 0, err);

  sym = "func";
  sym_func.type = TYPE_SYM;
  err = putsym(sym, sym + strlen(sym) - 1, &sym_func.symi);
  CuAssertIntEquals(tc, 0, err);

  sym = "a";
  sym_a.type = TYPE_SYM;
  err = putsym(sym, sym + strlen(sym) - 1, &sym_a.symi);
  CuAssertIntEquals(tc, 0, err);

  err = eval((struct astnode *) &first_pair, env, &ret);
  CuAssertIntEquals(tc, 0, err);

  // The return value of define is undefined; instead we lookup the environment
  err = lookup_env(env, &sym_func, &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_COMPPROC, ret->type);
}

void TestEval_Env(CuTest *tc) {
  int err;
  struct astnode_env *ret;

  err = eval((struct astnode *) env, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_ENV, ret->type);
  CuAssertPtrEquals(tc, env, ret);
}

void TestEval_Keyword(CuTest *tc) {
  int err;
  struct astnode_keyword kw;
  struct astnode *ret;

  kw.type = TYPE_KEYWORD;
  kw.handler = NULL;

  err = eval((struct astnode *) &kw, env, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestEval_Prmtproc(CuTest *tc) {
  int err;
  struct astnode_prmtproc proc;
  struct astnode_prmtproc *ret;

  proc.type = TYPE_PRMTPROC;
  proc.handler = prmt_cons;

  err = eval((struct astnode *) &proc, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_PRMTPROC, ret->type);
  CuAssertPtrEquals(tc, prmt_cons, ret->handler);
}

void TestEval_Compproc(CuTest *tc) {
  int err;
  struct astnode_compproc proc;
  struct astnode_compproc *ret;

  proc.type = TYPE_COMPPROC;

  err = eval((struct astnode *) &proc, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_COMPPROC, ret->type);
}

void TestEvalMany_NullArg(CuTest *tc) {
  int err;

  err = eval_many(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

// A better test would make sure that the first element was also evaluated, for
// example with a define keyword.
void TestEvalMany_ValidObj(CuTest *tc) {
  const int VAL1 = 55;
  const int VAL2 = 66;
  int err;
  struct astnode_int num2 = {
    .type = TYPE_INT,
    .intval = VAL2
  };
  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num2,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };
  struct astnode_int *ret;

  err = eval_many(&first_pair, env, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL2, ret->intval);
}

void TestApply_NullArg(CuTest *tc) {
  int err;

  err = apply(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

CuSuite* EvalGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, TestEval_InitEnv);
  SUITE_ADD_TEST(suite, TestEval_NullArg);
  SUITE_ADD_TEST(suite, TestEval_Int);
  SUITE_ADD_TEST(suite, TestEval_Sym);
  SUITE_ADD_TEST(suite, TestEval_Boolean);
  SUITE_ADD_TEST(suite, TestEval_PairPrmt);
  SUITE_ADD_TEST(suite, TestEval_PairCompproc);
  SUITE_ADD_TEST(suite, TestEval_Env);
  SUITE_ADD_TEST(suite, TestEval_Keyword);
  SUITE_ADD_TEST(suite, TestEval_Prmtproc);
  SUITE_ADD_TEST(suite, TestEval_Compproc);
  SUITE_ADD_TEST(suite, TestEvalMany_NullArg);
  SUITE_ADD_TEST(suite, TestEvalMany_ValidObj);
  SUITE_ADD_TEST(suite, TestApply_NullArg);

  return suite;
}
