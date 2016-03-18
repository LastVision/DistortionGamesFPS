#pragma once
#include <Matrix44.h>

namespace Prism
{
	class Frustum;

	class Camera
	{
	public:
		Camera(CU::Matrix44f& aPlayerMatrix);
		Camera(CU::Matrix44f& aPlayerMatrix, float aWidth, float aHeight);
		~Camera();

		void ReadXML(const std::string& aFileName);

		void OnResize(const int aWidth, const int aHeight);
		void InitShadowCamera(float aWidth, float aHeight);

		const CU::Matrix44<float>& GetOrientation() const;
		const CU::Matrix44<float>& GetProjection() const;
		const CU::Matrix44<float>& GetViewProjection() const;

		void Update(float aDeltaTime);

		const Frustum& GetFrustum() const;

		void SetOrientation(const CU::Matrix44<float>& aOrientation);
		void SetPosition(const CU::Vector3<float>& aPosition);

		void RotateX(const float aDegrees);
		void RotateY(const float aDegrees);
		void RotateZ(const float aDegrees);

		void MoveForward(const float aDistance);
		void MoveRight(const float aDistance);
		void MoveUp(const float aDistance);

		void ShakeCamera(float aRotationRate, float aMaxRotation, float aTime);

		CU::Vector3<float> RayCast(const CU::Vector2<float>& aMousePosition) const;

	private:
		void operator= (const Camera&) = delete;

		CU::Matrix44<float>& myOrientation;
		CU::Matrix44<float> myProjectionMatrix;
		CU::Matrix44<float> myProjectionMatrixNonInverted; //used to go from projection to view space
		CU::Matrix44<float> myViewProjectionMatrix;
		float myNear;
		float myFar;
		float myFOV;
		Frustum* myFrustum;

		bool myShakeCamera;
		float myRotateRate;
		float myMaxShake;
		float myCurrentShake;
		float myMaxShakeTime;
		float myCurrentShakeTime;

	};

	inline const Frustum& Camera::GetFrustum() const
	{
		return *myFrustum;
	}
}