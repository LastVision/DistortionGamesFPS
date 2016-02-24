#pragma once
#include <Matrix.h>
#include <BoneName.h>
namespace Prism
{
	class Instance;
	class Scene;
	class Effect;
}

namespace GUI
{
	class GUIManager3D
	{
	public:
		GUIManager3D(const Prism::Instance* aModel, Prism::Scene* aScene);
		~GUIManager3D();

		void Update(const CU::Matrix44<float>& aOrientation, float aDeltaTime);

	private:
		float myTestValue;
		CU::Matrix44<float> myWristOrientation;
		Prism::Instance* my3DPlane;
		Prism::Scene* myScene;
		GUIBone myGUIBone;
		Prism::Effect* myEffect;
	};

}