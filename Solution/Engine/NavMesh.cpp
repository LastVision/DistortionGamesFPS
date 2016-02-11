#include "stdafx.h"

#include "Edge.h"
#include <fstream>
#include <Intersection.h>
#include <MathHelper.h>
#include "NavMesh.h"
#include "Triangle.h"
#include "Vertex.h"
#include <StaticArray.h>

#define QUADS_PER_SIDE 16

namespace Prism
{
	namespace Navigation
	{
		NavMesh::NavMesh()
			: myTriangles(4096)
			, myEdges(4096)
			, myNewTriangles(1024)
			, myTotalSize(255.f)
			, myCellSize(myTotalSize / QUADS_PER_SIDE)
		{
			Vertex* botLeft = new Vertex(CU::Vector2<float>(0.0f, 0.0f));

			Edge* outRight = nullptr;
			//Edge* outTop = nullptr;
			//Edge* bot = nullptr;
			//Edge* left = nullptr;


			CU::StaticArray<Edge*, QUADS_PER_SIDE> topEdges;

			for (int i = 0; i < QUADS_PER_SIDE; ++i)
			{
				topEdges[i] = nullptr;
			}

			for (int j = 0; j < QUADS_PER_SIDE; ++j)
			{
				for (int i = 0; i < QUADS_PER_SIDE; ++i)
				{
					CreateQuad(botLeft, outRight, topEdges[i]);
					//calc new botLeft Vertex
				}

				botLeft = topEdges[0]->myVertex1;
				outRight = nullptr;
			}
		}

		NavMesh::NavMesh(const std::string& aBinaryPath)
			: myTotalSize(255.f)
			, myCellSize(myTotalSize / QUADS_PER_SIDE)
		{
			Load(aBinaryPath);
		}

		NavMesh::NavMesh(std::fstream& aStream)
			: myTotalSize(255.f)
			, myCellSize(myTotalSize / QUADS_PER_SIDE)
		{
			Load(aStream);
		}

		NavMesh::~NavMesh()
		{
			myTriangles.DeleteAll();
		}

		void NavMesh::Render(bool aRenderNavmeshLines)
		{
			if (aRenderNavmeshLines == true)
			{
				for (int i = 0; i < myEdges.Size(); ++i)
				{
					myEdges[i]->Render();
				}
			}

			DEBUG_PRINT(myTriangles.Size());
			DEBUG_PRINT(myEdges.Size());
		}

		void NavMesh::Cut(Edge* anEdge)
		{
			CU::GrowingArray<Edge*> edgesToCut(8);
			for (int i = 0; i < myTriangles.Size(); ++i)
			{
				myTriangles[i]->GetEdges(anEdge, edgesToCut);
			}

			for (int i = 0; i < edgesToCut.Size(); ++i)
			{
				if (edgesToCut[i]->myTriangle1 != nullptr && myTriangles.Find(edgesToCut[i]->myTriangle1) >= 0)
				{
					myTriangles.RemoveCyclic(edgesToCut[i]->myTriangle1);
				}
				if (edgesToCut[i]->myTriangle2 != nullptr && myTriangles.Find(edgesToCut[i]->myTriangle2) >= 0)
				{
					myTriangles.RemoveCyclic(edgesToCut[i]->myTriangle2);
				}

				Vertex* intersectionVertex = new Vertex(edgesToCut[i]->myIntersection);
				Edge* newEdge1 = new Edge(edgesToCut[i]->myVertex1, intersectionVertex);
				Edge* newEdge2 = new Edge(intersectionVertex, edgesToCut[i]->myVertex2);


				if (edgesToCut[i]->myTriangle1 != nullptr)
				{
					CutTriangle(edgesToCut[i], edgesToCut[i]->myTriangle1, newEdge1, newEdge2, intersectionVertex);
				}
				if (edgesToCut[i]->myTriangle2 != nullptr)
				{
					CutTriangle(edgesToCut[i], edgesToCut[i]->myTriangle2, newEdge1, newEdge2, intersectionVertex);
				}
			}
			
			for (int i = 0; i < edgesToCut.Size(); ++i)
			{
				delete edgesToCut[i]->myTriangle1;
				delete edgesToCut[i]->myTriangle2;
			}
		}

		void NavMesh::CreateQuad(Vertex*& aBotLeftVertex, Edge*& aLeftEdge, Edge*& aBotEdge)
		{
			Vertex* topRight = new Vertex(CU::Vector2<float>(aBotLeftVertex->myPosition.x + myCellSize, aBotLeftVertex->myPosition.y + myCellSize));

			Vertex* botRight;
			Edge* bot;
			if (aBotEdge != nullptr)
			{
				botRight = aBotEdge->myVertex2; // ensure this is correct
				bot = aBotEdge;
			}
			else
			{
				botRight = new Vertex(CU::Vector2<float>(aBotLeftVertex->myPosition.x + myCellSize, aBotLeftVertex->myPosition.y));
				bot = new Edge(aBotLeftVertex, botRight);
			}

			Vertex* topLeft;
			Edge* left;
			if (aLeftEdge != nullptr)
			{
				topLeft = aLeftEdge->myVertex2; // ensure this is correct
				left = aLeftEdge;
			}
			else
			{
				topLeft = new Vertex(CU::Vector2<float>(aBotLeftVertex->myPosition.x, aBotLeftVertex->myPosition.y + myCellSize));
				left = new Edge(aBotLeftVertex, topLeft);
			}
			
			Edge* right = new Edge(botRight, topRight);
			Edge* top = new Edge(topLeft, topRight);
			Edge* middle = new Edge(aBotLeftVertex, topRight);

			myTriangles.Add(new Triangle(bot, right, middle));
			myTriangles.Add(new Triangle(left, top, middle));

			aBotLeftVertex = botRight;
			aBotEdge = top;
			aLeftEdge= right;
		}

