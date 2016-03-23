#pragma once
#include "Vector.h"
#include "Sphere.h"
#include "AABB.h"
#include "LineSegment2D.h"
#include "LineSegment3D.h"
#include "Plane.h"
#include "Line.h"


namespace CU
{
	namespace Intersection
	{
		//2D
		bool LineSegmentVsLineSegment(const LineSegment2D& aLine1, const LineSegment2D& aLine2, CU::Vector2<float>& aIntersectionPoint);
		bool LineVsLineSegment(const LineSegment2D& aLine, const LineSegment2D& aLineSegment, CU::Vector2<float>& aIntersectionPoint);
		bool PointInsideTriangle(const Vector2<float>& aPoint, const Vector2<float>& aTriangle1, const Vector2<float>& aTriangle2, const Vector2<float>& aTriangle3);
		bool PointVsRect(Vector2<float> aPoint, Vector2<float> aRectTopLeft, Vector2<float> aRectBottomRight);
		bool CircleVsCircle(Vector2<float> aCenter1, float aRadius1, Vector2<float> aCenter2, float aRadius2);
		bool CircleVsRect(Vector2<float> aCenter, float aRadius, Vector2<float> aRectTopLeft, Vector2<float> aRectBottomRight);

		//3D
		bool AABBvsAABB(const AABB& anAABB1, const AABB& anAABB2);
		bool AABBvsAABB(const Vector3<float>& aMin1, const Vector3<float>& aMax1, const Vector3<float>& aMin2, const Vector3<float>& aMax2);
		bool PointInsideSphere(Sphere aSphere, CU::Vector3<float> aPoint);
		bool SphereVsSphere(Sphere aSphere, Sphere aOtherSphere);
		bool PointInsideAABB(const AABB& aAABB, const CU::Vector3<float>& aPoint);
		bool SphereInsideAABB(const AABB& aAABB, const CU::Vector3<float>& aPoint, float aRadius);
		bool LineVsSphere(LineSegment3D aLineSegment3D, Sphere aSphere, CU::Vector3<float>& aIntersectionPoint);
		int PlaneVsSphere(CU::Plane<float> aPlane, Sphere aSphere);
		bool SwepthSphereVsSphere(LineSegment3D aLineSegment3D, float aRadius, Sphere aSphere);
		bool SwepthSphereVsAABB(LineSegment3D aLineSegment3D, float aRadius, AABB aAABB);
		bool LineVsAABB(LineSegment3D aLineSegment3D, AABB aAABB, CU::Vector3<float>& aIntersectoinPoint);
		double perpDot(const CU::Vector2<float>& a, const CU::Vector2<float>& b);
	}


	inline bool Intersection::CircleVsCircle(Vector2<float> aCenter1, float aRadius1, Vector2<float> aCenter2, float aRadius2)
	{
		if (CU::Length2(aCenter2 - aCenter1) <= (aRadius1 + aRadius2) * (aRadius1 + aRadius2))
		{
			return true;
		}
		return false;
	}

	static float sign(const Vector2<float>& p1, const Vector2<float>& p2, const Vector2<float>& p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}

	inline bool Intersection::PointInsideTriangle(const Vector2<float>& aPoint, const Vector2<float>& aTriangle1, const Vector2<float>& aTriangle2, const Vector2<float>& aTriangle3)
	{
		bool b1, b2, b3;

		b1 = sign(aPoint, aTriangle1, aTriangle2) < 0.0f;
		b2 = sign(aPoint, aTriangle2, aTriangle3) < 0.0f;
		b3 = sign(aPoint, aTriangle3, aTriangle1) < 0.0f;

		return ((b1 == b2) && (b2 == b3));
	}

	inline bool Intersection::PointVsRect(Vector2<float> aPoint, Vector2<float> aRectTopLeft, Vector2<float> aRectBottomRight)
	{
		if (aPoint.x < aRectTopLeft.x) return false;
		if (aPoint.y < aRectTopLeft.y) return false;
		if (aPoint.x > aRectBottomRight.x) return false;
		if (aPoint.y > aRectBottomRight.y) return false;

		return true;
	}

