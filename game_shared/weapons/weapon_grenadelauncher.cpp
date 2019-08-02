#include "weapon_grenadelauncher.h"
#include "ammodefs.h"
#include "skill.h"
#include "gamerules.h"
#include "weapon_pref_weights.h"

namespace
{
	enum GrenadeLauncherAnimations_e
	{
		GRENADELAUNCHER_IDLE1 = 0,
		GRENADELAUNCHER_IDLE2,
		GRENADELAUNCHER_FIRE,
		GRENADELAUNCHER_RELOAD,
		GRENADELAUNCHER_DRAW,
		GRENADELAUNCHER_HOLSTER
	};

	static constexpr const char* GRENADELAUNCHER_GRENADE_MODEL = "models/weapon_grenadelauncher/w_grenade_projectile.mdl";
	static const Vector GRENADELAUNCHER_HALF_BBOX = Vector(4,4,4);
	static constexpr float GRENADELAUNCHER_GRENADE_FRICTION = 0.95;
	static constexpr float GRENADELAUNCHER_GRENADE_EXPLOSION_SCALE = 60;

	static constexpr float GRENADELAUNCHER_FIRE_INTERVAL = 0.8f;	// Secs
	static constexpr float GRENADELAUNCHER_LAUNCH_SPEED = 1000.0f;
	static constexpr float GRENADELAUNCHER_TUMBLEVEL_MIN = -100.0f;
	static constexpr float GRENADELAUNCHER_TUMBLEVEL_MAX = -500.0f;
	static constexpr float GRENADELAUNCHER_FUSE_TIME = 4.0f;
	static constexpr float GRENADELAUNCHER_AMMOBOX_GIVE = 6;

#ifdef CLIENT_DLL
	CWeaponGrenadeLauncher PredictionWeapon;
#endif
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponGrenadeLauncher)
	.Classname("weapon_grenadelauncher")
	.Flags(0)
	.SwitchWeight(WeaponPref_GrenadeLauncher)
	.PrimaryAmmoDef(&AmmoDef_GrenadeLauncher)
	.PrimaryAmmoClassname("ammo_grenadelauncher")
	.MaxClip(6)
	.PrimaryAmmoOnFirstPickup(6)
	.ViewModelName("models/weapon_grenadelauncher/v_grenadelauncher.mdl")
	.PlayerModelName("models/weapon_grenadelauncher/p_grenadelauncher.mdl")
	.WorldModelName("models/weapon_grenadelauncher/w_grenadelauncher.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
// Contact
.FireMode(0, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_grenadelauncher/fire01.sc")
	.FiresUnderwater(false)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
	.AnimIndex_FireNotEmpty(GRENADELAUNCHER_FIRE)
	.Volume(LOUD_GUN_VOLUME)
	.MuzzleFlashBrightness(BRIGHT_GUN_FLASH)
	.FireRate(1.0f / GRENADELAUNCHER_FIRE_INTERVAL)
	.FullAuto(false)
	.UniformSpread(1.0f)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_grenadelauncher/grenade_launcher_fire.wav")
		.MinPitch(98)
		.MaxPitch(100)
	)
	.Mechanic(new CGenericWeaponAtts_ProjectileFireMechanic())
)
// Timed
.FireMode(1, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_grenadelauncher/fire02.sc")
	.FiresUnderwater(false)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
	.AnimIndex_FireNotEmpty(GRENADELAUNCHER_FIRE)
	.Volume(LOUD_GUN_VOLUME)
	.MuzzleFlashBrightness(BRIGHT_GUN_FLASH)
	.FireRate(1.0f / GRENADELAUNCHER_FIRE_INTERVAL)
	.FullAuto(false)
	.UniformSpread(1.0f)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_grenadelauncher/grenade_launcher_fire.wav")
		.MinPitch(98)
		.MaxPitch(100)
	)
	.Mechanic(new CGenericWeaponAtts_ProjectileFireMechanic())
)
.Animations(CGenericWeaponAtts_Animations()
	.Extension("gauss")
	.Index_Draw(GRENADELAUNCHER_DRAW)
	.Index_ReloadWhenNotEmpty(GRENADELAUNCHER_RELOAD)
)
.Skill(
	CGenericWeaponAttributes_Skill()
	.Record("sk_plr_dmg_grenadelauncher", &skilldata_t::plrDmgGrenadeLauncher)
);

LINK_ENTITY_TO_CLASS(weapon_grenadelauncher, CWeaponGrenadeLauncher);

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known weapons to this one.
LINK_ENTITY_TO_CLASS(weapon_ronin, CWeaponGrenadeLauncher)
LINK_ENTITY_TO_CLASS(weapon_rocketlauncher, CWeaponGrenadeLauncher)
#endif