		void NavMesh::CutTriangle(Edge* anEdgeToCut, Triangle* aTriangle, Edge* aNewEdge1, Edge* aNewEdge2, Vertex* anIntersectionVertex)
		{
			Vertex* oppositeVertex = aTriangle->GetOpposite(anEdgeToCut);

			Edge* newEdge3 = new Edge(anIntersectionVertex, oppositeVertex);

			Edge* oldEdge1 = aTriangle->GetOther(anEdgeToCut->myVertex1, anEdgeToCut);
			oldEdge1->Remove(aTriangle);
			myTriangles.Add(new Triangle(oldEdge1, newEdge3, aNewEdge1));
			myNewTriangles.Add(myTriangles.GetLast());

			Edge* oldEdge2 = aTriangle->GetOther(anEdgeToCut->myVertex2, anEdgeToCut);
			oldEdge2->Remove(aTriangle);
			myTriangles.Add(new Triangle(oldEdge2, newEdge3, aNewEdge2));
			myNewTriangles.Add(myTriangles.GetLast());
		}

		void NavMesh::Cut(const CU::GrowingArray<CU::Vector2<float>>& someVertices)
		{
			//only tests AABB for now
			DL_ASSERT_EXP(someVertices.Size() == 4, "can only cut AABB");

			for (int i = 0; i < 4; ++i)
			{
				int first = i;
				int second = i + 1;
				if (second >= someVertices.Size())
				{
					second = 0;
				}
				Edge* edge = new Edge(new Vertex(someVertices[first]), new Vertex(someVertices[second]));

				Cut(edge);
				
				SAFE_DELETE(edge);
			}

			//CU::Vector2<float> topLeft(someVertices[0]);

			//for (int i = 1; i < someVertices.Size(); ++i)
			//{
			//	topLeft.x = fminf(topLeft.x, someVertices[i].x);
			//	topLeft.y = fminf(topLeft.y, someVertices[i].y);
			//}
			//CU::Vector2<float> botRight(someVertices[0]);

			//for (int i = 1; i < someVertices.Size(); ++i)
			//{
			//	botRight.x = fmaxf(botRight.x, someVertices[i].x);
			//	botRight.y = fmaxf(botRight.y, someVertices[i].y);
			//}
			
			for (int i = 0; i < myNewTriangles.Size(); ++i)
			{
				Triangle* current = myNewTriangles[i];
				//if (CU::Intersection::PointVsRect(current->GetCenter2D(), topLeft, botRight) == true)
				if (Inside(someVertices, current->GetCenter2D()) == true)
				{
					UniqueAddIfExist(current->GetOther(current->myEdge1, true), myNewTriangles);
					UniqueAddIfExist(current->GetOther(current->myEdge2, true), myNewTriangles);
					UniqueAddIfExist(current->GetOther(current->myEdge3, true), myNewTriangles);
					if (myTriangles.Find(current) >= 0)
					{
						myTriangles.DeleteCyclic(current);
					}
				}
			}

			myNewTriangles.RemoveAll();
		}

		void NavMesh::CalcHeights(Terrain* aTerrain)
		{
			for (int i = 0; i < myTriangles.Size(); ++i)
			{
				UniqueAdd(myTriangles[i]->myEdge1, myEdges);
				UniqueAdd(myTriangles[i]->myEdge2, myEdges);
				UniqueAdd(myTriangles[i]->myEdge3, myEdges);
			}

			for (int i = 0; i < myEdges.Size(); ++i)
			{
				myEdges[i]->CalcHeights(aTerrain);
			}
		}

		void NavMesh::Save(const std::string& aFilePath)
		{
			std::fstream file;
			file.open(aFilePath, std::ios::out | std::ios::binary);

			Save(file);

			file.close();
		}

