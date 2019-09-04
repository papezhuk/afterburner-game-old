#include "genericmeleeweapon.h"
#include "util.h"
#include "weaponatts_meleeattack.h"
#include "gamerules.h"

namespace
{
	void FindHullIntersection( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
	{
		float* minmaxs[2] = {mins, maxs};
		TraceResult tmpTrace;
		Vector vecEnd;

		float distance = 1e6f;

		Vector vecHullEnd = vecSrc + ( (tr.vecEndPos - vecSrc) * 2 );
		UTIL_TraceLine(vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace);

		if( tmpTrace.flFraction < 1.0 )
		{
			tr = tmpTrace;
			return;
		}

		for( int i = 0; i < 2; i++ )
		{
			for( int j = 0; j < 2; j++ )
			{
				for( int k = 0; k < 2; k++ )
				{
					vecEnd.x = vecHullEnd.x + minmaxs[i][0];
					vecEnd.y = vecHullEnd.y + minmaxs[j][1];
					vecEnd.z = vecHullEnd.z + minmaxs[k][2];

					UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace);

					if( tmpTrace.flFraction < 1.0 )
					{
						float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();

						if( thisDistance < distance )
						{
							tr = tmpTrace;
							distance = thisDistance;
						}
					}
				}
			}
		}
	}
}

void CGenericMeleeWeapon::Precache()
{
	CGenericWeapon::Precache();
}

bool CGenericMeleeWeapon::InvokeWithAttackMode(WeaponAttackType type, const WeaponAtts::WABaseAttack* attackMode)
{
	if ( !attackMode || attackMode->Classify() != WeaponAtts::WABaseAttack::Classification::Melee )
	{
		return false;
	}

	if ( !CGenericWeapon::InvokeWithAttackMode(type, attackMode) )
	{
		return false;
	}

	const WeaponAtts::WAMeleeAttack* meleeAttack = static_cast<const WeaponAtts::WAMeleeAttack*>(attackMode);
	InitTraceVecs(meleeAttack);

	TraceResult tr;
	bool madeContact = CheckForContact(meleeAttack, tr);

	FireEvent(meleeAttack);
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	if ( madeContact )
	{
#ifndef CLIENT_DLL
		CBaseEntity* pEntity = CBaseEntity::Instance(tr.pHit);
		const bool hitBody = pEntity && pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE;

		if ( pEntity )
		{
			ClearMultiDamage();

			float damagePerShot = 1.0f;
			const WeaponAtts::WASkillRecord::SkillDataEntryPtr dmgPtr = meleeAttack->BaseDamagePerHit;

			if ( dmgPtr )
			{
				damagePerShot = gSkillData.*dmgPtr;
			}

			TraceResult attackTr;
			pEntity->TraceAttack(m_pPlayer->pev, damagePerShot, gpGlobals->v_forward, &attackTr, DMG_CLUB);

			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);
		}

		if ( hitBody )
		{
			PlaySound(meleeAttack->BodyHitSounds, CHAN_ITEM);
			m_pPlayer->m_iWeaponVolume = meleeAttack->Volume;
		}
		else
		{
			TraceResult texTraceResult;
			vec3_t traceEnd = m_vecAttackTraceStart + ((m_vecAttackTraceEnd - m_vecAttackTraceStart) * 2);

			float texSoundVolume = TEXTURETYPE_PlaySound(&texTraceResult, m_vecAttackTraceStart, traceEnd, BULLET_MELEE);

			if( g_pGameRules->IsMultiplayer() )
			{
				// override the volume here, cause we don't play texture sounds in multiplayer,
				// and fvolbar is going to be 0 from the above call.

				texSoundVolume = 1.0f;
			}

			PlaySound(meleeAttack->WorldHitSounds, CHAN_ITEM, texSoundVolume);
			m_pPlayer->m_iWeaponVolume = static_cast<int>(static_cast<float>(meleeAttack->Volume) * texSoundVolume);
		}
#endif
	}

	DelayFiring(1.0f / meleeAttack->AttackRate);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);

	return true;
}

bool CGenericMeleeWeapon::CheckForContact(const WeaponAtts::WAMeleeAttack* meleeAttack, TraceResult& tr)
{
	UTIL_TraceLine(m_vecAttackTraceStart, m_vecAttackTraceEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

#ifndef CLIENT_DLL
	if( tr.flFraction >= 1.0f )
	{
		// Line didn't hit - try more expensive hull check instead.
		UTIL_TraceHull( m_vecAttackTraceStart, m_vecAttackTraceEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);

		if( tr.flFraction < 1.0f )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);

			if( !pHit || pHit->IsBSPModel() )
			{
				FindHullIntersection(m_vecAttackTraceStart, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict());
			}

			m_vecAttackTraceEnd = tr.vecEndPos;	// This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	return tr.flFraction < 1.0f;
}

void CGenericMeleeWeapon::FireEvent(const WeaponAtts::WAMeleeAttack* meleeAttack)
{
	PLAYBACK_EVENT_FULL(DefaultEventFlags(),
						m_pPlayer->edict(),
						m_AttackModeEvents[meleeAttack->Signature()->Index],
						0.0,
						(float*)&g_vecZero,
						(float*)&g_vecZero,
						0,
						0,
						0,
						0,
						0,
						0);
}

void CGenericMeleeWeapon::InitTraceVecs(const WeaponAtts::WAMeleeAttack* meleeAttack)
{
	UTIL_MakeVectors(m_pPlayer->pev->v_angle);
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + (gpGlobals->v_forward * meleeAttack->Reach);
}
