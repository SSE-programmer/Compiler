/* AUTOGENERATED FILE. DO NOT EDIT. */

/*=======Automagically Detected Files To Include=====*/
#include "unity.h"
#include "../source/lexer.h"
#include "../source/parser_ast.h"
#include <stdio.h>

/*=======External Functions This Runner Calls=====*/
extern void setUp(void);
extern void tearDown(void);
extern void test_Numbers_Token(void);
extern void test_Identifier_Token(void);
extern void test_Keyword_Token(void);
extern void test_Comparison_Token(void);
extern void test_Valid_Character_Token(void);
extern void test_Invalid_Character_Token(void);
extern void test_Declaration_And_Print_Constant(void);


/*=======Mock Management=====*/
static void CMock_Init(void)
{
}
static void CMock_Verify(void)
{
}
static void CMock_Destroy(void)
{
}

/*=======Test Reset Options=====*/
void resetTest(void);
void resetTest(void)
{
  tearDown();
  CMock_Verify();
  CMock_Destroy();
  CMock_Init();
  setUp();
}
void verifyTest(void);
void verifyTest(void)
{
  CMock_Verify();
}
/*=======Test Runner Used To Run Each Test=====*/
static void run_test(UnityTestFunction func, const char* name, int line_num)
{
    Unity.CurrentTestName = name;
    Unity.CurrentTestLineNumber = line_num;
#ifdef UNITY_USE_COMMAND_LINE_ARGS
    if (!UnityTestMatches())
        return;
#endif
    Unity.NumberOfTests++;
    UNITY_CLR_DETAILS();
    UNITY_EXEC_TIME_START();
    CMock_Init();
    if (TEST_PROTECT())
    {

            setUp();
            func();

    }
    if (TEST_PROTECT())
    {
        tearDown();
        CMock_Verify();
    }
    CMock_Destroy();
    UNITY_EXEC_TIME_STOP();
    UnityConcludeTest();
}


/*=======MAIN=====*/
int main(void)
{
  UnityBegin("lexer_and_parser_tests.c");
  run_test(test_Numbers_Token, "test_Numbers_Token", 34);
  run_test(test_Identifier_Token, "test_Identifier_Token", 63);
  run_test(test_Keyword_Token, "test_Keyword_Token", 88);
  run_test(test_Comparison_Token, "test_Comparison_Token", 117);
  run_test(test_Valid_Character_Token, "test_Valid_Character_Token", 141);
  run_test(test_Invalid_Character_Token, "test_Invalid_Character_Token", 166);
  run_test(test_Declaration_And_Print_Constant, "test_Declaration_And_Print_Constant", 594);

  return UnityEnd();
}
