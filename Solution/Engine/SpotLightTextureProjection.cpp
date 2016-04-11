#include "stdafx.h"
#include "SpotLightTextureProjection.h"
#include "Instance.h"
#include "TextureContainer.h"

namespace Prism
{
	SpotLightTextureProjection::SpotLightTextureProjection(unsigned int aGID, bool aIsForSpot)
		: SpotLight(aGID, false)
	{

		SAFE_DELETE(myLightMesh);
		ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Light_mesh/SM_cone_textureprojection.fbx"
			, "Data/Resource/Shader/S_effect_deferred_light_mesh_spot_textureprojection.fx");
		myLightMesh = new Instance(*model, myOrientation);
		while (ModelLoader::GetInstance()->IsLoading() == true)
		{
		}
		if (aIsForSpot == false)
		{
			//myLightMesh->GetModel().GetEffect()->SetTexture(TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/textureProjectionTest2.dds"));
			myTexture = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/T_squarelamp_projection.dds");
		}
		else
		{
			//myLightMesh->GetModel().GetEffect()->SetTexture(TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/T_spotlight_projection.dds"));
			myTexture = TextureContainer::GetInstance()->GetTexture("Data/Resource/Texture/T_spotlight_projection.dds");
		}
	}

	SpotLightTextureProjection::~SpotLightTextureProjection()
	{
		SAFE_DELETE(myLightMesh);
	}
	
	void SpotLightTextureProjection::Init()
	{
		CU::Vector2<float> windowsSize = Prism::Engine::GetInstance()->GetWindowSize();
		myProjectionMatrix = CU::Matrix44<float>::CreateProjectionMatrixLH(0.f, myRange, windowsSize.x / windowsSize.x, myAngleInRad);

		
	}

	void SpotLightTextureProjection::Render(const Camera& aCamera)
	{
		if (myLightMesh != nullptr)
		{
			myOrientation.SetPos(myPosition);
			myLightMesh->GetModel().GetEffect()->SetViewMatrix(myViewMatrix);
			myLightMesh->GetModel().GetEffect()->SetProjectionMatrix(myProjectionMatrix);
			myLightMesh->GetModel().GetEffect()->SetTexture(myTexture);
			myLightMesh->Render(aCamera);
			//myOrientation.SetPos(CU::Vector3<float>(0.f, 0.f, 0.f));
		}
	}

	void SpotLightTextureProjection::Update()
	{
		SpotLight::Update();
		myOrientation.SetPos(myPosition);
		myViewMatrix = CU::InverseSimple(myOrientation);
	}

	const CU::Matrix44<float>& SpotLightTextureProjection::GetView() const
	{
		return myViewMatrix;
	}

	const CU::Matrix44<float>& SpotLightTextureProjection::GetProjection() const
	{
		return myProjectionMatrix;
	}

	const CU::Matrix44<float>& SpotLightTextureProjection::GetOrientation() const
	{
		return myOrientation;
	}
	

}