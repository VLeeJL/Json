#ifndef LEPTJSON_H_
#define LEPTJSON_H_

#include <stddef.h> /* size_t */
namespace lept
{
	enum class JsonType
	{
		JSON_NULL,
		JSON_FALSE,
		JSON_TRUE,
		JSON_NUMBER,
		JSON_STRING,
		JSON_ARRAY,
		JSON_OBJECT
	};

	struct JsonValue/* \ anonymous struct/union \ */
	{
		union
		{
			struct
			{
				size_t strlen_;
				char* str_;
			};
			double num_;
		};		
		JsonType jsonType_;		
	};

	enum PARSE_STATE
	{
		PARSE_OK,
		PARSE_EXPECT_VALUE,
		PARSE_INVALID_VALUE,
		PARSE_ROOT_NOT_SINGULAR,
		PARSE_NUMBER_TOO_BIG,
		PARSE_MISS_QUOTATION_MARK,
		PARSE_INVALID_STRING_ESCAPE,
		PARSE_INVALID_STRING_CHAR
	};

#define lept_init(val) do { val.jsonType_ = lept::JsonType::JSON_NULL; } while(0)
	PARSE_STATE parse(JsonValue& value, const char* jsonStr);
	void lept_free(JsonValue &val);
	JsonType get_type(JsonValue const& value);
#define set_null(val) lept_free(val)

	bool get_boolean(JsonValue const& val);
	void set_boolean(JsonValue& val, bool b);

	double get_number(JsonValue const& value);
	void set_number(JsonValue& val, double n);

	const char* get_string(JsonValue const& val);
	size_t get_string_length(JsonValue const& val);
	void set_string(JsonValue& val, const char* str, size_t len);
}
#endif // !LEPTJSON_H_
