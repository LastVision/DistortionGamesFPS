#include "stdafx.h"

#include <Camera.h>
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

	CU::Vector3<float> eyePosition;
	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "eyeHeight"), "value", eyePosition.y);

	tinyxml2::XMLElement* movementElement = reader.ForceFindFirstChild(element, "movement");

	myOrientation.SetPos(eyePosition);
	myCamera = new Prism::Camera(myOrientation);
	myMovement = new Movement(myOrientation, reader, movementElement);
	myShooting = new Shooting();

	reader.CloseDocument();
	CU::Vector2<float> size(128.f, 128.f);
	myCrosshair = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_crosshair.dds", size, size * 0.5f);

	myModel = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/First_person/SK_arm.fbx", "Data/Resource/Shader/S_effect_pbldebug.fx"), myArmOrientation);

	aScene->AddInstance(myModel);
}


Player::~Player()
{
}

void Player::Update(float aDelta)
{
	myMovement->Update(aDelta);
	myShooting->Update(aDelta, myOrientation);

	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);

	myCamera->Update(aDelta);

	myShooting->Render(*myCamera);
}

void Player::Render()
{
	const CU::Vector2<float>& windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myCrosshair->Render(windowSize * 0.5f);
}