#include "stdafx.h"

#include <AudioInterface.h>
#include "DamageNote.h"
#include "Entity.h"
#include <EmitterMessage.h>
#include <ModelLoader.h>
#include <NetMessageRayCastRequest.h>
#include <HitmarkerMessage.h>
#include <Instance.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include "Pistol.h"
#include <Scene.h>
#include <XMLReader.h>
#include <SharedNetworkManager.h>
#include <NetMessageOnHit.h>
#include <PostMaster.h>
#include <PrintTextMessage.h>
#include "SoundComponent.h"

Pistol::Pistol(Entity* aOwnerEntity)
	: Weapon(eWeaponType::PISTOL, aOwnerEntity)
	, myOrientation(nullptr)
	, myMuzzleflashTimer(0.f)
	, myCurrentMuzzleflash(0)
{
	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition, aHitNormal);
	};

	for (int i = 0; i < 5; ++i)
	{
		myMuzzleflash[i] = nullptr;
	}
}

Pistol::~Pistol()
{
	SAFE_DELETE(myMuzzleflash[0]);
	SAFE_DELETE(myMuzzleflash[1]);
	SAFE_DELETE(myMuzzleflash[2]);
	SAFE_DELETE(myMuzzleflash[3]);
	SAFE_DELETE(myMuzzleflash[4]);
}

void Pistol::Init(Prism::Scene* aScene, const CU::Matrix44<float>& aOrientation)
{
	myOrientation = &aOrientation;
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash0.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[0] = new Prism::Instance(*model, myMuzzleflashOrientation);
	aScene->AddInstance(myMuzzleflash[0], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash1.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[1] = new Prism::Instance(*model, myMuzzleflashOrientation);
	aScene->AddInstance(myMuzzleflash[1], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash2.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[2] = new Prism::Instance(*model, myMuzzleflashOrientation);
	aScene->AddInstance(myMuzzleflash[2], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash3.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[3] = new Prism::Instance(*model, myMuzzleflashOrientation);
	aScene->AddInstance(myMuzzleflash[3], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash4.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[4] = new Prism::Instance(*model, myMuzzleflashOrientation);
	aScene->AddInstance(myMuzzleflash[4], eObjectRoomType::ALWAYS_RENDER);
}

void Pistol::Init(std::string aWeaponSettingsPath, std::string aXMLTagName)
{
	Weapon::Init(aWeaponSettingsPath, aXMLTagName);

	XMLReader reader;
	reader.OpenDocument(aWeaponSettingsPath);
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* shotgunElement = reader.ForceFindFirstChild(root, "pistol");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "minspreadrotation"), "value", myMinSpreadRotation);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "maxspreadrotation"), "value", myMaxSpreadRotation);

	reader.CloseDocument();

	myAmmoInClip = myClipSize;
	myAmmoTotal = 10;
	myShootTimer = myShootTime;
}

bool Pistol::Shoot(const CU::Matrix44<float>& aOrientation)
{
	//if (myAmmoInClip > 0 && myShootTimer <= 0.f)
	{
		CU::Vector3<float> forward = CU::Vector3<float>(0, 0, -1.f)
			* (CU::Matrix44<float>::CreateRotateAroundX(CU::Math::RandomRange(myMinSpreadRotation, myMaxSpreadRotation))
			* aOrientation);
		forward = forward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(myMinSpreadRotation, myMaxSpreadRotation));

		Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
			, forward, 500.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());
		myAmmoInClip -= 1;
		myShootTimer = myShootTime;
		myMuzzleflashTimer = 0.2f;
		myMuzzleflash[myCurrentMuzzleflash]->SetShouldRender(true);
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Pistol", 0);
		//SendRayCastRequest(aOrientation.GetPos(), forward, 500.f, myOwnerEntity->GetGID());
		SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(aOrientation.GetPos()
			, forward, int(eNetRayCastType::CLIENT_SHOOT_PISTOL), 500.f, myOwnerEntity->GetGID()));
		return true;
	}
	/*else if (myShootTimer <= 0.f)
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_NoAmmo", 0);
		myShootTimer = myShootTime;
		if (myAmmoTotal == 0)
		{
			PostMaster::GetInstance()->SendMessage(PrintTextMessage("No ammo", 1.f, { 0.7f, 0.2f, 0.2f, 1.f }));
		}
		else
		{
			PostMaster::GetInstance()->SendMessage(PrintTextMessage("Clip empty", 1.f, { 0.7f, 0.2f, 0.2f, 1.f }));
		}

	}*/
	return false;
}

