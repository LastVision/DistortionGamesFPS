#pragma once
#include <Matrix.h>
#include <BoneName.h>
namespace Prism
{
	class Bar3D;
	class Instance;
	class Scene;
	class Effect;
}

namespace GUI
{
	class GUIManager3D
	{
	public:
		GUIManager3D(const Prism::Instance* aModel, Prism::Scene* aScene
			, int& aPistolClipSize, int& aPistolAmmoInClip
			, int& aShotgunClipSize, int& aShotgunAmmoInClip
			, int& aGrenadeLauncherClipSize, int& aGrenadeLauncherAmmoInClip);
		~GUIManager3D();

		void Update(const CU::Matrix44<float>& aOrientation, float aDeltaTime);
		void Render();

	private:
		float myTestValue;
		CU::Matrix44<float> myWristOrientation;
		GUIBone myGUIBone;
		CU::Matrix44<float> myHealthOrientation;
		GUIBone myHealthBone;
		Prism::Scene* myScene;
		Prism::Effect* myEffect;
		Prism::Bar3D* myLeftBar;
		Prism::Bar3D* myRightBar;
		Prism::Bar3D* myTopBar;
		Prism::Bar3D* myHealthBar;
		int& myPistolClipSize;
		int& myPistolAmmoInClip;
		int& myShotgunClipSize;
		int& myShotgunAmmoInClip;
		int& myGrenadeLauncherClipSize;
		int& myGrenadeLauncherAmmoInClip;
	};

}