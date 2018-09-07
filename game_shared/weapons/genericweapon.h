#pragma once

#include "standard_includes.h"
#include "weapons.h"
#include "genericweaponattributes.h"

// Build on top of CBasePlayerWeapon, because this is so tied into the engine
// already it'd be a pain to replace it (at least at this stage).
class CGenericWeapon : public CBasePlayerWeapon
{
public:
	CGenericWeapon();
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
	void FireUsingMode(int index);
	void SetViewModelBody(int body);

private:
	void PrecacheFireMode(uint8_t fireModeIndex);
	void PrecacheHitscanResources(const CGenericWeaponAtts_HitscanFireMode& fireMode);
	void PrecacheCore(const CGenericWeaponAtts_Core& core);
	void PrecacheSounds(const CGenericWeaponAttributes_Sound& sounds);

	void HitscanFire(int index, const CGenericWeaponAtts_HitscanFireMode& fireMode);
	void GetSharedCircularGaussianSpread(uint32_t shot, int shared_rand, float& x, float& y);

	Vector FireBulletsPlayer(const CGenericWeaponAtts_HitscanFireMode& fireMode,
							 uint32_t numShots,
							 const Vector& vecSrc,
							 const Vector& vecDirShooting);

#ifdef CLIENT_DLL
	Vector FireBulletsPlayer_Client(const CGenericWeaponAtts_HitscanFireMode& fireMode, uint32_t numShots);
#endif

	unsigned short m_FireEvents[2];
	int m_iViewModelBody;
};
