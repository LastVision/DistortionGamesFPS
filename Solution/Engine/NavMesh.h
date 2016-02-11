#pragma once
#include <GrowingArray.h>
#include <Vector.h>
#include <fstream>

namespace Prism
{
	class Terrain;

	namespace Navigation
	{
		class Edge;
		class Triangle;
		struct Vertex;

		class NavMesh
		{
		public:
			NavMesh();
			NavMesh(const std::string& aBinaryPath);
			NavMesh(std::fstream& aStream);
			~NavMesh();

			void Render(bool aRenderNavmeshLines);
			void Cut(Edge* anEdge);

			const CU::GrowingArray<Triangle*>& GetTriangles() const;
			void Cut(const CU::GrowingArray<CU::Vector2<float>>& someVertices); // needs to be clock wise convex hull

			void CalcHeights(Terrain* aTerrain); // run after all cuts are done

			void Save(const std::string& aFilePath);
			void Save(std::fstream& aStream);

		private:
			void Load(const std::string& aBinaryPath);
			void Load(std::fstream& aStream);
			void operator=(NavMesh&) = delete;
			void CreateQuad(Vertex*& aBotLeftVertex, Edge*& aLeftEdge, Edge*& aBotEdge);
			void CutTriangle(Edge* anEdgeToCut, Triangle* aTriangle, Edge* aNewEdge1, Edge* aNewEdge2, Vertex* anIntersectionVertex);
			void UniqueAdd(Edge* anEdge, CU::GrowingArray<Edge*>& someEdgesOut) const;
			void UniqueAdd(Vertex* aVertex, CU::GrowingArray<Vertex*>& someVerticesOut) const;
			void UniqueAddIfExist(Triangle* aTriangle, CU::GrowingArray<Triangle*>& someTrianglesOut) const;
			bool Inside(const CU::GrowingArray<CU::Vector2<float>>& someVertices, const CU::Vector2<float>& aPosition) const;
			CU::GrowingArray<Triangle*> myTriangles;
			CU::GrowingArray<Edge*> myEdges; // for render only
			CU::GrowingArray<Triangle*> myNewTriangles;
			const float myTotalSize;
			const float myCellSize;

		};

		inline const CU::GrowingArray<Triangle*>& NavMesh::GetTriangles() const
		{
			return myTriangles;
		}
	}
}