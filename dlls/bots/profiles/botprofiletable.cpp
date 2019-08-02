#include "botprofiletable.h"

#include <algorithm>
#include <random>

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "enginecallback.h"

CBotProfileTable::CBotProfileTable()
{
}

bool CBotProfileTable::ProfileExists(const CUtlString& name) const
{
	return m_Table.find(name) != m_Table.cend();
}

CBotProfileTable::ProfileData& CBotProfileTable::CreateProfile(const CUtlString& name)
{
	auto it = m_Table.find(name);

	if ( it != m_Table.end() )
	{
		m_Table.erase(it);
	}

	return m_Table[name];
}

CBotProfileTable::ProfileData* CBotProfileTable::GetProfile(const CUtlString& name)
{
	auto it = m_Table.find(name);

	if ( it == m_Table.end() )
	{
		return NULL;
	}

	return &it->second;
}

const CBotProfileTable::ProfileData* CBotProfileTable::GetProfile(const CUtlString& name) const
{
	auto it = m_Table.find(name);

	if ( it == m_Table.cend() )
	{
		return NULL;
	}

	return &it->second;
}

void CBotProfileTable::RemoveProfile(const CUtlString& name)
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

void CBotProfileTable::RandomProfileNameList(std::vector<CUtlString>& list, size_t count) const
{
	list.clear();

	if ( m_Table.size() < 1 )
	{
		return;
	}

	list.reserve(count);
	auto rng = std::default_random_engine {(unsigned int)RANDOM_LONG(0, 1000)};

	while ( list.size() < count )
	{
		std::vector<const char*> intermediateList;
		intermediateList.reserve(m_Table.size());

		for ( auto it = m_Table.cbegin(); it != m_Table.cend(); ++it )
		{
			intermediateList.push_back(it->first.String());
		}

		std::shuffle(std::begin(intermediateList), std::end(intermediateList), rng);

		for ( const char* name : intermediateList )
		{
			list.push_back(CUtlString(name));

			if ( list.size() >= count )
			{
				break;
			}
		}
	}
}