#include "stdafx.h"
#include "Camera.h"
#include "Frustum.h"
#include <xnamath.h>
#include <XMLReader.h>
#include <InputWrapper.h>
namespace Prism
{
	Camera::Camera(CU::Matrix44f& aPlayerMatrix)
		: myOrientation(aPlayerMatrix)
		, myNear(0.1f)
		, myFar(128.f)
		, myShakeCamera(false)
		, myCurrentShake(0.f)
		, myMaxShake(0.f)
		, myRotateRate(0.f)
		, myMaxShakeTime(0.f)
		, myCurrentShakeTime(0.f)
	{
		myFrustum = new Frustum(aPlayerMatrix, myNear, myFar);
		ReadXML("Data/Setting/SET_camera.xml");
		OnResize(Engine::GetInstance()->GetWindowSizeInt().x, Engine::GetInstance()->GetWindowSizeInt().y);
	}

	Camera::Camera(CU::Matrix44f& aPlayerMatrix, float aWidth, float aHeight)
		: myOrientation(aPlayerMatrix)
		, myNear(0.1f)
		, myFar(128.f)
		, myShakeCamera(false)
		, myCurrentShake(0.f)
		, myMaxShake(0.f)
		, myRotateRate(0.f)
		, myMaxShakeTime(0.f)
		, myCurrentShakeTime(0.f)
	{
		myFrustum = new Frustum(aPlayerMatrix, myNear, myFar);
		ReadXML("Data/Setting/SET_camera.xml");
		OnResize(int(aWidth), int(aHeight));
	}

	Camera::~Camera()
	{
		delete myFrustum;
	}

	void Camera::ReadXML(const std::string& aFileName)
	{
		XMLReader reader;
		reader.OpenDocument(aFileName);
		tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("camera");
		reader.ForceReadAttribute(levelElement, "fov", myFOV);
		reader.ForceReadAttribute(levelElement, "nearplane", myNear);
		reader.ForceReadAttribute(levelElement, "farplane", myFar);
		myFOV *= 3.14159f / 180.f;
		
		reader.CloseDocument();
	}

	void Camera::OnResize(const int aWidth, const int aHeight)
	{
		myProjectionMatrix = CU::Matrix44<float>::CreateProjectionMatrixLH(myNear, myFar, static_cast<float>(aHeight) / static_cast<float>(aWidth), myFOV);
		mySpecialFoVProjectionMatrix = CU::Matrix44<float>::CreateProjectionMatrixLH(myNear, myFar, static_cast<float>(aHeight) / static_cast<float>(aWidth), 1.04719755f);
		myProjectionMatrixNonInverted = CU::InverseSimple(myProjectionMatrix);
		if (myFrustum != nullptr)
		{
			myFrustum->OnResize(myNear, myFar);
		}
	}

	void Camera::InitShadowCamera(float aWidth, float aHeight)
	{
		aWidth;
		aHeight;
		myFar = 1000.f;
		myProjectionMatrix = CU::Matrix44<float>::CreateOrthogonalMatrixLH(90.f, 90.f, myNear, myFar);
		myProjectionMatrixNonInverted = CU::InverseSimple(myProjectionMatrix);
		myFrustum->OnResize(myNear, myFar);
	}

	CU::Matrix44<float> Camera::GetOrientation() const
	{
		return GC::View;
	}

	const CU::Matrix44<float>& Camera::GetProjection() const
	{
		return myProjectionMatrix;
	}

	CU::Matrix44<float> Camera::GetViewProjection() const
	{
		//return GC::View * myProjectionMatrix;
		return myViewProjectionMatrix;
	}

	const CU::Matrix44<float>& Camera::GetSpecialFoVViewProjection() const
	{
		return myViewProjectionMatrix;

		//return mySpecialFoVViewProjectionMatrix;
	}

	void Camera::Update(float aDeltaTime)
	{
		if (myShakeCamera == true)
		{
			if (myCurrentShake < -myMaxShake || myCurrentShake > myMaxShake)
			{
				myRotateRate = -myRotateRate;
			}
			myCurrentShake += myRotateRate * aDeltaTime;

			myCurrentShakeTime += aDeltaTime;
			if (myCurrentShakeTime >= myMaxShakeTime)
			{
				myShakeCamera = false;
				myCurrentShakeTime = 0.f;
				myCurrentShake = 0.f;
			}
			RotateZ(myCurrentShake * aDeltaTime);
		}

		//myViewProjectionMatrix = CU::InverseSimple(myOrientation) * myProjectionMatrix;
		mySpecialFoVOrientation = myOrientation;
		CU::Vector3<float> specialPos(mySpecialFoVOrientation.GetPos());
		specialPos -= mySpecialFoVOrientation.GetForward() * 0.375f;
		specialPos -= mySpecialFoVOrientation.GetUp() * 0.08f;
		mySpecialFoVOrientation.SetPos(specialPos);
		mySpecialFoVViewProjectionMatrix = CU::InverseSimple(mySpecialFoVOrientation) * mySpecialFoVProjectionMatrix;
		myFrustum->Update();

		GC::CameraOrientation = myOrientation;
	}


