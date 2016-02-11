#include "stdafx.h"
#include "Edge.h"
#include "Terrain.h"
#include "Triangle.h"
#include "Vertex.h"

//#include "tga2d\engine.h"
//#include "tga2d\drawers\debug_drawer.h"

namespace Prism
{
	namespace Navigation
	{
		Edge::Edge(Vertex* aVertex1, Vertex* aVertex2)
			: myVertex1(aVertex1)
			, myVertex2(aVertex2)
			, myLineSegment(aVertex1->myPosition, aVertex2->myPosition)
			, myTriangle1(nullptr)
			, myTriangle2(nullptr)
			, myPortal1(nullptr)
			, myPortal2(nullptr)
			, myIndex(0)
		{
			++myVertex1->myEdgeCount;
			++myVertex2->myEdgeCount;

			myPosition3D1.x = myVertex1->myPosition.x;
			myPosition3D1.y = 26.f;
			myPosition3D1.z = myVertex1->myPosition.y;


			myPosition3D2.x = myVertex2->myPosition.x;
			myPosition3D2.y = 26.f;
			myPosition3D2.z = myVertex2->myPosition.y;
		}


		Edge::~Edge()
		{
			// minska count på båda vertex, ta bort om = 0

			--myVertex1->myEdgeCount;
			--myVertex2->myEdgeCount;

			if (myVertex1->myEdgeCount <= 0)
			{
				SAFE_DELETE(myVertex1);
			}
			if (myVertex2->myEdgeCount <= 0)
			{
				SAFE_DELETE(myVertex2);
			}

			// needed for cutting in navMesh..?
			SAFE_DELETE(myTriangle1);
			SAFE_DELETE(myTriangle2);
			SAFE_DELETE(myPortal1);
			SAFE_DELETE(myPortal2);
		}

		void Edge::Render()
		{
			if (myTriangle1 == nullptr || myTriangle2 == nullptr)
			{
				RenderLine3D(myPosition3D1 + CU::Vector3<float>(0, 0.01f, 0), myPosition3D2 + CU::Vector3<float>(0, 0.01f, 0), eColorDebug::YELLOW);
			}
			else
			{
				RenderLine3D(myPosition3D1, myPosition3D2, eColorDebug::RED);
			}
			//DX2D::CDebugDrawer& drawer = DX2D::CEngine::GetInstance()->GetDebugDrawer();

			//DX2D::Vector2f pos1(myVertex1->myPosition.x, myVertex1->myPosition.y);
			//DX2D::Vector2f pos2(myVertex2->myPosition.x, myVertex2->myPosition.y);

			//if (myTriangle1 == nullptr || myTriangle2 == nullptr)
			//{
			//	drawer.DrawLine(pos1, pos2, { 1.f, 0, 0, 1.f });
			//}
			//else
			//{
			//	drawer.DrawLine(pos1, pos2, { 0, 0, 1.f, 1.f });
			//}
		}

		void Edge::Remove(Triangle* aTriangle)
		{
			if (myTriangle1 == aTriangle)
			{
				myTriangle1 = nullptr;
				return;
			}
			else if (myTriangle2 == aTriangle)
			{
				myTriangle2 = nullptr;
				return;
			}

			DL_ASSERT("Triangle not found.");
		}

		void Edge::CalcHeights(Terrain* aTerrain)
		{
			myPosition3D1 = aTerrain->GetHeight(myPosition3D1, 2.f);
			myPosition3D2 = aTerrain->GetHeight(myPosition3D2, 2.f);
		}

		Portal* Edge::GetPortal(const Triangle* aTriangle) const
		{
			if (aTriangle == myTriangle1)
			{
				return myPortal1;
			}
			DL_ASSERT_EXP(aTriangle == myTriangle2, "Triangle not part of edge");
			return myPortal2;
		}

		void Edge::AddTriangle(Triangle* aTriangle)
		{
			if (myTriangle1 == nullptr)
			{
				myTriangle1 = aTriangle;
			}
			else
			{
				DL_ASSERT_EXP(myTriangle2 == nullptr, "edge already has two triangles");
				myTriangle2 = aTriangle;
			}
		}

		void Edge::CalcPortals()
		{
			if (myTriangle1 != nullptr)
			{
				static CU::Matrix33<float> rotationMatrix = CU::Matrix33<float>::CreateRotateAroundZ(CU_PI_DIV_2);

				CU::Vector2<float> edge = myVertex2->myPosition - myVertex1->myPosition;

				CU::Vector2<float> normal = edge * rotationMatrix;

				CU::Vector2<float> fromCenter = myVertex1->myPosition - myTriangle1->myCenter2D;

				if (CU::Dot(normal, fromCenter) > 0)
				{
					SAFE_DELETE(myPortal1);
					myPortal1 = new Portal(myVertex1->myPosition, myVertex2->myPosition);
					if (myTriangle2 != nullptr)
					{
						SAFE_DELETE(myPortal2);
						myPortal2 = new Portal(myVertex2->myPosition, myVertex1->myPosition);
					}
				}
				else
				{
					SAFE_DELETE(myPortal1);
					myPortal1 = new Portal(myVertex2->myPosition, myVertex1->myPosition);
					if (myTriangle2 != nullptr)
					{
						SAFE_DELETE(myPortal2);
						myPortal2 = new Portal(myVertex1->myPosition, myVertex2->myPosition);
					}
				}
			}
			else if (myTriangle2 != nullptr)
			{
				DL_ASSERT("Triangle 2 exists without Triangle 1");
			}
		}
	}
}