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

	void Shoot();
	void Reload();
	int& GetAmmoInClip();
	int& GetClipSize();

private:
	int myAmmoInClip;
	int myClipSize;
	int myAmmoTotal;

	float myDamage;

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