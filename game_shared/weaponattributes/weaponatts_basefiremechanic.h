#pragma once

class CGenericWeaponAtts_BaseFireMechanic
{
public:
	enum FireMechanic_e
	{
		Hitscan = 0,
		Projectile
	};

	virtual ~CGenericWeaponAtts_BaseFireMechanic() {}

	virtual FireMechanic_e Id() const = 0;
	virtual CGenericWeaponAtts_BaseFireMechanic* Clone() const = 0;

	template<typename T>
	T* AsType()
	{
		return dynamic_cast<T*>(this);
	}

	template<typename T>
	const T* AsType() const
	{
		return dynamic_cast<const T*>(this);
	}
};