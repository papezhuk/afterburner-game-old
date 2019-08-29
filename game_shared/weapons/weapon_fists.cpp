#include "weapon_fists.h"
#include "weapon_pref_weights.h"

namespace
{
	enum FistsAnimations_e
	{
		FISTS_IDLE1 = 0,
		FISTS_JAB,
		FISTS_COMBO,
		FISTS_KARATE_CHOP
	};

#ifdef CLIENT_DLL
	static CWeaponFists PredictionWeapon;
#endif
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponFists)
	.Classname("weapon_fists")
	.Flags(0)
	.SwitchWeight(WeaponPref_Fists)
	.ViewModelName("models/weapon_fists/v_fists.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
.Animations(
	CGenericWeaponAtts_Animations()
	.Extension("crowbar")
	.Index_Draw(FISTS_IDLE1)
)
.IdleAnimations(
	CGenericWeaponAtts_IdleAnimations()
	.Animation(FISTS_IDLE1)
)
.Skill(
	CGenericWeaponAttributes_Skill()
	.Record("sk_plr_dmg_fists", &skilldata_t::plrDmgFists)
	.Record("sk_plr_dmg_fists_alt", &skilldata_t::plrDmgFistsAlt)
)
;

LINK_ENTITY_TO_CLASS(weapon_fists, CWeaponFists);
LINK_ENTITY_TO_CLASS(weapon_dukes, CWeaponFists);	// For NF compatibility

CWeaponFists::CWeaponFists() : CGenericWeapon()
{
}

const CGenericWeaponAttributes& CWeaponFists::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}

bool CWeaponFists::SwitchFire(int index,
							  const CGenericWeaponAtts_FireMode& fireMode,
							  const CGenericWeaponAtts_BaseFireMechanic& mechanic)
{
	// TODO
	return false;
}

void CWeaponFists::SwitchPrecache(const CGenericWeaponAtts_BaseFireMechanic& mechanic)
{
}

// TODO: Need weapon sprites!