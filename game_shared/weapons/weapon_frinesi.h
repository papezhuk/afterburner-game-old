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

	virtual bool ReadPredictionData(const weapon_data_t* from) override;
	virtual bool WritePredictionData(weapon_data_t* to) override;

protected:
	virtual int HandleSpecialReload(int currentState) override;

private:
	void PlayPumpSound();
	bool FlagReloadInterrupt();

	float m_flReloadStartDuration;
	float m_flReloadDuration;
	float m_flPumpDuration;
	float m_flNextPumpTime;
};
