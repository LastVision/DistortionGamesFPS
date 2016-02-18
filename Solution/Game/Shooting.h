#pragma once

namespace Prism
{
	class Camera;
	class Instance;
	class Scene;
}

class Shooting
{
public:
	Shooting(Prism::Scene* aScene);
	~Shooting();

	void Update(float aDelta, const CU::Matrix44<float>& aOrientation);
private:
	void ShootAtDirection(const CU::Matrix44<float>& aOrientation);
	
	Prism::Instance* myBullet;
	CU::Matrix44<float> myBulletOrientation;
	float myBulletSpeed;
};

