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
	void ReadXMLSettings(const std::string& aXMLPath);

	void Update(float aDelta) override;
	void Render();


	Weapon* GetWeapon(eWeaponType aWeaponType);
	Weapon* GetCurrentWeapon();

	void ReceiveNote(const UpgradeNote&) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	float GetWeaponForceStrength(eWeaponType aWeaponType) const;
	void ReloadWeapon();


	void SetIsInOptionsMenu(bool aIsInOptionsMenu);
private:
	void ReloadWeaponIntention();
	Weapon* myCurrentWeapon;
	Pistol* myPistol;
	Shotgun* myShotgun;
	GrenadeLauncher* myGrenadeLauncher;

	bool myIsInOptionsMenu;
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

inline void ShootingComponent::SetIsInOptionsMenu(bool aIsInOptionsMenu)
{
	myIsInOptionsMenu = aIsInOptionsMenu;
}


