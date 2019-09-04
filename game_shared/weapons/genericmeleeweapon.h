#pragma once

#include "genericweapon.h"
#include "eiface.h"

namespace WeaponAtts
{
	struct WAMeleeAttack;
}

class CGenericMeleeWeapon : public CGenericWeapon
{
public:
	// Required so that derived classes can call through.
	virtual void Precache() override;

protected:
	virtual bool InvokeWithAttackMode(WeaponAttackType type, const WeaponAtts::WABaseAttack* attackMode) override;

private:
	void InitTraceVecs(const WeaponAtts::WAMeleeAttack* meleeAttack);
	bool CheckForContact(const WeaponAtts::WAMeleeAttack* meleeAttack, TraceResult& tr);
	void FireEvent(const WeaponAtts::WAMeleeAttack* meleeAttack);

	vec3_t m_vecAttackTraceStart;
	vec3_t m_vecAttackTraceEnd;
};
