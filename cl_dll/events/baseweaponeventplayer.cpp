#include "baseweaponeventplayer.h"
#include "hitscanweaponeventplayer.h"
#include "weaponregistry.h"
#include "event_args.h"
#include "cdll_int.h"
#include "cl_dll.h"
#include "event_api.h"
#include "vector_classes.h"
#include "eventscripts.h"
#include "hud.h"
#include "cl_util.h"
#include "view.h"
#include "in_defs.h"
#include "rapidjson/document.h"
#include "rapidjson_helpers/rapidjson_helpers.h"
#include "weaponatts_hitscanfiremechanic.h"

namespace
{
	// TODO: These probably need to be different per weapon?
	static constexpr float SHELLEJECT_FWD_SCALE = 20;
	static constexpr float SHELLEJECT_RIGHT_SCALE = -12;
	static constexpr float SHELLEJECT_UP_SCALE = 4;
}

void BaseWeaponEventPlayer::LoadEventScript(const CUtlString& path)
{
	rapidjson::Document document;

	if ( !rapidjson::LoadFileFromClient(path, document, "BaseWeaponEventPlayer") )
	{
		return;
	}

	if ( !document.IsObject() )
	{
		ALERT(at_error, "BaseWeaponEventPlayer: Weapon script %s root is not an object.\n",
			  path.String());

		return;
	}

	ParseEventScript(document);
}

void BaseWeaponEventPlayer::PlayEvent(const event_args_t* eventArgs,
				   					  const CGenericWeaponAtts_FireMode::FireModeSignature* signature)
{
	m_pEventArgs = eventArgs;
	m_pSignature = signature;

	if ( !Initialise() )
	{
		ASSERT(false);
		return;
	}

	EventStart();
}

void BaseWeaponEventPlayer::ParseEventScript(const rapidjson::Document& document)
{
}

bool BaseWeaponEventPlayer::Initialise()
{
	// Cache all the variables we need to use when generating trace effects.

	if ( !m_pSignature || !m_pEventArgs )
	{
		return false;
	}

	const uint8_t fireModeIndex = static_cast<const uint8_t>(m_pSignature->m_iFireMode);
	if ( fireModeIndex >= 2 )
	{
		return false;
	}

	const WeaponId_e weaponId = static_cast<const WeaponId_e>(m_pSignature->m_iWeaponId);
	const CGenericWeaponAttributes* atts = CWeaponRegistry::StaticInstance().Get(weaponId);

	if ( !atts )
	{
		return false;
	}

	m_pFireMode = &atts->FireMode(fireModeIndex);
	const CGenericWeaponAtts_BaseFireMechanic* mechanic = m_pFireMode->Mechanic();

	if ( !mechanic || mechanic->Id() != CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Hitscan )
	{
		return false;
	}

	m_pMechanic = m_pFireMode->Mechanic()->AsType<const CGenericWeaponAtts_HitscanFireMechanic>();
	m_iEntIndex = m_pEventArgs->entindex;
	m_bWeaponIsEmpty = m_pEventArgs->bparam1;
	m_iShellModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex(m_pMechanic->ShellModelName());

	// We make local copies of the event vectors so that they can be operated on as objects instead of arrays.
	VectorCopy(m_pEventArgs->angles, m_vecEntAngles);
	VectorCopy(m_pEventArgs->origin, m_vecEntOrigin);
	VectorCopy(m_pEventArgs->velocity, m_vecEntVelocity);

	AngleVectors(m_vecEntAngles, m_vecFwd, m_vecRight, m_vecUp);
	EV_GetGunPosition(m_pEventArgs, m_vecGunPosition, m_vecEntOrigin);

	return true;
}

void BaseWeaponEventPlayer::AnimateViewModel()
{
	int animIndex = m_bWeaponIsEmpty ? m_pFireMode->AnimIndex_FireEmpty() : m_pFireMode->AnimIndex_FireNotEmpty();

	if ( m_bWeaponIsEmpty && animIndex < 0 )
	{
		// If the weapon doesn't have an animation for firing on empty,
		// fall back to the normal animation.
		animIndex = m_pFireMode->AnimIndex_FireNotEmpty();
	}

	int body = m_pFireMode->ViewModelBodyOverride();

	if ( body < 0 )
	{
		// No model override, so use weapon's current body.
		const struct cl_entity_s* const viewModelEnt = GetViewEntity();

		if ( viewModelEnt )
		{
			body = viewModelEnt->curstate.body;
		}
		else
		{
			body = 0;
		}
	}

	gEngfuncs.pEventAPI->EV_WeaponAnimation(animIndex, body);
	V_PunchAxis(0, m_pFireMode->ViewPunchY());
}

void BaseWeaponEventPlayer::EjectShellFromViewModel()
{
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;

	// TODO: This is an awful function call. Refactor?
	EV_GetDefaultShellInfo(m_pEventArgs,
						   m_vecEntOrigin,
						   m_vecEntVelocity,
						   ShellVelocity,
						   ShellOrigin,
						   m_vecFwd,
						   m_vecRight,
						   m_vecUp,
						   SHELLEJECT_FWD_SCALE,
						   SHELLEJECT_RIGHT_SCALE,
						   SHELLEJECT_UP_SCALE);

	EV_EjectBrass(ShellOrigin, ShellVelocity, m_vecEntAngles[YAW], m_iShellModelIndex, TE_BOUNCE_SHELL);
}

void BaseWeaponEventPlayer::PlayFireSound()
{
	if ( m_pFireMode->HasSounds() )
	{
		const CGenericWeaponAttributes_Sound& fireSound = m_pFireMode->Sounds();

		const float volume = (fireSound.MinVolume() < fireSound.MaxVolume())
			? gEngfuncs.pfnRandomFloat(fireSound.MinVolume(), fireSound.MaxVolume())
			: fireSound.MaxVolume();

		const int pitch = (fireSound.MinPitch() < fireSound.MaxPitch())
			? gEngfuncs.pfnRandomLong(fireSound.MinPitch(), fireSound.MaxPitch())
			: fireSound.MaxPitch();

		const char* const soundName = fireSound.SoundList().ItemByProbabilisticValue(gEngfuncs.pfnRandomFloat(0.0f, 1.0f));

		gEngfuncs.pEventAPI->EV_PlaySound(m_iEntIndex,
										  m_vecEntOrigin,
										  CHAN_WEAPON,
										  soundName,
										  volume,
										  ATTN_NORM,
										  0,
										  pitch);
	}
}