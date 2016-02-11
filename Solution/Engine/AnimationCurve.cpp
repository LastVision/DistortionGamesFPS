#include "stdafx.h"

#include "AnimationCurve.h"
#include <MathHelper.h>

namespace Prism
{
	AnimationCurve::AnimationCurve()
		: myKeyFrames(4)
		, myStartTime(-1)
		, myEndTime(-1)
		, myFPS(0.f)
	{
	}


	AnimationCurve::~AnimationCurve()
	{
	}

	void AnimationCurve::AddKeyFrame(const KeyFrame& aFrame)
	{
		myKeyFrames.Add(aFrame);
	}

	float AnimationCurve::GetValueFromTime(float aTime)
	{
		if (aTime < myStartTime)
		{
			return myKeyFrames[0].myValue;
		}
		if (aTime > myEndTime)
		{
			return myKeyFrames.GetLast().myValue;
		}

		float duration = myEndTime - 0;
		float timeInAnim = aTime - 0;

		float position = timeInAnim / duration;

		int posInArray = int(position * float(myKeyFrames.Size() / myFPS));

		if (posInArray + 1 < myKeyFrames.Size())
		{
			float min = myKeyFrames[posInArray].myValue;
			float max = myKeyFrames[posInArray + 1].myValue;

			float alpha = myStartTime + timeInAnim - myKeyFrames[posInArray].myTime;
			return CU::Math::Lerp<float>(min, max, alpha);
		}

		return myKeyFrames[posInArray].myValue;
	}

	void AnimationCurve::FinalizeCurve()
	{
		myStartTime = myKeyFrames[0].myTime;
		myEndTime = myKeyFrames.GetLast().myTime;
		myFPS = (myEndTime - myStartTime) / (myKeyFrames.Size() - 1);
	}
}