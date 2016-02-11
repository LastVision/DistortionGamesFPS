#include "stdafx.h"

#include "AnimationCurve.h"
#include "TransformationNodeCurves.h"

namespace Prism
{
	TransformationNodeCurves::TransformationNodeCurves()
		: myAnimationFPS(0)
		, myAnimationType(AnimationType::ABSOLUTE_TRANSFORM)
		, myStartTime(0.f)
		, myEndTime(0.f)
		, myNumberOfKeyFrames(0)
	{
		myRotationCurves[0] = nullptr;
		myRotationCurves[1] = nullptr;
		myRotationCurves[2] = nullptr;
		myRotationCurves[3] = nullptr;
		myTranslationCurves[0] = nullptr;
		myTranslationCurves[1] = nullptr;
		myTranslationCurves[2] = nullptr;
	}


	TransformationNodeCurves::~TransformationNodeCurves()
	{
	}


	void TransformationNodeCurves::Init(AnimationType aAnimationType, int aNumberOfKeyFrames
		, int aAnimationFPS, float aStartTime, float aEndTime)
	{
		myStartTime = aStartTime;
		myEndTime = aEndTime;
		myAnimationFPS = aAnimationFPS;
		myAnimationType = aAnimationType;
		myNumberOfKeyFrames = aNumberOfKeyFrames;
	}

	void TransformationNodeCurves::SetBaseFrame(const CU::Vector3<float>& aDirection
		, const CU::Vector3<float>& aTranslation, float aAngle)
	{
		myBaseFrame.myDirection = aDirection;
		myBaseFrame.myTranslation = aTranslation;
		myBaseFrame.myAngle = aAngle;
	}

	void TransformationNodeCurves::SetRotationCurve(int aIndex, AnimationCurve* aCurve)
	{
		DL_ASSERT_EXP(aIndex <= 3, "Tried to SetRotationCurve to an index higher than 3");
		myRotationCurves[aIndex] = aCurve;
	}

	void TransformationNodeCurves::SetTranslationCurve(int aIndex, AnimationCurve* aCurve)
	{
		DL_ASSERT_EXP(aIndex <= 3, "Tried to SetTranslationCurve to an index higher than 2");
		myTranslationCurves[aIndex] = aCurve;
	}


	//TransformationNodeCurves::CurveAnimationFrame GetFrameForTime(double aElapsedTime);
	const CU::Matrix44<float> TransformationNodeCurves::GetTransformationForTime(float aTime)
	{
		CU::Matrix44<float> matrix;

		matrix *= CU::Matrix44<float>::CreateRotateAroundX(myRotationCurves[0]->GetValueFromTime(aTime) * 3.14f / 180.f);
		matrix *= CU::Matrix44<float>::CreateRotateAroundY(myRotationCurves[1]->GetValueFromTime(aTime) * 3.14f / 180.f);
		matrix *= CU::Matrix44<float>::CreateRotateAroundZ(-myRotationCurves[2]->GetValueFromTime(aTime) * 3.14f / 180.f);

		CU::Vector3<float> translation(
			-myTranslationCurves[0]->GetValueFromTime(aTime),
			myTranslationCurves[1]->GetValueFromTime(aTime),
			myTranslationCurves[2]->GetValueFromTime(aTime));

		matrix.SetPos(translation);

		return matrix;
	}
}