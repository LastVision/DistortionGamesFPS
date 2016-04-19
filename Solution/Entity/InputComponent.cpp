#include "stdafx.h"

#include <AudioInterface.h>
#include <Camera.h>
#include "DamageNote.h"
#include "FirstPersonRenderComponent.h"
#include "HealthNote.h"
#include "HealthComponent.h"
#include "InputComponent.h"
#include "InputComponentData.h"
#include <InputWrapper.h>
#include <NetMessagePosition.h>
#include <NetMessagePressE.h>
#include <NetMessageEntityState.h>
#include "NetworkComponent.h"
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include "PhysicsComponent.h"
#include <SharedNetworkManager.h>
#include "ShootingComponent.h"
#include <XMLReader.h>

InputComponent::InputComponent(Entity& anEntity, const InputComponentData& aData)
	: Component(anEntity)
	, myPitch(CU::Vector3<float>(0, 1.f, 0), 0)
	, myYaw(CU::Vector3<float>(1.f, 0, 0), 0)
	, mySprintEnergy(0.f)
	, myEnergyOverheat(false)
	, myIsInOptionsMenu(false)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_player.xml");
	myData = &aData;
	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "eyeHeight"), "value", myHeight);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "crouchHeight"), "value", myCrouchHeight);

	myOrientation.SetPos(CU::Vector3<float>(0, 1.5f, 0));
	myCamera = new Prism::Camera(myEyeOrientation);
	reader.CloseDocument();

	myJumpAcceleration = 0;
	myJumpOffset = 0;

	mySendTime = 0.f;

	if (myEntity.GetIsClient() == true)
	{
		Prism::PhysicsInterface::GetInstance()->SetClientID(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId());
		Prism::PhysicsInterface::GetInstance()->SetPlayerOrientation(&myOrientation);
		Prism::PhysicsInterface::GetInstance()->SetPlayerInputData(*myData);
		Prism::PhysicsInterface::GetInstance()->SetPlayerGID(myEntity.GetGID());
	}

	myPreviousState = eEntityState::IDLE;
}

InputComponent::~InputComponent()
{
	SAFE_DELETE(myCamera);
}

void InputComponent::Update(float aDelta)
{
	if (myEntity.IsAlive() == false)
	{
		return;
	}

	if (myEntity.GetState() == eEntityState::DIE)
	{
		CU::Vector3<float> offset(0, 1.f, 0);
		myEyeOrientation.SetPos(myOrientation.GetPos() + offset);
	}

	myPrevOrientation = myOrientation;
	myPreviousState = myEntity.GetState();

	Prism::Audio::AudioInterface::GetInstance()->SetListenerPosition(myEyeOrientation.GetPos().x, myEyeOrientation.GetPos().y, myEyeOrientation.GetPos().z
		, myEyeOrientation.GetForward().x, myEyeOrientation.GetForward().y, myEyeOrientation.GetForward().z
		, myEyeOrientation.GetUp().x, myEyeOrientation.GetUp().y, myEyeOrientation.GetUp().z);
	if (myEntity.GetState() != eEntityState::DIE)
	{
		if (myIsInOptionsMenu == false)
		{
			UpdateMovement(aDelta);



			if (CU::InputWrapper::GetInstance()->KeyDown(DIK_E) == true)
			{
				//Prism::PhysicsInterface::GetInstance()->RayCast(myEntity.GetOrientation().GetPos()
				//	, myEntity.GetOrientation().GetForward(), 10.f, myRaycastHandler, myEntity.GetComponent<PhysicsComponent>());
				//SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(myEntity.GetOrientation().GetPos(), myEntity.GetOrientation().GetForward()
				//	, int(eNetRayCastType::CLIENT_PRESSED_E), 10.f, myEntity.GetGID()));
				SharedNetworkManager::GetInstance()->AddMessage(NetMessagePressE(myEntity.GetGID()));
			}

			if (GC::PlayerShouldPlaySprintErrorSound == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Error", 0);
				GC::PlayerShouldPlaySprintErrorSound = false;
			}

			if (GC::PlayerShouldPlaySprintSound == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Sprint", 0);
				GC::PlayerShouldPlaySprintSound = false;
			}
			if (GC::PlayerShouldStopSprintSound == true)
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_Sprint", 0);
				GC::PlayerShouldStopSprintSound = false;
			}

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
		}
	}

	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);


	mySendTime -= aDelta;	
	if (mySendTime < 0.f)
	{
		if (myEntity.GetGID() != 0 && (myOrientation != myPrevOrientation))
		{
			SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(myOrientation.GetPos(), myCursorPosition.x, myEntity.GetGID()));
			mySendTime = NETWORK_UPDATE_INTERVAL;
		}
	}

	if (myEntity.GetState() == eEntityState::DIE && myPreviousState != eEntityState::DIE)
	{
		CU::Vector3<float> diePosition = myEyeOrientation.GetPos();
		diePosition.y -= 1.f;
		myEyeOrientation.SetPos(diePosition);
		myOrientation = myEyeOrientation;

		SharedNetworkManager::GetInstance()->AddMessage(NetMessagePosition(diePosition, myCursorPosition.x, myEntity.GetGID()));
	}

	myCamera->Update(aDelta);
}

