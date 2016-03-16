#pragma once
#include "Component.h"
#include "Weapon.h"

class Weapon;
class Pistol;
class Shotgun;
class GrenadeLauncher;

class ShootingComponent : public Component
{
public:
	ShootingComponent(Entity& anEntity, Prism::Scene* aScene);
	~ShootingComponent();

	void Init(Prism::Scene* aScene);

	void Update(float aDelta) override;
	void Render();


	Weapon* GetWeapon(eWeaponType aWeaponType);
	Weapon* GetCurrentWeapon();

	void ReceiveNote(const UpgradeNote&) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	Weapon* myCurrentWeapon;
	Pistol* myPistol;
	Shotgun* myShotgun;
	GrenadeLauncher* myGrenadeLauncher;
};

inline Weapon* ShootingComponent::GetCurrentWeapon()
{
	return myCurrentWeapon;
}

inline eComponentType ShootingComponent::GetTypeStatic()
{
	return eComponentType::SHOOTING;
}

inline eComponentType ShootingComponent::GetType()
{
	return GetTypeStatic();
}

