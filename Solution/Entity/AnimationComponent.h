#pragma once
#include "Component.h"
#include "BoneName.h"

namespace Prism
{
	class Animation;
	class Instance;
	struct MeshData;
	class Camera;
	class Texture;
	class Terrain;
};

struct AnimationComponentData;

class AnimationComponent : public Component
{
public:
	AnimationComponent(Entity& aEntity, const AnimationComponentData& aComponentData);
	~AnimationComponent();

	void Reset() override;

	void AddAnimation(eEntityState aState, const std::string& aAnimationPath, bool aLoopFlag, bool aResetTimeOnRestart);

	void Update(float aDeltaTime);
	Prism::Instance* GetInstance();
	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	bool IsCurrentAnimationDone() const;
	void RestartCurrentAnimation();

	void SetRotation(const CU::Vector3<float>& aRotation);
	void SetScale(const CU::Vector3<float>& aScale);

	Prism::Animation* GetCurrent() const;

	void AddWeaponToScene(Prism::Scene* aScene);
	void RemoveWeaponFromScene(Prism::Scene* aScene);

private:
	void PlayAnimation(eEntityState aAnimationState);
	struct AnimationData
	{
		AnimationData() 
			: myFile("Not loaded")
			, myShouldLoop(false)
			, myResetTimeOnRestart(false)
			, myElapsedTime(0)
		{}
		std::string myFile;
		bool myShouldLoop;
		bool myResetTimeOnRestart;
		float myElapsedTime;
	};



	Prism::Instance* myInstance;
	float myCullingRadius;
	CU::StaticArray<AnimationData, int(eEntityState::_COUNT)> myAnimations;
	eEntityState myPrevEntityState;

	const AnimationComponentData myComponentData;


	Prism::Instance* myWeapon;
	bool myIsEnemy;
	CU::Matrix44<float> myWeaponJoint;
	struct EnemyAnimationBone
	{
		GUIBone myWeaponBone;

		bool IsValid()
		{
			return myWeaponBone.IsValid();
		}
	};
	CU::StaticArray<EnemyAnimationBone, int(eEntityState::_COUNT)> myEnemyAnimations;
};

inline Prism::Instance* AnimationComponent::GetInstance()
{
	return myInstance;
}

inline eComponentType AnimationComponent::GetTypeStatic()
{
	return eComponentType::ANIMATION;
}

inline eComponentType AnimationComponent::GetType()
{
	return GetTypeStatic();
}