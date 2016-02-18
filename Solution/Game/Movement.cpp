#include "stdafx.h"
#include <InputWrapper.h>
#include "Movement.h"
#include <XMLReader.h>

Movement::Movement(CU::Matrix44<float>& anOrientation, XMLReader& aReader, tinyxml2::XMLElement* anElement)
	: myPitch(CU::Vector3<float>(0, 1.f, 0), 0)
	, myYaw(CU::Vector3<float>(1.f, 0, 0), 0)
	, myOrientation(anOrientation)
{
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(anElement, "speed"), "value", mySpeed);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(anElement, "rotationSpeed"), "value", myRotationSpeed);
	aReader.ForceReadAttribute(aReader.ForceFindFirstChild(anElement, "sprintMultiplier"), "value", mySprintMultiplier);
}


Movement::~Movement()
{
}


void Movement::Update(float aDelta)
{
	Rotate();
	Move(aDelta);
}

void Movement::Move(float aDelta)
{
	CU::Vector3<float> movement;
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S))
	{
		movement.z -= 1.f;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_W))
	{
		movement.z += 1.f;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A))
	{
		movement.x -= 1.f;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_D))
	{
		movement.x += 1.f;
	}

	CU::Normalize(movement);

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT))
	{
		movement *= mySprintMultiplier;
	}

	CU::Vector3<float> forward = CU::Cross(myOrientation.GetRight(), CU::Vector3<float>(0.f, 1.f, 0.f));

	CU::Normalize(forward);

	myOrientation.SetPos(myOrientation.GetPos() + forward * movement.z * mySpeed * aDelta);
	myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetRight() * movement.x * mySpeed * aDelta);
}

void Movement::Rotate()
{
	myCursorPosition.x += static_cast<float>(CU::InputWrapper::GetInstance()->GetMouseDX()) * myRotationSpeed;
	myCursorPosition.y += static_cast<float>(CU::InputWrapper::GetInstance()->GetMouseDY()) * myRotationSpeed;

	myCursorPosition.y = fmaxf(fminf(3.1415f / 2.f, myCursorPosition.y), -3.1415f / 2.f);

	myPitch = CU::Quaternion(CU::Vector3<float>(1.f, 0, 0), myCursorPosition.y);
	myYaw = CU::Quaternion(CU::Vector3<float>(0, 1.f, 0), myCursorPosition.x);

	CU::Vector3<float> axisX(1.f, 0, 0);
	CU::Vector3<float> axisY(0, 1.f, 0);
	CU::Vector3<float> axisZ(0, 0, 1.f);

	axisX = myYaw * myPitch * axisX;
	axisY = myYaw * myPitch * axisY;
	axisZ = myYaw * myPitch * axisZ;

	myOrientation.myMatrix[0] = axisX.x;
	myOrientation.myMatrix[1] = axisX.y;
	myOrientation.myMatrix[2] = axisX.z;
	myOrientation.myMatrix[4] = axisY.x;
	myOrientation.myMatrix[5] = axisY.y;
	myOrientation.myMatrix[6] = axisY.z;
	myOrientation.myMatrix[8] = axisZ.x;
	myOrientation.myMatrix[9] = axisZ.y;
	myOrientation.myMatrix[10] = axisZ.z;
}