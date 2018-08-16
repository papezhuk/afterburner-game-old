#pragma once

#include "standard_includes.h"
#include "weapons.h"
#include "genericweaponattributes.h"

// Build on top of CBasePlayerWeapon, because this is so tied into the engine
// already it'd be a pain to replace it (at least at this stage).
class CGenericWeapon : public CBasePlayerWeapon
{
public:
	virtual ~CGenericWeapon();
	virtual void Spawn() override;
	virtual void Precache() override;
	virtual int GetItemInfo(ItemInfo *p) override;
	virtual int AddToPlayer(CBasePlayer *pPlayer) override;
	virtual BOOL Deploy() override;
	virtual void PrimaryAttack() override;
	virtual void SecondaryAttack() override;
	virtual void Reload() override;
	virtual void WeaponIdle() override;
	virtual int iItemSlot() override;

	virtual const CGenericWeaponAttributes& WeaponAttributes() const = 0;

protected:
	void Fire(int index, const CGenericWeaponAtts_BaseFireMode* fireMode);
	void HitscanFire(int index, const CGenericWeaponAtts_HitscanFireMode* fireMode);

private:
	unsigned short m_FireEvents[2];
};
