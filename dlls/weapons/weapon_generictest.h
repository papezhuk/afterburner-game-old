#pragma once

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "genericweapon.h"

class CWeaponGenericTest : public CGenericWeapon,
							public IGenericWeaponStatics<CWeaponGenericTest>
{
public:
	virtual void Precache() override;
private:
	friend class IGenericWeaponStatics<CWeaponGenericTest>;

	static const CGenericWeaponAttributes m_StaticWeaponAttributes;
};
