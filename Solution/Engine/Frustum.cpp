#include "stdafx.h"
#include "Frustum.h"
#include "Portal.h"

Prism::Frustum::Frustum(const CU::Matrix44<float>& anOrientation, float aNear, float aFar)
	: myOrientation(anOrientation)
	, myIntersectionFrustum(aNear, aFar)
{
	Update();
}

void Prism::Frustum::Update()
{
	myOrientationInverted = CU::InverseSimple(myOrientation);
}

void Prism::Frustum::Resize(Portal* aPortal, const CU::Matrix44<float>& aCameraOrientation, bool aDebugDraw)
{
	//bool resize(true);
	//for (int i = 0; i < 4; ++i)
	//{
	//	if (Inside(aPortal->GetPoint(i), 0) == false)
	//	{
	//		resize = false;
	//	}
	//}

	//if (resize == true)
	{
		//CU::Vector4<float> bottomLeft(CU::Vector4<float>(aPortal->GetBottomLeft(), 1.f) * myOrientationInverted);
		//CU::Vector4<float> topRight(CU::Vector4<float>(aPortal->GetTopRight(), 1.f) * myOrientationInverted);

		//if (bottomLeft.x > topRight.x)
		//{
		//	float temp = bottomLeft.x;
		//	bottomLeft.x = topRight.x;
		//	topRight.x = temp;
		//}


		int leftId(0);
		int rightId(0);


		//left = left * aProjection;
		//right = right * aProjection;

		//left /= left.w;
		//right /= right.w;

		//CU::Vector4<float> left;// *aProjection);
		//CU::Vector4<float> right;// *aProjection);
		float minAngle = FLT_MAX;
		float maxAngle = -FLT_MAX;

		for (int i = 0; i < 4; ++i)
		{
			//CU::Vector4<float> current(CU::Vector4<float>(aPortal->GetPoint(i), 1.f) * myOrientationInverted);

			//CU::Vec
			//current = current * aProjection;
			//current /= current.w;

			static CU::Vector3<float> y(0, 1.f, 0);
			static CU::Vector3<float> z(0, 0, 1.f);

			CU::Vector3<float> toPoint(aPortal->GetPoint(i) - myOrientation.GetPos());
			CU::Normalize(toPoint);

			CU::Vector3<float> forward(myOrientation.GetForward());
			//forward.y = 0;
			//CU::Normalize(forward);

			float angle = acosf(CU::Dot(forward, toPoint));
			CU::Vector3<float> cross = CU::Cross(forward, toPoint);
			if (CU::Dot(y, cross) < 0)
			{
				angle = -angle;
			}

			if (angle < minAngle)
			{
				minAngle = angle;
				leftId = i;
			}
			if (angle > maxAngle)
			{
				maxAngle = angle;
				rightId = i;
			}
		}

		CU::Vector4<float> leftCut(CU::Vector4<float>(aPortal->GetPoint(leftId), 1.f) * myOrientationInverted);
		CU::Vector4<float> rightCut(CU::Vector4<float>(aPortal->GetPoint(rightId), 1.f) * myOrientationInverted);

		bool cut = leftCut.z > 0 && rightCut.z > 0;


		if (aDebugDraw == true)
		{
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(0), eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(1), eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(2), eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(3), eColorDebug::GREEN);
		}

		if (cut && Inside(aPortal->GetPoint(leftId), 0) == true)
		{
			if (aDebugDraw == true)
			{
				Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(leftId), eColorDebug::RED);
			}
			myIntersectionFrustum.ResizeLeft(leftCut.GetVector3());
		}
		if (cut && Inside(aPortal->GetPoint(rightId), 0) == true)
		{
			if (aDebugDraw == true)
			{
				Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(rightId), eColorDebug::BLUE);
			}
			myIntersectionFrustum.ResizeRight(rightCut.GetVector3());
		}
	}
}

bool Prism::Frustum::Inside(const CU::Vector3<float>& aPosition, float aRadius) const
{
	CU::Vector4<float> position = CU::Vector4<float>(aPosition.x, aPosition.y, aPosition.z, 1.f) * myOrientationInverted;
	return myIntersectionFrustum.Inside({ position.x, position.y, position.z }, aRadius);
}

bool Prism::Frustum::Inside(const CU::Vector3<float>& aPosition, float aRadius, int& aFailPlaneOut, bool& aPointBehind) const
{
	CU::Vector4<float> position = CU::Vector4<float>(aPosition.x, aPosition.y, aPosition.z, 1.f) * myOrientationInverted;
	if (position.z < 0)
	{
		aPointBehind = true;
	}
	else
	{
		aPointBehind = false;
	}
	return myIntersectionFrustum.Inside({ position.x, position.y, position.z }, aRadius, aFailPlaneOut);
}

bool Prism::Frustum::CheckAABBInside(const CU::Vector3<float>& someMinValues, const CU::Vector3<float>& someMaxValues) const
{
	return myIntersectionFrustum.CheckAABBInside(someMinValues, someMaxValues);
}

void Prism::Frustum::OnResize(float aNearPlane, float aFarPlane)
{
	myIntersectionFrustum.OnResize(aNearPlane, aFarPlane);
	Update();
}

void Prism::Frustum::CalcWorldPlanes()
{
	myIntersectionFrustum.CalcWorldPlanes(myOrientation);
}