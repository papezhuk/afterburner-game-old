#ifndef BOT_COLLECTABLE_WEAPON_H
#define BOT_COLLECTABLE_WEAPON_H

#include "ibotcollectable.h"

class CGenericWeapon;
class CBaseBotFightStyle;

// Interface for a weapon that a bot can query. The bot can use attributes about
// the weapon to make AI decisions.
class IBotCollectableWeapon : public IBotCollectable
{
public:
	virtual ~IBotCollectableWeapon() {};

	virtual Type CollectableType() const override
	{
		return IBotCollectable::Type::Weapon;
	}

	// Returns the weapon instance itself.
	// Attributes about the weapon can be found by calling WeaponAttributes()
	// on the returned weapon.
	virtual CGenericWeapon& Weapon() = 0;

	// Convenience override for AI - is this weapon allowed to be used by bots at all?
	// Certain items (eg. planted explosives) we might not want to allow until we have
	// adequate AI for them.
	virtual bool CanBeUsed() const = 0;

	// Returns the desire to use this weapon given the distance to the enemy,
	// as a float between 0 and 1 inclusive.
	virtual float DesireToUse(float distToEnemy) const = 0;

	// Set attributes on fightStyle in order to tell the bot how to use this weapon.
	virtual void UseWeapon(CBaseBotFightStyle& fightStyle) = 0;
};

#endif // BOT_COLLECTABLE_WEAPON_H