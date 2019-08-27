#pragma once

#include "baseweaponeventplayer.h"

class HitscanWeaponEventPlayer : public BaseWeaponEventPlayer
{
protected:
	virtual void EventStart() override;
	virtual bool Initialise() override;

private:
	void CreateBulletTracers();

	float m_flSpreadX = 0.0f;
	float m_flSpreadY = 0.0f;
	int m_iRandomSeed = 0;
};