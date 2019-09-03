#pragma once

#ifdef CLIENT_DLL
#include "weaponatts_base.h"

class CBasePlayerWeapon;

namespace WeaponAtts
{
	struct WAClient : public WABase
	{
		CBasePlayerWeapon* PredictionWeapon = nullptr;

		virtual void Validate() const override
		{
			ASSERTSZ_Q(PredictionWeapon, "Prediction weapon must be provided.");
		}
	};
}
#endif // CLIENT_DLL