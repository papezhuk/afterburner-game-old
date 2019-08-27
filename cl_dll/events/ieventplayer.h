#pragma once

#include "genericweaponattributes.h"

typedef struct event_args_s event_args_t;

class IEventPlayer
{
public:
	virtual void PlayEvent(const event_args_t* eventArgs,
						   const CGenericWeaponAtts_FireMode::FireModeSignature* signature) = 0;
};