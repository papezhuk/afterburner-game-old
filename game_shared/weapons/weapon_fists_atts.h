#include "weapon_fists.h"
#include "weaponatts_collection.h"
#include "weapon_pref_weights.h"

enum FistsAnimations_e
{
	FISTS_IDLE1 = 0,
	FISTS_JAB,
	FISTS_COMBO,
	FISTS_KARATE_CHOP
};

static const WeaponAtts::WACollection StaticWeaponAttributes([](WeaponAtts::WACollection& obj)
{
	using namespace WeaponAtts;

	WACore& core = obj.Core;
	core.Classname = "weapon_fists";
	core.Id = WeaponId_e::WeaponFists;
	core.SwitchWeight = WeaponPref_Fists;

	WAViewModel& vm = obj.ViewModel;
	vm.ModelName = "models/weapon_fists/v_fists.mdl";
	vm.Anim_Draw = FISTS_IDLE1;
	vm.AnimList_Idle << FISTS_IDLE1;

	WAPlayerModel& pm = obj.PlayerModel;
	pm.PlayerAnimExtension = "crowbar";

	obj.SkillRecords.AddToTail(WASkillRecord("sk_plr_dmg_fists", &skilldata_t::plrDmgFists));
	obj.SkillRecords.AddToTail(WASkillRecord("sk_plr_dmg_fists_alt", &skilldata_t::plrDmgFistsAlt));
});
