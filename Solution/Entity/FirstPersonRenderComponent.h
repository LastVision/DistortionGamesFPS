#pragma once
#include <AnimationData.h>
#include <BoneName.h>
#include "Component.h"
#include <StaticArray.h>

namespace GUI
{
	class GUIManager3D;
}

namespace Prism
{
	class Instance;
	class ModelProxy;
	class SpriteProxy;
}

struct CoOpRespawn
{
	CU::Vector3<float> myPosition;
	int myCurrentValue;
	int myMaxValue;
	int myGID;
};

class FirstPersonRenderComponent : public Component, public NetworkSubscriber
{
public:
	FirstPersonRenderComponent(Entity& aEntity, Prism::Scene* aScene);
	~FirstPersonRenderComponent();

	void Init();

	void Update(float aDelta) override;
	void UpdateCoOpPositions(const CU::GrowingArray<Entity*>& somePlayers);
	void Render();
	bool IsCurrentAnimationDone() const;
	void RestartCurrentAnimation();

	const CU::Matrix44<float>& GetUIJointOrientation() const;
	const CU::Matrix44<float>& GetMuzzleJointOrientation() const;

	void PlayAnimation(ePlayerState aAnimationState);

	void AddIntention(ePlayerState aPlayerState, bool aClearIntentions);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	void ReceiveNetworkMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageHealth& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageDisplayMarker& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessageDisplayRespawn& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePressEText& aMessage, const sockaddr_in& aSenderAddress) override;

	void ReceiveNote(const UpgradeNote& aNote) override;

private:
	void UpdateJoints();

	const CU::Matrix44<float>& myInputComponentEyeOrientation;
	Prism::Instance* myModel;
	Prism::Instance* myCurrentWeaponModel;
	Prism::Instance* myPistolModel;
	Prism::Instance* myShotgunModel;
	Prism::Instance* myGrenadeLauncherModel;

	Prism::ModelProxy* myPistolProxy;
	bool myPistolHasUpdated;
	Prism::ModelProxy* myShotgunProxy;
	bool myShotgunHasUpdated;
	Prism::ModelProxy* myGrenadeLauncherProxy;
	bool myGrenadeLauncherHasUpdated;

	GUI::GUIManager3D* my3DGUIManager;

	Prism::SpriteProxy* myCrosshair;
	Prism::SpriteProxy* myDamageIndicator;
	Prism::SpriteProxy* myMarker;
	Prism::SpriteProxy* myCoOpSprite;

	CU::GrowingArray<CU::Vector3<float>> myCoOpPositions;
	CU::GrowingArray<CoOpRespawn> myCoOpRespawns;
	CU::Vector3<float> myMarkerPosition;

	bool myRenderMarker;

	struct PlayerAnimationData
	{
		Prism::AnimationData myData;
		GUIBone myUIBone;
		GUIBone myHealthBone;

		bool IsValid()
		{
			return myUIBone.IsValid() && myHealthBone.IsValid();
		}
	};

	void AddAnimation(ePlayerState aState, const std::string& aAnimationPath, bool aLoopFlag, bool aResetTimeOnRestart);
	CU::StaticArray<PlayerAnimationData, int(ePlayerState::_COUNT)> myAnimations;

	struct WeaponAnimationData
	{
		Prism::AnimationData myData;
		GUIBone myMuzzleBone;

		bool IsValid()
		{
			return myMuzzleBone.IsValid();
		}
	};

	void AddWeaponAnimation(ePlayerState aState, const std::string& aAnimationPath, bool aLoopFlag, bool aResetTimeOnRestart);
	CU::StaticArray<WeaponAnimationData, int(ePlayerState::_COUNT)> myWeaponAnimations;
	CU::Matrix44<float> myMuzzleJoint;

	ePlayerState myCurrentState;
	CU::Matrix44<float> myUIJoint;
	CU::Matrix44<float> myHealthJoint;
	bool myFirstTimeActivateAnimation;
	float myDisplayDamageIndicatorTimer;
	CU::GrowingArray<ePlayerState> myIntentions;

	int myMaxHealth;
	int myCurrentHealth;
	bool myHasDied;
	Prism::Scene* myScene;

	struct PressEText
	{
		unsigned int myGID;
		CU::Vector3<float> myPosition;

	};

	CU::GrowingArray<PressEText> myPressETexts;
};


inline eComponentType FirstPersonRenderComponent::GetTypeStatic()
{
	return eComponentType::FIRST_PERSON_RENDER;
}

inline eComponentType FirstPersonRenderComponent::GetType()
{
	return GetTypeStatic();
}


