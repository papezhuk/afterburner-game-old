#ifndef BOTPROFILETABLE_H
#define BOTPROFILETABLE_H

#include <unordered_map>
#include <string>

class CBotProfileTable
{
public:
	struct ProfileData
	{
		std::string skin;
		std::string playerName;
	};

	CBotProfileTable();

	void Clear();
	size_t Count() const;

	bool ProfileExists(const std::string& name) const;
	ProfileData* GetProfile(const std::string& name);
	const ProfileData* GetProfile(const std::string& name) const;

	ProfileData& CreateProfile(const std::string& name);
	void RemoveProfile(const std::string& name);

private:
	std::unordered_map<std::string, ProfileData> m_Table;
};

#endif // BOTPROFILETABLE_H