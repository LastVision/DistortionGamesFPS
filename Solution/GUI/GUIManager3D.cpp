#include "stdafx.h"

#include <Animation.h>
#include "GUIManager3D.h"
#include <Engine.h>
#include <Effect.h>
#include <EffectContainer.h>
#include <Instance.h>
#include <Scene.h>
namespace GUI
{
	GUIManager3D::GUIManager3D(const Prism::Instance* aModel,  Prism::Scene* aScene)
		: myScene(aScene)
		, myTestValue(0.f)
	{
		myEffect = Prism::EffectContainer::GetInstance()->GetEffect("Data/Resource/Shader/S_effect_3dgui.fx");
		//my3DPlane = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/First_person/UI_plane.fbx", "Data/Resource/Shader/S_effect_3dgui.fx"), myWristOrientation);
		my3DPlane = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/First_person/UI_plane.fbx", "Data/Resource/Shader/S_effect_3dgui.fx"), myWristOrientation);
		myScene->AddInstance(my3DPlane);

		myGUIBone = aModel->GetCurrentAnimation()->GetHiearchyToBone("r_wrist_jnt1");

	}


	GUIManager3D::~GUIManager3D()
	{
		myScene->RemoveInstance(my3DPlane);
		SAFE_DELETE(my3DPlane);
	}

	void GUIManager3D::Update(const CU::Matrix44<float>& aOrientation, float aDeltaTime)
	{
		myTestValue += aDeltaTime;
		myEffect->SetGradiantValue(cos(myTestValue));
		myEffect->SetGradiantDirection({ 0.f, 1.f });

		myWristOrientation = CU::InverseSimple(*myGUIBone.myBind) * (*myGUIBone.myJoint) * aOrientation;
		//myWristOrientation *= aOrientation;
		//my3DPlane->Update(aDeltaTime);
	}
}