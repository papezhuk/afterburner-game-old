#pragma once

#include "projectInterface_mainui.h"
#include "rapidjson_helpers/rapidjson_helpers.h"

RAPIDJSON_NAMESPACE_BEGIN

inline bool LoadJsonFileForUI(const CUtlString& path, Document& document, const char* moduleName = nullptr)
{
	return LoadJsonFile(path, document, ProjectInterface_MainUI::StaticInstance(), moduleName);
}

RAPIDJSON_NAMESPACE_END
