#pragma once

#include "projectInterface_gamelib.h"
#include "rapidjson_helpers/rapidjson_helpers.h"

RAPIDJSON_NAMESPACE_BEGIN

inline bool LoadJsonFileOnServer(const CUtlString& path, Document& document, const char* moduleName = nullptr)
{
	return LoadJsonFile(path, document, ProjectInterface_Server::StaticInstance(), moduleName);
}

#ifdef CLIENT_DLL
inline bool LoadJsonFileOnClient(const CUtlString& path, Document& document, const char* moduleName = nullptr)
{
	return LoadJsonFile(path, document, ProjectInterface_Client::StaticInstance(), moduleName);
}
#endif

RAPIDJSON_NAMESPACE_END
