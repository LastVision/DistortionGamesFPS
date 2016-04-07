#include "stdafx.h"

#include "FirstPersonRenderComponent.h"
#include "GrenadeLauncher.h"
#include <InputWrapper.h>
#include "InputComponent.h"
#include <NetMessageEntityState.h>
#include "Pistol.h"
#include <SharedNetworkManager.h>
#include "Shotgun.h"
#include "ShootingComponent.h"
#include "UpgradeComponentData.h"
#include "UpgradeNote.h"

ShootingComponent::ShootingComponent(Entity& anEntity, Prism::Scene* aScene)
	: Component(anEntity)
	, myCurrentWeapon(nullptr)
	, myPistol(nullptr)
	, myShotgun(nullptr)
	, myGrenadeLauncher(nullptr)
	, myIsInOptionsMenu(false)
{

	myPistol = new Pistol(&myEntity);
	myShotgun = new Shotgun(&myEntity);
	myGrenadeLauncher = new GrenadeLauncher(aScene, myEntity.GetGID(), &myEntity);
	myCurrentWeapon = myPistol;
}

ShootingComponent::~ShootingComponent()
{
	SAFE_DELETE(myPistol);
	SAFE_DELETE(myShotgun);
	SAFE_DELETE(myGrenadeLauncher);
}

void ShootingComponent::Init(Prism::Scene* aScene)
{
	myPistol->Init(aScene, myEntity.GetComponent<FirstPersonRenderComponent>()->GetMuzzleJointOrientation());
}

void ShootingComponent::ReadXMLSettings(const std::string& aXMLPath)
{
	myPistol->Init(aXMLPath, "pistol");
	myShotgun->Init(aXMLPath, "shotgun");
	myGrenadeLauncher->Init(aXMLPath, "grenadelauncher");
}

void ShootingComponent::Update(float aDelta)
{
	if (myIsInOptionsMenu == false && myEntity.GetState() != eEntityState::DIE)
	{
		CU::Matrix44<float> aOrientation = myEntity.GetComponent<InputComponent>()->GetEyeOrientation();
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1) == true)
		{
			if (myCurrentWeapon != myPistol)
			{
				switch (myCurrentWeapon->GetWeaponType())
				{
				case eWeaponType::SHOTGUN:
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_HOLSTER, true);
					break;
				case eWeaponType::GRENADE_LAUNCHER:
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_HOLSTER, false);
					break;
				}
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_DRAW, false);
				myCurrentWeapon = myPistol;
			}
		}
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_2) == true)
		{
			if (myCurrentWeapon != myShotgun)
			{
				switch (myCurrentWeapon->GetWeaponType())
				{
				case eWeaponType::PISTOL:
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_HOLSTER, true);
					break;
				case eWeaponType::GRENADE_LAUNCHER:
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_HOLSTER, false);
					break;
				}
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_DRAW, false);
				myCurrentWeapon = myShotgun;
			}
		}
		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_3) == true)
		{
			if (myCurrentWeapon != myGrenadeLauncher)
			{
				switch (myCurrentWeapon->GetWeaponType())
				{
				case eWeaponType::PISTOL:
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_HOLSTER, true);
					break;
				case eWeaponType::SHOTGUN:
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_HOLSTER, false);
					break;
				}
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_DRAW, false);
				myCurrentWeapon = myGrenadeLauncher;
			}
		}

		if (CU::InputWrapper::GetInstance()->KeyDown(DIK_R) == true)
		{
			ReloadWeaponIntention();
		}

		if (CU::InputWrapper::GetInstance()->MouseDown(0) == true && myCurrentWeapon->GetAmmoInClip() == 0)
		{
			ReloadWeaponIntention();
		}
		else if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) == true)
		{
			if (myCurrentWeapon == myPistol)
			{
				if (myCurrentWeapon->Shoot(aOrientation) == true)
				{
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_FIRE, true);
					SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(eEntityState::ATTACK, myEntity.GetGID()));
				}
			}
			else if (myCurrentWeapon == myShotgun)
			{
				if (myCurrentWeapon->Shoot(aOrientation) == true)
				{
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_FIRE, true);
					SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(eEntityState::ATTACK, myEntity.GetGID()));
				}
			}
			else if (myCurrentWeapon == myGrenadeLauncher)
			{
				//ShootAtDirection(aOrientation);
				if (myCurrentWeapon->Shoot(aOrientation) == true)
				{
					myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_FIRE, true);
					SharedNetworkManager::GetInstance()->AddMessage<NetMessageEntityState>(NetMessageEntityState(eEntityState::ATTACK, myEntity.GetGID()));
				}
			}
		}
	}
	myPistol->Update(aDelta);
	myGrenadeLauncher->Update(aDelta);
	myShotgun->Update(aDelta);
}

