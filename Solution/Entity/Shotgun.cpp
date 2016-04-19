#include "stdafx.h"

#include <InputWrapper.h>

#include <AudioInterface.h>
#include "DamageNote.h"
#include "Entity.h"
#include <EmitterMessage.h>
#include <Instance.h>
#include <ModelLoader.h>
#include <ModelProxy.h>
#include <NetMessageOnHit.h>
#include <NetMessageRayCastRequest.h>
#include <HitmarkerMessage.h>
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include <PrintTextMessage.h>
#include <Scene.h>
#include "Shotgun.h"
#include <SharedNetworkManager.h>
#include "SoundComponent.h"
#include <XMLReader.h>

Shotgun::Shotgun(Entity* aOwnerEntity)
	: Weapon(eWeaponType::SHOTGUN, aOwnerEntity)
	, myOrientation(nullptr)
	, myMuzzleflashTimer(0.f)
	, myCurrentMuzzleflash1(2)
	, myCurrentMuzzleflash2(0)
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

Shotgun::~Shotgun()
{
	SAFE_DELETE(myMuzzleflash[0]);
	SAFE_DELETE(myMuzzleflash[1]);
	SAFE_DELETE(myMuzzleflash[2]);
	SAFE_DELETE(myMuzzleflash[3]);
	SAFE_DELETE(myMuzzleflash[4]);
}

void Shotgun::Init(Prism::Scene* aScene, const CU::Matrix44<float>& aOrientation)
{
	myOrientation = &aOrientation;
	Prism::ModelProxy* model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash_shotgun0.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[0] = new Prism::Instance(*model, myMuzzleflashOrientation2);
	aScene->AddInstance(myMuzzleflash[0], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash_shotgun1.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[1] = new Prism::Instance(*model, myMuzzleflashOrientation2);
	aScene->AddInstance(myMuzzleflash[1], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash_shotgun2.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[2] = new Prism::Instance(*model, myMuzzleflashOrientation1);
	aScene->AddInstance(myMuzzleflash[2], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash_shotgun3.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[3] = new Prism::Instance(*model, myMuzzleflashOrientation1);
	aScene->AddInstance(myMuzzleflash[3], eObjectRoomType::ALWAYS_RENDER);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash_shotgun4.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[4] = new Prism::Instance(*model, myMuzzleflashOrientation1);
	aScene->AddInstance(myMuzzleflash[4], eObjectRoomType::ALWAYS_RENDER);
}

void Shotgun::Init(std::string aWeaponSettingsPath, std::string aXMLTagName)
{
	Weapon::Init(aWeaponSettingsPath, aXMLTagName);

	XMLReader reader;
	reader.OpenDocument(aWeaponSettingsPath);
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* shotgunElement = reader.ForceFindFirstChild(root, "shotgun");
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "startammo"), "value", myAmmoTotal);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "minspreadrotation"), "value", myMinSpreadRotation);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(shotgunElement, "maxspreadrotation"), "value", myMaxSpreadRotation);

	reader.CloseDocument();
}

bool Shotgun::Shoot(const CU::Matrix44<float>& aOrientation)
{
	if (myAmmoInClip > 0 && myShootTimer <= 0.f)
	{
		ShootRowAround(aOrientation, CU::Vector3<float>(0, 0, 1.f) * (CU::Matrix44<float>::CreateRotateAroundX(CU::Math::RandomRange(-myMaxSpreadRotation, -myMinSpreadRotation)) * aOrientation));
		ShootRowAround(aOrientation, aOrientation.GetForward());
		ShootRowAround(aOrientation, CU::Vector3<float>(0, 0, 1.f) * (CU::Matrix44<float>::CreateRotateAroundX(CU::Math::RandomRange(myMinSpreadRotation, myMaxSpreadRotation)) * aOrientation));
		myAmmoInClip -= 1;
		myShootTimer = myShootTime;
		myMuzzleflashTimer = 0.2f;
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_Shotgun", 0);
		return true;
	}
	else if (myShootTimer <= 0.f)
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
	}
	return false;
}

void Shotgun::Reload()
{
	int ammoLeft = myAmmoTotal;
	int toPutInWeapon = myClipSize - myAmmoInClip;
	int allowedToPutIn = min(toPutInWeapon, myAmmoTotal);

	myAmmoInClip += allowedToPutIn;
	myAmmoTotal -= allowedToPutIn;
	//int ammoLeft = myAmmoTotal;
	//myAmmoTotal -= min(ammoLeft, myClipSize - myAmmoInClip);
	//myAmmoInClip = min(myClipSize, myAmmoTotal);
}

