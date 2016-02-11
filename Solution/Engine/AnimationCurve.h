#pragma once
#include <GrowingArray.h>
#include <Vector.h>


namespace Prism
{
	struct KeyFrame
	{
		float myTime;
		float myValue;
	};

	class AnimationCurve
	{
	public:
		AnimationCurve();
		~AnimationCurve();

		void AddKeyFrame(const KeyFrame& aFrame);
		float GetValueFromTime(float aTime);
		void FinalizeCurve();

	private:
		float myStartTime;
		float myEndTime;
		CU::GrowingArray<KeyFrame> myKeyFrames;
		float myFPS;
	};
}