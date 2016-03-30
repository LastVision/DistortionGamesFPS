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
namespace GUI
{
	GUIManager3D::GUIManager3D(const Prism::Instance*, Prism::Scene* aScene
		, const int& aPistolClipSize, const int& aPistolAmmoInClip
		, const int& aShotgunClipSize, const int& aShotgunAmmoInClip
		, const int& aGrenadeLauncherClipSize, const int& aGrenadeLauncherAmmoInClip)
		: myScene(aScene)
		, myTestValue(0.f)
		, myPistolClipSize(aPistolClipSize)
		, myPistolAmmoInClip(aPistolAmmoInClip)
		, myShotgunClipSize(aShotgunClipSize)
		, myShotgunAmmoInClip(aShotgunAmmoInClip)
		, myGrenadeLauncherClipSize(aGrenadeLauncherClipSize)
		, myGrenadeLauncherAmmoInClip(aGrenadeLauncherAmmoInClip)
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
		
		myPistolIcon = new Prism::Bar3D({ 0.025f, 0.025f }, 1, myEffect, eBarPosition::PISTOL_ICON, "Data/Resource/Texture/UI/T_pistolIcon.dds");
		myPistolIcon->SetValue(1.f);
		myShotgunIcon = new Prism::Bar3D({ 0.025f, 0.025f }, 1, myEffect, eBarPosition::SHOTGUN_ICON, "Data/Resource/Texture/UI/T_shotgunIcon.dds");
		myShotgunIcon->SetValue(1.f);
		myGrenadeLauncherIcon = new Prism::Bar3D({ 0.025f, 0.025f }, 1, myEffect, eBarPosition::GRENADE_LAUNCHER_ICON, "Data/Resource/Texture/UI/T_grenadeLauncherIcon.dds");
		myGrenadeLauncherIcon->SetValue(1.f);
			
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
	}

	void GUIManager3D::Update(const CU::Matrix44<float>& aUIJointOrientation
		, const CU::Matrix44<float>& aHealthJointOrientation
		, int aCurrentHealth, int aMaxHealth, float aDeltaTime)
	{
		myTestValue += aDeltaTime;
		myEffect->SetGradiantValue(cos(myTestValue));
		myEffect->SetGradiantDirection({ 0.f, 1.f });

		//myWristOrientation = CU::InverseSimple(*myGUIBone.myBind) * (*myGUIBone.myJoint) * aOrientation;
		//myHealthOrientation = CU::InverseSimple(*myHealthBone.myBind) * (*myHealthBone.myJoint) * aOrientation;
		myWristOrientation = aUIJointOrientation;
		myHealthOrientation = aHealthJointOrientation;

		myLeftBar->SetValue(myShotgunAmmoInClip / float(myShotgunClipSize));
		myRightBar->SetValue(myGrenadeLauncherAmmoInClip / float(myGrenadeLauncherClipSize));
		myTopBar->SetValue(myPistolAmmoInClip / float(myPistolClipSize));
		myHealthBar->SetValue(aCurrentHealth / float(aMaxHealth));
	}

	void GUIManager3D::Render()
	{
		myLeftBar->Render(*myScene->GetCamera(), myWristOrientation);
		myRightBar->Render(*myScene->GetCamera(), myWristOrientation);
		myTopBar->Render(*myScene->GetCamera(), myWristOrientation);
		myHealthBar->Render(*myScene->GetCamera(), myHealthOrientation);
		
		myHealthIcon->Render(*myScene->GetCamera(), myHealthOrientation);
		myPistolIcon->Render(*myScene->GetCamera(), myWristOrientation);
		myShotgunIcon->Render(*myScene->GetCamera(), myWristOrientation);
		myGrenadeLauncherIcon->Render(*myScene->GetCamera(), myWristOrientation);
	}
}