#pragma once

class Movement;

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
	Prism::Camera* myCamera;
	CU::Matrix44<float> myOrientation;
};

inline Prism::Camera* Player::GetCamera() const
{
	return myCamera;
}