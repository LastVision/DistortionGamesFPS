#include "stdafx.h"
#include "Edge.h"
#include "PathFinderAStar.h"
#include "PathFinderFunnel.h"

namespace Prism
{
	namespace Navigation
	{
		PathFinderFunnel::PathFinderFunnel(NavMesh* aNavMesh)
			: myAStar(new PathFinderAStar(aNavMesh))
			, myPortalStart(new Portal({ 0, 0 }, { 0, 0 }))
			, myPortalEnd(new Portal({ 0, 0 }, { 0, 0 }))
		{
		}


		PathFinderFunnel::~PathFinderFunnel()
		{
			SAFE_DELETE(myAStar);
			SAFE_DELETE(myPortalStart);
			SAFE_DELETE(myPortalEnd);
		}

		bool PathFinderFunnel::FindPath(const CU::Vector3<float>& aStart
			, const CU::Vector3<float>& anEnd, CU::GrowingArray<CU::Vector3<float>>& someFunneledPointsOut) const
		{
			CU::GrowingArray<CU::Vector2<float>> funneledPoints(16);
			bool returnValue = FindPath({ aStart.x, aStart.z }, { anEnd.x, anEnd.z }, funneledPoints);

			if (returnValue == true)
			{
				for (int i = 0; i < funneledPoints.Size(); ++i)
				{
					someFunneledPointsOut.Add({ funneledPoints[i].x, 0, funneledPoints[i].y });
				}
			}
			return returnValue;
		}

		bool PathFinderFunnel::FindPath(const CU::Vector2<float>& aStart
			, const CU::Vector2<float>& anEnd, CU::GrowingArray<CU::Vector2<float>>& someFunneledPointsOut) const
		{
			CU::GrowingArray<Prism::Navigation::Triangle*> path(16);
			if (myAStar->FindPath(anEnd, aStart, path) == true)
			{

				CalcFunneledPath(aStart, anEnd, path, someFunneledPointsOut);

				return true;
			}
			return false;
		}

		bool PathFinderFunnel::IsOutside(const CU::Vector2<float>& aPosition)
		{
			return myAStar->IsOutside(aPosition);
		}

		Triangle* PathFinderFunnel::GetCurrentTriangle(const CU::Vector2<float>& aPosition)
		{
			return myAStar->GetCurrentTriangle(aPosition);
		}

		void PathFinderFunnel::CalcFunneledPath(const CU::Vector2<float>& aStart, const CU::Vector2<float>& anEnd
			, const CU::GrowingArray<Prism::Navigation::Triangle*>& someTriangles
			, CU::GrowingArray<CU::Vector2<float>>& someFunneledPointsOut) const
		{
			//for (int i = 0; i < someTriangles.Size(); ++i)
			//{
			//	someFunneledPointsOut.Add({ someTriangles[i]->GetCenter().x, someTriangles[i]->GetCenter().z });
			//}

			CU::GrowingArray<Portal*> portals(16);

			myPortalStart->myLeft = aStart;
			myPortalStart->myRight = aStart;
			portals.Add(myPortalStart);

			for (int i = 0; i < someTriangles.Size() - 1; ++i)
			{
				Portal* portal = GetPortal(someTriangles[i], someTriangles[i + 1]);
				portals.Add(portal);
			}

			myPortalEnd->myLeft = anEnd;
			myPortalEnd->myRight = anEnd;
			portals.Add(myPortalEnd);

			StringPull(portals, someFunneledPointsOut);
		}


		float PathFinderFunnel::TriArea2(const CU::Vector2<float>& aA, const CU::Vector2<float>& aB
			, const CU::Vector2<float>& aC) const
		{
			float ax = aB.x - aA.x;
			float ay = aB.y - aA.y;
			float bx = aC.x - aA.x;
			float by = aC.y - aA.y;
			
			return bx*ay - ax*by;
		}

