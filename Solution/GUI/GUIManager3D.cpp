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
		, int& aPistolClipSize, int& aPistolAmmoInClip
		, int& aShotgunClipSize, int& aShotgunAmmoInClip
		, int& aGrenadeLauncherClipSize, int& aGrenadeLauncherAmmoInClip)
		: myScene(aScene)
		, myTestValue(0.f)
		, myPistolClipSize(aPistolClipSize)
		, myPistolAmmoInClip(aPistolAmmoInClip)
		, myShotgunClipSize(aShotgunClipSize)
		, myShotgunAmmoInClip(aShotgunAmmoInClip)
		, myGrenadeLauncherClipSize(aGrenadeLauncherClipSize)
		, myGrenadeLauncherAmmoInClip(aGrenadeLauncherAmmoInClip)
	{
		myEffect = Prism::EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_3dgui.fx");
		myEffect->SetTexture(Prism::TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/UI/T_ammo_pistol.dds"));

		//myGUIBone = aModel->GetCurrentAnimation()->GetHiearchyToBone("ui_jnt3");
		//myHealthBone = aModel->GetCurrentAnimation()->GetHiearchyToBone("health_jnt3");
		myLeftBar = new Prism::Bar3D({ 0.02f, 0.005f }, myShotgunClipSize, myEffect, eBarPosition::LEFT);
		myRightBar = new Prism::Bar3D({ 0.02f, 0.005f }, myGrenadeLauncherClipSize, myEffect, eBarPosition::RIGHT);
		myTopBar = new Prism::Bar3D({ 0.02f, 0.005f }, myPistolClipSize, myEffect, eBarPosition::TOP);
		myHealthBar = new Prism::Bar3D({ 0.01f, 0.025f }, 15, myEffect, eBarPosition::HEALTH);

		myTopAmmoLeft = new Prism::Bar3D({ 0.05f, 0.025f }, 1, myEffect, eBarPosition::TOP_AMMOLEFT);
		myTopAmmoLeft->SetValue(1.f);
	}


	GUIManager3D::~GUIManager3D()
	{
		SAFE_DELETE(myLeftBar);
		SAFE_DELETE(myTopBar);
		SAFE_DELETE(myRightBar);
		SAFE_DELETE(myHealthBar);
		SAFE_DELETE(myTopAmmoLeft);
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
		myTopAmmoLeft->Render(*myScene->GetCamera(), myWristOrientation);
	}
}