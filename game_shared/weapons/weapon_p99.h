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

#ifndef CLIENT_DLL
	virtual int Save(CSave &save) override;
	virtual int Restore(CRestore &restore) override;
	static TYPEDESCRIPTION m_SaveData[];
#endif

private:
	bool m_bSilenced;
};
