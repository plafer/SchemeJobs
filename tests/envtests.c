#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "tests/CuTest.h"
#include "inc/ast.h"
#include "inc/env.h"
#include "inc/symbols.h"

static struct astnode_env *top_level_env;

// This test MUST run first. Since the CuTest doesn't clean up between tests, we
// will call make_top_level_env once in this test, and save a pointer to it for
// the other tests to use.
void TestMakeTopEnv_NoFail(CuTest *tc) {
  int err;

  err = make_top_level_env(&top_level_env);
  CuAssertIntEquals(tc, 0, err);
}

void TestMakeTopEnv_NullArg(CuTest *tc) {
  int err;

  err = make_top_level_env(NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestExtendEnv_NullArg(CuTest *tc) {
  int err;

  err = extend_env(NULL, NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestLookupEnv_NullArg(CuTest *tc) {
  int err;

  err = lookup_env(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestDefineBinding_NullArg(CuTest *tc) {
  int err;

  err = define_binding(NULL, NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestDefineBinding_AndLookup(CuTest *tc) {
  char *sym;
  int err;
  struct astnode_sym sym_node;
  struct astnode_boolean *val;
  struct astnode *ret;

  sym = "true";
  sym_node.type = TYPE_SYM;
  err = putsym(sym, sym, &sym_node.symi);
  CuAssertIntEquals(tc, 0, err);

  val = BOOLEAN_TRUE;

  err = define_binding(top_level_env, &sym_node, (struct astnode *) val);
  CuAssertIntEquals(tc, 0, err);

  err = lookup_env(top_level_env, &sym_node, &ret);
  CuAssertIntEquals(tc, 0, err);

  CuAssertPtrEquals(tc, val, ret);
}

void TestDefineBinding_Twice(CuTest *tc) {
  char *sym;
  int err;
  struct astnode_sym sym_node;
  struct astnode_boolean *val;
  struct astnode *ret;

  sym = "true";
  sym_node.type = TYPE_SYM;
  err = putsym(sym, sym, &sym_node.symi);
  CuAssertIntEquals(tc, 0, err);

  // Define sym as BOOLEAN_TRUE
  val = BOOLEAN_TRUE;
  err = define_binding(top_level_env, &sym_node, (struct astnode *) val);
  CuAssertIntEquals(tc, 0, err);

  err = lookup_env(top_level_env, &sym_node, &ret);
  CuAssertIntEquals(tc, 0, err);

  CuAssertPtrEquals(tc, val, ret);

  // Define sym as BOOLEAN_FALSE
  val = BOOLEAN_FALSE;
  err = define_binding(top_level_env, &sym_node, (struct astnode *) val);
  CuAssertIntEquals(tc, 0, err);

  err = lookup_env(top_level_env, &sym_node, &ret);
  CuAssertIntEquals(tc, 0, err);

  CuAssertPtrEquals(tc, val, ret);
}

void TestExtendEnv_LookupParam(CuTest *tc) {
  const int BINDING_VAL = 42;
  int err;
  char *param_sym;
  struct astnode_sym sym_node;
  struct astnode_env *extended_env;
  struct astnode_pair formal_params;
  struct astnode_pair args;
  struct astnode_int *ret;

  param_sym = "param-symbol";
  sym_node.type = TYPE_SYM;
  err = putsym(param_sym, param_sym + strlen(param_sym) - 1, &sym_node.symi);
  CuAssertIntEquals(tc, 0, err);

  // Prepare formal parameters list
  formal_params.type = TYPE_PAIR;
  formal_params.car = malloc(sizeof(struct astnode_sym));
  formal_params.car->type = TYPE_SYM;
  ((struct astnode_sym *)formal_params.car)->symi = sym_node.symi;
  formal_params.cdr = (struct astnode *) EMPTY_LIST;

  // Prepare argument list
  args.type = TYPE_PAIR;
  args.car = malloc(sizeof(struct astnode_int));
  args.car->type = TYPE_INT;
  ((struct astnode_int *)args.car)->intval = BINDING_VAL;
  args.cdr = (struct astnode *) EMPTY_LIST;

  // Extend env
  err = extend_env(top_level_env,&formal_params, &args, &extended_env);
  CuAssertIntEquals(tc, 0, err);

  err = lookup_env(extended_env, &sym_node, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, BINDING_VAL, ret->intval);
}

void TestExtendEnv_DefineLocalBinding(CuTest *tc) {
  const int BINDING_VAL = 42;
  int err;
  char *param_sym;
  struct astnode_sym sym_node;
  struct astnode_env *extended_env;
  struct astnode_pair *formal_params;
  struct astnode_pair *args;
  struct astnode_int local_binding_val;
  struct astnode_int *ret;

  // Prepare formal parameters list
  formal_params = EMPTY_LIST;

  // Prepare argument list
  args = EMPTY_LIST;

  // Extend env
  err = extend_env(top_level_env, formal_params, args, &extended_env);
  CuAssertIntEquals(tc, 0, err);

  // Insert local symbol
  param_sym = "TestExtendEnv_DefineLocalBinding";
  sym_node.type = TYPE_SYM;
  err = putsym(param_sym, param_sym + strlen(param_sym) - 1, &sym_node.symi);
  CuAssertIntEquals(tc, 0, err);

  local_binding_val.type = TYPE_INT;
  local_binding_val.intval = BINDING_VAL;
  err = define_binding(extended_env, &sym_node,
		       (struct astnode *) &local_binding_val);
  CuAssertIntEquals(tc, 0, err);

  // Ensure that the symbol was bound in extended env
  err = lookup_env(extended_env, &sym_node, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, BINDING_VAL, ret->intval);

  // Ensure that the symbol was NOT bound in top level env
  err = lookup_env(top_level_env, &sym_node, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

CuSuite* EnvGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, TestMakeTopEnv_NoFail);
  SUITE_ADD_TEST(suite, TestMakeTopEnv_NullArg);
  SUITE_ADD_TEST(suite, TestExtendEnv_NullArg);
  SUITE_ADD_TEST(suite, TestLookupEnv_NullArg);
  SUITE_ADD_TEST(suite, TestDefineBinding_NullArg);
  SUITE_ADD_TEST(suite, TestDefineBinding_AndLookup);
  SUITE_ADD_TEST(suite, TestDefineBinding_Twice);
  SUITE_ADD_TEST(suite, TestExtendEnv_LookupParam);
  SUITE_ADD_TEST(suite, TestExtendEnv_DefineLocalBinding);

  return suite;
}
