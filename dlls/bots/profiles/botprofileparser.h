#ifndef BOTPROFILEPARSER_H
#define BOTPROFILEPARSER_H

#include "utlstring.h"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "botprofiletable.h"

class CBotProfileParser
{
public:
	CBotProfileParser(CBotProfileTable& table);

	bool Parse(const CUtlString& filePath);

private:
	bool ReadParsedDocument(const rapidjson::Document& document);
	void ReadProfileEntry(rapidjson::SizeType index, const rapidjson::Value& object);

	CBotProfileTable& m_Table;
};

#endif // BOTPROFILEPARSER_H