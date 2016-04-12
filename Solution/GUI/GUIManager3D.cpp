#include "stdafx.h"

#include <Bar3D.h>
#include <Animation.h>
#include "GUIManager3D.h"
#include <Engine.h>
#include <Effect.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Scene.h>
#include <TextureContainer.h>
#include <TextProxy.h>

#define SHOWTUTORIALTIME 20.f

namespace GUI
{
	GUIManager3D::GUIManager3D(const Prism::Instance* aModel, Prism::Scene* aScene
		, const int& aPistolClipSize, const int& aPistolAmmoInClip
		, const int& aShotgunClipSize, const int& aShotgunAmmoInClip, const int& aShotgunAmmoTotal
		, const int& aGrenadeLauncherClipSize, const int& aGrenadeLauncherAmmoInClip, const int& aGrenadeLauncherAmmoTotal)
		: myScene(aScene)
		, myShowGUITutorial(0.f)
		, myPistolClipSize(aPistolClipSize)
		, myPistolAmmoInClip(aPistolAmmoInClip)
		, myShotgunClipSize(aShotgunClipSize)
		, myShotgunAmmoInClip(aShotgunAmmoInClip)
		, myShotgunAmmoTotal(aShotgunAmmoTotal)
		, myGrenadeLauncherClipSize(aGrenadeLauncherClipSize)
		, myGrenadeLauncherAmmoInClip(aGrenadeLauncherAmmoInClip)
		, myGrenadeLauncherAmmoTotal(aGrenadeLauncherAmmoTotal)
		, myCurrentHealth(0)
		, myMaxHealth(0)
		, myLowHealthTimer(0.f)
	{
		Prism::ModelLoader::GetInstance()->Pause();
		myEffect = Prism::EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_3dgui.fx");

		myEffect->SetTexture(Prism::TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/UI/T_ammo_pistol.dds"));

		//myGUIBone = aModel->GetCurrentAnimation()->GetHiearchyToBone("ui_jnt3");
		//myHealthBone = aModel->GetCurrentAnimation()->GetHiearchyToBone("health_jnt3");
		myLeftBar = new Prism::Bar3D({ 0.02f, 0.005f }, myShotgunClipSize, myEffect, eBarPosition::LEFT, "Data/Resource/Texture/UI/T_ammo_shotgun.dds");
		myRightBar = new Prism::Bar3D({ 0.02f, 0.005f }, myGrenadeLauncherClipSize, myEffect, eBarPosition::RIGHT, "Data/Resource/Texture/UI/T_ammo_grenade_launcher.dds");
		myTopBar = new Prism::Bar3D({ 0.02f, 0.005f }, myPistolClipSize, myEffect, eBarPosition::TOP, "Data/Resource/Texture/UI/T_ammo_pistol.dds");
		myHealthBar = new Prism::Bar3D({ 0.01f, 0.025f }, 15, myEffect, eBarPosition::HEALTH, "Data/Resource/Texture/UI/T_healthbar.dds");

		myHealthIcon = new Prism::Bar3D({ 0.025f, 0.025f }, 1, myEffect, eBarPosition::HEALTH_ICON, "Data/Resource/Texture/UI/T_healthIcon.dds");
		myHealthIcon->SetValue(1.f);
		
		myPistolIcon = new Prism::Bar3D({ 1.f, 1.f }, 1, myEffect, eBarPosition::PISTOL_ICON, "Data/Resource/Texture/UI/T_pistolIcon.dds");
		myPistolIcon->SetValue(1.f);
		myShotgunIcon = new Prism::Bar3D({ 0.025f, 0.025f }, 1, myEffect, eBarPosition::SHOTGUN_ICON, "Data/Resource/Texture/UI/T_shotgunIcon.dds");
		myShotgunIcon->SetValue(1.f);
		myGrenadeLauncherIcon = new Prism::Bar3D({ 0.025f, 0.025f }, 1, myEffect, eBarPosition::GRENADE_LAUNCHER_ICON, "Data/Resource/Texture/UI/T_grenadeLauncherIcon.dds");
		myGrenadeLauncherIcon->SetValue(1.f);
			
		myAmmoTotalText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE), true);
		myAmmoTotalText->Rotate3dText(-0.8f);
		myAmmoTotalText->SetOffset({ -1.f, 1.f, 2.f });
		

