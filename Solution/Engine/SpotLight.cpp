#include "stdafx.h"

#include "EngineEnums.h"
#include "Model.h"
#include "ModelLoader.h"
#include "Instance.h"
#include "SpotLight.h"


namespace Prism
{

	SpotLight::SpotLight(unsigned int aGID, bool aShouldDoInstance)
		: myGID(aGID)
		, myRange(0)
		, myCone(0)
		, myLightMesh(nullptr)
	{
		if (aShouldDoInstance == true)
		{
			ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Light_mesh/SM_cone.fbx"
				, "Data/Resource/Shader/S_effect_deferred_light_mesh_spot.fx");
			myLightMesh = new Instance(*model, myOrientation);
		}
	}


	SpotLight::~SpotLight()
	{
		SAFE_DELETE(myLightMesh);
	}

	void SpotLight::Render(const Camera& aCamera)
	{
		if (myLightMesh != nullptr)
		{
			myOrientation.SetPos(myPosition);
			myLightMesh->Render(aCamera);
			myOrientation.SetPos(CU::Vector3<float>(0.f, 0.f, 0.f));
		}
	}

};