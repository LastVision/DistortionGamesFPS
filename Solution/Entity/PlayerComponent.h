#pragma once
#include <Quaternion.h>

namespace Prism
{
	class Camera;
}

class PlayerComponent
{
public:
	PlayerComponent();
	~PlayerComponent();

	void Update(float aDelta);

	Prism::Camera* GetCamera() const;

private:
	void Movement(float aDelta);
	void Rotation(float aDelta);
	CU::Vector2<float> myCursorPosition;
	float mySteeringModifier;
	float myRotationSpeed;
	float myMovementSpeed;
	float myMaxSteeringSpeed;

	CU::Quaternion myPitch;
	CU::Quaternion myYaw;

	Prism::Camera* myCamera;
	CU::Matrix44<float> myOrientation;


	float mySpeed;
};

inline Prism::Camera* PlayerComponent::GetCamera() const
{
	return myCamera;
}