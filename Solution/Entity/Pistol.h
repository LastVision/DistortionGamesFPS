#pragma once

#include "Weapon.h"

class PhysicsComponent;

namespace Prism
{
	class Instance;
	class Scene;
}

class Pistol : public Weapon
{
public:
	Pistol();
	~Pistol();
	void Init(Prism::Scene* aScene, const CU::Matrix44<float>& aOrientation);


	bool Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;
	void Update(float aDelta) override;
	void Render() override;

	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition);


private:
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myRaycastHandler;
	Prism::Instance* myMuzzleflash[5];
	int myCurrentMuzzleflash;
	const CU::Matrix44<float>* myOrientation;
	float myMuzzleflashTimer;

};
