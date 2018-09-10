#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponFrinesi : public CGenericWeapon
{
public:
	CWeaponFrinesi();
	virtual void Precache() override;
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;

protected:
	virtual int HandleSpecialReload(int currentState) override;

private:
	float m_flReloadStartDuration;
	float m_flReloadDuration;
	float m_flPumpDuration;
};
