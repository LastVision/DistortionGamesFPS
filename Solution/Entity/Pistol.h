#pragma once

#include "Weapon.h"

class PhysicsComponent;

class Pistol : public Weapon
{
public:
	Pistol();
	~Pistol();

	bool Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;
	void Update(float aDelta) override;

	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition);

private:
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myRaycastHandler;



};
