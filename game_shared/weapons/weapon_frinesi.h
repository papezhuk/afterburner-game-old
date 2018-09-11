#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponFrinesi : public CGenericWeapon
{
public:
	CWeaponFrinesi();
	virtual void Precache() override;
	virtual void PrimaryAttack() override;
	virtual void SecondaryAttack() override;
	virtual void Holster(int skipLocal = 0) override;
	virtual void WeaponTick() override;
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;

protected:
	virtual int HandleSpecialReload(int currentState) override;

private:
	bool FlagReloadInterrupt();

	float m_flReloadStartDuration;
	float m_flReloadDuration;
	float m_flPumpDuration;
};
