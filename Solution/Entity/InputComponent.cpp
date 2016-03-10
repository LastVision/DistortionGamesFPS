#include "stdafx.h"

#include <Camera.h>
#include "DamageNote.h"
#include "FirstPersonRenderComponent.h"
#include "InputComponent.h"
#include "InputComponentData.h"
#include <InputWrapper.h>
#include "NetworkComponent.h"
#include <NetworkSendPositionMessage.h>
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include "PhysicsComponent.h"
#include "ShootingComponent.h"
#include <XMLReader.h>

InputComponent::InputComponent(Entity& anEntity, const InputComponentData& aData)
	: Component(anEntity)
	, myNetworkID(0)
	, myPitch(CU::Vector3<float>(0, 1.f, 0), 0)
	, myYaw(CU::Vector3<float>(1.f, 0, 0), 0)
	, myData(aData)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_player.xml");

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "eyeHeight"), "value", myHeight);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "crouchHeight"), "value", myCrouchHeight);

	myOrientation.SetPos(CU::Vector3<float>(0, 1.5f, 0));
	myCamera = new Prism::Camera(myEyeOrientation);
	reader.CloseDocument();

	myJumpAcceleration = 0;
	myJumpOffset = 0;

	mySendTime = 3;

	//myCapsuleControllerId = Prism::PhysicsInterface::GetInstance()->CreatePlayerController(myOrientation.GetPos());
}

InputComponent::~InputComponent()
{
	SAFE_DELETE(myCamera);
}

void InputComponent::Update(float aDelta)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_H) == true)
	{
		myEntity.SendNote<DamageNote>(DamageNote(1));
	}

	CU::Vector3<float> prevPos(myOrientation.GetPos());

	UpdateMovement(aDelta);
	myEyeOrientation = myOrientation;

	CU::Vector3<float> position(myOrientation.GetPos());

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LCONTROL) == true)
	{
		position.y += myCrouchHeight;
	}
	else
	{
		position.y += myHeight;
	}

	myEyeOrientation.SetPos(position);

	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);


	mySendTime -= aDelta;
	if (mySendTime < 0.f)
	{
		if (myNetworkID != 0 && (myOrientation.GetPos().x != prevPos.x || myOrientation.GetPos().y != prevPos.y || myOrientation.GetPos().z != prevPos.z))
		{
			PostMaster::GetInstance()->SendMessage(NetworkSendPositionMessage(myOrientation.GetPos(), myCursorPosition.x, myNetworkID));
			mySendTime = 1 / 30.f;
		}
	}

	myCamera->Update(aDelta);
}

Prism::Camera* InputComponent::GetCamera() const
{
	return myCamera;
}

void InputComponent::UpdateMovement(float aDelta)
{
	myCursorPosition.x += static_cast<float>(CU::InputWrapper::GetInstance()->GetMouseDX()) * myData.myRotationSpeed;
	myCursorPosition.y += static_cast<float>(CU::InputWrapper::GetInstance()->GetMouseDY()) * myData.myRotationSpeed;

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

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE))
	{
		/*if (Prism::PhysicsInterface::GetInstance()->GetAllowedToJump(myCapsuleControllerId) == true)
		{*/
		myVerticalSpeed = 0.25f;
		//}
	}

	myVerticalSpeed -= aDelta;

	myVerticalSpeed = fmaxf(myVerticalSpeed, -0.5f);

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


	movement = movement * myOrientation;

	movement.y = 0;
	CU::Normalize(movement);
	movement *= myData.mySpeed;
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT))
	{
		movement *= myData.mySprintMultiplier;
	}
	movement.y = myVerticalSpeed;

	Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);

	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);

	myOrientation.SetPos(pos);
	CU::Vector3<float> forward = CU::Cross(myOrientation.GetRight(), CU::Vector3<float>(0.f, 1.f, 0.f));

	CU::Normalize(forward);

	myOrientation.SetPos(myOrientation.GetPos() + forward * movement.z * myData.mySpeed * aDelta);
	myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetRight() * movement.x * myData.mySpeed * aDelta);
}