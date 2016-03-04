#include "stdafx.h"

#include <DamageNote.h>
#include <InputWrapper.h>
#include <Instance.h>
#include <ModelLoader.h>
#include <Scene.h>
#include "Shooting.h"

#include <EntityFactory.h>
#include <PhysEntity.h>
#include <PhysicsInterface.h>
#include "Player.h"

Shooting::Shooting(Prism::Scene* aScene, Player* aPlayer)
	: myBulletSpeed(0.f)
	, myScene(aScene)
	, myCurrentWeapon(nullptr)
	, myPistol(nullptr)
	, myShotgun(nullptr)
	, myGrenadeLauncher(nullptr)
	, myPlayer(aPlayer)
{
	/*myBullet = EntityFactory::CreateEntity(eEntityType::PROJECTILE, *aScene, myBulletOrientation.GetPos());
	myBullet->Reset();
	myBullet->AddToScene();*/

	myBullets.Init(64);

	myPistol = new Weapon(eWeaponType::PISTOL);
	myShotgun = new Weapon(eWeaponType::SHOTGUN);
	myGrenadeLauncher = new Weapon(eWeaponType::GRENADE_LAUNCHER);
	myCurrentWeapon = myPistol;
}

Shooting::~Shooting()
{
	myBullets.DeleteAll();
	SAFE_DELETE(myPistol);
	SAFE_DELETE(myShotgun);
	SAFE_DELETE(myGrenadeLauncher);
}

void Shooting::Update(float aDelta, const CU::Matrix44<float>& aOrientation)
{
	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_1) == true)
	{
		if (myCurrentWeapon != myPistol)
		{
			switch (myCurrentWeapon->GetWeaponType())
			{
			case eWeaponType::SHOTGUN:
				myPlayer->AddIntention(ePlayerState::SHOTGUN_HOLSTER, true);
				break;
			case eWeaponType::GRENADE_LAUNCHER:
				myPlayer->AddIntention(ePlayerState::GRENADE_LAUNCHER_HOLSTER, false);
				break;
			}
			myPlayer->AddIntention(ePlayerState::PISTOL_DRAW, false);
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
				myPlayer->AddIntention(ePlayerState::PISTOL_HOLSTER, true);
				break;
			case eWeaponType::GRENADE_LAUNCHER:
				myPlayer->AddIntention(ePlayerState::GRENADE_LAUNCHER_HOLSTER, false);
				break;
			}
			myPlayer->AddIntention(ePlayerState::SHOTGUN_DRAW, false);
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
				myPlayer->AddIntention(ePlayerState::PISTOL_HOLSTER, true);
				break;
			case eWeaponType::SHOTGUN:
				myPlayer->AddIntention(ePlayerState::SHOTGUN_HOLSTER, false);
				break;
			}
			myPlayer->AddIntention(ePlayerState::GRENADE_LAUNCHER_DRAW, false);
			myCurrentWeapon = myGrenadeLauncher;
		}	
	}

	if (CU::InputWrapper::GetInstance()->KeyDown(DIK_R) == true)
	{
		switch (myCurrentWeapon->GetWeaponType())
		{
		case eWeaponType::PISTOL:
			myPlayer->AddIntention(ePlayerState::PISTOL_RELOAD, true);
			break;
		case eWeaponType::SHOTGUN:
			myPlayer->AddIntention(ePlayerState::SHOTGUN_RELOAD, true);
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			myPlayer->AddIntention(ePlayerState::GRENADE_LAUNCHER_RELOAD, true);
			break;
		default:
			break;
		}
		myCurrentWeapon->Reload();
	}

	if (myCurrentWeapon == myPistol)
	{
		if (CU::InputWrapper::GetInstance()->MouseDown(0) == true)
		{
			if (myPistol->GetAmmoInClip() > 0)
			{
				myPistol->Shoot(aOrientation);
				myPlayer->AddIntention(ePlayerState::PISTOL_FIRE, true);
				//ShootAtDirection(aOrientation);
			}
		}
	}
	else if (myCurrentWeapon == myGrenadeLauncher)
	{
		if (CU::InputWrapper::GetInstance()->MouseDown(0) == true && myBullets.Size() < 1024)
		{
			ShootAtDirection(aOrientation);
			myCurrentWeapon->Shoot(aOrientation);
			myPlayer->AddIntention(ePlayerState::GRENADE_LAUNCHER_FIRE, true);
		}
	}
	else if (myCurrentWeapon == myShotgun)
	{
		if (CU::InputWrapper::GetInstance()->MouseDown(0) == true)
		{
			if (myShotgun->GetAmmoInClip() > 0)
			{
				myCurrentWeapon->Shoot(aOrientation);
				myPlayer->AddIntention(ePlayerState::SHOTGUN_FIRE, true);
			}
		}
	}

	/*myBullet->Update(aDelta);
	DEBUG_PRINT(myBullet->GetOrientation().GetPos());*/
	//myBullet->SetOrientation(myBulletOrientation);

	//myBulletOrientation.SetPos(myBulletOrientation.GetPos() + myBulletOrientation.GetForward() * myBulletSpeed);

	for (int i = 0; i < myBullets.Size(); ++i)
	{
		myBullets[i]->Update(aDelta);
	}
}

Weapon* Shooting::GetWeapon(eWeaponType aWeaponType)
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

void Shooting::ShootAtDirection(const CU::Matrix44<float>& aOrientation)
{
	/*myBulletOrientation = aOrientation;
	myBulletSpeed = 100.f;

	myBullet->GetPhysEntity()->SetPosition(aOrientation.GetPos());
	myBullet->GetPhysEntity()->AddForce(aOrientation.GetForward(), 20.f);*/

	SET_RUNTIME(false);
	Entity* bullet = EntityFactory::CreateEntity(eEntityType::PROJECTILE, *myScene, myBulletOrientation.GetPos());
	RESET_RUNTIME;
	bullet->Reset();
	bullet->AddToScene();
	bullet->GetPhysEntity()->SetPosition(aOrientation.GetPos());
	bullet->GetPhysEntity()->AddForce(aOrientation.GetForward(), 20.f);

	myBullets.Add(bullet);
}