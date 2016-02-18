#include "stdafx.h"

#include <Camera.h>
#include <InputWrapper.h>
#include <Instance.h>
#include <ModelLoader.h>
#include "Movement.h"
#include "Player.h"
#include "Shooting.h"
#include <Scene.h>
#include <SpriteProxy.h>
#include <XMLReader.h>


Player::Player(Prism::Scene* aScene)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_player.xml");

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "eyeHeight"), "value", myHeight);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "crouchHeight"), "value", myCrouchHeight);
	CU::Vector3<float> eyePosition(0, myHeight, 0);

	tinyxml2::XMLElement* movementElement = reader.ForceFindFirstChild(element, "movement");

	myOrientation.SetPos(eyePosition);
	myCamera = new Prism::Camera(myOrientation);
	myMovement = new Movement(myOrientation, reader, movementElement);
	myShooting = new Shooting(aScene);

	reader.CloseDocument();
	CU::Vector2<float> size(128.f, 128.f);
	myCrosshair = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_crosshair.dds", size, size * 0.5f);

	myModel = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/SK_arm.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx"), myOrientation);

	aScene->AddInstance(myModel);

	myJumpAcceleration = 0;
	myJumpOffset = 0;
}


Player::~Player()
{
}

void Player::Update(float aDelta)
{
	myMovement->Update(aDelta);

	CU::Vector3<float> position(myOrientation.GetPos());

	myJumpAcceleration -= aDelta * 9.82f;

	myJumpOffset += myJumpAcceleration;

	myJumpOffset = fmaxf(0, myJumpOffset);

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_SPACE) == true)
	{
		myJumpAcceleration = 4.f;
	}

	if (CU::InputWrapper::GetInstance()->KeyIsPressed(DIK_LCONTROL) == true)
	{
		position.y = myCrouchHeight;
	}
	else
	{
		position.y = myHeight;
	}

	position.y += myJumpOffset;

	myOrientation.SetPos(position);

	myShooting->Update(aDelta, myOrientation);

	myModel->Update(aDelta);

	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);

	myCamera->Update(aDelta);
}

void Player::Render()
{
	const CU::Vector2<float>& windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myCrosshair->Render(windowSize * 0.5f);
}