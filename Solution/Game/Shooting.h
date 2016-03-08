#pragma once
#include "Weapon.h"
class Entity;
class Weapon;
class Player;
class Pistol;
class Shotgun;
class GrenadeLauncher;

namespace Prism
{
	class Camera;
	class Scene;
}

class Shooting
{
public:
	Shooting(Prism::Scene* aScene, Player* aPlayer);
	~Shooting();

	void Update(float aDelta, const CU::Matrix44<float>& aOrientation);

	Weapon* GetWeapon(eWeaponType aWeaponType);
	Weapon* GetCurrentWeapon();

private:
	Weapon* myCurrentWeapon;
	Pistol* myPistol;
	Shotgun* myShotgun;
	GrenadeLauncher* myGrenadeLauncher;

	Player* myPlayer;
};

inline Weapon* Shooting::GetCurrentWeapon()
{
	return myCurrentWeapon;
}