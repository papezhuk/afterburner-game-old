#pragma once

#include <memory>

#include "standard_includes.h"
#include "weapons.h"
#include "weaponids.h"
#include "ammodefs.h"
#include "skill.h"
#include "utlstring.h"
#include "utlvector.h"
#include "weightedvaluelist.h"
#include "genericweaponattributedefs.h"
#include "weaponatts_skill.h"
#include "weaponatts_core.h"
#include "weaponatts_cvars.h"
#include "weaponatts_animations.h"
#include "weaponatts_firemode.h"

#ifndef CLIENT_DLL
#include "botweaponattributes.h"
#endif

class CGenericWeaponAttributes
{
public:
	CGenericWeaponAttributes(const CGenericWeaponAtts_Core& core)
		: m_Core(core),
		  m_Skill(),
		  m_CustomCVars(),
		  m_NewFireModes{},
		  m_Animations(),
		  m_IdleAnimations()
	{
	}

	CGenericWeaponAttributes(const CGenericWeaponAttributes& other)
		: m_Core(other.m_Core),
		  m_Skill(other.m_Skill),
		  m_CustomCVars(other.m_CustomCVars),
		  m_NewFireModes{},
		  m_Animations(),
		  m_IdleAnimations()
	{
		m_Animations = other.m_Animations;
		m_IdleAnimations = other.m_IdleAnimations;

		for ( int mode = 0; mode < 2; ++mode )
		{
			m_NewFireModes[mode] = other.m_NewFireModes[mode];
		}

		Register();
	}

	inline CGenericWeaponAttributes& operator =(const CGenericWeaponAttributes& other)
	{
		m_Core = other.m_Core;
		m_Animations = other.m_Animations;
		m_IdleAnimations = other.m_IdleAnimations;

		for ( int mode = 0; mode < 2; ++mode )
		{
			m_NewFireModes[mode] = other.m_NewFireModes[mode];
		}

		Register();
		return *this;
	}

	// Implemented in .cpp to remove cyclic dependencies.
	void Register();

	inline const CGenericWeaponAtts_Core& Core() const
	{
		return m_Core;
	}

	inline const CGenericWeaponAttributes_Skill& Skill() const
	{
		return m_Skill;
	}

	inline const CGenericWeaponAttributes_CustomCVars& CustomCVars() const
	{
		return m_CustomCVars;
	}

	inline const CGenericWeaponAtts_Animations& Animations() const
	{
		return m_Animations;
	}

	inline const CGenericWeaponAtts_IdleAnimations& IdleAnimations() const
	{
		return m_IdleAnimations;
	}

	inline CGenericWeaponAttributes& Skill(const CGenericWeaponAttributes_Skill& skill)
	{
		m_Skill = skill;
		return *this;
	}

	inline CGenericWeaponAttributes& CustomCVars(const CGenericWeaponAttributes_CustomCVars& vars)
	{
		m_CustomCVars = vars;
		return *this;
	}

	inline CGenericWeaponAttributes& Animations(const CGenericWeaponAtts_Animations& anims)
	{
		m_Animations = anims;
		return *this;
	}

	inline CGenericWeaponAttributes& IdleAnimations(const CGenericWeaponAtts_IdleAnimations& anims)
	{
		m_IdleAnimations = anims;
		return *this;
	}

	inline const CGenericWeaponAtts_FireMode& FireMode(uint8_t mode) const
	{
		static const CGenericWeaponAtts_FireMode dummy;

		if ( mode < 0 || mode >= WEAPON_MAX_FIRE_MODES )
		{
			ASSERTSZ_Q(false, "Invalid fire mode index");
			return dummy;
		}

		return m_NewFireModes[mode];
	}

	inline CGenericWeaponAttributes& FireMode(uint8_t mode, const CGenericWeaponAtts_FireMode fireMode)
	{
		if ( mode < 0 || mode > 1 )
		{
			ASSERTSZ_Q(false, "Invalid fire mode index");
			return *this;
		}

		m_NewFireModes[mode] = fireMode;
		return *this;
	}

#ifndef CLIENT_DLL
	inline const CBotWeaponAttributes& BotWeaponAttributes() const
	{
		return m_BotWeaponAttributes;
	}

	inline CGenericWeaponAttributes& BotWeaponAttributes(const CBotWeaponAttributes& val)
	{
		m_BotWeaponAttributes = val;
		return *this;
	}
#endif

private:
	inline void SetFireModeSignatures()
	{
		for ( int index = 0; index < 2; ++index )
		{
			m_NewFireModes[index].SetSignature(m_Core.Id(), index);
		}
	}

	CGenericWeaponAtts_Core m_Core;
	CGenericWeaponAttributes_Skill m_Skill;
	CGenericWeaponAttributes_CustomCVars m_CustomCVars;
	CGenericWeaponAtts_Animations m_Animations;
	CGenericWeaponAtts_IdleAnimations m_IdleAnimations;
	CGenericWeaponAtts_FireMode m_NewFireModes[2];

#ifndef CLIENT_DLL
	CBotWeaponAttributes m_BotWeaponAttributes;
#endif
};