const CGenericWeaponAttributes& CWeaponGrenadeLauncher::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}

void CWeaponGrenadeLauncher::Precache()
{
	CGenericProjectileWeapon::Precache();

	PRECACHE_MODEL(GRENADELAUNCHER_GRENADE_MODEL);
}

#ifndef CLIENT_DLL
void CWeaponGrenadeLauncher::CreateProjectile(int index,
											  const CGenericWeaponAtts_FireMode& fireMode,
											  const CGenericWeaponAtts_BaseFireMechanic& mechanic)
{
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
	const Vector forward = gpGlobals->v_forward;
	const Vector location = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + forward * 16.0f;

	CWeaponGrenadeLauncher_Grenade* grenade = CreateGrenade(m_pPlayer->pev, location, forward);
	grenade->SetExplodeOnContact(index == 0);
	grenade->SetRandomTumbleAngVel(GRENADELAUNCHER_TUMBLEVEL_MIN, GRENADELAUNCHER_TUMBLEVEL_MAX);
	grenade->SetDamageOnExplode(gSkillData.plrDmgGrenadeLauncher);
	grenade->SetSpeed(GRENADELAUNCHER_LAUNCH_SPEED);
	grenade->SetFuseTime(index == 1 ? GRENADELAUNCHER_FUSE_TIME : -1);
}

CWeaponGrenadeLauncher_Grenade* CWeaponGrenadeLauncher::CreateGrenade(entvars_t *pevOwner, const Vector& location, const Vector& launchDir)
{
	CWeaponGrenadeLauncher_Grenade *pGrenade = GetClassPtr<CWeaponGrenadeLauncher_Grenade>(NULL);
	pGrenade->Spawn();

	UTIL_SetOrigin(pGrenade->pev, location);
	pGrenade->pev->velocity = launchDir;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	pGrenade->pev->owner = ENT(pevOwner);
	pGrenade->SetFuseTime(-1.0f);
	pGrenade->SetExplodeSpriteScale(GRENADELAUNCHER_GRENADE_EXPLOSION_SCALE);

	return pGrenade;
}
#endif

void CWeaponGrenadeLauncher_Grenade::Spawn()
{
	CGrenade::Spawn();
	SET_MODEL(ENT(pev), GRENADELAUNCHER_GRENADE_MODEL);
	UTIL_SetSize(pev, -GRENADELAUNCHER_HALF_BBOX, GRENADELAUNCHER_HALF_BBOX);
	pev->friction = GRENADELAUNCHER_GRENADE_FRICTION;
}

#ifndef CLIENT_DLL
void CWeaponGrenadeLauncher_Grenade::SetExplodeOnContact(bool explodeOnContact)
{
	SetTouch(explodeOnContact ? &CGrenade::ExplodeTouch : &CGrenade::BounceTouch);
}

void CWeaponGrenadeLauncher_Grenade::SetTumbleAngVel(float vel)
{
	pev->avelocity.x = vel;
}

void CWeaponGrenadeLauncher_Grenade::SetRandomTumbleAngVel(float min, float max)
{
	SetTumbleAngVel(RANDOM_FLOAT(min, max));
}

void CWeaponGrenadeLauncher_Grenade::SetDamageOnExplode(float damage)
{
	pev->dmg = damage;
}

void CWeaponGrenadeLauncher_Grenade::SetSpeed(float speed)
{
	Vector& vel = pev->velocity;

	if ( vel.Length() == 0.0f )
	{
		return;
	}

	vel = vel.Normalize() * speed;
}

void CWeaponGrenadeLauncher_Grenade::SetFuseTime(float fuseTime)
{
	if ( fuseTime < 0.0f )
	{
		SetThink(&CGrenade::DangerSoundThink);
		pev->nextthink = gpGlobals->time;
		return;
	}

	SetThink(&CGrenade::TumbleThink);

	pev->dmgtime = gpGlobals->time + fuseTime;
	pev->nextthink = gpGlobals->time + 0.1;

	if( fuseTime < 0.1 )
	{
		pev->nextthink = gpGlobals->time;
		pev->velocity = Vector(0, 0, 0);
	}
}
#endif

class CAmmoGrenadeLauncher : public CGenericAmmo
{
public:
	CAmmoGrenadeLauncher()
		: CGenericAmmo("models/weapon_grenadelauncher/w_ammo_grenadelauncher.mdl", AmmoDef_GrenadeLauncher, GRENADELAUNCHER_AMMOBOX_GIVE)
	{
	}
};

LINK_ENTITY_TO_CLASS(ammo_grenadelauncher, CAmmoGrenadeLauncher)

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known ammo to this one.
LINK_ENTITY_TO_CLASS(ammo_rocketlauncher, CAmmoGrenadeLauncher)
#endif