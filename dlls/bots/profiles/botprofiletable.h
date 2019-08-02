#ifndef BOTPROFILETABLE_H
#define BOTPROFILETABLE_H

#include <vector>
#include "utlstring.h"
#include "utlhashmap.h"

class CBotProfileTable
{
public:
	struct ProfileData
	{
		CUtlString skin;
		CUtlString playerName;
	};

	CBotProfileTable();

	void Clear();
	size_t Count() const;

	bool ProfileExists(const CUtlString& name) const;
	ProfileData* GetProfile(const CUtlString& name);
	const ProfileData* GetProfile(const CUtlString& name) const;

	ProfileData& CreateProfile(const CUtlString& name);
	void RemoveProfile(const CUtlString& name);

	void RandomProfileNameList(std::vector<CUtlString>& list, size_t count) const;

private:
	typedef CUtlHashMap<CUtlString, ProfileData> HashTable;
	HashTable m_Table;
};

#endif // BOTPROFILETABLE_H