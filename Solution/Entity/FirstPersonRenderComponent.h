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
	class SpriteProxy;
}

class FirstPersonRenderComponent : public Component
{
public:
	FirstPersonRenderComponent(Entity& aEntity, Prism::Scene* aScene);
	~FirstPersonRenderComponent();

	void Update(float aDelta) override;
	void Render();
	bool IsCurrentAnimationDone() const;
	void RestartCurrentAnimation();

	void PlayAnimation(ePlayerState aAnimationState);

	void AddIntention(ePlayerState aPlayerState, bool aClearIntentions);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

private:
	void UpdateJoints();

	const CU::Matrix44<float>& myInputComponentEyeOrientation;
	Prism::Instance* myModel;
	GUI::GUIManager3D* my3DGUIManager;
	Prism::SpriteProxy* myCrosshair;

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

	ePlayerState myCurrentState;
	CU::Matrix44<float> myUIJoint;
	CU::Matrix44<float> myHealthJoint;
	bool myFirstTimeActivateAnimation;

	CU::GrowingArray<ePlayerState> myIntentions;
};


inline eComponentType FirstPersonRenderComponent::GetTypeStatic()
{
	return eComponentType::FIRST_PERSON_RENDER;
}

inline eComponentType FirstPersonRenderComponent::GetType()
{
	return GetTypeStatic();
}


