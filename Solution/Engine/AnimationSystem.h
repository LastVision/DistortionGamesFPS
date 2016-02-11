#pragma once

#include <unordered_map>
namespace Prism
{
	class Animation;
	class AnimationProxy;
	class AnimationSystem
	{
	public:
		static AnimationSystem* GetInstance();
		static void Destroy();

		Animation* GetAnimation(const char* aPath);

	private:
		AnimationSystem(){};
		~AnimationSystem();

		std::unordered_map<std::string, AnimationProxy*> myAnimations;

		static AnimationSystem* myInstance;
	};
}