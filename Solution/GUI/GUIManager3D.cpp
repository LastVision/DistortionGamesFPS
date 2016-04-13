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

#define SHOWTUTORIALTIME 7.f

namespace GUI
{
	GUIManager3D::GUIManager3D(const Prism::Instance* aModel, Prism::Scene* aScene
		, const int& aPistolClipSize, const int& aPistolAmmoInClip
		, const int& aShotgunClipSize, const int& aShotgunAmmoInClip, const int& aShotgunAmmoTotal
		, const int& aGrenadeLauncherClipSize, const int& aGrenadeLauncherAmmoInClip, const int& aGrenadeLauncherAmmoTotal)
		: myScene(aScene)
		, myShowFirstTutorial(0.f)
		, myShowSecondTutorial(0.f)
		, myShowThirdTutorial(0.f)
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
		, myIsFirstLevel(false)
		, myRenderAmmo(false)
		, myRenderAmmoTotal(false)
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

		myGUITutorialHealth = new Prism::Bar3D({ 1.f, 1.f }, 1, myEffect, eBarPosition::PISTOL_ICON, "Data/Resource/Texture/UI/T_tutorial_GUI_health.dds");
		myGUITutorialHealth->SetValue(1.f);
		myGUITutorialAmmo = new Prism::Bar3D({ 1.f, 1.f }, 1, myEffect, eBarPosition::PISTOL_ICON, "Data/Resource/Texture/UI/T_tutorial_GUI_ammo.dds");
		myGUITutorialAmmo->SetValue(1.f);
		myGUITutorialAmmoTotal = new Prism::Bar3D({ 1.f, 1.f }, 1, myEffect, eBarPosition::PISTOL_ICON, "Data/Resource/Texture/UI/T_tutorial_GUI_ammo_total.dds");
		myGUITutorialAmmoTotal->SetValue(1.f);

		myAmmoTotalText = Prism::ModelLoader::GetInstance()->LoadText(Prism::Engine::GetInstance()->GetFont(Prism::eFont::DIALOGUE), true);
		myAmmoTotalText->Rotate3dText(-0.8f);
		myAmmoTotalText->SetOffset({ 0.1f, 0.f, 0.f });


		Prism::ModelLoader::GetInstance()->UnPause();

		myRotationMatrix = CU::Matrix44<float>::CreateRotateAroundZ(-3.14f * 0.7f);
	}


	GUIManager3D::~GUIManager3D()
	{
		SAFE_DELETE(myLeftBar);
		SAFE_DELETE(myTopBar);
		SAFE_DELETE(myRightBar);
		SAFE_DELETE(myHealthBar);
		SAFE_DELETE(myHealthIcon);
		SAFE_DELETE(myGUITutorialHealth);
		SAFE_DELETE(myGUITutorialAmmo);
		SAFE_DELETE(myGUITutorialAmmoTotal);
		SAFE_DELETE(myAmmoTotalText);
	}

	void GUIManager3D::Update(const CU::Matrix44<float>& aUIJointOrientation
		, const CU::Matrix44<float>& aHealthJointOrientation
		, int aCurrentHealth, int aMaxHealth, float aDeltaTime, eWeaponType aCurrentWeaponType)
	{
		myMaxHealth = aMaxHealth;
		myCurrentHealth = aCurrentHealth;
		float colorValue = 0.f;
		if (aCurrentWeaponType == eWeaponType::PISTOL)
		{
			colorValue = myPistolAmmoInClip / float(myPistolClipSize);
			myAmmoTotalText->SetText("");
		}
		else if (aCurrentWeaponType == eWeaponType::SHOTGUN)
		{
			colorValue = myShotgunAmmoInClip / float(myShotgunClipSize);
			myAmmoTotalText->SetText(std::to_string(myShotgunAmmoTotal));
		}
		else if (aCurrentWeaponType == eWeaponType::GRENADE_LAUNCHER)
		{
			colorValue = myGrenadeLauncherAmmoInClip / float(myGrenadeLauncherClipSize);
			myAmmoTotalText->SetText(std::to_string(myGrenadeLauncherAmmoTotal));
		}

		myAmmoTotalText->SetColor({ 1.f, colorValue, colorValue, 0.75f });


		if (myIsFirstLevel == true)
		{
			if (myShowFirstTutorial < SHOWTUTORIALTIME)
			{
				myShowFirstTutorial += aDeltaTime;
			}
			else if (myShowSecondTutorial < SHOWTUTORIALTIME)
			{
				myShowSecondTutorial += aDeltaTime;
			}
			else if (myShowThirdTutorial < SHOWTUTORIALTIME)
			{
				myShowThirdTutorial += aDeltaTime;
			}
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
		myIsFirstLevel = aIsFirstLevel;

		myEffect->SetGradiantValue(1.f);

		if (myIsFirstLevel == false || (myIsFirstLevel == true && myRenderAmmo))
		{
			myLeftBar->Render(*myScene->GetCamera(), myWristOrientation);
			myRightBar->Render(*myScene->GetCamera(), myWristOrientation);
			myTopBar->Render(*myScene->GetCamera(), myWristOrientation);
		}

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

		if (myIsFirstLevel == true)
		{
			if (myShowFirstTutorial < SHOWTUTORIALTIME)
			{
				myGUITutorialHealth->GetEffect()->SetGradiantValue(fminf(fminf(1.f, myShowFirstTutorial), SHOWTUTORIALTIME - myShowFirstTutorial));
				myGUITutorialHealth->Render(*myScene->GetCamera(), myWristOrientation);
			}
			else if (myShowSecondTutorial < SHOWTUTORIALTIME)
			{
				myRenderAmmo = true;
				myGUITutorialAmmo->GetEffect()->SetGradiantValue(fminf(fminf(1.f, myShowSecondTutorial), SHOWTUTORIALTIME - myShowSecondTutorial));
				myGUITutorialAmmo->Render(*myScene->GetCamera(), myWristOrientation);
			}
			else if (myShowThirdTutorial < SHOWTUTORIALTIME)
			{
				myRenderAmmoTotal = true;
				myGUITutorialAmmoTotal->GetEffect()->SetGradiantValue(fminf(fminf(1.f, myShowThirdTutorial), SHOWTUTORIALTIME - myShowThirdTutorial));
				myGUITutorialAmmoTotal->Render(*myScene->GetCamera(), myWristOrientation);
			}
		}

		if (alpha < 1.f)
		{
			myEffect->SetGradiantValue(alpha);
		}
		myHealthIcon->Render(*myScene->GetCamera(), myHealthOrientation, healthColor);

		CU::Vector3<float> oldPos(myHealthOrientation.GetPos());
		CU::Vector3<float> offset(myHealthOrientation.GetForward() * -0.25f);
		offset += myHealthOrientation.GetUp() * 0.1f;
		myHealthOrientation.SetPos(CU::Vector3<float>());
		myHealthOrientation = myRotationMatrix * myHealthOrientation;

		myHealthOrientation.SetPos(oldPos + offset);
		myAmmoTotalText->SetOrientation(myHealthOrientation);

		if (myIsFirstLevel == false || (myIsFirstLevel == true && myRenderAmmoTotal))
		{
			myAmmoTotalText->Render(myScene->GetCamera(), 800.f);
		}
		myHealthOrientation.SetPos(oldPos);
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