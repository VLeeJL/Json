#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;


#define EXPECT_EQ_BASE(equality, expect, actual, format) \
	do {\
		test_count++;\
		if (equality)\
			test_pass++;\
		else {\
			fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, format);\
			main_ret = 1; \
		}\
	} while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")



static void test_parse_null()
{
	lept::JsonValue jsonVal;
	jsonVal.jsonType_ = lept::JsonType::JSON_NULL;
	EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, "null"));
	EXPECT_EQ_INT(lept::JsonType::JSON_NULL, lept::get_type(jsonVal));
}

static void test_parse_true()
{
	lept::JsonValue jsonVal;
	jsonVal.jsonType_ = lept::JsonType::JSON_FALSE;
	EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, "true"));
	EXPECT_EQ_INT(lept::JsonType::JSON_TRUE, get_type(jsonVal));
}

static void test_parse_false()
{
	lept::JsonValue jsonVal;
	jsonVal.jsonType_ = lept::JsonType::JSON_TRUE;
	EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, "false"));
	EXPECT_EQ_INT(lept::JsonType::JSON_FALSE, get_type(jsonVal));
}

#define TEST_NUMBER(expectNum, actualNum)\
	do{\
		lept::JsonValue jsonVal;\
		EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, actualNum));\
		EXPECT_EQ_INT(lept::JsonType::JSON_NUMBER, get_type(jsonVal));\
		EXPECT_EQ_DOUBLE(expectNum, get_number(jsonVal));\
	} while(0)

static void test_parse_number()
{
	TEST_NUMBER(0.0, "0");
	TEST_NUMBER(0.0, "-0");
	TEST_NUMBER(0.0, "-0.0");
	TEST_NUMBER(1.0, "1");
	TEST_NUMBER(-1.0, "-1");
	TEST_NUMBER(1.5, "1.5");
	TEST_NUMBER(-1.5, "-1.5");
	TEST_NUMBER(3.1416, "3.1416");
	TEST_NUMBER(1E10, "1E10");
	TEST_NUMBER(1e10, "1e10");
	TEST_NUMBER(1E+10, "1E+10");
	TEST_NUMBER(1E-10, "1E-10");
	TEST_NUMBER(-1E10, "-1E10");
	TEST_NUMBER(-1e10, "-1e10");
	TEST_NUMBER(-1E+10, "-1E+10");
	TEST_NUMBER(-1E-10, "-1E-10");
	TEST_NUMBER(1.234E+10, "1.234E+10");
	TEST_NUMBER(1.234E-10, "1.234E-10");

	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

#define TEST_ERROR(error, jsonStr)\
	do{\
		lept::JsonValue jsonVal;\
		jsonVal.jsonType_ = lept::JsonType::JSON_FALSE;\
		EXPECT_EQ_INT(error, parse(jsonVal, jsonStr));\
		EXPECT_EQ_INT(lept::JsonType::JSON_NULL, get_type(jsonVal));\
	} while(0)

static void test_parse_expect_value()
{
	TEST_ERROR(lept::PARSE_EXPECT_VALUE, "");
	TEST_ERROR(lept::PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value()
{
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "val");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "?");
#if 1
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "+0");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "+1");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, ".123");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "1.");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "INF");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "inf");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "NAN");
	TEST_ERROR(lept::PARSE_INVALID_VALUE, "nan");
#endif // 0
}

static void test_root_not_singular()
{
	TEST_ERROR(lept::PARSE_ROOT_NOT_SINGULAR, "null x");
#if 1
	TEST_ERROR(lept::PARSE_ROOT_NOT_SINGULAR, "0123");
	TEST_ERROR(lept::PARSE_ROOT_NOT_SINGULAR, "0x0");
	TEST_ERROR(lept::PARSE_ROOT_NOT_SINGULAR, "0x123");
#endif // 0
}

static void test_parse_number_too_big()
{
#if 1
	TEST_ERROR(lept::PARSE_NUMBER_TOO_BIG, "1e309");
	TEST_ERROR(lept::PARSE_NUMBER_TOO_BIG, "-1e309");
	TEST_ERROR(lept::PARSE_NUMBER_TOO_BIG, "1e-10000");/* must underflow */
#endif // 0
}

static void test_parse()
{
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_root_not_singular();
	test_parse_number_too_big();
}

int main()
{
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	system("pause");
	return main_ret;
}