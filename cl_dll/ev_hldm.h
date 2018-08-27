//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#pragma once
#if !defined ( EV_HLDMH )
#define EV_HLDMH

extern "C"
{
	// HLDM
	void EV_FireGlock1( struct event_args_s *args  );
	void EV_FireGlock2( struct event_args_s *args  );
	void EV_FireGenericTest1( struct event_args_s *args  );
	void EV_FireGenericTest2( struct event_args_s *args  );
	void EV_FireShotGunSingle( struct event_args_s *args  );
	void EV_FireShotGunDouble( struct event_args_s *args  );
	void EV_FireMP5( struct event_args_s *args  );
	void EV_FireMP52( struct event_args_s *args  );
	void EV_FirePython( struct event_args_s *args  );
	void EV_FireGauss( struct event_args_s *args  );
	void EV_SpinGauss( struct event_args_s *args  );
	void EV_Crowbar( struct event_args_s *args );
	void EV_FireCrossbow( struct event_args_s *args );
	void EV_FireCrossbow2( struct event_args_s *args );
	void EV_FireRpg( struct event_args_s *args );
	void EV_EgonFire( struct event_args_s *args );
	void EV_EgonStop( struct event_args_s *args );
	void EV_HornetGunFire( struct event_args_s *args );
	void EV_TripmineFire( struct event_args_s *args );
	void EV_SnarkFire( struct event_args_s *args );

	void EV_FireP991( struct event_args_s *args  );
	void EV_FireP992( struct event_args_s *args  );

	void EV_TrainPitchAdjust( struct event_args_s *args );

	void EV_WeaponGenericTest_Fire1(struct event_args_s* args);
	void EV_WeaponGenericTest_Fire2(struct event_args_s* args);
}

typedef struct pmtrace_s pmtrace_t;

void EV_HLDM_GunshotDecalTrace( pmtrace_t *pTrace, char *decalName );
void EV_HLDM_DecalGunshot( pmtrace_t *pTrace, int iBulletType );
void EV_HLDM_CheckTracer( int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType);
void EV_HLDM_FireBullets( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int iTracerFreq, float flSpreadX, float flSpreadY );
#endif // EV_HLDMH
