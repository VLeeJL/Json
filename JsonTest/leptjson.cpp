#include "leptjson.h"

Leptjson::ParseSstate Leptjson::parse(JsonValue & jsonVal, const char * jsonStr)
{
	JsonContext jsonContext;
	jsonContext.jsonStr_ = jsonStr;
	jsonVal.jsonType_ = JSON_NULL;
	parseWhitespace(jsonContext);
	ParseSstate ret = NONE_PARSE;
	if ((ret = parseVal(jsonContext, jsonVal)) == PARSE_OK)
	{
		parseWhitespace(jsonContext);
		if (*jsonContext.jsonStr_ != '\0')
			ret = PARSE_ROOT_NOT_SINGULAR;
	}
	return ret;
}

Leptjson::JsonType Leptjson::getType(const JsonValue & jasonVal)
{
	return jasonVal.jsonType_;
}

double Leptjson::getNumber(const Leptjson::JsonValue& jasonVal)
{
	assert(jasonVal.jsonType_ != Leptjson::JSON_NUMBER);
	return jasonVal.number_;
}

/* value = null / false / true */
Leptjson::ParseSstate Leptjson::parseVal(JsonContext & jsonContext, JsonValue & jsonVal)
{
	switch (*jsonContext.jsonStr_)
	{
	case 'n':
		return parseLiteral(jsonContext, jsonVal, "null", JSON_NULL);
	case 't':
		return parseLiteral(jsonContext, jsonVal, "true", JSON_TRUE);
	case 'f':
		return parseLiteral(jsonContext, jsonVal, "false", JSON_FALSE);
	default:
		return parseNumber(jsonContext, jsonVal);
	case '\0':
		return PARSE_EXPECT_VALUE;	
	}
}


/* ws = *(%x20 / %x09 / %x0A / %x0D) */
void Leptjson::parseWhitespace(JsonContext & jsonContext)
{
	const char* str = jsonContext.jsonStr_;
	while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')
		++str;
	jsonContext.jsonStr_ = str;
}

///* null  = "null" */
//Leptjson::ParseSstate Leptjson::parseNull(JsonContext & jsonContext, JsonValue & jsonVal)
//{
//	EXPECT(jsonContext, 'n');
//	if (jsonContext.jsonStr_[0] != 'u' || jsonContext.jsonStr_[1] != 'l' || jsonContext.jsonStr_[2] != 'l')
//		return PARSE_INVALID_VALUE;
//	jsonContext.jsonStr_ += 3;
//	jsonVal.jsonType_ = JSON_NULL;
//	return PARSE_OK;
//}
//
//Leptjson::ParseSstate Leptjson::parseTrue(JsonContext & jsonContext, JsonValue & jsonVal)
//{
//	EXPECT(jsonContext, 't');
//	if (jsonContext.jsonStr_[0] != 'r' || jsonContext.jsonStr_[1] != 'u' || jsonContext.jsonStr_[2] != 'e')
//		return PARSE_INVALID_VALUE;
//	jsonContext.jsonStr_ += 3;
//	jsonVal.jsonType_ = JSON_TRUE;
//	return PARSE_OK;
//}
//
//Leptjson::ParseSstate Leptjson::parseFalse(JsonContext & jsonContext, JsonValue & jsonVal)
//{
//	EXPECT(jsonContext, 'f');
//	if (jsonContext.jsonStr_[0] != 'a' || jsonContext.jsonStr_[1] != 'l' || jsonContext.jsonStr_[2] != 's' || jsonContext.jsonStr_[3] != 'e')
//		return PARSE_INVALID_VALUE;
//	jsonContext.jsonStr_ += 4;
//	jsonVal.jsonType_ = JSON_FALSE;
//	return PARSE_OK;
//}

Leptjson::ParseSstate Leptjson::parseLiteral(JsonContext& jsonContext, JsonValue& jsonVal, const char* concreptLiteral, const JsonType& jsonType)
{
	EXPECT(jsonContext, concreptLiteral[0]);
	size_t  i = 0;
	for (; concreptLiteral[i + 1]; ++i)
		if (jsonContext.jsonStr_[i] != concreptLiteral[i + 1])
			return PARSE_INVALID_VALUE;
	jsonContext.jsonStr_ += i;
	jsonVal.jsonType_ = jsonType;
	return PARSE_OK;
}

Leptjson::ParseSstate Leptjson::parseNumber(JsonContext& jsonContext, JsonValue& jsonVal)
{
	char* end = nullptr;
	/* TODO validate number*/
	jsonVal.number_ = strtod(jsonContext.jsonStr_, &end);
	if (jsonContext.jsonStr_ == end)//若end和jsonStr_相等，即jsonStr_为空
		return PARSE_INVALID_VALUE;
	jsonVal.jsonType_ = JSON_NUMBER;
	jsonContext.jsonStr_ = end;
	return PARSE_OK;
}