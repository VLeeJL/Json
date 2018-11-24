#ifndef LEPTJSON_H_
#define LEPTJSON_H_

#include <memory>
#include <string.h>
#include <assert.h>

namespace Leptjson 
{
	enum JsonType 
	{
		NONE_TYPE = -1,
		JSON_NULL,
		JSON_FALSE,
		JSON_TRUE,
		JSON_NUMBER,
		JSON_STRING,
		JSON_ARRAY,
		JSON_OBJECT
	};

	enum ParseSstate 
	{
		NONE_PARSE = -1,
		PARSE_OK = 0, 
		PARSE_EXPECT_VALUE,
		PARSE_INVALID_VALUE,
		PARSE_ROOT_NOT_SINGULAR,
		PARSE_NUMBER_TOO_BIG,
	};

	struct JsonValue
	{
		double number_;
		JsonType jsonType_;
	};

	struct JsonContext
	{
		const char* jsonStr_;
	};

	
	JsonType getType(const JsonValue& jasonVal);
	double getNumber(const JsonValue& jasonVal);

#define EXPECT(jsonContext, jsonStr) do {assert(*(jsonContext.jsonStr_) == (jsonStr)); ++(jsonContext.jsonStr_);} while(0)

	/* ws = *(%x20 / %x09 / %x0A / %x0D) */
	static void parseWhitespace(JsonContext& jsonContext);
	/* null  = "null" */
	//static Leptjson::ParseSstate parseNull(JsonContext& jsonContext, JsonValue& jsonVal);
	///* true  = "true" */
	//static Leptjson::ParseSstate parseTrue(JsonContext& jsonContext, JsonValue& jsonVal);
	///* false  = "false" */
	//static Leptjson::ParseSstate parseFalse(JsonContext& jsonContext, JsonValue& jsonVal);
	static Leptjson::ParseSstate parseLiteral(JsonContext& jsonContext, JsonValue& jsonVal, const char* concreptLiteral, const JsonType& jsonType);
	/*  */
	static Leptjson::ParseSstate parseNumber(JsonContext& jsonContext, JsonValue& jsonVal);
	/* value = null / false / true */
	static ParseSstate parseVal(JsonContext& jsonContext, JsonValue& jsonVal);
	/* parse */
	ParseSstate parse(JsonValue& jsonVal, const char* jsonStr);
}
#endif // !LEPTJSON_H_
