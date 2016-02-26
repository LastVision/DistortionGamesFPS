#include "stdafx.h"


#include <Camera.h>
#include <GUIManager3D.h>
#include "Health.h"
#include <InputWrapper.h>
#include <Instance.h>
#include <ModelLoader.h>
#include "Movement.h"
#include "Player.h"
#include "Shooting.h"
#include <Scene.h>
#include <SpriteProxy.h>
#include <XMLReader.h>
#include <NetMessagePosition.h>
Player::Player(Prism::Scene* aScene)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_player.xml");

	tinyxml2::XMLElement* element = reader.ForceFindFirstChild("root");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "eyeHeight"), "value", myHeight);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(element, "crouchHeight"), "value", myCrouchHeight);

	tinyxml2::XMLElement* movementElement = reader.ForceFindFirstChild(element, "movement");
	tinyxml2::XMLElement* healthElement = reader.ForceFindFirstChild(element, "health");

	myOrientation.SetPos(CU::Vector3<float>(0, 1.5f, 0));
	myCamera = new Prism::Camera(myEyeOrientation);
	myMovement = new Movement(myOrientation, reader, movementElement);
	myHealth = new Health(reader, movementElement);
	myShooting = new Shooting(aScene);

	reader.CloseDocument();
	CU::Vector2<float> size(128.f, 128.f);
	myCrosshair = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_crosshair.dds", size, size * 0.5f);

	myModel = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/SK_arm_idle_UI_cm.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx"), myEyeOrientation);

	aScene->AddInstance(myModel);

	myJumpAcceleration = 0;
	myJumpOffset = 0;

	myModel->Update(1.f / 30.f);
	mySendTime = 3;


	//myWristOrientation = myOrientation * myModel->GetCurrentAnimation()->GetHiearchyToBone("r_wrist_jnt1");

	my3DGUIManager = new GUI::GUIManager3D(myModel, aScene);

}


Player::~Player()
{
	SAFE_DELETE(my3DGUIManager);
	SAFE_DELETE(myModel);
	SAFE_DELETE(myCamera);
	SAFE_DELETE(myMovement);
	SAFE_DELETE(myShooting);
	SAFE_DELETE(myHealth);
	SAFE_DELETE(myCrosshair);
}

void Player::Update(float aDelta)
{
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

	myShooting->Update(aDelta, myEyeOrientation);

	myModel->Update(aDelta);
	my3DGUIManager->Update(myEyeOrientation, aDelta);

	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);


	mySendTime -= aDelta;
	//if (mySendTime < 0.f)
	//{
	//	if (myOrientation.GetPos().x != prevPos.x || myOrientation.GetPos().y != prevPos.y || myOrientation.GetPos().z != prevPos.z)
	//	{
	//		NetMessagePosition pos;
	//		pos.Init();
	//		pos.mySenderID = NetworkManager::GetInstance()->GetNetworkID();
	//		pos.myPosition = myOrientation.GetPos();
	//		pos.PackMessage();
	//		NetworkManager::GetInstance()->AddMessage(pos.myStream);
	//		mySendTime = 1/120.f;
	//	}
	//}

	myCamera->Update(aDelta);
}

void Player::Render()
{
	const CU::Vector2<float>& windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myCrosshair->Render(windowSize * 0.5f);
	my3DGUIManager->Render();
}