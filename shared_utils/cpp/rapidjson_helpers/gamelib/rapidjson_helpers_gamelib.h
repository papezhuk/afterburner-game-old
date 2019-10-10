#pragma once

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

class CUtlString;

RAPIDJSON_NAMESPACE_BEGIN

bool LoadFileFromServer(const CUtlString& path, Document& document, const char* moduleName = nullptr);

#ifdef CLIENT_DLL
bool LoadFileFromClient(const CUtlString& path, Document& document, const char* moduleName = nullptr);
#endif

RAPIDJSON_NAMESPACE_END
