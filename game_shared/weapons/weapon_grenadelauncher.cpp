#include "weapon_grenadelauncher.h"
#include "ammodefs.h"
#include "skill.h"

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
	.SwitchWeight(0)
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
	.Mechanic(&((*new CGenericWeaponAtts_ProjectileFireMechanic())
		.AnimIndex_FireNotEmpty(GRENADELAUNCHER_FIRE)
		.Volume(LOUD_GUN_VOLUME)
		.MuzzleFlashBrightness(BRIGHT_GUN_FLASH)
		.FireRate(1.0f / 1.5f)
		.FullAuto(false)
		.UniformSpread(1.0f)
		.Projectile([](){/*TODO*/ return (CGrenade*)NULL;})
		.Sounds(CGenericWeaponAttributes_Sound()
			.Sound("weapons/weapon_grenadelauncher/grenade_launcher_fire.wav")
			.MinPitch(98)
			.MaxPitch(100)
		)
	))
)
// Timed
.FireMode(1, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_grenadelauncher/fire02.sc")
	.FiresUnderwater(false)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
)
.Animations(CGenericWeaponAtts_Animations()
	.Extension("gauss")
	.Index_Draw(GRENADELAUNCHER_DRAW)
	.Index_ReloadWhenNotEmpty(GRENADELAUNCHER_RELOAD)
);

LINK_ENTITY_TO_CLASS(weapon_grenadelauncher, CWeaponGrenadeLauncher);

const CGenericWeaponAttributes& CWeaponGrenadeLauncher::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}

class CAmmoGrenadeLauncher : public CGenericAmmo
{
public:
	CAmmoGrenadeLauncher()
		: CGenericAmmo("models/weapon_grenadelauncher/w_ammo_grenadelauncher.mdl", AmmoDef_GrenadeLauncher, GRENADELAUNCHER_AMMOBOX_GIVE)
	{
	}
};

LINK_ENTITY_TO_CLASS(ammo_grenadelauncher, CAmmoGrenadeLauncher)