	inline bool Intersection::CircleVsRect(Vector2<float> aCenter, float aRadius, Vector2<float> aRectTopLeft, Vector2<float> aRectBottomRight)
	{
		return PointVsRect(aCenter, Vector2<float>(aRectTopLeft.x - aRadius, aRectTopLeft.y - aRadius), Vector2<float>(aRectBottomRight.x + aRadius, aRectBottomRight.y + aRadius));
	}

	inline bool Intersection::LineSegmentVsLineSegment(const LineSegment2D& aLine1, const LineSegment2D& aLine2, CU::Vector2<float>& aIntersectionPoint)
	{
		CU::Line<float> line1(aLine1.myStartPos, aLine1.myEndPos);
		CU::Line<float> line2(aLine2.myStartPos, aLine2.myEndPos);


		float numX = line2.myABC.y * line1.myABC.z - line1.myABC.y * line2.myABC.z;
		float denX = line1.myABC.x * line2.myABC.y - line2.myABC.x * line1.myABC.y;

		float numY = line1.myABC.x * line2.myABC.z - line2.myABC.x * line1.myABC.z;
		float denY = line1.myABC.x * line2.myABC.y - line2.myABC.x * line1.myABC.y;

		if (denX == 0 || denY == 0)
		{
			if (line1.myABC.z != line2.myABC.z)
			{
				return false;
			}

			// check segment overlap

			CU::Vector2<float> dir1 = aLine1.myEndPos - aLine1.myStartPos;
			CU::Vector2<float> dir2 = aLine2.myEndPos - aLine2.myStartPos;

			float lengthDir1 = CU::Length(dir1);
			dir1 /= lengthDir1;

			float lengthDir2 = CU::Length(dir2);
			dir2 /= lengthDir2;

			if (CU::Dot(dir1, aLine2.myEndPos - aLine1.myStartPos) < lengthDir1 &&
				CU::Dot(dir1, aLine2.myEndPos - aLine1.myStartPos) > 0)
			{
				aIntersectionPoint = aLine2.myEndPos;
				return true;
			}

			if (CU::Dot(dir1, aLine2.myStartPos - aLine1.myStartPos) < lengthDir1 &&
				CU::Dot(dir1, aLine2.myStartPos - aLine1.myStartPos) > 0)
			{
				aIntersectionPoint = aLine2.myStartPos;
				return true;
			}

			if (CU::Dot(dir2, aLine1.myEndPos - aLine2.myStartPos) < lengthDir2 &&
				CU::Dot(dir2, aLine1.myEndPos - aLine2.myStartPos) > 0)
			{
				aIntersectionPoint = aLine1.myEndPos;
				return true;
			}

			if (CU::Dot(dir2, aLine1.myStartPos - aLine2.myStartPos) < lengthDir2 &&
				CU::Dot(dir2, aLine1.myStartPos - aLine2.myStartPos) > 0)
			{
				aIntersectionPoint = aLine1.myStartPos;
				return true;
			}

			return false;
		}

		aIntersectionPoint = CU::Vector2<float>(numX / denX, numY / denY);


		CU::Vector2<float> v1(aLine1.myEndPos - aLine1.myStartPos);

		CU::Vector2<float> v1intersect(aIntersectionPoint - aLine1.myStartPos);

		if (CU::Dot(v1, v1intersect) < 0 || CU::Length2(v1intersect) > CU::Length2(v1))
		{
			return false;
		}

		CU::Vector2<float> v2(aLine2.myEndPos - aLine2.myStartPos);

		CU::Vector2<float> v2intersect(aIntersectionPoint - aLine2.myStartPos);

		if (CU::Dot(v2, v2intersect) < 0 || CU::Length2(v2intersect) > CU::Length2(v2))
		{
			return false;
		}

		return true;
	}

