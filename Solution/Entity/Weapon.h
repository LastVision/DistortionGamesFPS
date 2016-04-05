#pragma once

struct UpgradeComponentData;

class Weapon
{
public:
	Weapon(eWeaponType aWeaponType, Entity* aOwnerEntity);
	virtual ~Weapon();

	virtual void Init(std::string aWeaponSettingsPath, std::string aXMLTagName);

	virtual bool Shoot(const CU::Matrix44<float>& aOrientation) = 0;
	virtual void Reload() = 0;
	virtual void Update(float) {}
	virtual void Render(){}
	const int& GetAmmoInClip() const;
	const int& GetClipSize() const;
	eWeaponType GetWeaponType();
	float GetForceStrength() const;
	const int GetAmmoTotal() const;

	virtual void Upgrade(const UpgradeComponentData& aData);

protected:	
	int myAmmoInClip;
	int myClipSize;
	int myAmmoTotal;
	int myDamage;

	float myShootTime;
	float myShootTimer;

	float myForceStrength;

	eWeaponType myWeaponType;
	const Entity* myOwnerEntity;
};

inline const int& Weapon::GetAmmoInClip() const
{
	return myAmmoInClip;
}

inline const int& Weapon::GetClipSize() const
{
	return myClipSize;
}

inline eWeaponType Weapon::GetWeaponType()
{
	return myWeaponType;
}

inline float Weapon::GetForceStrength() const
{
	return myForceStrength;
}

inline const int Weapon::GetAmmoTotal() const
{
	return myAmmoTotal;
}