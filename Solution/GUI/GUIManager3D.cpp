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
	GUIManager3D::GUIManager3D(const Prism::Instance* aModel, Prism::Scene* aScene, int& aClipSize, int& aAmmoInClip)
		: myScene(aScene)
		, myTestValue(0.f)
		, myLeftBar(nullptr)
		, myPistolClipSize(aClipSize)
		, myPistolAmmoInClip(aAmmoInClip)
	{
		myEffect = Prism::EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_3dgui.fx");
		myEffect->SetTexture(Prism::TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/UI/T_ammo_pistol.dds"));

		myGUIBone = aModel->GetCurrentAnimation()->GetHiearchyToBone("ui_jnt3");
		myHealthBone = aModel->GetCurrentAnimation()->GetHiearchyToBone("health_jnt5");
		myLeftBar = new Prism::Bar3D({ 0.02f, 0.005f }, 32, myEffect, eBarPosition::LEFT);
		myRightBar = new Prism::Bar3D({ 0.02f, 0.005f }, 32, myEffect, eBarPosition::RIGHT);
		myTopBar = new Prism::Bar3D({ 0.02f, 0.005f }, aClipSize, myEffect, eBarPosition::TOP);
		myHealthBar = new Prism::Bar3D({ 0.01f, 0.025f }, 15, myEffect, eBarPosition::HEALTH);
	}


	GUIManager3D::~GUIManager3D()
	{
		SAFE_DELETE(myLeftBar);
		SAFE_DELETE(myTopBar);
		SAFE_DELETE(myRightBar);
		SAFE_DELETE(myHealthBar)
	}

	void GUIManager3D::Update(const CU::Matrix44<float>& aOrientation, float aDeltaTime)
	{
		myTestValue += aDeltaTime;
		myEffect->SetGradiantValue(cos(myTestValue));
		myEffect->SetGradiantDirection({ 0.f, 1.f });

		myWristOrientation = CU::InverseSimple(*myGUIBone.myBind) * (*myGUIBone.myJoint) * aOrientation;
		myHealthOrientation = CU::InverseSimple(*myHealthBone.myBind) * (*myHealthBone.myJoint) * aOrientation;
		myLeftBar->SetValue(1.f);
		myRightBar->SetValue(1.f);
		myTopBar->SetValue(myPistolAmmoInClip / float(myPistolClipSize));
		myHealthBar->SetValue(cos(myTestValue));
	}

	void GUIManager3D::Render()
	{
		myLeftBar->Render(*myScene->GetCamera(), myWristOrientation);
		myRightBar->Render(*myScene->GetCamera(), myWristOrientation);
		myTopBar->Render(*myScene->GetCamera(), myWristOrientation);
		myHealthBar->Render(*myScene->GetCamera(), myHealthOrientation);
	}
}