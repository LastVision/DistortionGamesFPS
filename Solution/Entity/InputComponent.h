#pragma once
#include <AnimationData.h>
#include "Component.h"
#include <GameEnum.h>
#include <StaticArray.h>
#include <BoneName.h>

class Movement;
class Shooting;
class Health;

namespace GUI
{
	class GUIManager3D;
}

namespace Prism
{
	class Camera;
	class SpriteProxy;
	class Instance;
}

class InputComponent : public Component
{
public:
	InputComponent(Entity& anEntity, Prism::Scene* aScene);
	~InputComponent();

	void Update(float aDelta) override;
	void UpdateAnimation(float aDelta);

	void Render();

	void Respawn();

	Prism::Camera* GetCamera() const;


	bool IsCurrentAnimationDone() const;
	void RestartCurrentAnimation();

	void PlayAnimation(ePlayerState aAnimationState);
	void AddIntention(ePlayerState aPlayerState, bool aClearIntentions);

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	const CU::Matrix44<float>& GetEyeOrientation() const;

private:
	Movement* myMovement;
	Shooting* myShooting;
	GUI::GUIManager3D* my3DGUIManager;
	Prism::Camera* myCamera;
	CU::Matrix44<float> myOrientation;
	CU::Matrix44<float> myEyeOrientation;
	Prism::SpriteProxy* myCrosshair;
	Prism::Instance* myModel;
	float myHeight;
	float myCrouchHeight;
	float myJumpAcceleration;
	float myJumpOffset;
	float mySendTime;
	bool myAlive;
	CU::Vector3<float> mySpawnPosition;

	struct PlayerAnimationData
	{
		Prism::AnimationData myData;
		GUIBone myUIBone;
		GUIBone myHealthBone;
	};

	void AddAnimation(ePlayerState aState, const std::string& aAnimationPath, bool aLoopFlag, bool aResetTimeOnRestart);
	CU::StaticArray<PlayerAnimationData, int(ePlayerState::_COUNT)> myAnimations;

	ePlayerState myCurrentState;


	CU::GrowingArray<ePlayerState> myIntentions;
};

inline eComponentType InputComponent::GetTypeStatic()
{
	return eComponentType::INPUT;
}

inline eComponentType InputComponent::GetType()
{
	return GetTypeStatic();
}

inline const CU::Matrix44<float>& InputComponent::GetEyeOrientation() const
{
	return myEyeOrientation;
}

