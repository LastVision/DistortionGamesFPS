#pragma once


#include "Weapon.h"
class PhysicsComponent;


class Shotgun : public Weapon
{
public:
	Shotgun();
	~Shotgun();

	bool Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;
	void Update(float aDelta) override;

	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition);

private:
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myRaycastHandler;
	void ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward);

	float myMinSpreadRotation;
	float myMaxSpreadRotation;
};

