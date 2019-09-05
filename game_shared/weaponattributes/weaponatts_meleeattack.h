#pragma once

#include "weaponatts_baseattack.h"
#include "weaponatts_skillrecord.h"
#include "weaponatts_soundset.h"

namespace WeaponAtts
{
	struct WAMeleeAttack : public WABaseAttack
	{
		typedef WASkillRecord::SkillDataEntryPtr SkillDataEntryPtr;

		float Reach = 32.0f;	// Defaults to crowbar's reach
		SkillDataEntryPtr BaseDamagePerHit = nullptr;
		bool DecalOnImpact = true;

		// If no body hitsounds provided, world hitsounds are used for bodies too.
		WASoundSet WorldHitSounds;
		WASoundSet BodyHitSounds;

		virtual WABaseAttack::Classification Classify() const override
		{
			return WABaseAttack::Classification::Melee;
		}

		virtual void Validate() const override
        {
            WABaseAttack::Validate();

            ASSERTSZ_Q(BaseDamagePerHit, "Skill entry for base damage per hit must be specified.");
        }
	};
}