Prism::Camera* InputComponent::GetCamera() const
{
	return myCamera;
}

void InputComponent::UpdateMovement(float aDelta)
{
	myCursorPosition.x += static_cast<float>(CU::InputWrapper::GetInstance()->GetMouseDX()) * myData->myRotationSpeed;
	myCursorPosition.y += static_cast<float>(CU::InputWrapper::GetInstance()->GetMouseDY()) * myData->myRotationSpeed;

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

/*
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_SPACE))
	{
#ifdef RELEASE_BUILD
		if (Prism::PhysicsInterface::GetInstance()->GetAllowedToJump(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId()) == true)
#endif
		{
			myVerticalSpeed = 0.25f;
		}
	}

	myVerticalSpeed -= aDelta;

	myVerticalSpeed = fmaxf(myVerticalSpeed, -0.5f);

	CU::Vector3<float> movement;
	float magnitude = 0.f;
	int count = 0;*/

	/*if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_S))
	{
		movement.z -= 1.f;
		magnitude += myData->myBackwardMultiplier;
		++count;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_W))
	{
		movement.z += 1.f;
		magnitude += myData->myForwardMultiplier;
		++count;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_A))
	{
		movement.x -= 1.f;
		magnitude += myData->mySidewaysMultiplier;
		++count;
	}
	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_D))
	{
		movement.x += 1.f;
		magnitude += myData->mySidewaysMultiplier;
		++count;
	}

	if (count > 0)
	{
	magnitude /= count;
	}

	if (CU::Length(movement) < 0.02f)
	{
	if (myEntity.GetState() != eEntityState::IDLE)
	{
	myEntity.SetState(eEntityState::IDLE);
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
	}
	}
	else if (myEntity.GetState() != eEntityState::WALK)
	{
	myEntity.SetState(eEntityState::WALK);
	SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(myEntity.GetState(), myEntity.GetGID()));
	}*/
//
//	bool isSprinting = false;
//
//#ifdef RELEASE_BUILD
//	bool shouldDecreaseEnergy = true;
//	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT))
//	{
//		if (mySprintEnergy < myData->myMaxSprintEnergy && myEnergyOverheat == false)
//		{
//			mySprintEnergy += myData->mySprintIncrease * aDelta;
//			if (mySprintEnergy >= myData->myMaxSprintEnergy)
//			{
//				myEnergyOverheat = true;
//			}
//
//			if (movement.z > 0.f)
//			{
//				movement.z *= myData->mySprintMultiplier;
//				isSprinting = true;
//			}
//
//			shouldDecreaseEnergy = false;
//		}
//	}
//
//	if (shouldDecreaseEnergy == true)
//	{
//		mySprintEnergy -= myData->mySprintDecrease * aDelta;
//		mySprintEnergy = fmaxf(mySprintEnergy, 0.f);
//	}
//
//	if (myEnergyOverheat == true && mySprintEnergy <= 0.f)
//	{
//		myEnergyOverheat = false;
//	}
//#endif
//
//	movement = movement * myOrientation;
//
//	if (aDelta > 0.f)
//	{
//		if (CU::Length2(movement / aDelta) > 0.25f* 0.25f)
//		{
//			movement.y = 0;
//			CU::Normalize(movement);
//			movement *= myData->mySpeed * magnitude;
//
//		}
//		else
//		{
//			movement = CU::Vector3<float>();
//		}
//	}
//
//	
//#ifdef RELEASE_BUILD
//	if (isSprinting == true)
//	{
//		movement *= myData->mySprintMultiplier;
//	}
//#else
//	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LSHIFT))
//	{
//		movement *= 10.f;
//	}
//#endif
//
//	movement.y = myVerticalSpeed;
//
//	//Prism::PhysicsInterface::GetInstance()->Move(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), movement, 0.05f, aDelta);

	CU::Vector3<float> pos;
	Prism::PhysicsInterface::GetInstance()->GetPosition(myEntity.GetComponent<PhysicsComponent>()->GetCapsuleControllerId(), pos);

	myOrientation.SetPos(pos);
}