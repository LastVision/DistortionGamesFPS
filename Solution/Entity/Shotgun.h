#pragma once


#include "Weapon.h"
class PhysicsComponent;


class Shotgun : public Weapon
{
public:
	Shotgun(Entity* aOwnerEntity);
	~Shotgun();

	bool Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;
	void Update(float aDelta) override;

	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition);

	void Upgrade(const UpgradeComponentData& aData) override;

private:
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myRaycastHandler;
	void ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward);

	float myMinSpreadRotation;
	float myMaxSpreadRotation;
};