		Prism::ModelLoader::GetInstance()->UnPause();
	}


	GUIManager3D::~GUIManager3D()
	{
		SAFE_DELETE(myLeftBar);
		SAFE_DELETE(myTopBar);
		SAFE_DELETE(myRightBar);
		SAFE_DELETE(myHealthBar);
		SAFE_DELETE(myHealthIcon);
		SAFE_DELETE(myPistolIcon);
		SAFE_DELETE(myShotgunIcon);
		SAFE_DELETE(myGrenadeLauncherIcon);
		SAFE_DELETE(myAmmoTotalText);
	}

	void GUIManager3D::Update(const CU::Matrix44<float>& aUIJointOrientation
		, const CU::Matrix44<float>& aHealthJointOrientation
		, int aCurrentHealth, int aMaxHealth, float aDeltaTime, eWeaponType aCurrentWeaponType)
	{
		myMaxHealth = aMaxHealth;
		myCurrentHealth = aCurrentHealth;

		if (aCurrentWeaponType == eWeaponType::PISTOL)
		{
			float colorValue = myPistolAmmoInClip / float(myPistolClipSize);
			myAmmoTotalText->SetText("-");
			myAmmoTotalText->SetColor({ 1.f - colorValue, colorValue, 0.f, 0.5f });
		}
		else if (aCurrentWeaponType == eWeaponType::SHOTGUN)
		{
			float colorValue = myShotgunAmmoInClip / float(myShotgunClipSize);
			myAmmoTotalText->SetText(std::to_string(myShotgunAmmoTotal));
			myAmmoTotalText->SetColor({ 1.f - colorValue, colorValue, 0.5f, 0.5f });
		}
		else if (aCurrentWeaponType == eWeaponType::GRENADE_LAUNCHER)
		{
			float colorValue = myGrenadeLauncherAmmoInClip / float(myGrenadeLauncherClipSize);
			myAmmoTotalText->SetText(std::to_string(myGrenadeLauncherAmmoTotal));
			myAmmoTotalText->SetColor({ 1.f - colorValue, colorValue, 1.f, 0.5f });
		}

		if (myShowGUITutorial < SHOWTUTORIALTIME)
		{
			myShowGUITutorial += aDeltaTime;
		}

		myLowHealthTimer -= aDeltaTime;
		if (myLowHealthTimer <= 0.f)
		{
			myLowHealthTimer = (float(myCurrentHealth) / float(myMaxHealth) + 0.3f);
		}

		//myEffect->SetGradiantValue(cos(myTestValue));
		//myEffect->SetGradiantDirection({ 0.f, 1.f });

		//myWristOrientation = CU::InverseSimple(*myGUIBone.myBind) * (*myGUIBone.myJoint) * aOrientation;
		//myHealthOrientation = CU::InverseSimple(*myHealthBone.myBind) * (*myHealthBone.myJoint) * aOrientation;
		myWristOrientation = aUIJointOrientation;
		myHealthOrientation = aHealthJointOrientation;

		myLeftBar->SetValue(myShotgunAmmoInClip / float(myShotgunClipSize));
		myRightBar->SetValue(myGrenadeLauncherAmmoInClip / float(myGrenadeLauncherClipSize));
		myTopBar->SetValue(myPistolAmmoInClip / float(myPistolClipSize));
		myHealthBar->SetValue(aCurrentHealth / float(aMaxHealth));
	}

	void GUIManager3D::Render(bool aIsFirstLevel)
	{
		myEffect->SetGradiantValue(1.f);
		myLeftBar->Render(*myScene->GetCamera(), myWristOrientation);
		myRightBar->Render(*myScene->GetCamera(), myWristOrientation);
		myTopBar->Render(*myScene->GetCamera(), myWristOrientation);

		CU::Vector4<float> healthColor = { 0.f, 0.f, 0.f, 1.f };
		float life = float(myCurrentHealth) / float(myMaxHealth);
		float alpha = 1.f;

		if (life <= 0.4f)
		{
			healthColor.x = 1.f - life;
			// add rest of colors here
			alpha = fminf(fminf(1.f, myLowHealthTimer), 1.f - myLowHealthTimer);
		}

		myHealthBar->Render(*myScene->GetCamera(), myHealthOrientation, healthColor);

		if (aIsFirstLevel == true)
		{
			if (myShowGUITutorial < SHOWTUTORIALTIME)
			{
				myPistolIcon->GetEffect()->SetGradiantValue(fminf(fminf(1.f, myShowGUITutorial), SHOWTUTORIALTIME - myShowGUITutorial));
				myPistolIcon->Render(*myScene->GetCamera(), myWristOrientation);
			}
		}

		if (alpha < 1.f)
		{
			myEffect->SetGradiantValue(alpha);
		}
		myHealthIcon->Render(*myScene->GetCamera(), myHealthOrientation, healthColor);

		myAmmoTotalText->SetOrientation(myHealthOrientation);
		myAmmoTotalText->Render(myScene->GetCamera(), 500.f);
	}

	void GUIManager3D::Rebuild(const eWeaponType aWeaponType, int aSize)
	{
		switch (aWeaponType)
		{
		case eWeaponType::PISTOL:
			myTopBar->Rebuild(myPistolClipSize);
			break;
		case eWeaponType::SHOTGUN:
			myRightBar->Rebuild(myShotgunClipSize);
			break;
		case eWeaponType::GRENADE_LAUNCHER:
			myLeftBar->Rebuild(myGrenadeLauncherClipSize);
			break;
		default:
			break;
		}


	}
}