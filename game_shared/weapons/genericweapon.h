#pragma once

#include "standard_includes.h"
#include "weapons.h"
#include "genericweaponattributes.h"
#include <vector>

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
	void DelayPendingActions(float secs, bool allowIfEarlier = false);
	void DelayFiring(float secs, bool allowIfEarlier = false);

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

private:
	void PrecacheFireMode(uint8_t fireModeIndex);
	void PrecacheHitscanResources(const CGenericWeaponAtts_HitscanFireMode& fireMode);
	void PrecacheCore(const CGenericWeaponAtts_Core& core);
	void PrecacheSounds(const CGenericWeaponAttributes_Sound& sounds);

	void SetFireOnEmptyState(uint8_t mode);
	bool HitscanFire(int index, const CGenericWeaponAtts_HitscanFireMode& fireMode);
	Vector FireBulletsPlayer(const CGenericWeaponAtts_HitscanFireMode& fireMode,
							 const Vector& vecSrc,
							 const Vector& vecDirShooting);

#ifdef CLIENT_DLL
	Vector FireBulletsPlayer_Client(const CGenericWeaponAtts_HitscanFireMode& fireMode);
#endif

	// Return true if reload action occurred, or false otherwise.
	bool IdleProcess_CheckReload();
	bool IdleProcess_CheckSpecialReload();
	void IdleProcess_PlayIdleAnimation();

	void FindWeaponSlotInfo();

	unsigned short m_FireEvents[2];
	int m_iViewModelIndex;
	int m_iViewModelBody;
	std::vector<float> m_ViewAnimDurations;
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

private:
	static constexpr const char* DEFAULT_PICKUP_SOUND = "items/ammopickup1.wav";

	const char* m_szModelName;
	const CAmmoDef& m_AmmoDef;
	int m_iGiveAmount;
	const char* m_szPickupSoundName;
};
