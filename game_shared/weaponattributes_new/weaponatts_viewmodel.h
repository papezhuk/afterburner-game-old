#pragma once

#include <functional>
#include "weaponatts_base.h"
#include "weightedvaluelist.h"

namespace WeaponAtts
{
	struct CWAViewModel : public CWABase
	{
		const char* ModelName = NULL;
		int Anim_Draw = -1;

		WeightedValueList<int> AnimList_Idle;

		// These animations are specific to when the clip is empty.
		// If there are no empty-specific animations, leave this list empty.
		WeightedValueList<int> AnimList_IdleEmpty;

		virtual void Validate() const override
		{
			ASSERTSZ_Q(ModelName, "View model must be specified.");
			ASSERTSZ_Q(AnimList_Idle.Count() > 0, "At least one idle animation must be specified.");
		}
	};
}
