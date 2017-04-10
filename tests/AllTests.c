#include <stdio.h>

#include "CuTest.h"

CuSuite* SymbolsGetSuite();
CuSuite* EnvGetSuite();
CuSuite* PrmtGetSuite();
CuSuite* EvalGetSuite();
CuSuite* KwGetSuite();


// Note: CuSuite runs all the tests in the same process (i.e. changes made in
// one test are still there in all other tests that run after).
void RunAllTests(void)
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, SymbolsGetSuite());
	CuSuiteAddSuite(suite, EnvGetSuite());
	CuSuiteAddSuite(suite, PrmtGetSuite());
	CuSuiteAddSuite(suite, EvalGetSuite());
	CuSuiteAddSuite(suite, KwGetSuite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

int main(void)
{
	RunAllTests();
}
