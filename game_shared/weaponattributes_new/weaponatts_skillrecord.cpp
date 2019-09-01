#include "weaponatts_skillrecord.h"
#include "util.h"
#include "eiface.h"

namespace WeaponAtts
{
	CWASkillRecord::CWASkillRecord(const char* cvarBaseName, SkillDataEntryPtr entry) :
		m_Entry(entry),
		m_BaseName(cvarBaseName)
	{
		for ( uint32_t index = 0; index < TOTAL_SKILL_LEVELS; ++index )
		{
			// These must not change after this point, or the cvar name string pointers will be wrong!
			m_NameBuffers[index] = m_BaseName;
			m_NameBuffers[index].AppendFormat("%u", index + 1);

			cvar_t* cvar = &m_Cvars[index];
			memset(cvar, 0, sizeof(cvar_t));
			cvar->name = m_NameBuffers[index].String();
			cvar->string = "0";
		}
	}

	CWASkillRecord::CWASkillRecord(const CWASkillRecord& other) :
		m_Entry(other.m_Entry),
		m_BaseName(other.m_BaseName)
	{
		for ( uint32_t index = 0; index < TOTAL_SKILL_LEVELS; ++index )
		{
			m_NameBuffers[index] = other.m_NameBuffers[index];
			m_Cvars[index] = other.m_Cvars[index];
			m_Cvars[index].name = m_NameBuffers[index].String();
		}
	}

	void CWASkillRecord::RegisterCvars() const
	{
		for ( uint32_t index = 0; index < TOTAL_SKILL_LEVELS; ++index )
		{
			CVAR_REGISTER(&m_Cvars[index]);
		}
	}

	void CWASkillRecord::UpdateSkillValue(skilldata_t* instance) const
	{
		ASSERTSZ_Q(instance, "skilldata_t instance is not valid.");
		ASSERTSZ_Q(instance->iSkillLevel > 0 && instance->iSkillLevel <= TOTAL_SKILL_LEVELS, "Skill level is not valid.");

		if ( !instance || instance->iSkillLevel < 1 || instance->iSkillLevel > TOTAL_SKILL_LEVELS )
		{
			return;
		}

		float value = m_Cvars[instance->iSkillLevel].value;

		if ( value <= 0.0f )
		{
			ALERT(at_warning, "UpdateSkillValue: Got invalid value of %f for %s at skill level %d.\n", value, m_BaseName.String(), instance->iSkillLevel);
			value = 0.0f;
		}

		instance->*m_Entry = value;
	}
}
