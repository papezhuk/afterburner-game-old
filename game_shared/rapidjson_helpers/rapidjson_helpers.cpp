#include "rapidjson_helpers.h"
#include "standard_includes.h"
#include "utlstring.h"
#include "eiface.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

#ifdef CLIENT_DLL
#include "cl_dll.h"
#endif

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

bool LoadFileFromServer(const CUtlString& path, Document& document, const char* moduleName)
{
	int length = 0;
	byte* fileData = LOAD_FILE_FOR_ME(path.String(), &length);

	if ( !fileData )
	{
		if ( moduleName )
		{
			ALERT(at_error, "%s: Could not load file %s.\n", moduleName, path.String());
		}
		else
		{
			ALERT(at_error, "Could not load file %s.\n", path.String());
		}
		
		return false;
	}

	rapidjson::ParseResult parseResult = document.Parse(reinterpret_cast<char*>(fileData));
	FREE_FILE(fileData);

	if ( parseResult.IsError() )
	{
		if ( moduleName )
		{
			ALERT(at_error, "%s: JSON document %s could not be parsed. Error at offset %u: %s\n",
				moduleName,
				path.String(),
				parseResult.Offset(),
				rapidjson::GetParseError_En(parseResult.Code()));
		}
		else
		{
			ALERT(at_error, "JSON document %s could not be parsed. Error at offset %u: %s\n",
				path.String(),
				parseResult.Offset(),
				rapidjson::GetParseError_En(parseResult.Code()));
		}

		return false;
	}

	return true;
}

#ifdef CLIENT_DLL
bool LoadFileFromClient(const CUtlString& path, Document& document, const char* moduleName)
{
	int length = 0;
	byte* fileData = gEngfuncs.COM_LoadFile(path.String(), 5, &length);

	if ( !fileData )
	{
		if ( moduleName )
		{
			gEngfuncs.Con_Printf("^1Error:^7 %s: Could not load file %s.\n", moduleName, path.String());
		}
		else
		{
			gEngfuncs.Con_Printf("^1Error:^7 Could not load file %s.\n", path.String());
		}
		
		return false;
	}

	rapidjson::ParseResult parseResult = document.Parse(reinterpret_cast<char*>(fileData));
	gEngfuncs.COM_FreeFile(fileData);

	if ( parseResult.IsError() )
	{
		if ( moduleName )
		{
			gEngfuncs.Con_Printf("^1Error:^7 %s: JSON document %s could not be parsed. Error at offset %u: %s\n",
				moduleName,
				path.String(),
				parseResult.Offset(),
				rapidjson::GetParseError_En(parseResult.Code()));
		}
		else
		{
			gEngfuncs.Con_Printf("^1Error:^7 JSON document %s could not be parsed. Error at offset %u: %s\n",
				path.String(),
				parseResult.Offset(),
				rapidjson::GetParseError_En(parseResult.Code()));
		}

		return false;
	}

	return true;
}
#endif

RAPIDJSON_NAMESPACE_END