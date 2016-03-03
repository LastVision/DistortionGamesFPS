#include "stdafx.h"

#include <AnimationSystem.h>
#include <Camera.h>
#include "ClientNetworkManager.h"
#include <GUIManager3D.h>
#include "Health.h"
#include <InputWrapper.h>
#include <Instance.h>
#include <ModelLoader.h>
#include "Movement.h"
#include <NetMessagePosition.h>
#include "Player.h"
#include "Shooting.h"
#include <Scene.h>
#include <SpriteProxy.h>
#include <XMLReader.h>

Player::Player(Prism::Scene* aScene)
	: myAlive(true)
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
	myHealth = new Health(reader, healthElement);
	myShooting = new Shooting(aScene, this);
	mySpawnPosition = myOrientation.GetPos();
	reader.CloseDocument();
	CU::Vector2<float> size(128.f, 128.f);
	myCrosshair = Prism::ModelLoader::GetInstance()->LoadSprite("Data/Resource/Texture/UI/T_crosshair.dds", size, size * 0.5f);

	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModelAnimated("Data/Resource/Model/First_person/SK_arm_pistol_idle.fbx", "Data/Resource/Shader/S_effect_pbl_animated.fx");
	myModel = new Prism::Instance(*model, myEyeOrientation);
	aScene->AddInstance(myModel, true);

	AddAnimation(ePlayerState::PISTOL_IDLE, "Data/Resource/Model/First_person/SK_arm_pistol_idle.fbx", true, true);
	AddAnimation(ePlayerState::PISTOL_HOLSTER, "Data/Resource/Model/First_person/SK_arm_pistol_holster.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_DRAW, "Data/Resource/Model/First_person/SK_arm_pistol_draw.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_RELOAD, "Data/Resource/Model/First_person/SK_arm_pistol_reload.fbx", false, true);
	AddAnimation(ePlayerState::PISTOL_SHOOT, "Data/Resource/Model/First_person/SK_arm_pistol_fire.fbx", false, true);
	

	myJumpAcceleration = 0;
	myJumpOffset = 0;

	myModel->Update(1.f / 30.f);
	mySendTime = 3;

	while (Prism::ModelLoader::GetInstance()->IsLoading() == true)
	{

	}
	PlayAnimation(ePlayerState::PISTOL_IDLE);
	//myWristOrientation = myOrientation * myModel->GetCurrentAnimation()->GetHiearchyToBone("r_wrist_jnt1");

	my3DGUIManager = new GUI::GUIManager3D(myModel, aScene
		, myShooting->GetWeapon(eWeaponType::PISTOL)->GetClipSize(), myShooting->GetWeapon(eWeaponType::PISTOL)->GetAmmoInClip()
		, myShooting->GetWeapon(eWeaponType::SHOTGUN)->GetClipSize(), myShooting->GetWeapon(eWeaponType::SHOTGUN)->GetAmmoInClip()
		, myShooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetClipSize(), myShooting->GetWeapon(eWeaponType::GRENADE_LAUNCHER)->GetAmmoInClip());

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
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_H) == true)
	{
		myAlive = myHealth->TakeDamage(5);
	}

	if (myAlive == false)
	{
		Respawn();
	}

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
	
	Prism::AnimationData& data = myAnimations[int(myPlayerState)];
	if (myModel->IsAnimationDone() == false || data.myShouldLoop == true)
	{
		myModel->Update(aDelta);
	}
	if (myModel->IsAnimationDone() == true && data.myShouldLoop == false)
	{
		eWeaponType weaponType = myShooting->GetCurrentWeapon()->GetWeaponType();
		switch (weaponType)
		{
		case eWeaponType::PISTOL:
			myPlayerState = ePlayerState::PISTOL_IDLE;
			PlayAnimation(myPlayerState);
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			break;
		case eWeaponType::SHOTGUN:
			break;
		}
	}
	data.myElapsedTime += aDelta;

	my3DGUIManager->Update(myEyeOrientation, myHealth->GetCurrentHealth(), myHealth->GetMaxHealth(), aDelta);

	CU::Vector3<float> playerPos(myOrientation.GetPos());
	DEBUG_PRINT(playerPos);


	mySendTime -= aDelta;
	if (mySendTime < 0.f)
	{
		if (myOrientation.GetPos().x != prevPos.x || myOrientation.GetPos().y != prevPos.y || myOrientation.GetPos().z != prevPos.z)
		{
			ClientNetworkManager::GetInstance()->AddMessage(NetMessagePosition(myOrientation.GetPos()));
			mySendTime = 1 / 30.f;
		}
	}

	myCamera->Update(aDelta);
}

void Player::Render()
{
	const CU::Vector2<float>& windowSize = Prism::Engine::GetInstance()->GetWindowSize();
	myCrosshair->Render(windowSize * 0.5f);
	my3DGUIManager->Render();
}

void Player::Respawn()
{
	myMovement->SetPosition(mySpawnPosition);
	myHealth->Reset();
	myAlive = true;
}

bool Player::IsCurrentAnimationDone() const
{
	return myModel->IsAnimationDone();
}

void Player::RestartCurrentAnimation()
{
	myModel->ResetAnimationTime(0.f);
}

void Player::AddAnimation(ePlayerState aState, const std::string& aAnimationPath
	, bool aLoopFlag, bool aResetTimeOnRestart)
{
	Prism::ModelLoader::GetInstance()->LoadModelAnimated(aAnimationPath, "Data/Resource/Shader/S_effect_pbl_animated.fx");
	//Prism::AnimationSystem::GetInstance()->GetAnimation(aAnimationPath.c_str());
	Prism::AnimationData newData;
	newData.myElapsedTime = 0.f;
	newData.myFile = aAnimationPath;
	newData.myShouldLoop = aLoopFlag;
	newData.myResetTimeOnRestart = aResetTimeOnRestart;
	myAnimations[int(aState)] = newData;
}

void Player::PlayAnimation(ePlayerState aAnimationState)
{
	myPlayerState = aAnimationState;
	Prism::AnimationData& data = myAnimations[int(aAnimationState)];
	myModel->SetAnimation(Prism::AnimationSystem::GetInstance()->GetAnimation(data.myFile.c_str()));

	if (data.myResetTimeOnRestart == true)
	{
		myModel->ResetAnimationTime(0.f);
	}
	else
	{
		myModel->ResetAnimationTime(data.myElapsedTime);
	}
}