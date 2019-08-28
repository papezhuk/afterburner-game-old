#pragma once

#include "cvardef.h"
#include "skill.h"
#include "utlvector.h"
#include "utlstring.h"

class CGenericWeaponAttributes_SkillRecord
{
public:
	typedef float skilldata_t::* SkillDataEntryPtr;

	CGenericWeaponAttributes_SkillRecord(const char* cvarBaseName, SkillDataEntryPtr entry);
	CGenericWeaponAttributes_SkillRecord(const CGenericWeaponAttributes_SkillRecord& other);

	// After registring cvars, the instance of the class must persist until shutdown.
	// This class is designed to be used statically, so it should be OK.
	void RegisterCvars() const;
	void UpdateSkillValue(skilldata_t* instance) const;

private:
	SkillDataEntryPtr m_Entry;
	CUtlString m_BaseName;
	CUtlString m_NameBuffers[TOTAL_SKILL_LEVELS];
	mutable cvar_t m_Cvars[TOTAL_SKILL_LEVELS];
};

class CGenericWeaponAttributes_Skill
{
public:
	CGenericWeaponAttributes_Skill();
	CGenericWeaponAttributes_Skill(const CGenericWeaponAttributes_Skill& other);
	CGenericWeaponAttributes_Skill& operator =(const CGenericWeaponAttributes_Skill& other);

	inline CGenericWeaponAttributes_Skill& Record(const CGenericWeaponAttributes_SkillRecord& record)
	{
		m_Records.AddToTail(record);
		return *this;
	}

	inline CGenericWeaponAttributes_Skill& Record(const char* cvarBaseName, CGenericWeaponAttributes_SkillRecord::SkillDataEntryPtr entry)
	{
		m_Records.AddToTail(CGenericWeaponAttributes_SkillRecord(cvarBaseName, entry));
		return *this;
	}

	inline void RegisterCvars() const
	{
		FOR_EACH_VEC(m_Records, index)
		{
			m_Records[index].RegisterCvars();
		}
	}

	inline void UpdateSkillValues(skilldata_t* instance) const
	{
		FOR_EACH_VEC(m_Records, index)
		{
			m_Records[index].UpdateSkillValue(instance);
		}
	}

private:
	CUtlVector<CGenericWeaponAttributes_SkillRecord> m_Records;
};