#pragma once
#include <Matrix.h>

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

		void Update(const CU::Matrix44<float>& aUIJointOrientation, const CU::Matrix44<float>& aHealthJointOrientation
			, int aCurrentHealth, int aMaxHealth, float aDeltaTime);
		void Render();

	private:
		void operator=(GUIManager3D&) = delete;

		float myTestValue;
		CU::Matrix44<float> myWristOrientation;
		//GUIBone myGUIBone;
		CU::Matrix44<float> myHealthOrientation;
		//GUIBone myHealthBone;
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

		Prism::Bar3D* myTopAmmoLeft;
	};

}