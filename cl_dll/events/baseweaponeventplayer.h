#pragma once

#include "genericweaponattributes.h"
#include "utlstring.h"
#include "rapidjson/document.h"

typedef struct event_args_s event_args_t;

class BaseWeaponEventPlayer
{
public:
	virtual ~BaseWeaponEventPlayer() {}

	void LoadEventScript(const CUtlString& path);
	void PlayEvent(const event_args_t* eventArgs,
				   const CGenericWeaponAtts_FireMode::FireModeSignature* signature);

protected:
	virtual void EventStart() = 0;
	virtual bool Initialise();
	virtual void ParseEventScript(const rapidjson::Document& document);

	void AnimateViewModel();
	void EjectShellFromViewModel();
	void PlayFireSound();

	const event_args_t* m_pEventArgs = nullptr;
	const CGenericWeaponAtts_FireMode::FireModeSignature* m_pSignature = nullptr;
	const CGenericWeaponAtts_FireMode* m_pFireMode = nullptr;
	const CGenericWeaponAtts_HitscanFireMechanic* m_pMechanic = nullptr;

	int m_iEntIndex = -1;
	bool m_bWeaponIsEmpty = false;
	int m_iShellModelIndex = -1;

	vec3_t m_vecEntAngles;
	vec3_t m_vecEntOrigin;
	vec3_t m_vecEntVelocity;
	vec3_t m_vecFwd;
	vec3_t m_vecRight;
	vec3_t m_vecUp;
	vec3_t m_vecGunPosition;
};