		bool PathFinderFunnel::VEqual(const CU::Vector2<float>& aA, const CU::Vector2<float>& aB) const
		{
			float eq = 0.001f * 0.001f;
			return CU::Length2(aA - aB) < eq;
		}

		int PathFinderFunnel::StringPull(const CU::GrowingArray<Portal*>& somePortals
			, CU::GrowingArray<CU::Vector2<float>>& someFunneledPointsOut) const
		{
			//find straight path
			int nPts = 0;
			int maxPts = INT_MAX;
			//init scan state
			CU::Vector2<float> portalApex(somePortals[0]->myLeft);
			CU::Vector2<float> portalLeft(somePortals[0]->myLeft);
			CU::Vector2<float> portalRight(somePortals[0]->myRight);

			int apexIndex = 0;
			int leftIndex = 0;
			int rightIndex = 0;

			//add start point
			//someFunneledPointsOut.Add(portalApex);

			++nPts;

			for (int i = 1; i < somePortals.Size() && nPts < maxPts; ++i)
			{
				CU::Vector2<float> left(somePortals[i]->myLeft);
				CU::Vector2<float> right(somePortals[i]->myRight);

				// update right vertex
				if (TriArea2(portalApex, portalRight, right) <= 0.0f)
				{
					if (VEqual(portalApex, portalRight) || TriArea2(portalApex, portalLeft, right) > 0.0f)
					{
						// tighten the funnel
						portalRight = right;
						rightIndex = i;
					}
					else
					{
						// Right over left, insert left to path and restart scan from portal left point.

						someFunneledPointsOut.Add(portalLeft);
						++nPts;
						// make current left the new apex.
						portalApex = portalLeft;
						apexIndex = leftIndex;
						// reset portal
						portalLeft = portalApex;
						portalRight = portalApex;
						leftIndex = apexIndex;
						rightIndex = apexIndex;
						i = apexIndex;
						continue;
					}
				}

				// update left vertex
				if (TriArea2(portalApex, portalLeft, left) >= 0.0f)
				{
					if (VEqual(portalApex, portalLeft) || TriArea2(portalApex, portalRight, left) < 0.0f)
					{
						// tighten the funnel
						portalLeft = left;
						leftIndex = i;
					}
					else
					{
						// Left over reft, insert right to path and restart scan from portal right point.

						someFunneledPointsOut.Add(portalRight);
						++nPts;
						// make current right the new apex.
						portalApex = portalRight;
						apexIndex = rightIndex;
						// reset portal
						portalLeft = portalApex;
						portalRight = portalApex;
						leftIndex = apexIndex;
						rightIndex = apexIndex;
						i = apexIndex;
						continue;
					}
				}
			}

			if (nPts < maxPts)
			{
				//someFunneledPointsOut.Add(somePortals.GetLast()->myLeft);
			}

			//someFunneledPointsOut.Add(anEnd);

			return -1;
		}

		Portal* PathFinderFunnel::GetPortal(const Triangle* aTriangle0, const Triangle* aTriangle1) const
		{
			if (aTriangle0->myEdge1 == aTriangle1->myEdge1
				|| aTriangle0->myEdge1 == aTriangle1->myEdge2
				|| aTriangle0->myEdge1 == aTriangle1->myEdge3)
			{
				return aTriangle0->myEdge1->GetPortal(aTriangle0);
			}
			else if (aTriangle0->myEdge2 == aTriangle1->myEdge1
				|| aTriangle0->myEdge2 == aTriangle1->myEdge2
				|| aTriangle0->myEdge2 == aTriangle1->myEdge3)
			{
				return aTriangle0->myEdge2->GetPortal(aTriangle0);
			}
			DL_ASSERT_EXP(aTriangle0->myEdge3 == aTriangle1->myEdge1
				|| aTriangle0->myEdge3 == aTriangle1->myEdge2
				|| aTriangle0->myEdge3 == aTriangle1->myEdge3, "Triangles are not connected.");
			return aTriangle0->myEdge3->GetPortal(aTriangle0);
		}
	}
}