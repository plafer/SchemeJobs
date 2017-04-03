#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "tests/CuTest.h"
#include "inc/symbols.h"

void TestPutSym_StartGtEnd(CuTest *tc) {
  char dummybuf[] = "TestPutSym_StartGtEnd";
  void *index;
  int err;

  err = putsym(&dummybuf[4], dummybuf, &index);

  // We get EINVAL
  CuAssertIntEquals_Msg(tc, "putsym should return EINVAL when start > end",
			EINVAL, err);
}

void TestPutSym_StartEqEnd(CuTest *tc) {
  char dummybuf[] = "TestPutSym_StartEqEnd";
  void *index;
  int err;

  err = putsym(dummybuf, dummybuf, &index);

  // We get no problem
  CuAssertIntEquals_Msg(tc, "putsym should not complain when start == end",
			0, err);
}

void TestPutSym_StartEndNull(CuTest *tc) {
  char dummybuf[] = "TestPutSym_StartEndNull";
  void *index;
  int err;

  err = putsym(NULL, dummybuf, &index);
  CuAssertIntEquals_Msg(tc, "A NULL symval_start should fail.", EINVAL, err);

  err = putsym(dummybuf, NULL, &index);
  CuAssertIntEquals_Msg(tc, "A NULL symval_end should fail.", EINVAL, err);

  err = putsym(NULL, NULL, &index);
  CuAssertIntEquals_Msg(tc, "A NULL symval_start and symval_end should fail.",
			EINVAL, err);
}

void TestPutSym_BufNoChange(CuTest *tc) {
  char dummybuf[] = "TestPutSym_BufNoChange";
  void *index;
  int err;

  err = putsym(dummybuf, dummybuf + strlen(dummybuf) - 1, &index);

  // Buffer doesn't change
  CuAssertIntEquals(tc, 0, err);
  CuAssertStrEquals_Msg(tc, "Buffer should be left unchanged", dummybuf,
			"TestPutSym_BufNoChange");
}

void TestPutSym_NullIndex(CuTest *tc) {
  char dummybuf[] = "TestPutSym_NullIndex";
  int err;

  err = putsym(dummybuf, dummybuf + strlen(dummybuf) - 1, NULL);

  // We shouldn't get an error
  CuAssertIntEquals(tc, 0, err);
}

void TestPutSym_AndGet(CuTest *tc) {
  char sym[] = "TestPutSym_AndGet";
  const char *retrieved_sym;
  void *putindex;
  int err;

  err = putsym(sym, sym + strlen(sym) - 1, &putindex);
  CuAssertIntEquals(tc, 0, err);

  err = getsym(putindex, &retrieved_sym);
  CuAssertIntEquals(tc, 0, err);

  CuAssertStrEquals_Msg(tc, "Put a sym, and retrieval failed.", sym, retrieved_sym);
}

void TestPutSym_TwiceAndGetSameVal(CuTest *tc) {
  char sym[] = "TestPutSym_TwiceAndGetSameVal";
  const char *getsym1;
  const char *getsym2;
  void *putindex1;
  void *putindex2;
  int err;

  err = putsym(sym, sym + strlen(sym) - 1, &putindex1);
  CuAssertIntEquals(tc, 0, err);

  err = putsym(sym, sym + strlen(sym) - 1, &putindex2);
  CuAssertIntEquals(tc, 0, err);

  err = getsym(putindex1, &getsym1);
  CuAssertIntEquals(tc, 0, err);

  err = getsym(putindex2, &getsym2);
  CuAssertIntEquals(tc, 0, err);

  CuAssertPtrEquals_Msg(tc, "Putting same symbol twice and getting should give "
			"same index", putindex1, putindex2);
  CuAssertStrEquals(tc, sym, getsym1);
  CuAssertStrEquals(tc, sym, getsym2);
}