		void NavMesh::Save(std::fstream& aStream)
		{
			CU::GrowingArray<Vertex*> vertices(8192);
			CU::GrowingArray<Edge*> edges(8192);

			for (int i = 0; i < myTriangles.Size(); ++i)
			{
				UniqueAdd(myTriangles[i]->myEdge1, edges);
				UniqueAdd(myTriangles[i]->myEdge2, edges);
				UniqueAdd(myTriangles[i]->myEdge3, edges);
			}

			for (int i = 0; i < edges.Size(); ++i)
			{
				UniqueAdd(edges[i]->myVertex1, vertices);
				UniqueAdd(edges[i]->myVertex2, vertices);
			}



			int count = vertices.Size();
			aStream.write((char*)&count, sizeof(int));

			for (int i = 0; i < vertices.Size(); ++i)
			{
				aStream.write((char*)&vertices[i]->myPosition.x, sizeof(float) * 2);
			}


			count = edges.Size();
			aStream.write((char*)&count, sizeof(int));

			for (int i = 0; i < edges.Size(); ++i)
			{
				aStream.write((char*)&edges[i]->myVertex1->myIndex, sizeof(int));
				aStream.write((char*)&edges[i]->myVertex2->myIndex, sizeof(int));
				aStream.write((char*)&edges[i]->myPosition3D1.x, sizeof(float) * 3);
				aStream.write((char*)&edges[i]->myPosition3D2.x, sizeof(float) * 3);
			}

			count = myTriangles.Size();
			aStream.write((char*)&count, sizeof(int));

			for (int i = 0; i < myTriangles.Size(); ++i)
			{
				aStream.write((char*)&myTriangles[i]->myEdge1->myIndex, sizeof(int));
				aStream.write((char*)&myTriangles[i]->myEdge2->myIndex, sizeof(int));
				aStream.write((char*)&myTriangles[i]->myEdge3->myIndex, sizeof(int));
			}
		}

		void NavMesh::Load(const std::string& aBinaryPath)
		{
			std::fstream file;
			file.open(aBinaryPath.c_str(), std::ios::in | std::ios::binary);

			Load(file);

			file.close();
		}

		void NavMesh::Load(std::fstream& aStream)
		{
			int count;
			aStream.read((char*)&count, sizeof(int));

			CU::GrowingArray<Vertex*> vertices(count);
			for (int i = 0; i < count; ++i)
			{
				CU::Vector2<float> position;
				aStream.read((char*)&position.x, sizeof(float) * 2);
				vertices.Add(new Vertex(position));
			}

			aStream.read((char*)&count, sizeof(int));

			myEdges.Init(count);
			for (int i = 0; i < count; ++i)
			{
				int index[2];

				aStream.read((char*)&index, sizeof(int) * 2);
				myEdges.Add(new Edge(vertices[index[0]], vertices[index[1]]));
				Edge* current = myEdges.GetLast();
				aStream.read((char*)&current->myPosition3D1.x, sizeof(float) * 3);
				aStream.read((char*)&current->myPosition3D2.x, sizeof(float) * 3);
			}


			aStream.read((char*)&count, sizeof(int));

			myTriangles.Init(count);

			for (int i = 0; i < count; ++i)
			{
				int index[3];

				aStream.read((char*)&index, sizeof(int) * 3);
				myTriangles.Add(new Triangle(myEdges[index[0]], myEdges[index[1]], myEdges[index[2]]));
			}
		}

		void NavMesh::UniqueAdd(Edge* anEdge, CU::GrowingArray<Edge*>& someEdgesOut) const
		{
			int index = someEdgesOut.Find(anEdge);
			if (index < 0)
			{
				someEdgesOut.Add(anEdge);
				anEdge->myIndex = someEdgesOut.Size() - 1;
			}
		}

		void NavMesh::UniqueAdd(Vertex* aVertex, CU::GrowingArray<Vertex*>& someVerticesOut) const
		{
			int index = someVerticesOut.Find(aVertex);
			if (index < 0)
			{
				someVerticesOut.Add(aVertex);
				aVertex->myIndex = someVerticesOut.Size() - 1;
			}
		}

		void NavMesh::UniqueAddIfExist(Triangle* aTriangle, CU::GrowingArray<Triangle*>& someTrianglesOut) const
		{
			if (aTriangle == nullptr)
			{
				return;
			}
			int index = someTrianglesOut.Find(aTriangle);
			if (index < 0)
			{
				someTrianglesOut.Add(aTriangle);
			}
		}

		bool NavMesh::Inside(const CU::GrowingArray<CU::Vector2<float>>& someVertices, const CU::Vector2<float>& aPosition) const
		{
			for (int i = 0; i < someVertices.Size(); ++i)
			{
				CU::Vector3<float> planeNormal(0, 1.f, 0);
				CU::Vector2<float> forward2D;
				if (i < someVertices.Size() - 1)
				{
					forward2D = someVertices[i + 1] - someVertices[i];

				}
				else
				{
					forward2D = someVertices[0] - someVertices[i];
				}
				CU::Normalize(forward2D);
				CU::Vector3<float> forward(forward2D.x, 0, forward2D.y);

				CU::Vector2<float> toTarget2D(aPosition - someVertices[i]);
				CU::Normalize(toTarget2D);
				CU::Vector3<float> toTarget(toTarget2D.x, 0, toTarget2D.y);

				float dot = CU::Dot(forward, toTarget);
				float det = CU::Dot(planeNormal, CU::Cross(forward, toTarget));

				float angle = atan2(det, dot);

				if (angle < 0)// && angle < CU_PI * 2.f)
				{
					return false;
				}
			}

			return true;
		}
	}
}