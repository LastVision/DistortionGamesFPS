#include "stdafx.h"

#include "Camera.h"
#include "EngineEnums.h"
#include "Instance.h"
#include "ModelLoader.h"
#include "Model.h"
#include "PointLight.h"

namespace Prism
{
	PointLight::PointLight()
		: myRange(0)
		, myInstance(nullptr)
	{
	}

	void PointLight::Initiate()
	{
		ModelProxy* model = ModelLoader::GetInstance()->LoadCube(1, 1, 1);
		myInstance = new Instance(*model, myOrientation);
	}

	void PointLight::Render(Camera* aCamera)
	{
		if (this != nullptr)
			myInstance->Render(*aCamera);
	}

};