void ShootingComponent::Render()
{
	myCurrentWeapon->Render();
}

Weapon* ShootingComponent::GetWeapon(eWeaponType aWeaponType)
{
	switch (aWeaponType)
	{
	case eWeaponType::PISTOL:
		return myPistol;
		break;
	case eWeaponType::SHOTGUN:
		return myShotgun;
		break;
	case eWeaponType::GRENADE_LAUNCHER:
		return myGrenadeLauncher;
		break;
	default:
		break;
	}
	DL_ASSERT("Get Weapon crash!");
	return myPistol;
}

float ShootingComponent::GetWeaponForceStrength(eWeaponType aWeaponType) const
{
	switch (aWeaponType)
	{
	case eWeaponType::PISTOL:
		return myPistol->GetForceStrength();
		break;
	case eWeaponType::SHOTGUN:
		return myShotgun->GetForceStrength();
		break;
	}
	DL_ASSERT("Get Weapon force crash!");
	return 0.f;
}

void ShootingComponent::ReloadWeaponIntention()
{
	if (myEntity.GetComponent<FirstPersonRenderComponent>()->IsReloading() == false)
	{
		if (myCurrentWeapon->GetAmmoTotal() > 0)
		{
			switch (myCurrentWeapon->GetWeaponType())
			{
			case eWeaponType::PISTOL:
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_RELOAD, true);
				break;
			case eWeaponType::SHOTGUN:
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_RELOAD, true);
				break;
			case eWeaponType::GRENADE_LAUNCHER:
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_RELOAD, true);
				break;
			default:
				break;
			}
		}
	}
}

void ShootingComponent::ReloadWeapon()
{
	myCurrentWeapon->Reload();
}

void ShootingComponent::ReceiveNote(const UpgradeNote& aNote)
{
	if (myCurrentWeapon->GetWeaponType() != aNote.myData.myWeaponType)
	{
		switch (myCurrentWeapon->GetWeaponType())
		{
		case eWeaponType::PISTOL:
			myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_HOLSTER, true);
			break;
		case eWeaponType::SHOTGUN:
			myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_HOLSTER, true);
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_HOLSTER, true);
			break;
		}
		switch (aNote.myData.myWeaponType)
		{
		case eWeaponType::PISTOL:
			myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_DRAW, false);
			myCurrentWeapon = myPistol;
			break;
		case eWeaponType::SHOTGUN:
			myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_DRAW, false);
			myCurrentWeapon = myShotgun;
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_HOLSTER, false);
			myCurrentWeapon = myGrenadeLauncher;
			break;
		}
	}

	if (aNote.myData.myWeaponType == eWeaponType::PISTOL)
	{
		myPistol->Upgrade(aNote.myData);
	}
	else if (aNote.myData.myWeaponType == eWeaponType::SHOTGUN)
	{
		myShotgun->Upgrade(aNote.myData);
	}
	else if (aNote.myData.myWeaponType == eWeaponType::GRENADE_LAUNCHER)
	{
		myGrenadeLauncher->Upgrade(aNote.myData);
	}
}