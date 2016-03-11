#pragma once
#include "Component.h"
#include <GameEnum.h>
#include <Quaternion.h>

namespace Prism
{
	class Camera;
}

struct InputComponentData;

class InputComponent : public Component
{
public:
	InputComponent(Entity& anEntity, const InputComponentData& aData);
	~InputComponent();

	void Update(float aDelta) override;

	Prism::Camera* GetCamera() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	const CU::Matrix44<float>& GetEyeOrientation() const;

private:
	void UpdateMovement(float aDelta);

	Prism::Camera* myCamera;
	CU::Matrix44<float> myOrientation;
	CU::Matrix44<float> myPrevOrientation;
	CU::Matrix44<float> myEyeOrientation;

	float myHeight;
	float myCrouchHeight;
	float myJumpAcceleration;
	float myJumpOffset;
	float mySendTime;

	void Move(float aDelta);
	void Rotate();

	CU::Vector2<float> myCursorPosition;
	float myVerticalSpeed;
	const InputComponentData& myData;
	//int myCapsuleControllerId;

	CU::Quaternion myPitch;
	CU::Quaternion myYaw;
};

inline eComponentType InputComponent::GetTypeStatic()
{
	return eComponentType::INPUT;
}

inline eComponentType InputComponent::GetType()
{
	return GetTypeStatic();
}

inline const CU::Matrix44<float>& InputComponent::GetEyeOrientation() const
{
	return myEyeOrientation;
}