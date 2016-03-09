#include "stdafx.h"

#include <Camera.h>
#include "DamageNote.h"
//#include <ClientNetworkManager.h>
#include "FirstPersonRenderComponent.h"
#include "InputComponent.h"
#include <InputWrapper.h>
#include "Movement.h"
//#include <NetMessagePosition.h>
#include "ShootingComponent.h"
#include <XMLReader.h>

InputComponent::InputComponent(Entity& anEntity)
	: myAlive(true)
	, Component(anEntity)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_player.xml");

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "eyeHeight"), "value", myHeight);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "crouchHeight"), "value", myCrouchHeight);

	tinyxml2::XMLElement* movementElement = reader.ForceFindFirstChild(element, "movement");

	myOrientation.SetPos(CU::Vector3<float>(0, 1.5f, 0));
	myCamera = new Prism::Camera(myEyeOrientation);
	myMovement = new Movement(myOrientation, reader, movementElement);
	mySpawnPosition = myOrientation.GetPos();
	reader.CloseDocument();
	

	myJumpAcceleration = 0;
	myJumpOffset = 0;

	mySendTime = 3;
}


InputComponent::~InputComponent()
{
	SAFE_DELETE(myCamera);
	SAFE_DELETE(myMovement);
}

void InputComponent::Update(float aDelta)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_H) == true)
	{
		myEntity.SendNote<DamageNote>(DamageNote(1));
		//myAlive = myHealth->TakeDamage(5);
	}

	//if (myAlive == false)
	//{
	//	Respawn();
	//}

	CU::Vector3<float> prevPos(myOrientation.GetPos());

	myMovement->Update(aDelta);
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

	//myShooting->Update(aDelta, myEyeOrientation);
	
	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);


	mySendTime -= aDelta;
	if (mySendTime < 0.f)
	{
		if (myOrientation.GetPos().x != prevPos.x || myOrientation.GetPos().y != prevPos.y || myOrientation.GetPos().z != prevPos.z)
		{
		//	ClientNetworkManager::GetInstance()->AddMessage(NetMessagePosition(myOrientation.GetPos(), ClientNetworkManager::GetInstance()->GetNetworkID()));
			mySendTime = 1 / 30.f;
		}
	}

	myCamera->Update(aDelta);
}

void InputComponent::Render()
{

}

void InputComponent::Respawn()
{
	myMovement->SetPosition(mySpawnPosition);
	myAlive = true;
}

Prism::Camera* InputComponent::GetCamera() const
{
	return myCamera;
}