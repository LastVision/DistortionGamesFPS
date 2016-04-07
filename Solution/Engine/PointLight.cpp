#include "stdafx.h"

#include "Camera.h"
#include "EngineEnums.h"
#include "Instance.h"
#include "ModelLoader.h"
#include "Model.h"
#include "PointLight.h"

namespace Prism
{
	PointLight::PointLight(unsigned int aGID, bool aAmbientOnly)
		: myGID(aGID)
		, myAmbientOnly(aAmbientOnly)
		, myRange(0)
		, myLightMesh(nullptr)
	{
		ModelProxy* model = ModelLoader::GetInstance()->LoadModel("Data/Resource/Model/Light_mesh/SM_sphere.fbx"
			, "Data/Resource/Shader/S_effect_deferred_light_mesh_point.fx");
		myLightMesh = new Instance(*model, myOrientation);
	}

	PointLight::~PointLight()
	{
		SAFE_DELETE(myLightMesh);
	}

	void PointLight::Update()
	{
		myLightData.myColor = GetColor();
		myLightData.myPosition = GetPosition();
		myLightData.myRange = GetRange();
	}

	void PointLight::Render(const Camera& aCamera)
	{
		myLightMesh->Render(aCamera);
	}

};