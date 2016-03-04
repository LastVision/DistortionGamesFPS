#pragma once


#include "Weapon.h"



class Shotgun : public Weapon
{
public:
	Shotgun();
	~Shotgun();

	void Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;

	void HandleRaycast(Entity* anEntity, const CU::Vector3<float>& aDirection);

private:
	std::function<void(Entity*, const CU::Vector3<float>&)> myRaycastHandler;
	void ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward);

	float myMinSpreadRotation;
	float myMaxSpreadRotation;
};