	inline bool Intersection::LineVsLineSegment(const LineSegment2D& aLine, const LineSegment2D& aLineSegment, CU::Vector2<float>& aIntersectionPoint)
	{
		CU::Line<float> line1(aLine.myStartPos, aLine.myEndPos);
		CU::Line<float> line2(aLineSegment.myStartPos, aLineSegment.myEndPos);


		float numX = line2.myABC.y * line1.myABC.z - line1.myABC.y * line2.myABC.z;
		float denX = line1.myABC.x * line2.myABC.y - line2.myABC.x * line1.myABC.y;

		float numY = line1.myABC.x * line2.myABC.z - line2.myABC.x * line1.myABC.z;
		float denY = line1.myABC.x * line2.myABC.y - line2.myABC.x * line1.myABC.y;

		if (denX == 0 || denY == 0)
		{
			if (line1.myABC.z != line2.myABC.z)
			{
				return false;
			}

			// check segment overlap

			CU::Vector2<float> dir1 = aLine.myEndPos - aLine.myStartPos;
			CU::Vector2<float> dir2 = aLineSegment.myEndPos - aLineSegment.myStartPos;

			float lengthDir1 = CU::Length(dir1);
			dir1 /= lengthDir1;

			float lengthDir2 = CU::Length(dir2);
			dir2 /= lengthDir2;

			if (CU::Dot(dir1, aLineSegment.myEndPos - aLine.myStartPos) < lengthDir1 &&
				CU::Dot(dir1, aLineSegment.myEndPos - aLine.myStartPos) > 0)
			{
				aIntersectionPoint = aLineSegment.myEndPos;
				return true;
			}

			if (CU::Dot(dir1, aLineSegment.myStartPos - aLine.myStartPos) < lengthDir1 &&
				CU::Dot(dir1, aLineSegment.myStartPos - aLine.myStartPos) > 0)
			{
				aIntersectionPoint = aLineSegment.myStartPos;
				return true;
			}

			aIntersectionPoint = aLineSegment.myStartPos;
			return true;
		}

		aIntersectionPoint = CU::Vector2<float>(numX / denX, numY / denY);


		//CU::Vector2<float> v1(aLine.myEndPos - aLine.myStartPos);

		//CU::Vector2<float> v1intersect(aIntersectionPoint - aLine.myStartPos);

		//if (CU::Dot(v1, v1intersect) < 0 || CU::Length2(v1intersect) > CU::Length2(v1))
		//{
		//	return false;
		//}

		CU::Vector2<float> v2(aLineSegment.myEndPos - aLineSegment.myStartPos);

		CU::Vector2<float> v2intersect(aIntersectionPoint - aLineSegment.myStartPos);

		if (CU::Dot(v2, v2intersect) < 0 || CU::Length2(v2intersect) > CU::Length2(v2))
		{
			return false;
		}

		return true;
	}

	inline bool Intersection::AABBvsAABB(const AABB& anAABB1, const AABB& anAABB2)
	{
		return AABBvsAABB(anAABB1.myMinPos, anAABB1.myMaxPos, anAABB2.myMinPos, anAABB2.myMaxPos);
	}

	inline bool Intersection::AABBvsAABB(const Vector3<float>& aMin1, const Vector3<float>& aMax1, const Vector3<float>& aMin2, const Vector3<float>& aMax2)
	{
		if (aMax1.x < aMin2.x) return false;
		if (aMax1.y < aMin2.y) return false;
		if (aMax1.z < aMin2.z) return false;
		if (aMin1.x > aMax2.x) return false;
		if (aMin1.y > aMax2.y) return false;
		if (aMin1.z > aMax2.z) return false;

		return true;
	}

	inline bool Intersection::PointInsideSphere(Sphere aSphere, CU::Vector3<float> aPoint)
	{
		if (CU::Length2(aPoint - aSphere.myCenterPosition) < aSphere.myRadiusSquared)
		{
			return true;
		}
		return false;
	}

