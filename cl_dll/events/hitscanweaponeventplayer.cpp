#include "hitscanweaponeventplayer.h"
#include "weaponregistry.h"
#include "event_args.h"
#include "cdll_int.h"
#include "cl_dll.h"
#include "event_api.h"
#include "vector_classes.h"
#include "eventscripts.h"
#include "cl_entity.h"
#include "view.h"
#include "in_defs.h"
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "genericweapon.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "ev_hldm.h"

namespace
{
	// TODO: These probably need to be different per weapon?
	static constexpr float SHELLEJECT_FWD_SCALE = 20;
	static constexpr float SHELLEJECT_RIGHT_SCALE = -12;
	static constexpr float SHELLEJECT_UP_SCALE = 4;
}

void HitscanWeaponEventPlayer::PlayEvent(const event_args_t* eventArgs,
										 const CGenericWeaponAtts_FireMode::FireModeSignature* signature)
{
	m_pEventArgs = eventArgs;
	m_pSignature = signature;

	if ( !Initialise() )
	{
		ASSERT(false);
		return;
	}

	if( EV_IsLocal(m_iEntIndex) )
	{
		EV_MuzzleFlash();
		AnimateViewModel();
	}

	EjectShellFromViewModel();
	PlayFireSound();
}

void HitscanWeaponEventPlayer::CreateBulletTracers()
{
	const uint32_t numShots = m_pMechanic->BulletsPerShot();

	for ( uint32_t shot = 0; shot < numShots; ++shot )
	{
		vec3_t shotDir;

		if ( numShots == 1 )
		{
			for ( uint8_t axis = 0; axis < 3; ++axis )
			{
				shotDir[axis] = m_vecFwd[axis] + (m_flSpreadX * m_vecRight[axis]) + (m_flSpreadY * m_vecUp [axis]);
			}
		}
		else
		{
			// We are firing multiple shots, so we need to generate the spread for each one.
			float spreadX = 0.0f;
			float spreadY = 0.0f;

			CGenericWeapon::GetSharedCircularGaussianSpread(m_iEntIndex, m_iRandomSeed, spreadX, spreadY);

			for ( uint8_t axis = 0; axis < 3; ++axis )
			{
				shotDir[axis] = m_vecFwd[axis] +
					(spreadX * m_pFireMode->SpreadX() * m_vecRight[axis]) +
					(spreadY * m_pFireMode->SpreadY() * m_vecUp[axis]);
			}
		}

		vec3_t traceEnd = m_vecGunPosition + (CGenericWeapon::DEFAULT_BULLET_TRACE_DISTANCE * shotDir);

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);

		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();

		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(m_iEntIndex - 1);

		gEngfuncs.pEventAPI->EV_SetTraceHull(2);	// TODO: What's 2?

		pmtrace_t traceResult;
		gEngfuncs.pEventAPI->EV_PlayerTrace(m_vecGunPosition, traceEnd, PM_STUDIO_BOX, -1, &traceResult);

		EV_HLDM_CheckTracer(m_iEntIndex, m_vecGunPosition, traceResult.endpos, m_vecFwd, m_vecRight, BULLET_GENERIC);

		// do damage, paint decals
		if ( traceResult.fraction != 1.0 )
		{
			EV_HLDM_PlayTextureSound(m_iEntIndex, &traceResult, m_vecGunPosition, traceEnd, BULLET_GENERIC);
			EV_HLDM_DecalGunshot(&traceResult, BULLET_GENERIC);
		}

		gEngfuncs.pEventAPI->EV_PopPMStates();
	}
}

void HitscanWeaponEventPlayer::AnimateViewModel()
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

void HitscanWeaponEventPlayer::EjectShellFromViewModel()
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

void HitscanWeaponEventPlayer::PlayFireSound()
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

bool HitscanWeaponEventPlayer::Initialise()
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
	m_flSpreadX = m_pEventArgs->fparam1;
	m_flSpreadY = m_pEventArgs->fparam2;
	m_iRandomSeed = m_pEventArgs->iparam1;

	// We make local copies of the event vectors so that they can be operated on as objects instead of arrays.
	VectorCopy(m_pEventArgs->angles, m_vecEntAngles);
	VectorCopy(m_pEventArgs->origin, m_vecEntOrigin);
	VectorCopy(m_pEventArgs->velocity, m_vecEntVelocity);

	AngleVectors(m_vecEntAngles, m_vecFwd, m_vecRight, m_vecUp);
	EV_GetGunPosition(m_pEventArgs, m_vecGunPosition, m_vecEntOrigin);

	return true;
}