#include "stdafx.h"
#include "Animation.h"
#include "AnimationProxy.h"
#include "AnimationSystem.h"
#include "ModelLoader.h"


namespace Prism
{
	AnimationSystem* AnimationSystem::myInstance = nullptr;

	AnimationSystem* AnimationSystem::GetInstance()
	{
		if (myInstance == nullptr)
		{
			myInstance = new AnimationSystem();
		}

		return myInstance;
	}

	void AnimationSystem::Destroy()
	{
		SAFE_DELETE(myInstance);
	}

	Animation* AnimationSystem::GetAnimation(const char* aPath)
	{
		if (myAnimations.find(aPath) == myAnimations.end())
		{
			myAnimations[aPath] = ModelLoader::GetInstance()->LoadAnimation(aPath);
		}

		return myAnimations[aPath]->myAnimation;
	}

	AnimationSystem::~AnimationSystem()
	{
		for (auto it = myAnimations.begin(); it != myAnimations.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}

		myAnimations.clear();
	}
}