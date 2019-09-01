#pragma once

#include "debug_assert.h"

namespace WeaponAtts
{
	struct CWABase
	{
		// Override to validate specific members of the class using assertions.
		virtual void Validate() const
		{
		}
	};
}
