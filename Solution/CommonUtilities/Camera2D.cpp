#include "stdafx.h"
#include "Camera2D.h"

Camera2D::Camera2D()
	: myResolutionPtr(nullptr)
	, myResolutionScale(1.f)
{
	myShaking = false;
	myShakeDuration = 0;
}

Camera2D::~Camera2D()
{
}

void Camera2D::StopShaking()
{
	myShaking = false;
	myShakeDuration = 0;
}

void Camera2D::Init(const CU::Vector2<float>* aResolutionPtr, const CU::Vector2<float>& aPosition)
{
	myResolutionPtr = aResolutionPtr;
	myProjection.myMatrix[0] = 2.f / aResolutionPtr->x;
	myProjection.myMatrix[4] = 2.f / aResolutionPtr->y;
	myOrientation.myMatrix[6] = aPosition.x;
	myOrientation.myMatrix[7] = aPosition.y;
	myResolutionScale = aResolutionPtr->y / 1024.f;
	myTargetPos = aPosition * myResolutionScale;
	myOrientation.SetPos(aPosition * myResolutionScale);
}

void Camera2D::Update(float aDelta)
{
	if (myShaking == true)
	{
		myShakeDuration -= aDelta;
		if (myShakeDuration < 0)
		{
			myShaking = false;
		}
	}

	CU::Vector2<float> pos = myOrientation.GetPos();
	pos += (myTargetPos - pos) * aDelta * 5.f;
	myOrientation.SetPos(pos);
}

void Camera2D::Shake(float aDuration)
{
	myShakeDuration = aDuration;
	myShaking = true;
}

const CU::Matrix33<float> Camera2D::GetInverse() const
{
	CU::Vector3<float> translation(-myOrientation.myMatrix[6], -myOrientation.myMatrix[7], 1);
	CU::Matrix33<float> inverse(myOrientation);
	inverse.myMatrix[6] = 0;
	inverse.myMatrix[7] = 0;
	inverse = CU::Transpose(inverse);
	translation *= inverse;
	inverse.myMatrix[6] = translation.x;
	inverse.myMatrix[7] = translation.y;
	return inverse;
}

const CU::Vector2<float> Camera2D::GetScreenOffset() const
{
	return (myOrientation.GetPos() - (*myResolutionPtr / 2.f)) / myResolutionScale;
}

void Camera2D::Move(CU::Vector3<float> aVector)
{
	aVector *= myOrientation;
	myOrientation.myMatrix[6] += aVector.x;
	myOrientation.myMatrix[7] += aVector.y;
}

void Camera2D::Zoom(float aFraction)
{
	myProjection.myMatrix[0] *= aFraction;
	myProjection.myMatrix[4] *= aFraction;
}

void Camera2D::SetProjection()
{
	myProjection.myMatrix[0] = 2.f / myResolutionPtr->x;
	myProjection.myMatrix[4] = 2.f / myResolutionPtr->y;
}