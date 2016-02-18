#pragma once

namespace Prism
{
	class Camera;
	class Instance;
}

class Shooting
{
public:
	Shooting();
	~Shooting();

	void Update(float aDelta, const CU::Matrix44<float>& aOrientation);
	void Render(const Prism::Camera& aCamera);
private:
	void ShootAtDirection(const CU::Matrix44<float>& aOrientation);
	
	Prism::Instance* myBullet;
	CU::Matrix44<float> myBulletOrientation;
	float myBulletSpeed;
};

