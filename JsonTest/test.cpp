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

#define TEST_ERROR(error, json)\
	do{\
		Leptjson::JsonValue jsonVal;\
		jsonVal.jsonType_ = Leptjson::JSON_FALSE;\
		EXPECT_EQ_INT(error, parse(jsonVal, json));\
		EXPECT_EQ_INT(Leptjson::JSON_NULL, getType(jsonVal));\
	} while(0)

static void parseExpectValue()
{
	TEST_ERROR(Leptjson::PARSE_EXPECT_VALUE, "");
	TEST_ERROR(Leptjson::PARSE_EXPECT_VALUE, " ");
}

static void testParseNull()
{
	Leptjson::JsonValue jsonVal;
	jsonVal.jsonType_ = Leptjson::JSON_TRUE;
	EXPECT_EQ_INT(Leptjson::PARSE_OK, parse(jsonVal, "null"));
	EXPECT_EQ_INT(Leptjson::JSON_NULL, getType(jsonVal));
}

static void testParseTrue()
{
	Leptjson::JsonValue jsonVal;
	jsonVal.jsonType_ = Leptjson::JSON_FALSE;
	EXPECT_EQ_INT(Leptjson::PARSE_OK, parse(jsonVal, "true"));
	EXPECT_EQ_INT(Leptjson::JSON_TRUE, getType(jsonVal));
}

static void testParseFalse()
{
	Leptjson::JsonValue jsonVal;
	jsonVal.jsonType_ = Leptjson::JSON_TRUE;
	EXPECT_EQ_INT(Leptjson::PARSE_OK, parse(jsonVal, "false"));
	EXPECT_EQ_INT(Leptjson::JSON_FALSE, getType(jsonVal));
}

static void testParseExpectValue()
{
	Leptjson::JsonValue jsonVal;
	jsonVal.jsonType_ = Leptjson::JSON_FALSE;
	EXPECT_EQ_INT(Leptjson::PARSE_EXPECT_VALUE, parse(jsonVal, ""));
	EXPECT_EQ_INT(Leptjson::JSON_NULL, getType(jsonVal));
}

static void testParseInvalidValue()
{
	Leptjson::JsonValue jsonVal;
	jsonVal.jsonType_ = Leptjson::JSON_FALSE;
	EXPECT_EQ_INT(Leptjson::PARSE_INVALID_VALUE, parse(jsonVal, "nul"));
	EXPECT_EQ_INT(Leptjson::JSON_NULL, getType(jsonVal));

	jsonVal.jsonType_ = Leptjson::JSON_FALSE;
	EXPECT_EQ_INT(Leptjson::PARSE_INVALID_VALUE, parse(jsonVal, "?"));
	EXPECT_EQ_INT(Leptjson::JSON_NULL, getType(jsonVal));

	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, "+0");
	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, "+1");
	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, ".123");
	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, "1.");
	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, "INF");
	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, "inf");
	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, "NAN");
	TEST_ERROR(Leptjson::PARSE_INVALID_VALUE, "nan");


}

static void testRootNotSigular()
{
	Leptjson::JsonValue jsonVal;
	jsonVal.jsonType_ = Leptjson::JSON_FALSE;
	EXPECT_EQ_INT(Leptjson::PARSE_ROOT_NOT_SINGULAR, parse(jsonVal, "null x"));
	EXPECT_EQ_INT(Leptjson::JSON_NULL, getType(jsonVal));

	TEST_ERROR(Leptjson::PARSE_ROOT_NOT_SINGULAR, "0123");
	TEST_ERROR(Leptjson::PARSE_ROOT_NOT_SINGULAR, "0x0");
	TEST_ERROR(Leptjson::PARSE_ROOT_NOT_SINGULAR, "0x123");

}

static void testParseNumberTooBig()
{
#if 0
	TEST_ERROR(Leptjson::PARSE_NUMBER_TOO_BIG, "1e309");
	TEST_ERROR(Leptjson::PARSE_NUMBER_TOO_BIG, "-1e309");
#endif // 0

}

#define TEST_NUMBER(expectNum, actualNum)\
	do{\
		Leptjson::JsonValue jsonVal;\
		EXPECT_EQ_INT(Leptjson::PARSE_OK, parse(jsonVal, actualNum));\
		EXPECT_EQ_INT(Leptjson::JSON_NUMBER, getType(jsonVal));\
		EXPECT_EQ_DOUBLE(expectNum, getNumber(jsonVal));\
	} while(0)

static void testParseNumber()
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
	TEST_NUMBER(0.0, "1e-10000");
}

static void testParse()
{
	testParseNull();
	testParseTrue();
	testParseFalse();
	testParseExpectValue();
	testParseInvalidValue();
	testRootNotSigular();
	testParseNumberTooBig();
}

int main()
{
	testParse();
	printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
	system("pause");
	return main_ret;
}