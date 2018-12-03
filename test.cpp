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
			fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
			main_ret = 1; \
		}\
	} while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")
#define EXPECT_EQ_STRING(expect, actual, alength)\
		EXPECT_EQ_BASE((sizeof(expect) -1 == alength && memcmp(expect, actual, alength) == 0), expect, actual, "%s")
#define EXPECT_EQ_TRUE(actual) EXPECT_EQ_BASE(actual, "true", "false", "%s")
#define EXPECT_EQ_FALSE(actual) EXPECT_EQ_BASE(!actual, "false", "true", "%s")



static void test_parse_null()
{
	lept::JsonValue jsonVal;
	lept_init(jsonVal);
	lept::set_boolean(jsonVal, false);
	EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, "null"));
	EXPECT_EQ_INT(lept::JsonType::JSON_NULL, lept::get_type(jsonVal));
	lept::lept_free(jsonVal);
}

static void test_parse_true()
{
	lept::JsonValue jsonVal;
	lept_init(jsonVal);
	lept::set_boolean(jsonVal, true);
	EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, "true"));
	EXPECT_EQ_INT(lept::JsonType::JSON_TRUE, get_type(jsonVal));
	lept::lept_free(jsonVal);
}

static void test_parse_false()
{
	lept::JsonValue jsonVal;
	lept::set_boolean(jsonVal, false);
	EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, "false"));
	EXPECT_EQ_INT(lept::JsonType::JSON_FALSE, get_type(jsonVal));
	lept::lept_free(jsonVal);
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
	TEST_NUMBER(0, "1e-10000");/* must underflow */

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

#define TEST_STRING(expect, json)\
	do\
	{\
		lept::JsonValue jsonVal;\
		lept_init(jsonVal);\
		EXPECT_EQ_INT(lept::PARSE_OK, parse(jsonVal, json));\
		EXPECT_EQ_INT(lept::JsonType::JSON_STRING, lept::get_type(jsonVal));\
		EXPECT_EQ_STRING(expect, lept::get_string(jsonVal), lept::get_string_length(jsonVal));\
	} while(0)

static void test_parse_string()
{
	TEST_STRING("", "\"\"");
	TEST_STRING("Hello", "\"Hello\"");
#if 1
	TEST_STRING("Hello\nWorld", "\"Hello\\nWorld\"");
    TEST_STRING("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
#endif
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
#endif // 0
}

static void test_parse_missing_quotation_mark() {
	TEST_ERROR(lept::PARSE_MISS_QUOTATION_MARK, "\"");
	TEST_ERROR(lept::PARSE_MISS_QUOTATION_MARK, "\"abc");
}

static void test_parse_invalid_string_escape()
{
#if 1
	TEST_ERROR(lept::PARSE_INVALID_STRING_ESCAPE, "\"\\v\"");
    TEST_ERROR(lept::PARSE_INVALID_STRING_ESCAPE, "\"\\'\"");
    TEST_ERROR(lept::PARSE_INVALID_STRING_ESCAPE, "\"\\0\"");
    TEST_ERROR(lept::PARSE_INVALID_STRING_ESCAPE, "\"\\x12\"");
#endif
}

static void test_parse_invalid_string_char()
{
#if 1
	TEST_ERROR(lept::PARSE_INVALID_STRING_CHAR, "\"\x01\"");
    TEST_ERROR(lept::PARSE_INVALID_STRING_CHAR, "\"\x1F\"");
#endif
}

static void test_access_null()
{
	lept::JsonValue jsonVal;
	lept_init(jsonVal);
	lept::set_string(jsonVal, "a", 1);
	set_null(jsonVal);
	EXPECT_EQ_INT(lept::JsonType::JSON_NULL, lept::get_type(jsonVal));
	lept::lept_free(jsonVal);
}

static void test_access_true()
{
	lept::JsonValue jsonVal;
	lept_init(jsonVal);
	lept::set_string(jsonVal, "a", 1);
	lept::set_boolean(jsonVal, true);
	EXPECT_EQ_TRUE(lept::get_boolean(jsonVal));
	lept::lept_free(jsonVal);
}

static void test_access_false()
{
	lept::JsonValue jsonVal;
	lept_init(jsonVal);
	lept::set_string(jsonVal, "a", 1);
	lept::set_boolean(jsonVal, false);
	EXPECT_EQ_FALSE(lept::get_boolean(jsonVal));
	lept::lept_free(jsonVal);
}

static void test_access_number()
{
	lept::JsonValue jsonVal;
	lept_init(jsonVal);
	lept::set_string(jsonVal, "a", 1);
	lept::set_number(jsonVal, 1.2345);
	EXPECT_EQ_DOUBLE(1.2345, lept::get_number(jsonVal));
	lept::lept_free(jsonVal);
}

static void test_access_string() {
	lept::JsonValue jsonVal;
	lept_init(jsonVal);
	lept::set_string(jsonVal, "", 0);
	EXPECT_EQ_STRING("", lept::get_string(jsonVal), lept::get_string_length(jsonVal));
	lept::set_string(jsonVal, "Hello", 5);
	EXPECT_EQ_STRING("Hello", lept::get_string(jsonVal), lept::get_string_length(jsonVal));
	lept_free(jsonVal);
}

static void test_parse()
{
	test_parse_null();
	test_parse_true();
	test_parse_false();
	test_parse_number();
	test_parse_string();
	test_parse_expect_value();
	test_parse_invalid_value();
	test_root_not_singular();
	test_parse_number_too_big();
	test_parse_missing_quotation_mark();
	test_parse_invalid_string_escape();
	test_parse_invalid_string_char();

	test_access_null();
	test_access_true();
	test_access_false();
	test_access_number();
	test_access_string();
}

int main()
{
	test_parse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	return main_ret;
}

/*字节序，即字节在电脑中存放时的序列与输入（输出）时的序列是先到的在前还是后到的在前。
 *
 */