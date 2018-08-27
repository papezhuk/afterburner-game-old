//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#pragma once
#if !defined ( EV_HLDMH )
#define EV_HLDMH

// bullet types
typedef	enum
{
	BULLET_NONE = 0,
	BULLET_PLAYER_9MM, // glock
	BULLET_PLAYER_MP5, // mp5
	BULLET_PLAYER_357, // python
	BULLET_PLAYER_BUCKSHOT, // shotgun
	BULLET_PLAYER_CROWBAR, // crowbar swipe

	BULLET_MONSTER_9MM,
	BULLET_MONSTER_MP5,
	BULLET_MONSTER_12MM
}Bullet;

enum glock_e
{
	GLOCK_IDLE1 = 0,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT,
	GLOCK_SHOOT_EMPTY,
	GLOCK_RELOAD,
	GLOCK_RELOAD_NOT_EMPTY,
	GLOCK_DRAW,
	GLOCK_HOLSTER,
	GLOCK_ADD_SILENCER
};

enum shotgun_e
{
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

enum mp5_e
{
	MP5_LONGIDLE = 0,
	MP5_IDLE1,
	MP5_LAUNCH,
	MP5_RELOAD,
	MP5_DEPLOY,
	MP5_FIRE1,
	MP5_FIRE2,
	MP5_FIRE3
};

enum python_e
{
	PYTHON_IDLE1 = 0,
	PYTHON_FIDGET,
	PYTHON_FIRE1,
	PYTHON_RELOAD,
	PYTHON_HOLSTER,
	PYTHON_DRAW,
	PYTHON_IDLE2,
	PYTHON_IDLE3
};

#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

enum gauss_e
{
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};

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
