#pragma once

#include "genericweaponattributedefs.h"
#include "weaponatts_basefiremechanic.h"
#include "weaponatts_skill.h"

class CGenericWeaponAtts_HitscanFireMechanic : public CGenericWeaponAtts_BaseFireMechanic
{
public:
	typedef CGenericWeaponAttributes_SkillRecord::SkillDataEntryPtr SkillBasedDamagePtr;

	virtual ~CGenericWeaponAtts_HitscanFireMechanic() {}

	virtual FireMechanic_e Id() const override { return CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Hitscan; }
	virtual CGenericWeaponAtts_BaseFireMechanic* Clone() const override { return new CGenericWeaponAtts_HitscanFireMechanic(*this); }

#define ATTR(type, name, defaultVal) DECLARE_ATTRIBUTE(CGenericWeaponAtts_HitscanFireMechanic, type, name, defaultVal)
	ATTR(uint8_t, BulletsPerShot, 1);
	ATTR(SkillBasedDamagePtr, BaseDamagePerShot, NULL);
	ATTR(float, AutoAim, 0.0f);
	ATTR(const char*, ShellModelName, NULL);
#undef ATTR
};