void Shotgun::Update(float aDelta)
{
	CU::Vector3<float> offset(0.08227f, 0.02079f, -0.09182f);

	//CU::InputWrapper::GetInstance()->TweakValue(offset.x, 0.01, aDelta, DIK_7, DIK_8);
	//CU::InputWrapper::GetInstance()->TweakValue(offset.y, 0.01, aDelta, DIK_9, DIK_0);
	//CU::InputWrapper::GetInstance()->TweakValue(offset.z, 0.01, aDelta, DIK_I, DIK_O);

	//DEBUG_PRINT(offset);

	myMuzzleflashOrientation1 = *myOrientation;
	myMuzzleflashOrientation1.SetPos(myMuzzleflashOrientation1.GetPos() + offset * myMuzzleflashOrientation1);


	CU::Vector3<float> offset2(0.1537f, 0.02079f, -0.09182f);

	//CU::InputWrapper::GetInstance()->TweakValue(offset2.x, 0.01, aDelta, DIK_7, DIK_8);
	//CU::InputWrapper::GetInstance()->TweakValue(offset2.y, 0.01, aDelta, DIK_9, DIK_0);
	//CU::InputWrapper::GetInstance()->TweakValue(offset2.z, 0.01, aDelta, DIK_I, DIK_O);

	//DEBUG_PRINT(offset2);

	myMuzzleflashOrientation2 = *myOrientation;
	myMuzzleflashOrientation2.SetPos(myMuzzleflashOrientation2.GetPos() + offset2 * myMuzzleflashOrientation2);

	myShootTimer -= aDelta;

	for (int i = 0; i < 5; ++i)
	{
		myMuzzleflash[i]->SetShouldRender(false);
	}
	myShootTimer -= aDelta;
	myMuzzleflashTimer -= aDelta;
	if (myMuzzleflashTimer > 0.f)
	{
		int prev = myCurrentMuzzleflash1;
		myCurrentMuzzleflash1 = 2 + rand() % 3;
		if (prev == myCurrentMuzzleflash1)
		{
			++myCurrentMuzzleflash1;
			if (myCurrentMuzzleflash1 > 4)
			{
				myCurrentMuzzleflash1 = 2;
			}
		}
		myMuzzleflash[myCurrentMuzzleflash1]->SetShouldRender(true);

		prev = myCurrentMuzzleflash2;
		myCurrentMuzzleflash2 = rand() % 2;
		if (prev == myCurrentMuzzleflash2)
		{
			++myCurrentMuzzleflash2;
			if (myCurrentMuzzleflash2 > 1)
			{
				myCurrentMuzzleflash2 = 0;
			}
		}
		myMuzzleflash[myCurrentMuzzleflash2]->SetShouldRender(true);
	}
}

void Shotgun::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition, const CU::Vector3<float>& aHitNormal)
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
			SharedNetworkManager::GetInstance()->AddMessage(NetMessageOnHit(myDamage, aComponent->GetEntity().GetGID()));
			CU::Vector3<float> toSend = CU::Reflect(aDirection, aHitNormal);

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
		}
	}
}

void Shotgun::Upgrade(const UpgradeComponentData& aData)
{
	Weapon::Upgrade(aData);
	myMinSpreadRotation += aData.myMinSpreadRotation;
	myMaxSpreadRotation += aData.myMaxSpreadRotation;
}

void Shotgun::ShootRowAround(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aForward)
{
	CU::Vector3<float> forward1 = aForward;
	CU::Vector3<float> forward2 = aForward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(-myMaxSpreadRotation, -myMinSpreadRotation));
	CU::Vector3<float> forward3 = aForward * CU::Matrix44<float>::CreateRotateAroundY(CU::Math::RandomRange(myMinSpreadRotation, myMaxSpreadRotation));

	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos(), forward1, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos(), forward2, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());
	Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos(), forward3, 100.f, myRaycastHandler, myOwnerEntity->GetComponent<PhysicsComponent>());

	SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(aOrientation.GetPos(), forward1, int(eNetRayCastType::CLIENT_SHOOT_SHOTGUN), 500.f, myOwnerEntity->GetGID()));
	SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(aOrientation.GetPos(), forward2, int(eNetRayCastType::CLIENT_SHOOT_SHOTGUN), 500.f, myOwnerEntity->GetGID()));
	SharedNetworkManager::GetInstance()->AddMessage(NetMessageRayCastRequest(aOrientation.GetPos(), forward3, int(eNetRayCastType::CLIENT_SHOOT_SHOTGUN), 500.f, myOwnerEntity->GetGID()));
}
