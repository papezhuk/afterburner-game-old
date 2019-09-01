#pragma once

#include "weaponatts_base.h"

class CBasePlayerWeapon;

namespace WeaponAtts
{
	struct CWAClient : public CWABase
	{
		CBasePlayerWeapon* PredictionWeapon = nullptr;

		virtual void Validate() const override
		{
			ASSERTSZ_Q(PredictionWeapon, "Prediction weapon must be provided.");
		}
	};
}
