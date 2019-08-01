#include "botprofiletable.h"

CBotProfileTable::CBotProfileTable()
{
}

bool CBotProfileTable::ProfileExists(const std::string& name) const
{
	return m_Table.find(name) != m_Table.cend();
}

CBotProfileTable::ProfileData& CBotProfileTable::CreateProfile(const std::string& name)
{
	auto it = m_Table.find(name);

	if ( it != m_Table.end() )
	{
		m_Table.erase(it);
	}

	return m_Table[name];
}

CBotProfileTable::ProfileData* CBotProfileTable::GetProfile(const std::string& name)
{
	auto it = m_Table.find(name);

	if ( it == m_Table.end() )
	{
		return NULL;
	}

	return &it->second;
}

const CBotProfileTable::ProfileData* CBotProfileTable::GetProfile(const std::string& name) const
{
	auto it = m_Table.find(name);

	if ( it == m_Table.cend() )
	{
		return NULL;
	}

	return &it->second;
}

void CBotProfileTable::RemoveProfile(const std::string& name)
{
	m_Table.erase(name);
}

void CBotProfileTable::Clear()
{
	m_Table.clear();
}

size_t CBotProfileTable::Count() const
{
	return m_Table.size();
}