#pragma once
#include "Component.h"

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