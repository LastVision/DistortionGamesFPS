#include "stdafx.h"

#include <Camera.h>
#include <InputWrapper.h>
#include "PlayerComponent.h"


PlayerComponent::PlayerComponent()
{
	myCamera = new Prism::Camera(myOrientation);
}


PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Update(float aDelta)
{
	myMousePosition.x += CU::InputWrapper::GetInstance()->GetMouseDX();
	myMousePosition.y += CU::InputWrapper::GetInstance()->GetMouseDY();

	//myMousePosition.y = fminf(fmaxf(myMousePosition.y, -M_PI_2), M_PI_2);


	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S))
	{
		myOrientation.SetPos(myOrientation.GetPos() + CU::Vector3<float>(0.f, 0.f, -100.f) * aDelta);
	}

	DEBUG_PRINT(myMousePosition);

	myYaw = CU::Matrix44<float>::CreateRotateAroundY(myMousePosition.x);
	myPitch = CU::Matrix44<float>::CreateRotateAroundX(myMousePosition.y);

	CU::Vector3<float> position = myOrientation.GetPos();
	myOrientation = myYaw * myPitch;
	myOrientation.SetPos(position);
}