#include "stdafx.h"

#include <Animation.h>
#include "GUIManager3D.h"
#include <Instance.h>
#include <Scene.h>
namespace GUI
{
	GUIManager3D::GUIManager3D(const Prism::Instance* aModel,  Prism::Scene* aScene)
		: myScene(aScene)
	{
		my3DPlane = new Prism::Instance(*Prism::ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/First_person/UI_plane.fbx", "Data/Resource/Shader/S_effect_pbldebug.fx"), myWristOrientation);
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
		//myWristOrientation = CU::Matrix44<float>();

		myWristOrientation = CU::InverseSimple(*myGUIBone.myBind) * (*myGUIBone.myJoint) * aOrientation;
		//myWristOrientation *= aOrientation;
		my3DPlane->Update(aDeltaTime);
	}
}