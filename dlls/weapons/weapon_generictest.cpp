#include "weapon_generictest.h"

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

const CGenericWeaponAttributes CWeaponGenericTest::m_StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponGenericTest)
	.Classname("weapon_generictest")
	.WeaponSlot(1)
	.WeaponSlotPosition(2)
	.Flags(0)
	.SwitchWeight(0)
	.PrimaryAmmoName("9mm")
	.PrimaryAmmoMax(64)
	.PrimaryAmmoOnFirstPickup(8)
	.MaxClip(8)
	.ViewModelName("models/v_p99.mdl")
	.PlayerModelName("models/p_p99.mdl")
	.WorldModelName("models/w_p99.mdl")
)
.FireMode(0, &((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_generictest_fire01.sc"))
	.FireRate(1.33f)
	.BulletType(BULLET_PLAYER_9MM)
	.Spread(0.01f)
	.AutoAim(true)))
.FireMode(1, &((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_generictest_fire02.sc"))
	.FireRate(3.0f)
	.BulletType(BULLET_PLAYER_9MM)
	.Spread(0.1f)
	.AutoAim(false)
	.FullAuto(true)
	.Volume(LOUD_GUN_VOLUME)))
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
