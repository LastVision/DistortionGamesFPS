#pragma once
#include <Matrix.h>

namespace Prism
{
	class Bar3D;
	class Instance;
	class Scene;
	class Effect;
	class TextProxy;
}

namespace GUI
{
	class GUIManager3D
	{
	public:
		GUIManager3D(const Prism::Instance* aModel, Prism::Scene* aScene
			, const int& aPistolClipSize, const int& aPistolAmmoInClip
			, const int& aShotgunClipSize, const int& aShotgunAmmoInClip, const int& aShotgunAmmoTotal
			, const int& aGrenadeLauncherClipSize, const int& aGrenadeLauncherAmmoInClip, const int& aGrenadeLauncherAmmoTotal);
		~GUIManager3D();

		void Update(const CU::Matrix44<float>& aUIJointOrientation, const CU::Matrix44<float>& aHealthJointOrientation
			, int aCurrentHealth, int aMaxHealth, float aDeltaTime, eWeaponType aCurrentWeaponType);
		void Render(bool aIsFirstLevel);

		void Rebuild(const eWeaponType aWeaponType, int aSize);

	private:
		void operator=(GUIManager3D&) = delete;

		float myShowFirstTutorial;
		float myShowSecondTutorial;
		float myShowThirdTutorial;

		bool myIsFirstLevel;

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
		const int& myPistolClipSize;
		const int& myPistolAmmoInClip;
		const int& myShotgunClipSize;
		const int& myShotgunAmmoInClip;
		const int& myShotgunAmmoTotal;
		const int& myGrenadeLauncherClipSize;
		const int& myGrenadeLauncherAmmoInClip;
		const int& myGrenadeLauncherAmmoTotal;

		Prism::Bar3D* myHealthIcon;
		Prism::Bar3D* myGUITutorialHealthIcon;
		Prism::Bar3D* myShotgunIcon;
		Prism::Bar3D* myGrenadeLauncherIcon;

		Prism::TextProxy* myAmmoTotalText;
		CU::Matrix44<float> myRotationMatrix;

		int myCurrentHealth;
		int myMaxHealth;

		float myLowHealthTimer;

		bool myRenderAmmo;
		bool myRenderAmmoTotal;
	};

}