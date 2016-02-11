#include "stdafx.h"

#include "Model.h"
#include "ModelAnimated.h"
#include "ModelProxy.h"

namespace Prism
{
	ModelProxy::ModelProxy()
		: myModel(nullptr)
		, myModelAnimated(nullptr)
	{
	}

	void ModelProxy::Render(const CU::Matrix44<float>& aOrientation, const CU::Vector3<float>& aCameraPosition, bool aIsDepthRender)
	{
		if (myModel != nullptr)
		{
			myModel->Render(aOrientation, aCameraPosition, aIsDepthRender);
		}
		else if (myModelAnimated != nullptr)
		{
			myModelAnimated->Render(aOrientation, aCameraPosition, aIsDepthRender);
		}
	}

	void ModelProxy::SetModel(Model* aModel)
	{
		myModel = aModel;
	}

	void ModelProxy::SetModelAnimated(ModelAnimated* aModel)
	{
		myModelAnimated = aModel;
	}

	Effect* ModelProxy::GetEffect()
	{
		if (myModel != nullptr)
		{
			return myModel->GetEffect();
		}

		if (myModelAnimated != nullptr)
		{
			return myModelAnimated->GetEffect();
		}

		return nullptr;
	}

	void ModelProxy::SetEffect(Effect* aEffect)
	{
		myModel->SetEffect(aEffect);
	}
}