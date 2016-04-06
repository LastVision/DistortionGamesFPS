#pragma once

#include "BaseModel.h"
#include <Matrix.h>

enum class eBarPosition
{
	LEFT,
	TOP,
	RIGHT,
	HEALTH,
	HEALTH_ICON,
	PISTOL_ICON,
	SHOTGUN_ICON,
	GRENADE_LAUNCHER_ICON,
};

namespace Prism
{
	class Effect;
	class Bar3D : public BaseModel
	{
	public:
		Bar3D(const CU::Vector2<float>& aQuadSize
			, int aNumberOfQuads, Effect* aEffect, eBarPosition aBarPosition, const std::string& aTexturePath);
		~Bar3D();

		void Render(const Camera& aCamera, const CU::Matrix44<float>& aWorld);
		void Render() override;

		void SetValue(float aValue);

		void Rebuild(int aNumberOfQuads);

	private:
		void CreateVertices(int aNumberOfQuads);
		CU::Vector2<float> myQuadSize;

		float myValue;
		int myNbrOfQuads;

		eBarPosition myBarPosition;
	};

}