	inline bool Intersection::SphereVsSphere(Sphere aSphere, Sphere aOtherSphere)
	{
		if (CU::Length2(aOtherSphere.myCenterPosition - aSphere.myCenterPosition)
			< (aSphere.myRadius + aOtherSphere.myRadius) * (aSphere.myRadius + aOtherSphere.myRadius))
		{
			return true;
		}
		return false;
	}

	inline bool Intersection::PointInsideAABB(const AABB& aAABB, const CU::Vector3<float>& aPoint)
	{
		if (aPoint.x > aAABB.myMaxPos.x) return false;
		if (aPoint.x < aAABB.myMinPos.x) return false;
		if (aPoint.y > aAABB.myMaxPos.y) return false;
		if (aPoint.y < aAABB.myMinPos.y) return false;
		if (aPoint.z > aAABB.myMaxPos.z) return false;
		if (aPoint.z < aAABB.myMinPos.z) return false;

		return true;
	}

	inline bool Intersection::SphereInsideAABB(const AABB& aAABB, const CU::Vector3<float>& aPoint, float aRadius)
	{
		if (aPoint.x > aAABB.myMaxPos.x + aRadius) return false;
		if (aPoint.x < aAABB.myMinPos.x - aRadius) return false;
		if (aPoint.y > aAABB.myMaxPos.y + aRadius) return false;
		if (aPoint.y < aAABB.myMinPos.y - aRadius) return false;
		if (aPoint.z > aAABB.myMaxPos.z + aRadius) return false;
		if (aPoint.z < aAABB.myMinPos.z - aRadius) return false;

		return true;
	}

	inline bool Intersection::LineVsSphere(LineSegment3D aLineSegment3D, Sphere aSphere, CU::Vector3<float>& aIntersectionPoint)
	{
		CU::Vector3<float> d = aLineSegment3D.myEndPos - aLineSegment3D.myStartPos;
		CU::Normalize(d);
		CU::Vector3<float> e = aSphere.myCenterPosition - aLineSegment3D.myStartPos;
		float a = CU::Dot(e, d);

		float fSquared = aSphere.myRadiusSquared - CU::Dot(e, e) + a * a;
		if (fSquared < 0)
		{
			return false;
		}

		float t = a - sqrt(fSquared);
		aIntersectionPoint = aLineSegment3D.myStartPos + d * t;

		if (t*t > CU::Length2(aLineSegment3D.myEndPos - aLineSegment3D.myStartPos))
		{
			return false;
		}

		return true;
	}

	inline int Intersection::PlaneVsSphere(CU::Plane<float> aPlane, Sphere aSphere)
	{
		float d = CU::Dot(aPlane.GetNormal(), aSphere.myCenterPosition) - aPlane.myABCD.w;

		if (d >= aSphere.myRadius)
		{
			return 1;
		}
		if (d <= -aSphere.myRadius)
		{
			return -1;
		}

		return 0;
	}

	inline bool Intersection::SwepthSphereVsSphere(LineSegment3D aLineSegment3D, float aRadius, Sphere aSphere)
	{
		aSphere.myRadius += aRadius;
		aSphere.myRadiusSquared = aSphere.myRadius * aSphere.myRadius;
		CU::Vector3<float> intersectionPoint;
		return LineVsSphere(aLineSegment3D, aSphere, intersectionPoint);
	}

	inline bool Intersection::SwepthSphereVsAABB(LineSegment3D aLineSegment3D, float aRadius, AABB aAABB)
	{
		aAABB.myMinPos -= aRadius;
		aAABB.myMaxPos += aRadius;
		CU::Vector3<float> intersectionPoint;
		return LineVsAABB(aLineSegment3D, aAABB, intersectionPoint);
	}