void Pistol::Reload()
{
	myAmmoInClip = myClipSize;
}

void Pistol::Update(float aDelta)
{
	CU::Vector3<float> offset(0.08227f, 0.02079f, -0.09182f);

	/*
	CU::InputWrapper::GetInstance()->TweakValue(offset.x, 0.01, aDelta, DIK_7, DIK_8);
	CU::InputWrapper::GetInstance()->TweakValue(offset.y, 0.01, aDelta, DIK_9, DIK_0);
	CU::InputWrapper::GetInstance()->TweakValue(offset.z, 0.01, aDelta, DIK_I, DIK_O);

	DEBUG_PRINT(offset);
	*/
	myMuzzleflashOrientation = *myOrientation;
	myMuzzleflashOrientation.SetPos(myMuzzleflashOrientation.GetPos() + offset * myMuzzleflashOrientation);
	for (int i = 0; i < 5; ++i)
	{
		myMuzzleflash[i]->SetShouldRender(false);
	}
	myShootTimer -= aDelta;
	myMuzzleflashTimer -= aDelta;
	if (myMuzzleflashTimer > 0.f)
	{
		int prev = myCurrentMuzzleflash;
		myCurrentMuzzleflash = rand() % 5;
		if (prev == myCurrentMuzzleflash)
		{
			++myCurrentMuzzleflash;
			if (myCurrentMuzzleflash > 4)
			{
				myCurrentMuzzleflash = 0;
			}
		}
		myMuzzleflash[myCurrentMuzzleflash]->SetShouldRender(true);
	}
}

void Pistol::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
{
	if (aComponent != nullptr)
	{
		if (aComponent->GetEntity().GetComponent<SoundComponent>() != nullptr)
		{
			if (aComponent->GetEntity().GetType() == eEntityType::UNIT && aComponent->GetEntity().GetSubType() != "player")
			{
				Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EnemyTakeDamage", aComponent->GetEntity().GetComponent<SoundComponent>()->GetAudioSFXID());
			}
		}
#ifdef RELEASE_BUILD
		if (aComponent->GetEntity().GetSubType() != "player")
#endif
		{
			if (aComponent->GetPhysicsType() == ePhysics::DYNAMIC)
			{
				aComponent->AddForce(aDirection, myForceStrength);
			}

			CU::Vector3<float> toSend = CU::Reflect<float>(aDirection, aHitNormal);

			if (aComponent->GetEntity().GetIsEnemy() == true)
			{
				PostMaster::GetInstance()->SendMessage<HitmarkerMessage>(HitmarkerMessage());
				PostMaster::GetInstance()->SendMessage(EmitterMessage("OnHit", aHitPosition));
			}
			else
			{
				CU::Vector3<float> toMove = aHitPosition;
				toMove += (aHitNormal * 0.1f);
				PostMaster::GetInstance()->SendMessage(EmitterMessage("OnEnvHit", toMove));
				PostMaster::GetInstance()->SendMessage(EmitterMessage("OnEnvHit_2", toMove));
			}

			//aComponent->GetEntity().SendNote<DamageNote>(DamageNote(myDamage));

			//SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(float(myDamage), aComponent->GetEntity().GetGID()));

			SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(myDamage, aComponent->GetEntity().GetGID()));
		}
	}
}

void Pistol::Upgrade(const UpgradeComponentData& aData)
{
	Weapon::Upgrade(aData);
	myMinSpreadRotation += aData.myMinSpreadRotation;
	myMaxSpreadRotation += aData.myMaxSpreadRotation;
}