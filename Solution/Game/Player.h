#pragma once

class Movement;
class Shooting;

namespace Prism
{
	class Camera;
}

class Player
{
public:
	Player();
	~Player();

	void Update(float aDelta);

	Prism::Camera* GetCamera() const;

private:
	Movement* myMovement;
	Shooting* myShooting;
	Prism::Camera* myCamera;
	CU::Matrix44<float> myOrientation;
};

inline Prism::Camera* Player::GetCamera() const
{
	return myCamera;
}