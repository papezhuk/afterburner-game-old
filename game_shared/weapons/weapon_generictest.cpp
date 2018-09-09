#include "weapon_generictest.h"
#include "weaponregistry.h"

enum Weapon_GenericTest_Animations_e
{
	W_GENERICTEST_IDLE1 = 0,
	W_GENERICTEST_IDLE2,
	W_GENERICTEST_IDLE3,
	W_GENERICTEST_SHOOT,
	W_GENERICTEST_SHOOT_EMPTY,
	W_GENERICTEST_RELOAD_EMPTY,
	W_GENERICTEST_RELOAD_NOT_EMPTY,
	W_GENERICTEST_DRAW
};

#ifdef CLIENT_DLL
static CWeaponGenericTest PredictionWeapon;
#endif

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponGenericTest)
	.Classname("weapon_generictest")
	.WeaponSlot(1)
	.WeaponSlotPosition(2)
	.Flags(0)
	.SwitchWeight(0)
	.PrimaryAmmoDef(&AmmoDef_9mm)
	.PrimaryAmmoOnFirstPickup(32)
	.MaxClip(16)
	.ViewModelName("models/weapon_p99/v_p99.mdl")
	.PlayerModelName("models/weapon_p99/p_p99.mdl")
	.WorldModelName("models/weapon_p99/w_p99.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
.FireMode(0, &((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_generictest/fire01.sc"))
	.FireRate(1.33f)
	.UniformSpread(0.01f)
	.DamagePerShot(10)
	.AutoAim(AUTOAIM_10DEGREES))
	.ShellModelName("models/shell.mdl")
	.AnimIndex_FireNotEmpty(W_GENERICTEST_SHOOT)
	.AnimIndex_FireEmpty(W_GENERICTEST_SHOOT_EMPTY)
	.ViewPunchY(-2.0f)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/pl_gun3.wav")
		.MinVolume(0.92f)
		.MaxVolume(1.0f)
		.MinPitch(98)
		.MaxPitch(101)
	)
)
.FireMode(1, &((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_generictest/fire02.sc"))
	.FireRate(3.0f)
	.UniformSpread(0.1f)
	.DamagePerShot(10)
	.FullAuto(true)
	.Volume(LOUD_GUN_VOLUME))
	.ShellModelName("models/shell.mdl")
	.AnimIndex_FireNotEmpty(W_GENERICTEST_SHOOT)
	.AnimIndex_FireEmpty(W_GENERICTEST_SHOOT_EMPTY)
	.ViewPunchY(-2.5f)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/pl_gun3.wav")
		.MinVolume(0.92f)
		.MaxVolume(1.0f)
		.MinPitch(98)
		.MaxPitch(101)
	)
)
.Animations(CGenericWeaponAtts_Animations()
	.Extension("onehanded")
	.Index_Draw(W_GENERICTEST_DRAW)
	.Index_ReloadWhenEmpty(W_GENERICTEST_RELOAD_EMPTY)
	.Index_ReloadWhenNotEmpty(W_GENERICTEST_RELOAD_NOT_EMPTY))
.IdleAnimations(CGenericWeaponAtts_IdleAnimations()
	.Animation(W_GENERICTEST_IDLE1, 1.0)
	.Animation(W_GENERICTEST_IDLE2, 2.0)
	.Animation(W_GENERICTEST_IDLE3, 3.0));

LINK_ENTITY_TO_CLASS(weapon_generictest, CWeaponGenericTest)

void CWeaponGenericTest::Precache()
{
	CGenericWeapon::Precache();

	// TODO: Add shell model to generic weapon defs?
	PRECACHE_MODEL("models/shell.mdl");// brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");

	PRECACHE_SOUND("weapons/pl_gun1.wav");//silenced handgun
	PRECACHE_SOUND("weapons/pl_gun2.wav");//silenced handgun
	PRECACHE_SOUND("weapons/pl_gun3.wav");//handgun
}

const CGenericWeaponAttributes& CWeaponGenericTest::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}
