#pragma once


#include "Weapon.h"
class PhysicsComponent;


class Shotgun : public Weapon
{
public:
	Shotgun(Entity* aOwnerEntity);
	~Shotgun();

	void Init(Prism::Scene* aScene, const CU::Matrix44<float>& aOrientation); // init for muzle flashes
	void Init(std::string aWeaponSettingsPath, std::string aXMLTagName) override;

	bool Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;
	void Update(float aDelta) override;

	void HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal);

	void Upgrade(const UpgradeComponentData& aData) override;

private:
	std::function<void(PhysicsComponent*, const CU::Vector3<float>&, const CU::Vector3<float>&, const CU::Vector3<float>&)> myRaycastHandler;
	void ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward);

	float myMinSpreadRotation;
	float myMaxSpreadRotation;

	Prism::Instance* myMuzzleflash[5];
	int myCurrentMuzzleflash1;
	int myCurrentMuzzleflash2;
	const CU::Matrix44<float>* myOrientation;
	CU::Matrix44<float> myMuzzleflashOrientation1;
	CU::Matrix44<float> myMuzzleflashOrientation2;
	float myMuzzleflashTimer;
};

