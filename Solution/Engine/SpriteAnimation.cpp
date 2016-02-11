#include "stdafx.h"
#include "SpriteAnimation.h"
#include "SpriteProxy.h"

namespace Prism
{
	SpriteAnimation::SpriteAnimation(float aTimePerFrame, int aNumberOfFrames, CU::Vector2<float> aFrameSize, SpriteProxy* aSpriteSheet, bool aIsLooping)
	{
		myTimePerFrame = aNumberOfFrames / aTimePerFrame;
		myNumberOfFrames = aNumberOfFrames;
		mySpriteSheet = aSpriteSheet;
		mySpriteSheetSize = mySpriteSheet->GetSize();
		myIsLooping = aIsLooping;
		myFrameSize = aFrameSize;

		myFramesPerRow = int(mySpriteSheetSize.x / myFrameSize.x);
		//myFrameSize.x = mySpriteSheet->GetSize().x / myFramesPerRow;
		//myFrameSize.y = mySpriteSheet->GetSize().y / aFramesPerColumn;
	}

	SpriteAnimation::~SpriteAnimation()
	{
		SAFE_DELETE(mySpriteSheet);
	}

	float SpriteAnimation::GetTimePerFrame() const
	{
		return myTimePerFrame;
	}

	int SpriteAnimation::GetNumberOfFrames() const
	{
		return myNumberOfFrames;
	}

	SpriteProxy* SpriteAnimation::GetSpriteSheet()
	{
		return mySpriteSheet;
	}
}