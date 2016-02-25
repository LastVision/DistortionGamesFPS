#pragma once

#include "BaseModel.h"
#include <Matrix.h>

enum class eBarPosition
{
	LEFT,
	TOP,
	RIGHT
};

namespace Prism
{
	class Effect;
	class Bar3D : public BaseModel
	{
	public:
		Bar3D(const CU::Vector2<float>& aPositionFromJoint, const CU::Vector2<float>& aQuadSize
			, int aNumberOfQuads, Effect* aEffect, eBarPosition aBarPosition);
		~Bar3D();

		void Render(const Camera& aCamera, const CU::Matrix44<float>& aWorld);
		void Render() override;

		void SetValue(float aValue);

	private:
		void CreateVertices(const CU::Vector2<float>& aPositionFromJoint, int aNumberOfQuads);
		CU::Vector2<float> myQuadSize;

		float myValue;
		int myNbrOfQuads;

		eBarPosition myBarPosition;
	};

}