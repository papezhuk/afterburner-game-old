#include "weapon_fists.h"
#include "weaponatts_collection.h"
#include "weapon_pref_weights.h"
#include "weaponatts_meleeattack.h"

enum FistsAnimations_e
{
	FISTS_IDLE1 = 0,
	FISTS_JAB,
	FISTS_COMBO,
	FISTS_KARATE_CHOP
};

enum FistsAttackMode_e
{
	ATTACKMODE_TEST = 0
};

static constexpr float FISTS_PUNCH_RATE_SINGLE = 2.5f;

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

	WAMeleeAttack* priAttack = new WAMeleeAttack();
	obj.AttackModes.AddToTail(std::shared_ptr<WABaseAttack>(priAttack));

	priAttack->EventScript = "events/weapon_fists/punch.sc";
	priAttack->FunctionsUnderwater = true;
	priAttack->IsContinuous = true;
	priAttack->AttackRate = FISTS_PUNCH_RATE_SINGLE;
	priAttack->BaseDamagePerHit = &skilldata_t::plrDmgFists;
	priAttack->DecalOnImpact = false;
	priAttack->StrikeDelay = 0.1f;
	priAttack->Volume = 128;
	priAttack->ViewModelAnimList_Attack << FISTS_JAB;

	priAttack->WorldHitSounds.MinPitch = 97;
	priAttack->WorldHitSounds.MaxPitch = 103;
	priAttack->WorldHitSounds.SoundNames << "weapons/weapon_fists/impact1.wav"
										 << "weapons/weapon_fists/impact2.wav"
										 << "weapons/weapon_fists/impact3.wav";

	priAttack->AttackSounds.MinPitch = 97;
	priAttack->AttackSounds.MaxPitch = 103;
	priAttack->AttackSounds.SoundNames << "weapons/weapon_fists/swing1.wav"
									   << "weapons/weapon_fists/swing2.wav"
									   << "weapons/weapon_fists/swing3.wav";
});
