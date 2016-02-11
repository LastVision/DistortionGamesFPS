#pragma once

#include <LineSegment2D.h>
#include <Vector.h>

namespace Prism
{
	class Terrain;

	namespace Navigation
	{
		class Triangle;
		struct Vertex;
		class NavMesh;

		struct Portal
		{
			Portal(const CU::Vector2<float>& aLeft, const CU::Vector2<float>& aRight)
				: myLeft(aLeft)
				, myRight(aRight)
			{}
			void operator=(Portal&) = delete;
			CU::Vector2<float> myLeft;
			CU::Vector2<float> myRight;
		};

		class Edge
		{
		public:
			friend class NavMesh;
			friend class Triangle;
			Edge(Vertex* aVertex1, Vertex* aVertex2);
			~Edge();

			void Render();
			void Remove(Triangle* aTriangle);

			const CU::Intersection::LineSegment2D myLineSegment;

			void CalcHeights(Terrain* aTerrain);
			Portal* GetPortal(const Triangle* aTriangle) const;
			void AddTriangle(Triangle* aTriangle);

		private:
			void CalcPortals();
			void operator=(Edge&) = delete;
			Vertex* myVertex1;
			Vertex* myVertex2;
			Triangle* myTriangle1;
			Triangle* myTriangle2;

			Portal* myPortal1;
			Portal* myPortal2;

			CU::Vector2<float> myIntersection;

			CU::Vector3<float> myPosition3D1;
			CU::Vector3<float> myPosition3D2;

			int myIndex;
		};
	}
}