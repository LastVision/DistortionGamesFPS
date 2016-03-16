#include "stdafx.h"

#include "DamageNote.h"
#include "Entity.h"
#include <EmitterMessage.h>
#include <ModelLoader.h>
#include <Instance.h>
#include <PostMasterNetOnHitMessage.h>
#include "PhysicsComponent.h"
#include <PostMaster.h>
#include <PhysicsInterface.h>
#include "Pistol.h"
#include <Scene.h>
#include <XMLReader.h>

Pistol::Pistol()
	: Weapon(eWeaponType::PISTOL)
	, myOrientation(nullptr)
	, myMuzzleflashTimer(0.f)
	, myCurrentMuzzleflash(0)
{
	XMLReader reader;
	reader.OpenDocument("Data/Setting/SET_weapons.xml");
	tinyxml2::XMLElement* root = reader.ForceFindFirstChild("root");

	tinyxml2::XMLElement* pistolElement = reader.ForceFindFirstChild(root, "pistol");

	reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "clipsize"), "value", myClipSize);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "damage"), "value", myDamage);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "shoottime"), "value", myShootTime);
	reader.ForceReadAttribute(reader.ForceFindFirstChild(pistolElement, "forceStrength"), "value", myForceStrength);

	myAmmoInClip = myClipSize;
	myAmmoTotal = INT_MAX;
	myShootTimer = myShootTime;

	reader.CloseDocument();

	myRaycastHandler = [=](PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition)
	{
		this->HandleRaycast(aComponent, aDirection, aHitPosition);
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
	myMuzzleflash[0] = new Prism::Instance(*model, *myOrientation);
	aScene->AddInstance(myMuzzleflash[0], true);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash1.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[1] = new Prism::Instance(*model, *myOrientation);
	aScene->AddInstance(myMuzzleflash[1], true);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash2.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[2] = new Prism::Instance(*model, *myOrientation);
	aScene->AddInstance(myMuzzleflash[2], true);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash3.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[3] = new Prism::Instance(*model, *myOrientation);
	aScene->AddInstance(myMuzzleflash[3], true);

	model = Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Muzzleflash/SM_muzzleflash4.fbx"
		, "Data/Resource/Shader/S_effect_pbl_deferred.fx");
	myMuzzleflash[4] = new Prism::Instance(*model, *myOrientation);
	aScene->AddInstance(myMuzzleflash[4], true);
}


bool Pistol::Shoot(const CU::Matrix44<float>& aOrientation)
{
	if (myAmmoInClip > 0 && myShootTimer <= 0.f)
	{
		Prism::PhysicsInterface::GetInstance()->RayCast(aOrientation.GetPos()
			, aOrientation.GetForward(), 500.f, myRaycastHandler);
		myAmmoInClip -= 1;
		myShootTimer = myShootTime;
		myMuzzleflashTimer = 0.2f;
		myMuzzleflash[myCurrentMuzzleflash]->SetShouldRender(true);
		return true;
	}
	return false;
}

void Pistol::Reload()
{
	myAmmoInClip = myClipSize;
}

void Pistol::Update(float aDelta)
{
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

void Pistol::Render()
{
	//myMuzzleflash->Render()
}

void Pistol::HandleRaycast(PhysicsComponent* aComponent, const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aHitPosition)
{
	if (aComponent != nullptr)
	{
		if (aComponent->GetPhysicsType() == ePhysics::DYNAMIC)
		{
			aComponent->AddForce(aDirection, myForceStrength);
		}
		PostMaster::GetInstance()->SendMessage(EmitterMessage("Shotgun", aHitPosition));
		//aComponent->GetEntity().SendNote<DamageNote>(DamageNote(myDamage));

		PostMaster::GetInstance()->SendMessage(PostMasterNetOnHitMessage(static_cast<float>(myDamage), aComponent->GetEntity().GetGID()));
	}
}