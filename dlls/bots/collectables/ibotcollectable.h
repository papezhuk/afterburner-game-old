#ifndef IBOTCOLLECTABLE_H
#define IBOTCOLLECTABLE_H

// Interface for specifying that a class represents something that a bot can collect in a match.
// This includes weapons, ammo, and other pickups like health kits and armour.
class IBotCollectable
{
public:
	virtual ~IBotCollectable() {}

	enum class Type
	{
		Weapon = 0,
		Ammo,
		Utility	// Health, armour, etc.
	};

	virtual Type CollectableType() const = 0;
};

#endif // IBOTCOLLECTABLE_H