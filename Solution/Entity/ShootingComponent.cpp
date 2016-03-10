#include "stdafx.h"

#include "FirstPersonRenderComponent.h"
#include "GrenadeLauncher.h"
#include <InputWrapper.h>
#include "InputComponent.h"
#include "Pistol.h"
#include "Shotgun.h"
#include "ShootingComponent.h"


ShootingComponent::ShootingComponent(Entity& anEntity, Prism::Scene* aScene)
	: Component(anEntity)
	, myCurrentWeapon(nullptr)
	, myPistol(nullptr)
	, myShotgun(nullptr)
	, myGrenadeLauncher(nullptr)
{
	/*myBullet = EntityFactory::CreateEntity(eEntityType::PROJECTILE, *aScene, myBulletOrientation.GetPos());
	myBullet->Reset();
	myBullet->AddToScene();*/

	myPistol = new Pistol();
	myShotgun = new Shotgun();
	myGrenadeLauncher = new GrenadeLauncher(aScene);
	myCurrentWeapon = myPistol;
}

ShootingComponent::~ShootingComponent()
{

	SAFE_DELETE(myPistol);
	SAFE_DELETE(myShotgun);
	SAFE_DELETE(myGrenadeLauncher);
}

void ShootingComponent::Update(float aDelta)
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
		myCurrentWeapon->Reload();
	}

	if (CU::InputWrapper::GetInstance()->MouseIsPressed(0) == true)
	{
		if (myCurrentWeapon == myPistol)
		{
			if (myCurrentWeapon->Shoot(aOrientation) == true)
			{
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::PISTOL_FIRE, true);
			}
		}
		else if (myCurrentWeapon == myShotgun)
		{
			if (myCurrentWeapon->Shoot(aOrientation) == true)
			{
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::SHOTGUN_FIRE, true);
			}
		}
		else if (myCurrentWeapon == myGrenadeLauncher)
		{
			//ShootAtDirection(aOrientation);
			if (myCurrentWeapon->Shoot(aOrientation) == true)
			{
				myEntity.GetComponent<FirstPersonRenderComponent>()->AddIntention(ePlayerState::GRENADE_LAUNCHER_FIRE, true);
			}
		}
	}
	myPistol->Update(aDelta);
	myGrenadeLauncher->Update(aDelta);
	myShotgun->Update(aDelta);
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