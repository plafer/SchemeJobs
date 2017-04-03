#include <stdint.h>
#include <errno.h>
#include <string.h>

#include "tests/CuTest.h"
#include "inc/symbols.h"

void TestPutSym_StartGtEnd(CuTest *tc) {
  char dummybuf[] = "Dummy buf";
  int ret;

  ret = putsym(&dummybuf[4], dummybuf, NULL);

  // We get EINVAL
  CuAssertIntEquals_Msg(tc, "putsym should return EINVAL when start > end",
			EINVAL, ret);
}

void TestPutSym_BufNoChange(CuTest *tc) {
  char dummybuf[] = "Dummy buf";
  uint32_t index;
  int ret;

  ret = putsym(dummybuf, dummybuf + strlen(dummybuf) - 1, &index);

  // Buffer doesn't change
  CuAssertIntEquals(tc, 0, ret);
  CuAssertStrEquals_Msg(tc, "Buffer should be left unchanged", dummybuf, "Dummy buf");
}

CuSuite* SymbolsGetSuite() {
  CuSuite* suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, TestPutSym_StartGtEnd);
  SUITE_ADD_TEST(suite, TestPutSym_BufNoChange);
  return suite;
}
