#pragma once

class Entity;

namespace Prism
{
	class Camera;
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
	
	Entity* myBullet;
	CU::Matrix44<float> myBulletOrientation;
	float myBulletSpeed;
};

