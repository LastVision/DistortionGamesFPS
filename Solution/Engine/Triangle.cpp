#include "stdafx.h"
#include "Edge.h"
#include "Triangle.h"
#include <Intersection.h>
#include "Vertex.h"

namespace Prism
{
	namespace Navigation
	{
		Triangle::Triangle(Edge* aEdge1, Edge* aEdge2, Edge* aEdge3)
			: myEdge1(aEdge1)
			, myEdge2(aEdge2)
			, myEdge3(aEdge3)
		{
			InitEdge(myEdge1);
			InitEdge(myEdge2);
			InitEdge(myEdge3);

			myVertex1 = myEdge1->myVertex1;
			myVertex2 = myEdge1->myVertex2;
			myVertex3 = GetOpposite(myEdge1);

			myCenter2D = (myVertex1->myPosition + myVertex2->myPosition + myVertex3->myPosition) / 3.f;

			myEdge1->CalcPortals();
			myEdge2->CalcPortals();
			myEdge3->CalcPortals();

			myCenter.x = myCenter2D.x;
			myCenter.y = 0;
			myCenter.z = myCenter2D.y;

			float area = ((myVertex2->myPosition.x - myVertex1->myPosition.x)
				*(myVertex3->myPosition.y - myVertex1->myPosition.y)
				- (myVertex3->myPosition.x - myVertex1->myPosition.x)
				*(myVertex2->myPosition.y - myVertex1->myPosition.y)) / 2.0f;
			myArea = (area > 0.0) ? area : -area;
		}


		Triangle::~Triangle()
		{
			DeleteEdgeIfSolo(myEdge1);
			DeleteEdgeIfSolo(myEdge2);
			DeleteEdgeIfSolo(myEdge3);
		}

		void Triangle::Render()
		{
			myEdge1->Render();
			myEdge2->Render();
			myEdge3->Render();
		}

