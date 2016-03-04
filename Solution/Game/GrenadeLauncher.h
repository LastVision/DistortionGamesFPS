#pragma once

#include "Weapon.h"

class GrenadeLauncher : public Weapon
{
public:
	GrenadeLauncher();
	~GrenadeLauncher();

	void Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;
};

