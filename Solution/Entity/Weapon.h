#pragma once

struct UpgradeComponentData;

class Weapon
{
public:
	Weapon(eWeaponType aWeaponType, std::string aXMLTagName, Entity* aOwnerEntity);
	virtual ~Weapon();

	virtual bool Shoot(const CU::Matrix44<float>& aOrientation) = 0;
	virtual void Reload() = 0;
	virtual void Update(float) {}
	virtual void Render(){}
	const int& GetAmmoInClip() const;
	const int& GetClipSize() const;
	eWeaponType GetWeaponType();

	virtual void Upgrade(const UpgradeComponentData& aData);
	void SendRayCastRequest(const CU::Vector3<float>& aPosition, const CU::Vector3<float>& aDirection, float aMaxLength, int aGID);

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