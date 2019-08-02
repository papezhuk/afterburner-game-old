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
	return m_Table.HasElement(name);
}

CBotProfileTable::ProfileData& CBotProfileTable::CreateProfile(const CUtlString& name)
{
	m_Table.Remove(name);
	HashTable::IndexType_t index = m_Table.Insert(name);
	return m_Table.Element(index);
}

CBotProfileTable::ProfileData* CBotProfileTable::GetProfile(const CUtlString& name)
{
	HashTable::IndexType_t index = m_Table.Find(name);
	return index != m_Table.InvalidIndex() ? &m_Table.Element(index) : NULL;
}

const CBotProfileTable::ProfileData* CBotProfileTable::GetProfile(const CUtlString& name) const
{
	HashTable::IndexType_t index = m_Table.Find(name);
	return index != m_Table.InvalidIndex() ? &m_Table.Element(index) : NULL;
}

void CBotProfileTable::RemoveProfile(const CUtlString& name)
{
	m_Table.Remove(name);
}

void CBotProfileTable::Clear()
{
	m_Table.Purge();
}

size_t CBotProfileTable::Count() const
{
	return m_Table.Count();
}

void CBotProfileTable::RandomProfileNameList(std::vector<CUtlString>& list, size_t count) const
{
	list.clear();

	if ( m_Table.Count() < 1 )
	{
		return;
	}

	list.reserve(count);
	auto rng = std::default_random_engine {(unsigned int)RANDOM_LONG(0, 1000)};

	while ( list.size() < count )
	{
		std::vector<const char*> intermediateList;
		intermediateList.reserve(m_Table.Count());

		FOR_EACH_HASHMAP(m_Table, iterator)
		{
			intermediateList.push_back(m_Table.Key(iterator).String());
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