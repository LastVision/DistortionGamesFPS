#pragma once
#include "Weapon.h"

class Entity;

namespace Prism
{
	class Scene;
}

class GrenadeLauncher : public Weapon
{
public:
	GrenadeLauncher(Prism::Scene* aScene);
	~GrenadeLauncher();

	bool Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;

	void Update(float aDelta) override;

private:
	void ShootAtDirection(const CU::Matrix44<float>& aOrientation);

	CU::GrowingArray<Entity*> myBullets;
	float myBulletSpeed;
	Prism::Scene* myScene;
};

