#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponP99 : public CGenericWeapon
{
public:
	CWeaponP99();
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
	virtual void PrimaryAttack() override;
	virtual void SecondaryAttack() override;

	virtual bool ReadPredictionData(const weapon_data_t* from) override;
	virtual bool WritePredictionData(weapon_data_t* to) override;

private:
	bool m_bSilenced;
};
