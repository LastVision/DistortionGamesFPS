#pragma once
#include <map>
#include "SpriteAnimation.h"

namespace Prism
{
	class SpriteProxy;

	class SpriteAnimator
	{
	public:
		SpriteAnimator(std::string aXMLPath);
		~SpriteAnimator();

		void Update(float aDelta);
		void Render(CU::Vector2<float> aPosition);

		void StartAnimation(int anID = 0);
		void RestartAnimation(int anID = 0);
		void StopAnimation();
		void ResetAnimation();

		void AddAnimation(float aFPS, int aNumberOfFrames, CU::Vector2<float> aFrameSize
			, SpriteProxy* aSpriteSheet, bool aIsLooping);

		SpriteProxy* GetSpriteFrame();
		bool IsPlayingAnimation() const;
		int GetCurrentFrame();

	private:
		float myTimeSinceLastFrame;
		int myCurrentFrame;
		CU::GrowingArray<SpriteAnimation*> myAnimations;
		SpriteAnimation *myCurrentAnimation;
		SpriteProxy* emptySprite;
	};
}