// We have Check1 and Check2 to test both off by one possibilities: either the
// longest string is already in the table, or it isn't.
void TestPutSym_OffByOneCheck1(CuTest *tc) {
  char sym1[] = "z";
  char sym2[] = "zz";
  void *index1;
  void *index2;
  int err;

  err = putsym(sym1, sym1 + strlen(sym1) - 1, &index1);
  CuAssertIntEquals(tc, 0, err);

  err = putsym(sym2, sym2 + strlen(sym2) - 1, &index2);
  CuAssertIntEquals(tc, 0, err);

  // Work around CuTest not having "CuAssertPtrNotEquals"
  if (index1 == index2)
    index1 = NULL;
  CuAssertPtrNotNullMsg(tc, "\"z\" and \"zz\" were treated as the same symbol",
			index1);
}

void TestPutSym_OffByOneCheck2(CuTest *tc) {
  char sym1[] = "a";
  char sym2[] = "aa";
  void *index1;
  void *index2;
  int err;

  err = putsym(sym2, sym2 + strlen(sym2) - 1, &index2);
  CuAssertIntEquals(tc, 0, err);

  err = putsym(sym1, sym1 + strlen(sym1) - 1, &index1);
  CuAssertIntEquals(tc, 0, err);

  // Work around CuTest not having "CuAssertPtrNotEquals"
  if (index1 == index2)
    index1 = NULL;
  CuAssertPtrNotNullMsg(tc, "\"a\" and \"aa\" were treated as the same symbol",
			index1);
}

void TestGetSym_InvalidIndex(CuTest *tc) {
  char dummyvar;
  const char *inexistant_sym;
  int err;

  // An index pointer should never point in our stack, so it's safe to assume
  // &dummyvar is an invalid index.
  err = getsym((void *)&dummyvar, &inexistant_sym);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestGetSym_NullSymval(CuTest *tc) {
  int err;

  err = getsym(0, NULL);
  CuAssertIntEquals(tc, EINVAL, err);
}

void TestPutSym_SecondPageAndGet(CuTest *tc) {
  #define BUFSIZE 40
  char *lastsymbol = "Hello, world!";
  const char *lastsymbolget;
  char buf[BUFSIZE];
  int i;
  int err;
  void *index;

  memset(buf, ' ', BUFSIZE);

  // Allocate a few full pages of symbols (4096 symbols on 32-bit machine)
  for (i = 0; i < BUFSIZE; i++)
    {
      // '~' == ASCII 126
      for ( ; buf[i] < '~'; ++buf[i])
	{
	  err = putsym(buf, buf + BUFSIZE - 1, NULL);
	  CuAssertIntEquals(tc, 0, err);
	}
    }

  // Put a symbol and retrieve
  err = putsym(lastsymbol, lastsymbol + strlen(lastsymbol) - 1, &index);
  CuAssertIntEquals(tc, 0, err);

  err = getsym(index, &lastsymbolget);
  CuAssertIntEquals(tc, 0, err);

  CuAssertStrEquals_Msg(tc, "Putting/getting on a second page fails.",
			lastsymbol, lastsymbolget);

  #undef BUFSIZE
}

CuSuite* SymbolsGetSuite() {
  CuSuite* suite = CuSuiteNew();

  SUITE_ADD_TEST(suite, TestPutSym_StartGtEnd);
  SUITE_ADD_TEST(suite, TestPutSym_StartEqEnd);
  SUITE_ADD_TEST(suite, TestPutSym_StartEndNull);
  SUITE_ADD_TEST(suite, TestPutSym_BufNoChange);
  SUITE_ADD_TEST(suite, TestPutSym_NullIndex);
  SUITE_ADD_TEST(suite, TestPutSym_AndGet);
  SUITE_ADD_TEST(suite, TestPutSym_TwiceAndGetSameVal);
  SUITE_ADD_TEST(suite, TestPutSym_OffByOneCheck1);
  SUITE_ADD_TEST(suite, TestPutSym_OffByOneCheck2);
  SUITE_ADD_TEST(suite, TestGetSym_InvalidIndex);
  SUITE_ADD_TEST(suite, TestGetSym_NullSymval);
  SUITE_ADD_TEST(suite, TestPutSym_SecondPageAndGet);

  return suite;
}
