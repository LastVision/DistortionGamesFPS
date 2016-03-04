#pragma once

#include <AnimationData.h>
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
}

class Player
{
public:
	Player(Prism::Scene* aScene);
	~Player();

	void Update(float aDelta);
	void UpdateAnimation(float aDelta);

	void Render();
	
	void Respawn();

	Prism::Camera* GetCamera() const;


	bool IsCurrentAnimationDone() const;
	void RestartCurrentAnimation();

	void PlayAnimation(ePlayerState aAnimationState);
	void AddIntention(ePlayerState aPlayerState, bool aClearIntentions);

private:
	Movement* myMovement;
	Shooting* myShooting;
	Health* myHealth;
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

inline Prism::Camera* Player::GetCamera() const
{
	return myCamera;
}