#pragma once

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "genericweaponattributes.h"

// Interface for generic weapon commands.
// Ideally we'd build a class that doesn't depend on CBasePlayerWeapon,
// but CBasePlayerWeapon is tied heavily to other engine functionality
// that it'd be non-trivial to extricate it. Perhaps we will one day?
// Until that time, Nightfire weapons should call functions through
// this interface, which CGenericWeapon will translate as an adapter
// to CBasePlayerWeapon. The reason for this is that CBasePlayerWeapon
// is quite loose with the way it manages weapon functionality -
// for example, ammo types are set when adding ammo to the weapon,
// which means a weapon that uses one type of ammo could just have it
// set to a new type on a whim. This interface is an attempt to solidify
// the functionality.
class IGenericWeapon
{
public:
	virtual ~IGenericWeapon() {}

	virtual const CGenericWeaponAttributes& WeaponAttributes() const = 0;
};

// Use on leaf classes (ie. the actual weapons that are linked to entities).
template<typename T>
class IGenericWeaponStatics : public virtual IGenericWeapon
{
public:
	virtual ~IGenericWeaponStatics() {}

	virtual const CGenericWeaponAttributes& WeaponAttributes() const override
	{
		return StaticWeaponAttributes();
	}

	const CGenericWeaponAttributes& StaticWeaponAttributes() const
	{
		// This must be implemented on the subclass.
		return T::m_StaticWeaponAttributes;
	}
};

// Build on top of CBasePlayerWeapon, because this is so tied into the engine
// already it'd be a pain to replace it (at least at this stage).
class CGenericWeapon : public CBasePlayerWeapon,
					   public virtual IGenericWeapon
{
public:
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

	inline IGenericWeapon* GenericWeapon() { return this; }
	inline const IGenericWeapon* GenericWeapon() const { return this; }

protected:
	void Fire(int index, const CGenericWeaponAtts_BaseFireMode* fireMode);
	void HitscanFire(int index, const CGenericWeaponAtts_HitscanFireMode* fireMode);

private:
	unsigned short m_FireEvents[2];
};
