#pragma once

namespace Prism
{
	class Animation;
	class AnimationProxy
	{
		friend class AnimationSystem;
		friend class ModelLoader;
	public:
		AnimationProxy();
		~AnimationProxy();

	private:
		Animation* myAnimation;
	};
}