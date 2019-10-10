#include "rapidjson_helpers.h"

RAPIDJSON_NAMESPACE_BEGIN

const char* ValueTypeAsString(Type valueType)
{
	switch ( valueType )
	{
		case kNullType:
		{
			return "Null";
		}

		case kFalseType:
		{
			return "False";
		}

		case kTrueType:
		{
			return "True";
		}

		case kObjectType:
		{
			return "Object";
		}

		case kArrayType:
		{
			return "Array";
		}

		case kStringType:
		{
			return "String";
		}

		case kNumberType:
		{
			return "Number";
		}

		default:
		{
			return "Unknown";
		}
	}
}

RAPIDJSON_NAMESPACE_END
