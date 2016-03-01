#pragma once
#include "Weapon.h"
class Entity;
class Weapon;

namespace Prism
{
	class Camera;
	class Scene;
}

class Shooting
{
public:
	Shooting(Prism::Scene* aScene);
	~Shooting();

	void Update(float aDelta, const CU::Matrix44<float>& aOrientation);

	Weapon* GetWeapon(eWeaponType aWeaponType);
private:
	void ShootAtDirection(const CU::Matrix44<float>& aOrientation);
	
	Entity* myBullet;
	CU::GrowingArray<Entity*> myBullets;
	CU::Matrix44<float> myBulletOrientation;
	float myBulletSpeed;
	Prism::Scene* myScene;

	Weapon* myCurrentWeapon;
	Weapon* myPistol;
	Weapon* myShotgun;
	Weapon* myGrenadeLauncher;
};
