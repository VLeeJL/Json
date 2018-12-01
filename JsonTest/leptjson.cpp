#include "leptjson.h"
#include "leptjson.h"
#include <assert.h>//assert
#include <stdlib.h>//strod()
#include <string.h>//strlen()
#include <errno.h>//error
#include <math.h>//HUGE_VAL

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif // !LEPT_PARSE_STACK_INIT_SIZE


#define EXPECT(c, ch)		do {assert(*c.jsonStr_ == (ch)); c.jsonStr_++;} while(0)
#define ISDIGIT(ch)			((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)		((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)			do { *(char*)context_push(c, sizeof(char)) = (ch); } while(0)

struct  JsonContext
{
	const char* jsonStr_;
	char* stack_;
	size_t sizeStack_/* \当前堆栈的容量 */, topStack_/*\当前栈顶的位置*/;
};

static void* context_push(JsonContext& jsonContext, size_t size)
{
	assert(size > 0);
	if (jsonContext.topStack_ + size >= jsonContext.sizeStack_)
	{
		if (jsonContext.sizeStack_ == 0)
			jsonContext.sizeStack_ = LEPT_PARSE_STACK_INIT_SIZE;
		while (jsonContext.topStack_ + size >= jsonContext.sizeStack_)
			jsonContext.sizeStack_ += jsonContext.sizeStack_ >> 1;
	}
}

//const char*：常量指针，指向的元素为常量
//可以联系这种const写法：int const var = 2;
static void parse_whitespace(JsonContext& jsonContext)
{
	const char* p = jsonContext.jsonStr_;
	while (*p == ' ' || *p == '\t' || *p == '\r')
		++p;
	jsonContext.jsonStr_ = p;
}

//parse null/true/false
static lept::PARSE_STATE lept_parse_literal(const char* expectLetter, const lept::JsonType& expectJsonType, JsonContext& jsonContext, lept::JsonValue& val)
{
	EXPECT(jsonContext, expectLetter[0]);
	int lenOfExpectStr = strlen(expectLetter);
	for (int i = 0; i < lenOfExpectStr - 1; ++i)
		if (jsonContext.jsonStr_[i] != expectLetter[i + 1])
			return lept::PARSE_INVALID_VALUE;
	jsonContext.jsonStr_ += (--lenOfExpectStr);//最开始已经移动了一个字符位置
	val.jsonType_ = expectJsonType;
	return lept::PARSE_OK;
}

static lept::PARSE_STATE parse_number(JsonContext& jsonContext, lept::JsonValue& val)
{
	const char* p = jsonContext.jsonStr_;
	if (*p == '-') ++p;//user state diagram		
	if (*p == '0') ++p;		
	else
	{
		if (!ISDIGIT1TO9(*p)) return lept::PARSE_INVALID_VALUE;			
		for (; ISDIGIT(*p); ++p);
	}
	if (*p == '.')
	{
		++p;
		if (!ISDIGIT(*p)) return lept::PARSE_INVALID_VALUE;			
		for (; ISDIGIT(*p); ++p);
	}
	if (*p == 'e' || *p == 'E')
	{
		++p;
		if (*p == '+' || *p == '-') ++p;			
		if (!ISDIGIT(*p)) return lept::PARSE_INVALID_VALUE;
		for (; ISDIGIT(*p); ++p);
	}
	errno = 0;
	val.num_ = strtod(jsonContext.jsonStr_, NULL);	
	if (errno == ERANGE)// && val.num_ == HUGE_VAL is invaild,return value of strtod() is inf
		return lept::PARSE_NUMBER_TOO_BIG;
	if (jsonContext.jsonStr_ == NULL)
		return lept::PARSE_INVALID_VALUE;
	jsonContext.jsonStr_ = p;
	val.jsonType_ = lept::JsonType::JSON_NUMBER;
	return lept::PARSE_OK;
}

static lept::PARSE_STATE parse_value(JsonContext& jsonContex, lept::JsonValue& val)
{
	switch (*jsonContex.jsonStr_)
	{
	case 'n':
		return lept_parse_literal("null", lept::JsonType::JSON_NULL, jsonContex, val);
	case 't':
		return lept_parse_literal("true", lept::JsonType::JSON_TRUE, jsonContex, val);
	case 'f':
		return lept_parse_literal("false", lept::JsonType::JSON_FALSE, jsonContex, val);
	default:
		return parse_number(jsonContex, val);
	case '\0':
		return lept::PARSE_EXPECT_VALUE;
	}
}

lept::PARSE_STATE lept::parse(JsonValue& val, const char* jsonStr)
{
	JsonContext jsonContex;
	jsonContex.jsonStr_ = jsonStr;	
	parse_whitespace(jsonContex);
	PARSE_STATE ret = PARSE_INVALID_VALUE;
	val.jsonType_ = JsonType::JSON_NULL;
	if ((ret = parse_value(jsonContex, val)) == lept::PARSE_OK)
	{
		parse_whitespace(jsonContex);
		if (jsonContex.jsonStr_[0] != '\0')
		{
			val.jsonType_ = JsonType::JSON_NULL;
			ret = PARSE_ROOT_NOT_SINGULAR;
		}			
	}
	return ret;
}

lept::JsonType lept::get_type(const JsonValue& value)
{
	return value.jsonType_;
}

double lept::get_number(const JsonValue& value)
{
	assert(value.jsonType_ == JsonType::JSON_NUMBER);
	return value.num_;
}