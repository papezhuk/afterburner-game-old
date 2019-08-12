#pragma once

#include "standard_includes.h"
#include "weapons.h"
#include "genericweaponattributes.h"
#include "utlvector.h"

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
	virtual void ItemPostFrame() override;

	virtual const CGenericWeaponAttributes& WeaponAttributes() const = 0;

	static constexpr float DEFAULT_BULLET_TRACE_DISTANCE = 8192;
	static void GetSharedCircularGaussianSpread(uint32_t shot, int shared_rand, float& x, float& y);

protected:
	void SetViewModelBody(int body, bool immediate = false);
	float ViewModelAnimationDuration(int anim) const;
	void PlaySound(const CGenericWeaponAttributes_Sound& sound, int channel = CHAN_WEAPON);

	// Returns true if firing succeeded.
	bool FireUsingMode(int index);
	virtual bool SwitchFire(int index,
							const CGenericWeaponAtts_FireMode& fireMode,
							const CGenericWeaponAtts_BaseFireMechanic& mechanic) = 0;

	virtual void SwitchPrecache(const CGenericWeaponAtts_BaseFireMechanic& mechanic) = 0;
	void PrecacheSounds(const CGenericWeaponAttributes_Sound& sounds);

	void DelayPendingActions(float secs, bool allowIfEarlier = false);
	void DelayFiring(float secs, bool allowIfEarlier = false, int mode = -1);
	bool HasAmmo(const CGenericWeaponAtts_FireMode& fireMode, int minCount = 1, bool useClip = true) const;
	bool DecrementAmmo(const CGenericWeaponAtts_FireMode& fireMode, int decrement);

	// Return the value to set m_fInSpecialReload to next.
	virtual int HandleSpecialReload(int currentState);

	// For convenience:
	inline void SetNextPrimaryAttack(float secsInFuture, bool allowIfEarlier = false)
	{
		const float delay = UTIL_WeaponTimeBase() + secsInFuture;
		if ( allowIfEarlier || delay > m_flNextPrimaryAttack )
		{
			m_flNextPrimaryAttack = GetNextAttackDelay(secsInFuture);
		}
	}

	inline void SetNextSecondaryAttack(float secsInFuture, bool allowIfEarlier = false)
	{
		const float delay = UTIL_WeaponTimeBase() + secsInFuture;
		if ( allowIfEarlier || delay > m_flNextSecondaryAttack )
		{
			m_flNextSecondaryAttack = delay;
		}
	}

	inline void SetNextIdleTime(float secsInFuture, bool allowIfEarlier = false)
	{
		const float delay = UTIL_WeaponTimeBase() + secsInFuture;
		if ( allowIfEarlier || delay > m_flTimeWeaponIdle )
		{
			m_flTimeWeaponIdle = delay;
		}
	}

	inline int DefaultEventFlags()
	{
#ifdef CLIENT_WEAPONS
		return FEV_NOTHOST;
#else
		return 0;
#endif
	}

	unsigned short m_FireEvents[2];

private:
	void PrecacheFireMode(uint8_t fireModeIndex);
	void PrecacheHitscanResources(const CGenericWeaponAtts_HitscanFireMechanic& fireMode);
	void PrecacheCore(const CGenericWeaponAtts_Core& core);

	void SetFireOnEmptyState(uint8_t mode);

	// Return true if reload action occurred, or false otherwise.
	bool IdleProcess_CheckReload();
	bool IdleProcess_CheckSpecialReload();
	void IdleProcess_PlayIdleAnimation();

	void FindWeaponSlotInfo();

	int m_iViewModelIndex;
	int m_iViewModelBody;
	CUtlVector<float> m_ViewAnimDurations;
	int m_iWeaponSlot;
	int m_iWeaponSlotPosition;
};

class CGenericAmmo : public CBasePlayerAmmo
{
public:
	CGenericAmmo(const char* modelName, const CAmmoDef& ammoDef, int giveAmount, const char* pickupSoundName = NULL)
		: CBasePlayerAmmo(),
		  m_szModelName(modelName),
		  m_AmmoDef(ammoDef),
		  m_iGiveAmount(giveAmount),
		  m_szPickupSoundName(pickupSoundName)
	{
		ASSERT(m_szModelName);
		ASSERT(m_iGiveAmount > 0);
	}

	void Spawn()
	{
		Precache();
		SET_MODEL(ENT(pev), m_szModelName);
		CBasePlayerAmmo::Spawn();
	}

	void Precache()
	{
		PRECACHE_MODEL(m_szModelName);
		PRECACHE_SOUND(m_szPickupSoundName ? m_szPickupSoundName : DEFAULT_PICKUP_SOUND);
	}

	BOOL AddAmmo(CBaseEntity *pOther)
	{
		if( pOther->GiveAmmo(m_iGiveAmount, m_AmmoDef.Name, m_AmmoDef.MaxCarry) != -1 )
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, DEFAULT_PICKUP_SOUND, 1, ATTN_NORM);
			return TRUE;
		}

		return FALSE;
	}

	inline const char* AmmoName() const
	{
		return m_AmmoDef.Name;
	}

private:
	static constexpr const char* DEFAULT_PICKUP_SOUND = "items/ammopickup1.wav";

	const char* m_szModelName;
	const CAmmoDef& m_AmmoDef;
	int m_iGiveAmount;
	const char* m_szPickupSoundName;
};
