#pragma once

#include <Vector.h>
#include "TransformationNode.h"

namespace Prism
{
	struct CurveAnimationFrame
	{
		CurveAnimationFrame()
			: myAngle(0.f)
		{}
		float myAngle;
		CU::Vector3<float> myDirection;
		CU::Vector3<float> myTranslation;
	};

	enum class AnimationType
	{
		ABSOLUTE_TRANSFORM,
		RELATIVE_TRANSFORM
	};

	class AnimationCurve;

	class TransformationNodeCurves : public TransformationNode
	{
	public:
		TransformationNodeCurves();
		~TransformationNodeCurves();

		void Init(AnimationType aAnimationType, int aNumberOfKeyFrames
			, int aAnimationFPS, float aStartTime, float aEndTime);

		void SetBaseFrame(const CU::Vector3<float>& aDirection, const CU::Vector3<float>& aTranslation, float aAngle);
		void SetRotationCurve(int aIndex, AnimationCurve* aCurve);
		void SetTranslationCurve(int aIndex, AnimationCurve* aCurve);

		//CurveAnimationFrame GetFrameForTime(float aElapsedTime);
		const CU::Matrix44<float> GetTransformationForTime(float aTime) override;


	private:
		int myAnimationFPS;
		AnimationType myAnimationType;
		float myStartTime;
		float myEndTime;
		AnimationCurve* myRotationCurves[4];
		AnimationCurve* myTranslationCurves[3];
		int myNumberOfKeyFrames;
		CurveAnimationFrame myBaseFrame;
	};
}