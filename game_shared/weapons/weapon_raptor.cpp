#include "weapon_raptor.h"
#include "skill.h"

namespace
{
	enum RaptorAnimations_e
	{
		RAPTOR_IDLE1,
		RAPTOR_IDLE2,
		RAPTOR_IDLE3,
		RAPTOR_IDLE4,
		RAPTOR_SHOOT,
		RAPTOR_RELOAD,
		RAPTOR_SHOOT_LAST,
		RAPTOR_RELOAD_EMPTY,
		RAPTOR_IDLE1_EMPTY,
		RAPTOR_IDLE2_EMPTY,
		RAPTOR_IDLE3_EMPTY,
		RAPTOR_IDLE4_EMPTY,
		RAPTOR_DRAW,
		RAPTOR_HOLSTER
	};

	static constexpr float RAPTOR_FIRE_RATE = 1.0f / 0.4f;
	static constexpr float RAPTOR_AUTOAIM_DEG = AUTOAIM_10DEGREES;
	static constexpr float RAPTOR_BASE_DAMAGE = 30.0f;
	static constexpr float RAPTOR_BASE_SPREAD = 0.0f;
	static constexpr int RAPTOR_AMMOBOX_GIVE = 9;

#ifdef CLIENT_DLL
	static CWeaponRaptor PredictionWeapon;
#endif
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponRaptor)
	.Classname("weapon_raptor")
	.Flags(0)
	.SwitchWeight(0)
	.PrimaryAmmoDef(&AmmoDef_Raptor)
	.PrimaryAmmoClassname("ammo_raptor")
	.MaxClip(9)
	.PrimaryAmmoOnFirstPickup(9)
	.ViewModelName("models/weapon_raptor/v_raptor.mdl")
	.PlayerModelName("models/weapon_raptor/p_raptor.mdl")
	.WorldModelName("models/weapon_raptor/w_raptor.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
.FireMode(0, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_raptor/fire01.sc")
	.FiresUnderwater(true)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
	.FireRate(RAPTOR_FIRE_RATE)
	.UniformSpread(RAPTOR_BASE_SPREAD)
	.AnimIndex_FireNotEmpty(RAPTOR_SHOOT)
	.AnimIndex_FireEmpty(RAPTOR_SHOOT_LAST)
	.ViewPunchY(-3.0f)
	.Volume(LOUD_GUN_VOLUME)
	.MuzzleFlashBrightness(BRIGHT_GUN_FLASH)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_raptor/raptor_fire1.wav")
		.MinVolume(0.97f)
		.MaxVolume(1.0f)
		.MinPitch(95)
		.MaxPitch(100)
	)
	.Mechanic(&((*new CGenericWeaponAtts_HitscanFireMechanic())
		.BaseDamagePerShot(&skilldata_t::plrDmgRaptor)
		.AutoAim(RAPTOR_AUTOAIM_DEG)
		.ShellModelName("models/shell.mdl") // TODO: Does Nightfire have shell models? Could we make some?
	))
)
.Animations(
	CGenericWeaponAtts_Animations()
	.Extension("python")
	.Index_Draw(RAPTOR_DRAW)
	.Index_ReloadWhenEmpty(RAPTOR_RELOAD_EMPTY)
	.Index_ReloadWhenNotEmpty(RAPTOR_RELOAD)
)
// TODO: Extend base class to allow specifying idle animations for when clip is empty.
.IdleAnimations(
	CGenericWeaponAtts_IdleAnimations()
	.Animation(RAPTOR_IDLE1)
	.Animation(RAPTOR_IDLE2)
	.Animation(RAPTOR_IDLE3)
	.Animation(RAPTOR_IDLE4)
);

LINK_ENTITY_TO_CLASS(weapon_raptor, CWeaponRaptor)

const CGenericWeaponAttributes& CWeaponRaptor::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}

class CAmmoRaptor : public CGenericAmmo
{
public:
	CAmmoRaptor()
		: CGenericAmmo("models/weapon_raptor/w_ammo_raptor.mdl", AmmoDef_Raptor, RAPTOR_AMMOBOX_GIVE)
	{
	}
};

LINK_ENTITY_TO_CLASS(ammo_raptor, CAmmoRaptor)