	void Camera::SetViewProjection(const CU::Matrix44<float>& aViewProjection)
	{
		myViewProjectionMatrix = aViewProjection;
	}

	void Camera::SetProjection(const CU::Matrix44<float>& aProjection)
	{
		myProjectionMatrix = aProjection;
	}

	void Camera::SetOrientation(const CU::Matrix44<float>& aOrientation)
	{
		myOrientation = aOrientation;
	}

	void Camera::SetPosition(const CU::Vector3<float>& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	void Camera::RotateX(const float aDegrees)
	{
		CU::Vector3<float> position = myOrientation.GetPos();
		myOrientation.SetPos({ 0.f, 0.f, 0.f, 1.f });
		myOrientation = CU::Matrix44<float>::CreateRotateAroundX(aDegrees * XM_PI / 180.f) * myOrientation;
		myOrientation.SetPos(position);
	}

	void Camera::RotateY(const float aDegrees)
	{
		CU::Vector3<float> position = myOrientation.GetPos();
		myOrientation.SetPos({ 0.f, 0.f, 0.f, 1.f });
		myOrientation = CU::Matrix44<float>::CreateRotateAroundY(aDegrees * XM_PI / 180.f) * myOrientation;
		myOrientation.SetPos(position);
	}

	void Camera::RotateZ(const float aDegrees)
	{
		CU::Vector3<float> position = myOrientation.GetPos();
		myOrientation.SetPos({ 0.f, 0.f, 0.0f, 1.f });
		myOrientation = CU::Matrix44<float>::CreateRotateAroundZ(aDegrees * XM_PI / 180.f) * myOrientation;
		myOrientation.SetPos(position);
	}

	void Camera::MoveForward(const float aDistance)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetForward() * aDistance);
	}

	void Camera::MoveRight(const float aDistance)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetRight() * aDistance);
	}

	void Camera::MoveUp(const float aDistance)
	{
		myOrientation.SetPos(myOrientation.GetPos() + myOrientation.GetUp() * aDistance);
	}

	void Camera::ShakeCamera(float aRotationRate, float aMaxRotation, float aTime)
	{
		if (myShakeCamera == false)
		{
			myRotateRate = aRotationRate;
			myMaxShake = aMaxRotation;
			myMaxShakeTime = aTime;
			myCurrentShake = 0.f;
			myCurrentShakeTime = 0.f;
			myShakeCamera = true;
		}
	}

	CU::Vector3<float> Camera::RayCast(const CU::Vector2<float>& aMousePosition) const
	{
		if (myOrientation.GetPos().y < 0.f || myOrientation.GetForward().y >= 0.f)
		{
			return CU::Vector3<float>();
		}
		CU::Vector2<float> mouse2D(aMousePosition);
		mouse2D *= 2.f;
		mouse2D -= 1.f;

		CU::Vector4<float> mouseNear(0, 0, 0, 0.f);
		CU::Vector4<float> mouseFar(mouse2D.x, mouse2D.y, 1.f, 0.f);

		mouseNear = mouseNear * myProjectionMatrixNonInverted;
		mouseNear = mouseNear * myOrientation;
		mouseFar = mouseFar * myProjectionMatrixNonInverted;
		mouseFar = mouseFar * myOrientation;

		CU::Vector4<float> ray(CU::GetNormalized<float>(mouseFar - mouseNear));
		
		CU::Vector3<float> intersection(1.f, 0.f, 0.f);
		float d = 0.f;
		float denom = CU::Dot(ray, CU::Vector4<float>(0.f, 1.f, 0.f, 0.f));
		float t = 1.f;
		if (denom != 0.f)
		{
			t = (d - CU::Dot(myOrientation.GetPos4(), CU::Vector4<float>(0.f, 1.f, 0.f, 0.f ))) / (denom);
		}

		intersection = myOrientation.GetPos() + CU::Vector3<float>(ray.x, ray.y, ray.z) * t;

		return intersection;
	}

	CU::Vector2<float> Camera::ConvertTo2D(const CU::Vector3<float>& aPostion)
	{
		CU::Matrix44<float> renderPos;
		renderPos.SetPos(aPostion);
		renderPos = renderPos * CU::InverseSimple(myOrientation);
		renderPos = renderPos * GetProjection();

		CU::Vector3<float> newRenderPos = renderPos.GetPos();
		CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

		newRenderPos /= renderPos.GetPos4().w;
		newRenderPos += 1.f;
		newRenderPos *= 0.5f;
		newRenderPos.x *= windowSize.x;
		newRenderPos.y *= windowSize.y;
		newRenderPos.y += 50.f;
		return { newRenderPos.x, newRenderPos.y };
	}
}