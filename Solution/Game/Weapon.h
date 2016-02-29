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
	~Weapon();

	void Shoot(const CU::Matrix44<float>& aOrientation);
	void Reload();
	int& GetAmmoInClip();
	int& GetClipSize();

private:
	void ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward);
	int myAmmoInClip;
	int myClipSize;
	int myAmmoTotal;

	int myDamage;

	eWeaponType myWeaponType;
};

inline int& Weapon::GetAmmoInClip()
{
	return myAmmoInClip;
}

inline int& Weapon::GetClipSize()
{
	return myClipSize;
}