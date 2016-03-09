#pragma once
#include "Component.h"
#include <GameEnum.h>


class Movement;

namespace Prism
{
	class Camera;
}

class InputComponent : public Component
{
public:
	InputComponent(Entity& anEntity);
	~InputComponent();

	void Update(float aDelta) override;

	void Render();

	void Respawn();

	Prism::Camera* GetCamera() const;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	const CU::Matrix44<float>& GetEyeOrientation() const;

	void SetNetworkID(const unsigned short aNetworkID);
private:
	Movement* myMovement;
	Prism::Camera* myCamera;
	CU::Matrix44<float> myOrientation;
	CU::Matrix44<float> myEyeOrientation;

	float myHeight;
	float myCrouchHeight;
	float myJumpAcceleration;
	float myJumpOffset;
	float mySendTime;
	bool myAlive;
	CU::Vector3<float> mySpawnPosition;

	unsigned short myNetworkID;
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

inline void InputComponent::SetNetworkID(const unsigned short aNetworkID)
{
	myNetworkID = aNetworkID;
}