		bool Triangle::Collide(const CU::Vector2<float>& aPosition) const
		{
			const CU::Vector2<float>& p(aPosition);
			const CU::Vector2<float>& p0(myVertex1->myPosition);
			const CU::Vector2<float>& p1(myVertex2->myPosition);
			const CU::Vector2<float>& p2(myVertex3->myPosition);

			float A = 0.5f * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y);
			float sign = A < 0 ? -1.f : 1.f;
			float s = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y) * sign;
			float t = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y) * sign;

			return s > 0 && t > 0 && (s + t) < 2 * A * sign;
		}

		void Triangle::GetEdges(Edge* aCutEdge, CU::GrowingArray<Edge*>& someEdgesOut) const
		{
			bool intersect(false);

			if (CU::Intersection::LineSegmentVsLineSegment(aCutEdge->myLineSegment, myEdge1->myLineSegment, myEdge1->myIntersection) == true)
			{
				intersect = true;
			}
			if (CU::Intersection::LineSegmentVsLineSegment(aCutEdge->myLineSegment, myEdge2->myLineSegment, myEdge2->myIntersection) == true)
			{
				intersect = true;
			}
			if (CU::Intersection::LineSegmentVsLineSegment(aCutEdge->myLineSegment, myEdge3->myLineSegment, myEdge3->myIntersection) == true)
			{
				intersect = true;
			}

			const CU::Vector2<float>* point3;
			if (myEdge1->myLineSegment.myStartPos != myEdge2->myLineSegment.myStartPos
				&& myEdge1->myLineSegment.myEndPos != myEdge2->myLineSegment.myStartPos)
			{
				point3 = &myEdge2->myLineSegment.myStartPos;
			}
			else
			{
				point3 = &myEdge2->myLineSegment.myEndPos;
			}

			if (CU::Intersection::PointInsideTriangle(aCutEdge->myLineSegment.myStartPos
				, myEdge1->myLineSegment.myStartPos
				, myEdge1->myLineSegment.myEndPos
				, *point3) == true)
			{
				intersect = true;
			}

			if (intersect == true)
			{
				if (CU::Intersection::LineVsLineSegment(aCutEdge->myLineSegment, myEdge1->myLineSegment, myEdge1->myIntersection) == true)
				{
					AddUnique(myEdge1, someEdgesOut);
				}
				if (CU::Intersection::LineVsLineSegment(aCutEdge->myLineSegment, myEdge2->myLineSegment, myEdge2->myIntersection) == true)
				{
					AddUnique(myEdge2, someEdgesOut);
				}
				if (CU::Intersection::LineVsLineSegment(aCutEdge->myLineSegment, myEdge3->myLineSegment, myEdge3->myIntersection) == true)
				{
					AddUnique(myEdge3, someEdgesOut);
				}
			}
		}

		Vertex* Triangle::GetOpposite(Edge* anEdge) const
		{
			if (myEdge1 == anEdge)
			{
				return GetShared(myEdge2, myEdge3);
			}
			if (myEdge2 == anEdge)
			{
				return GetShared(myEdge1, myEdge3);
			}
			if (myEdge3 == anEdge)
			{
				return GetShared(myEdge1, myEdge2);
			}
			DL_ASSERT("opposite not found!");
			return myEdge1->myVertex1;
		}

		Edge* Triangle::GetOther(Vertex* aSharedVertex, Edge* anEdge) const
		{
			if (myEdge1 == anEdge)
			{
				return GetEdgeWithVertex(aSharedVertex, myEdge2, myEdge3);
			}
			if (myEdge2 == anEdge)
			{
				return GetEdgeWithVertex(aSharedVertex, myEdge1, myEdge3);
			}
			if (myEdge3 == anEdge)
			{
				return GetEdgeWithVertex(aSharedVertex, myEdge1, myEdge2);
			}
			DL_ASSERT("Get other not found");
			return myEdge1;
		}

		Triangle* Triangle::GetOther(Edge* anEdge, bool anAllowNotPartOfTriangle) const
		{
			if (anEdge->myTriangle1 == this)
			{
				return anEdge->myTriangle2;
			}
			else if (anEdge->myTriangle2 == this)
			{
				return anEdge->myTriangle1;
			}

			if (anAllowNotPartOfTriangle == false)
			{
				DL_ASSERT_EXP(anEdge->myTriangle2 == this, "Edge not part of triangle");
			}
			return nullptr;
		}

		Edge* Triangle::GetEdgeWithVertex(Vertex* aVertex, Edge* anEdge1, Edge* anEdge2) const
		{
			if (anEdge1->myVertex1 == aVertex
				|| anEdge1->myVertex2 == aVertex)
			{
				return anEdge1;
			}
			if (anEdge2->myVertex1 == aVertex
				|| anEdge2->myVertex2 == aVertex)
			{
				return anEdge2;
			}
			DL_ASSERT("edge not found");
			return anEdge1;
		}

		Triangle* Triangle::GetOtherTriangle(Edge* anEdge) const
		{
			if (anEdge->myTriangle1 == this)
			{
				return anEdge->myTriangle2;
			}
			DL_ASSERT_EXP(anEdge->myTriangle2 == this, "edge not part of triangle");
			return anEdge->myTriangle1;
		}

		Vertex* Triangle::GetShared(Edge* anEdge1, Edge* anEdge2) const
		{
			if (anEdge1->myVertex1 == anEdge2->myVertex1
				|| anEdge1->myVertex1 == anEdge2->myVertex2)
			{
				return anEdge1->myVertex1;
			}
			if (anEdge1->myVertex2 == anEdge2->myVertex1
				|| anEdge1->myVertex2 == anEdge2->myVertex2)
			{
				return anEdge1->myVertex2;
			}
			DL_ASSERT("shared not found");
			return anEdge1->myVertex1;
		}

		void Triangle::AddUnique(Edge* anEdge, CU::GrowingArray<Edge*>& someEdgesOut) const
		{
			for (int i = 0; i < someEdgesOut.Size(); ++i)
			{
				if (someEdgesOut[i] == anEdge)
				{
					return;
				}
			}

			someEdgesOut.Add(anEdge);
		}

		void Triangle::InitEdge(Edge* anEdge)
		{
			anEdge->AddTriangle(this);
		}

		void Triangle::DeleteEdgeIfSolo(Edge* anEdge)
		{
			if (anEdge->myTriangle1 == nullptr && anEdge->myTriangle2 == this
				|| anEdge->myTriangle1 == this && anEdge->myTriangle2 == nullptr)
			{
				anEdge->myTriangle1 = nullptr;
				anEdge->myTriangle2 = nullptr;
				SAFE_DELETE(anEdge);
			}
			else if (anEdge->myTriangle1 == this)
			{
				anEdge->myTriangle1 = nullptr;
			}
			else if (anEdge->myTriangle2 == this)
			{
				anEdge->myTriangle2 = nullptr;
			}
			else
			{
				// this happens when a triangle has been cut in two and edges disconnected and connected to new triangles
			}
		}
	}
}