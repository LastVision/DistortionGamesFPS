#pragma once
#include <Matrix.h>

namespace Prism
{
	class Camera;
}

namespace CU
{
	class InputWrapper;
}

class DLLCamera
{
public:
	DLLCamera(CU::Vector2<float>& aWindowSize, float aMovementSpeed, float aRotationSpeed, float aZoomSpeed);
	~DLLCamera();

	void Zoom(float aDeltaTime, float aMouseSens);
	void Pan(float aDeltaTime, float aMouseSens);
	void Rotate(float aDeltaTime, float aMouseSens);

	void ResetCamera();

	void SetZoomSpeed(float aValue);
	float GetZoomSpeed() const;

	void SetMovementSpeed(float aValue);
	float GetMovementSpeed() const;

	void SetRotationSpeed(float aValue);
	float GetRotationSpeed() const;

	Prism::Camera* GetCamera() const;
private:
	bool HasMouseDeltaXMoved(float aSens);
	bool HasMouseDeltaYMoved(float aSens);

	Prism::Camera* myCamera;

	float myMovementSpeed;
	float myRotationSpeed;
	float myZoomSpeed;

	CU::Matrix44<float> myOrientation;
	CU::Matrix44<float> myOriginalOrientation;
};

inline void DLLCamera::SetZoomSpeed(float aValue)
{
	myZoomSpeed = aValue;
}
inline float DLLCamera::GetZoomSpeed() const
{
	return myZoomSpeed;
}

inline void DLLCamera::SetMovementSpeed(float aValue)
{
	myMovementSpeed = aValue;
}
inline float DLLCamera::GetMovementSpeed() const
{
	return myMovementSpeed;
}

inline void DLLCamera::SetRotationSpeed(float aValue)
{
	myRotationSpeed = aValue;
}
inline float DLLCamera::GetRotationSpeed() const
{
	return myRotationSpeed;
}

inline Prism::Camera* DLLCamera::GetCamera() const
{
	return myCamera;
}