#include <string>
#include <Camera.h>
#include <DL_Debug.h>
#include "DLLCamera.h"
#include <EngineEnums.h>
#include <InputWrapper.h>

#define InputInstanceCamera CU::InputWrapper::GetInstance()

DLLCamera::DLLCamera(CU::Vector2<float>& aWindowSize, float aMovementSpeed, float aRotationSpeed, float aZoomSpeed)
	: myMovementSpeed(aMovementSpeed)
	, myRotationSpeed(aRotationSpeed)
	, myZoomSpeed(aZoomSpeed)
{
	myOrientation.SetPos({ 0, 0, 0 });
	myOriginalOrientation = myOrientation;
	myCamera = new Prism::Camera(myOrientation);
	myCamera->OnResize(aWindowSize.x, aWindowSize.y);
}


DLLCamera::~DLLCamera()
{
	delete myCamera;
	myCamera = nullptr;
}

void DLLCamera::Zoom(float aDeltaTime, float aMouseSens)
{
	if (HasMouseDeltaXMoved(aMouseSens) == true)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetForward() * InputInstanceCamera->GetMouseDY() * myZoomSpeed * aDeltaTime);
	}
	if (HasMouseDeltaYMoved(aMouseSens) == true)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetForward() * InputInstanceCamera->GetMouseDX() * myZoomSpeed * aDeltaTime);
	}
}

void DLLCamera::Pan(float aDeltaTime, float aMouseSens)
{
	if (HasMouseDeltaXMoved(aMouseSens) == true)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetRight() * InputInstanceCamera->GetMouseDX() * myMovementSpeed * aDeltaTime);
	}
	if (HasMouseDeltaYMoved(aMouseSens) == true)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetUp() * InputInstanceCamera->GetMouseDY() * myMovementSpeed * aDeltaTime);
	}
}

void DLLCamera::Rotate(float aDeltaTime, float aMouseSens)
{
	CU::Matrix44f rotationAroundObject;
	if (HasMouseDeltaXMoved(aMouseSens) == true)
	{
		rotationAroundObject = myOrientation * CU::Matrix44f::CreateRotateAroundX(InputInstanceCamera->GetMouseDY()
			* myRotationSpeed * aDeltaTime);
		myOrientation = rotationAroundObject;
	}
	if (HasMouseDeltaYMoved(aMouseSens) == true)
	{
		rotationAroundObject = myOrientation * CU::Matrix44f::CreateRotateAroundY(InputInstanceCamera->GetMouseDX()
			* myRotationSpeed * aDeltaTime);
		myOrientation = rotationAroundObject;
	}
}

void DLLCamera::Update(float aDeltaTime)
{
	myCamera->Update(aDeltaTime);
}

void DLLCamera::ResetCamera()
{
	myOrientation = myOriginalOrientation;
}

bool DLLCamera::HasMouseDeltaYMoved(float aSens)
{
	return (InputInstanceCamera->GetMouseDY() < aSens || InputInstanceCamera->GetMouseDY() > aSens);
}

bool DLLCamera::HasMouseDeltaXMoved(float aSens)
{
	return (InputInstanceCamera->GetMouseDX() < aSens || InputInstanceCamera->GetMouseDX() > aSens);
}