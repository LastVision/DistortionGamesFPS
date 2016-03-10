#pragma once

#include "Weapon.h"


class Pistol : public Weapon
{
public:
	Pistol();
	~Pistol();

	bool Shoot(const CU::Matrix44<float>& aOrientation) override;
	void Reload() override;
	void Update(float aDelta) override;

	void HandleRaycast(Entity* anEntity, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition);

private:
	std::function<void(Entity*, const CU::Vector3<float>&, const CU::Vector3<float>&)> myRaycastHandler;



};

