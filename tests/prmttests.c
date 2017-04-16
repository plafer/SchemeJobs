#include <errno.h>
#include <stdbool.h>
#include <string.h>

#include "tests/CuTest.h"
#include "inc/ast.h"
#include "inc/prmt_handlers.h"


void TestCons_NullArgs(CuTest *tc) {
  int err;

  err = prmt_cons(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestCons_ValidInts(CuTest *tc) {
  const int VAL1 = 42;
  const int VAL2 = 56;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair first_pair;
  struct astnode_pair sec_pair;
  struct astnode_pair *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  first_pair.type = TYPE_PAIR;
  first_pair.car = (struct astnode *) &num1;
  first_pair.cdr = (struct astnode *) &sec_pair;

  sec_pair.type = TYPE_PAIR;
  sec_pair.car = (struct astnode *) &num2;
  sec_pair.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_cons(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);

  // Should should have received (1 . 2)
  CuAssertIntEquals(tc, TYPE_PAIR, ret->type);
  CuAssertIntEquals(tc, TYPE_INT, ret->car->type);
  CuAssertIntEquals(tc, TYPE_INT, ret->cdr->type);
  CuAssertIntEquals(tc,
		    ((struct astnode_int *) ret->car)->intval,
		    VAL1);
  CuAssertIntEquals(tc,
		    ((struct astnode_int *) ret->cdr)->intval,
		    VAL2);
}

void TestCons_NoArgs(CuTest *tc) {
  int err;
  struct astnode_pair *arglist;
  struct astnode *ret;

  arglist = EMPTY_LIST;
  err = prmt_cons(arglist, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestCons_OneArg(CuTest *tc) {
  const int VAL1 = 42;
  int err;
  struct astnode_int num1;
  struct astnode_pair first_pair;
  struct astnode_pair *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  first_pair.type = TYPE_PAIR;
  first_pair.car = (struct astnode *) &num1;
  first_pair.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_cons(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
 }

void TestCons_TooManyArgs(CuTest *tc) {
  const int VAL1 = 42;
  const int VAL2 = 56;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair first_pair;
  struct astnode_pair sec_pair;
  struct astnode_pair third_pair;
  struct astnode_pair *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  first_pair.type = TYPE_PAIR;
  first_pair.car = (struct astnode *) &num1;
  first_pair.cdr = (struct astnode *) &sec_pair;

  sec_pair.type = TYPE_PAIR;
  sec_pair.car = (struct astnode *) &num2;
  sec_pair.cdr = (struct astnode *) &third_pair;

  third_pair.type = TYPE_PAIR;
  third_pair.car = (struct astnode *) &num2;
  third_pair.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_cons(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestCar_NullArgs(CuTest *tc) {
  int err;

  err = prmt_car(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestCar_ValidObj(CuTest *tc) {
  const int VAL1 = 121;
  const int VAL2 = 232;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair obj;
  struct astnode_pair arglist;
  struct astnode_int *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  obj.type = TYPE_PAIR;
  obj.car = (struct astnode *) &num1;
  obj.cdr = (struct astnode *) &num2;

  arglist.type = TYPE_PAIR;
  arglist.car = (struct astnode *) &obj;
  arglist.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_car(&arglist, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);

  // Should should have received (1 . 2)
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL1, ret->intval);
}

void TestCar_NoArgs(CuTest *tc) {
  int err;
  struct astnode_pair *arglist;
  struct astnode *ret;

  arglist = EMPTY_LIST;
  err = prmt_car(arglist, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestCar_TooManyArgs(CuTest *tc) {
  const int VAL1 = 42;
  const int VAL2 = 56;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair first_pair;
  struct astnode_pair sec_pair;
  struct astnode *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  first_pair.type = TYPE_PAIR;
  first_pair.car = (struct astnode *) &num1;
  first_pair.cdr = (struct astnode *) &sec_pair;

  sec_pair.type = TYPE_PAIR;
  sec_pair.car = (struct astnode *) &num2;
  sec_pair.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_car(&first_pair, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestCdr_NullArgs(CuTest *tc) {
  int err;

  err = prmt_cdr(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestCdr_NoArgs(CuTest *tc) {
  int err;
  struct astnode_pair *arglist;
  struct astnode *ret;

  arglist = EMPTY_LIST;
  err = prmt_cdr(arglist, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestCdr_TooManyArgs(CuTest *tc) {
  const int VAL1 = 42;
  const int VAL2 = 56;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair first_pair;
  struct astnode_pair sec_pair;
  struct astnode *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  first_pair.type = TYPE_PAIR;
  first_pair.car = (struct astnode *) &num1;
  first_pair.cdr = (struct astnode *) &sec_pair;

  sec_pair.type = TYPE_PAIR;
  sec_pair.car = (struct astnode *) &num2;
  sec_pair.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_cdr(&first_pair, &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}


void TestCdr_ValidObj(CuTest *tc) {
  const int VAL1 = 121;
  const int VAL2 = 232;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair obj;
  struct astnode_pair arglist;
  struct astnode_int *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  obj.type = TYPE_PAIR;
  obj.car = (struct astnode *) &num1;
  obj.cdr = (struct astnode *) &num2;

  arglist.type = TYPE_PAIR;
  arglist.car = (struct astnode *) &obj;
  arglist.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_cdr(&arglist, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);

  // Should should have received (1 . 2)
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL2, ret->intval);
}

void TestIsPair_NullArgs(CuTest *tc) {
  int err;

  err = prmt_is_pair(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestIsPair_NoArgs(CuTest *tc) {
  int err;
  struct astnode_pair *arglist;
  struct astnode_boolean *ret;

  arglist = EMPTY_LIST;
  err = prmt_is_pair(arglist, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestIsPair_ValidObj(CuTest *tc) {
  int err;
  struct astnode_pair arglist;
  struct astnode_pair obj;
  struct astnode_int dummy;
  struct astnode_boolean *ret;

  arglist.type = TYPE_PAIR;
  arglist.car = (struct astnode *) &obj;
  arglist.cdr = (struct astnode *) EMPTY_LIST;

  obj.type = TYPE_PAIR;
  obj.car = (struct astnode *) &dummy;
  obj.cdr = (struct astnode *) &dummy;

  dummy.type = TYPE_INT;
  dummy.intval = 3;

  err = prmt_is_pair(&arglist, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 1, (int) ret->boolval);

}

void TestIsPair_EmptyList(CuTest *tc) {
  int err;
  struct astnode_pair arglist;
  struct astnode_boolean *ret;

  arglist.type = TYPE_PAIR;
  arglist.car = (struct astnode *) EMPTY_LIST;
  arglist.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_is_pair(&arglist, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 0, (int) ret->boolval);
}

void TestIsPair_InvalidObj(CuTest *tc) {
  int err;
  struct astnode_pair arglist;
  struct astnode_int obj;
  struct astnode_boolean *ret;

  arglist.type = TYPE_PAIR;
  arglist.car = (struct astnode *) &obj;
  arglist.cdr = (struct astnode *) EMPTY_LIST;

  obj.type = TYPE_INT;
  obj.intval = 3;

  err = prmt_is_pair(&arglist, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 0, (int) ret->boolval);
}

void TestIsPair_TooManyArgs(CuTest *tc) {
  const int VAL1 = 42;
  const int VAL2 = 56;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair first_pair;
  struct astnode_pair sec_pair;
  struct astnode_boolean *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  first_pair.type = TYPE_PAIR;
  first_pair.car = (struct astnode *) &num1;
  first_pair.cdr = (struct astnode *) &sec_pair;

  sec_pair.type = TYPE_PAIR;
  sec_pair.car = (struct astnode *) &num2;
  sec_pair.cdr = (struct astnode *) EMPTY_LIST;

  err = prmt_is_pair(&first_pair,  (struct astnode **)&ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestPlus_NullArgs(CuTest *tc) {
  int err;

  err = prmt_plus(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestPlus_ValidObj(CuTest *tc) {
  const int VAL1 = 3;
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
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_plus(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL1 + VAL1, ret->intval);
}

void TestPlus_NoArgs(CuTest *tc) {
  int err;
  struct astnode_int *ret;
  struct astnode_pair *first_pair = EMPTY_LIST;

  err = prmt_plus(first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, 0, ret->intval);
}

void TestPlus_WrongType(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_plus(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestMinus_NullArgs(CuTest *tc) {
  int err;

  err = prmt_minus(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestMinus_ValidObj(CuTest *tc) {
  const int VAL1 = 3;
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
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_minus(&first_pair, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL1 - VAL1, ret->intval);
}

void TestMinus_NoArgs(CuTest *tc) {
  int err;
  struct astnode_int *ret;
  struct astnode_pair *first_pair = EMPTY_LIST;

  err = prmt_minus(first_pair, (struct astnode **)&ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestMinus_OneArg(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) EMPTY_LIST
  };

  err = prmt_minus(&first_pair, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, -VAL1, ret->intval);
}

void TestMinus_WrongType(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_minus(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}


void TestMult_NullArgs(CuTest *tc) {
  int err;

  err = prmt_mult(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestMult_NoArgs(CuTest *tc) {
  int err;
  struct astnode_int *ret;
  struct astnode_pair *first_pair = EMPTY_LIST;

  err = prmt_mult(first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, 1, ret->intval);
}

void TestMult_ValidObj(CuTest *tc) {
  const int VAL1 = 3;
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
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_mult(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL1 * VAL1, ret->intval);
}

void TestMult_WrongType(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_mult(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestDiv_NullArgs(CuTest *tc) {
  int err;

  err = prmt_div(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestDiv_NoArgs(CuTest *tc) {
  int err;
  struct astnode_int *ret;
  struct astnode_pair *first_pair = EMPTY_LIST;

  err = prmt_div(first_pair, (struct astnode **)&ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestDiv_ValidObj(CuTest *tc) {
  const int VAL1 = 3;
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
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_div(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, VAL1 / VAL1, ret->intval);
}

void TestDiv_OneArg(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) EMPTY_LIST
  };

  err = prmt_div(&first_pair, (struct astnode **)&ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_INT, ret->type);
  CuAssertIntEquals(tc, 1 / VAL1, ret->intval);
}

void TestEqual_NullArgs(CuTest *tc) {
  int err;

  err = prmt_equal(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestEqual_NoArgs(CuTest *tc) {
  int err;
  struct astnode_boolean *ret;
  struct astnode_pair *first_pair = EMPTY_LIST;

  err = prmt_equal(first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 1, (int) ret->boolval);
}

void TestEqual_ValidObjTrue(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_boolean *ret;

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
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_equal(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 1, (int) ret->boolval);
}

void TestEqual_ValidObjFalse(CuTest *tc) {
  const int VAL1 = 3;
  const int VAL2 = 42;
  int err;
  struct astnode_boolean *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_int num2 = {
    .type = TYPE_INT,
    .intval = VAL2
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

  err = prmt_equal(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 0, (int) ret->boolval);
}

void TestEqual_WrongType(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_boolean *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_equal(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestIsEq_NullArgs(CuTest *tc) {
  int err;

  err = prmt_is_eq(NULL, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestIsEq_NoArgs(CuTest *tc) {
  int err;
  struct astnode_boolean *ret;
  struct astnode_pair *first_pair = EMPTY_LIST;

  err = prmt_is_eq(first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestIsEq_OneArg(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_int *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) EMPTY_LIST
  };

  err = prmt_is_eq(&first_pair, (struct astnode **)&ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}

void TestIsEq_ValidObjTrue(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_boolean *ret;

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
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_is_eq(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 1, (int) ret->boolval);
}

void TestIsEq_ValidObjFalse(CuTest *tc) {
  const int VAL1 = 3;
  int err;
  struct astnode_boolean *ret;

  struct astnode_int num1 = {
    .type = TYPE_INT,
    .intval = VAL1
  };
  struct astnode_pair sec_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) BOOLEAN_FALSE,
    .cdr = (struct astnode *) EMPTY_LIST
  };
  struct astnode_pair first_pair = {
    .type = TYPE_PAIR,
    .car = (struct astnode *) &num1,
    .cdr = (struct astnode *) &sec_pair
  };

  err = prmt_is_eq(&first_pair, (struct astnode **) &ret);
  CuAssertIntEquals(tc, 0, err);
  CuAssertIntEquals(tc, TYPE_BOOLEAN, ret->type);
  CuAssertIntEquals(tc, 0, (int) ret->boolval);
}

void TestIsEq_TooManyArgs(CuTest *tc) {
  const int VAL1 = 42;
  const int VAL2 = 56;
  int err;
  struct astnode_int num1;
  struct astnode_int num2;
  struct astnode_pair first_pair;
  struct astnode_pair sec_pair;
  struct astnode_pair third_pair;
  struct astnode_boolean *ret;

  num1.type = TYPE_INT;
  num1.intval = VAL1;

  num2.type = TYPE_INT;
  num2.intval = VAL2;

  first_pair.type = TYPE_PAIR;
  first_pair.car = (struct astnode *) &num1;
  first_pair.cdr = (struct astnode *) &sec_pair;

  sec_pair.type = TYPE_PAIR;
  sec_pair.car = (struct astnode *) &num2;
  sec_pair.cdr = (struct astnode *) &third_pair;

  third_pair.type = TYPE_PAIR;
  third_pair.car = (struct astnode *) &num2;
  third_pair.cdr = (struct astnode *)EMPTY_LIST;

  err = prmt_is_eq(&first_pair,  (struct astnode **)&ret);
  CuAssertIntEquals(tc, EBADMSG, err);
}


CuSuite* PrmtGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, TestCons_NullArgs);
  SUITE_ADD_TEST(suite, TestCons_NoArgs);
  SUITE_ADD_TEST(suite, TestCons_OneArg);
  SUITE_ADD_TEST(suite, TestCons_ValidInts);
  SUITE_ADD_TEST(suite, TestCons_TooManyArgs);
  SUITE_ADD_TEST(suite, TestCar_NullArgs);
  SUITE_ADD_TEST(suite, TestCar_NoArgs);
  SUITE_ADD_TEST(suite, TestCar_ValidObj);
  SUITE_ADD_TEST(suite, TestCar_TooManyArgs);
  SUITE_ADD_TEST(suite, TestCdr_NullArgs);
  SUITE_ADD_TEST(suite, TestCdr_NoArgs);
  SUITE_ADD_TEST(suite, TestCdr_ValidObj);
  SUITE_ADD_TEST(suite, TestCdr_TooManyArgs);
  SUITE_ADD_TEST(suite, TestIsPair_NullArgs);
  SUITE_ADD_TEST(suite, TestIsPair_NoArgs);
  SUITE_ADD_TEST(suite, TestIsPair_ValidObj);
  SUITE_ADD_TEST(suite, TestIsPair_EmptyList);
  SUITE_ADD_TEST(suite, TestIsPair_InvalidObj);
  SUITE_ADD_TEST(suite, TestIsPair_TooManyArgs);
  SUITE_ADD_TEST(suite, TestPlus_NullArgs);
  SUITE_ADD_TEST(suite, TestPlus_ValidObj);
  SUITE_ADD_TEST(suite, TestPlus_NoArgs);
  SUITE_ADD_TEST(suite, TestPlus_WrongType);
  SUITE_ADD_TEST(suite, TestMinus_NullArgs);
  SUITE_ADD_TEST(suite, TestMinus_ValidObj);
  SUITE_ADD_TEST(suite, TestMinus_NoArgs);
  SUITE_ADD_TEST(suite, TestMinus_OneArg);
  SUITE_ADD_TEST(suite, TestMinus_WrongType);
  SUITE_ADD_TEST(suite, TestMult_NullArgs);
  SUITE_ADD_TEST(suite, TestMult_NoArgs);
  SUITE_ADD_TEST(suite, TestMult_ValidObj);
  SUITE_ADD_TEST(suite, TestMult_WrongType);
  SUITE_ADD_TEST(suite, TestDiv_NullArgs);
  SUITE_ADD_TEST(suite, TestDiv_NoArgs);
  SUITE_ADD_TEST(suite, TestDiv_ValidObj);
  SUITE_ADD_TEST(suite, TestDiv_OneArg);
  SUITE_ADD_TEST(suite, TestEqual_NullArgs);
  SUITE_ADD_TEST(suite, TestEqual_NoArgs);
  SUITE_ADD_TEST(suite, TestEqual_ValidObjTrue);
  SUITE_ADD_TEST(suite, TestEqual_ValidObjFalse);
  SUITE_ADD_TEST(suite, TestEqual_WrongType);
  SUITE_ADD_TEST(suite, TestIsEq_NullArgs);
  SUITE_ADD_TEST(suite, TestIsEq_NoArgs);
  SUITE_ADD_TEST(suite, TestIsEq_OneArg);
  SUITE_ADD_TEST(suite, TestIsEq_ValidObjTrue);
  SUITE_ADD_TEST(suite, TestIsEq_ValidObjFalse);
  SUITE_ADD_TEST(suite, TestIsEq_TooManyArgs);

  return suite;
}
