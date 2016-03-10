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

void Prism::Frustum::Resize(Portal* aPortal, bool aDebugDraw)
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

		CU::Vector4<float> left(CU::Vector4<float>(aPortal->GetPoint(0), 1.f) * myOrientationInverted);
		CU::Vector4<float> right(CU::Vector4<float>(aPortal->GetPoint(0), 1.f) * myOrientationInverted);

		for (int i = 1; i < 4; ++i)
		{
			CU::Vector4<float> current(CU::Vector4<float>(aPortal->GetPoint(i), 1.f) * myOrientationInverted);
			if (current.x < left.x)
			{
				leftId = i;
				left = current;
			}
			if (current.x > right.x)
			{
				rightId = i;
				right = current;
			}
		}

		if (aDebugDraw == true)
		{
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(0), eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(1), eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(2), eColorDebug::GREEN);
			Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(3), eColorDebug::GREEN);
		}

		if (Inside(aPortal->GetPoint(leftId), 0) == true)
		{
			if (aDebugDraw == true)
			{
				Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(leftId), eColorDebug::RED);
			}
			myIntersectionFrustum.ResizeLeft(left.GetVector3());
		}
		if (Inside(aPortal->GetPoint(rightId), 0) == true)
		{
			if (aDebugDraw == true)
			{
				Prism::DebugDrawer::GetInstance()->RenderLine3D(myOrientation.GetPos() + myOrientation.GetForward(), aPortal->GetPoint(rightId), eColorDebug::BLUE);
			}
			myIntersectionFrustum.ResizeRight(right.GetVector3());
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

void Prism::Frustum::OnResize(float aNearPlane, float aFarPlane)
{
	myIntersectionFrustum.OnResize(aNearPlane, aFarPlane);
	Update();
}