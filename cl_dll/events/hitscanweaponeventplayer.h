#pragma once

#include "ieventplayer.h"

class HitscanWeaponEventPlayer : public IEventPlayer
{
public:
	virtual void PlayEvent(const event_args_t* eventArgs,
						   const CGenericWeaponAtts_FireMode::FireModeSignature* signature) override;

private:
	bool Initialise();
	void AnimateViewModel();
	void EjectShellFromViewModel();
	void PlayFireSound();
	void CreateBulletTracers();

	const event_args_t* m_pEventArgs = nullptr;
	const CGenericWeaponAtts_FireMode::FireModeSignature* m_pSignature = nullptr;
	const CGenericWeaponAtts_FireMode* m_pFireMode = nullptr;
	const CGenericWeaponAtts_HitscanFireMechanic* m_pMechanic = nullptr;

	int m_iEntIndex = -1;
	bool m_bWeaponIsEmpty = false;
	int m_iShellModelIndex = -1;
	float m_flSpreadX = 0.0f;
	float m_flSpreadY = 0.0f;
	int m_iRandomSeed = 0;

	vec3_t m_vecEntAngles;
	vec3_t m_vecEntOrigin;
	vec3_t m_vecEntVelocity;
	vec3_t m_vecFwd;
	vec3_t m_vecRight;
	vec3_t m_vecUp;
	vec3_t m_vecGunPosition;
};