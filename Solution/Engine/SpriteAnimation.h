#pragma once

namespace Prism
{
	class SpriteProxy;

	class SpriteAnimation
	{
	public:

		SpriteAnimation(float aTimePerFrame, int aNumberOfFrames, CU::Vector2<float> aFrameSize, SpriteProxy* aSpriteSheet, bool aIsLooping);
		~SpriteAnimation();

		float GetTimePerFrame() const;
		int GetNumberOfFrames() const;
		int GetFramesPerRow() const;

		SpriteProxy* GetSpriteSheet();

		const CU::Vector2<float>& GetFrameSize() const;
		const CU::Vector2<float>& GetSpriteSheetSize() const;

		bool IsLooping() const;

	private:

		CU::Vector2<float> mySpriteSheetSize;
		CU::Vector2<float> myFrameSize;

		float myTimePerFrame;

		int myNumberOfFrames;
		int myFramesPerRow;

		SpriteProxy* mySpriteSheet;

		bool myIsLooping;
	};

	inline const CU::Vector2<float>& SpriteAnimation::GetFrameSize() const
	{
		return myFrameSize;
	}

	inline const CU::Vector2<float>& SpriteAnimation::GetSpriteSheetSize() const
	{
		return mySpriteSheetSize;
	}

	inline int SpriteAnimation::GetFramesPerRow() const
	{
		return myFramesPerRow;
	}

	inline bool SpriteAnimation::IsLooping() const
	{
		return myIsLooping;
	}
}