	inline bool Intersection::LineVsAABB(LineSegment3D aLineSegment3D, AABB aAABB, CU::Vector3<float>& aIntersectoinPoint)
	{
		CU::Vector3<float> rayOrg = aLineSegment3D.myStartPos;
		CU::Vector3<float> rayDelta = aLineSegment3D.myEndPos - aLineSegment3D.myStartPos;

		bool inside = true;

		float xt;
		float xn;

		if (rayOrg.x < aAABB.myMinPos.x)
		{
			xt = aAABB.myMinPos.x - rayOrg.x;
			if (xt > rayDelta.x)
			{
				return false;
			}
			xt /= rayDelta.x;
			inside = false;
			xn = -1.f;
		}
		else if (rayOrg.x > aAABB.myMaxPos.x)
		{
			xt = aAABB.myMaxPos.x - rayOrg.x;
			if (xt < rayDelta.x)
			{
				return false;
			}
			xt /= rayDelta.x;
			inside = false;
			xn = 1.f;
		}
		else
		{
			xt = -1.f;
		}

		float yt;
		float yn;

		if (rayOrg.y < aAABB.myMinPos.y)
		{
			yt = aAABB.myMinPos.y - rayOrg.y;
			if (yt > rayDelta.y)
			{
				return false;
			}
			yt /= rayDelta.y;
			inside = false;
			yn = -1.f;
		}
		else if (rayOrg.y > aAABB.myMaxPos.y)
		{
			yt = aAABB.myMaxPos.y - rayOrg.y;
			if (yt < rayDelta.y)
			{
				return false;
			}
			yt /= rayDelta.y;
			inside = false;
			yn = 1.f;
		}
		else
		{
			yt = -1.f;
		}

		float zt;
		float zn;

		if (rayOrg.z < aAABB.myMinPos.z)
		{
			zt = aAABB.myMinPos.z - rayOrg.z;
			if (zt > rayDelta.z)
			{
				return false;
			}
			zt /= rayDelta.z;
			inside = false;
			zn = -1.f;
		}
		else if (rayOrg.z > aAABB.myMaxPos.z)
		{
			zt = aAABB.myMaxPos.z - rayOrg.z;
			if (zt < rayDelta.z)
			{
				return false;
			}
			zt /= rayDelta.z;
			inside = false;
			zn = 1.f;
		}
		else
		{
			zt = -1.f;
		}

		if (inside == true)
		{
			aIntersectoinPoint = aLineSegment3D.myStartPos;
			return true;
		}

		int which = 0;

		float t = xt;

		if (yt > t)
		{
			which = 1;
			t = yt;
		}

		if (zt > t)
		{
			which = 2;
			t = zt;
		}

		switch (which)
		{
		case 0:
		{
			float y = rayOrg.y + rayDelta.y * t;
			if (y < aAABB.myMinPos.y || y > aAABB.myMaxPos.y)
			{
				return false;
			}
			float z = rayOrg.z + rayDelta.z * t;
			if (z < aAABB.myMinPos.z || z > aAABB.myMaxPos.z)
			{
				return false;
			}
			break;
		}
		case 1:
		{
			float x = rayOrg.x + rayDelta.x * t;
			if (x < aAABB.myMinPos.x || x > aAABB.myMaxPos.x)
			{
				return false;
			}
			float z = rayOrg.z + rayDelta.z * t;
			if (z < aAABB.myMinPos.z || z > aAABB.myMaxPos.z)
			{
				return false;
			}
			break;
		}
		case 2:
		{
			float x = rayOrg.x + rayDelta.x * t;
			if (x < aAABB.myMinPos.x || x > aAABB.myMaxPos.x)
			{
				return false;
			}
			float y = rayOrg.y + rayDelta.y * t;
			if (y < aAABB.myMinPos.y || y > aAABB.myMaxPos.y)
			{
				return false;
			}
			break;
		}
		}

		aIntersectoinPoint = aLineSegment3D.myStartPos + rayDelta * t;
		return true;
	}

	inline double Intersection::perpDot(const CU::Vector2<float>& a, const CU::Vector2<float>& b)
	{
		return (a.y*b.x) - (a.x*b.y);
	}
}