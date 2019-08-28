#pragma once

#include "baseweaponeventplayer.h"

class HitscanWeaponEventPlayer : public BaseWeaponEventPlayer
{
public:
	virtual ~HitscanWeaponEventPlayer() {}

protected:
	virtual void EventStart() override;
	virtual bool Initialise() override;
	virtual void ParseEventScript(const rapidjson::Document& document) override;

private:
	void CreateBulletTracers();

	float m_flSpreadX = 0.0f;
	float m_flSpreadY = 0.0f;
	int m_iRandomSeed = 0;
	int m_iTracerStride = 1;
	uint32_t m_iShotsFired = 0;
};