#ifndef RAPIDJSON_HELPERS_H
#define RAPIDJSON_HELPERS_H

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

class CUtlString;

RAPIDJSON_NAMESPACE_BEGIN

const char* ValueTypeAsString(Type valueType);
bool LoadFileFromServer(const CUtlString& path, Document& document, const char* moduleName = nullptr);

#ifdef CLIENT_DLL
bool LoadFileFromClient(const CUtlString& path, Document& document, const char* moduleName = nullptr);
#endif

template<typename T>
inline T GetProperty(const Value& value, const char* name, Type type, const T& defaultValue = T())
{
	return (value.HasMember(name) && value[name].GetType() == type)
		? value[name].Get<T>()
		: defaultValue;
}

RAPIDJSON_NAMESPACE_END

#endif // RAPIDJSON_HELPERS_H