#pragma once

enum class eWeaponType
{
	PISTOL,
	SHOTGUN,
	GRENADE_LAUNCHER
};

class Weapon
{
public:
	Weapon(eWeaponType aWeaponType);
	virtual ~Weapon();

	virtual void Shoot(const CU::Matrix44<float>& aOrientation) = 0;
	virtual void Reload() = 0;
	const int& GetAmmoInClip() const;
	const int& GetClipSize() const;
	eWeaponType GetWeaponType();
	//virtual void HandleRaycast(Entity* anEntity, const CU::Vector3<float>& aDirection) {};

protected:	
	int myAmmoInClip;
	int myClipSize;
	int myAmmoTotal;
	int myDamage;



	eWeaponType myWeaponType;
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