#pragma once

#include <cstdint>

struct CAmmoDef
{
	const char* Name;
	uint32_t MaxCarry;
};

extern const CAmmoDef AmmoDef_9mm;
extern const CAmmoDef AmmoDef_357;
extern const CAmmoDef AmmoDef_Buckshot;
extern const CAmmoDef AmmoDef_Bolt;
extern const CAmmoDef AmmoDef_Rocket;
extern const CAmmoDef AmmoDef_HandGrenade;
extern const CAmmoDef AmmoDef_Satchel;
extern const CAmmoDef AmmoDef_Tripmine;
extern const CAmmoDef AmmoDef_Snark;
extern const CAmmoDef AmmoDef_Hornet;
extern const CAmmoDef AmmoDef_M203Grenade;
extern const CAmmoDef AmmoDef_Uranium;

// Nightfire weapon ammo
extern const CAmmoDef AmmoDef_P99;
extern const CAmmoDef AmmoDef_Frinesi;
extern const CAmmoDef AmmoDef_GrenadeLauncher;
extern const CAmmoDef AmmoDef_Raptor;
