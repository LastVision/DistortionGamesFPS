#pragma once

#include <GrowingArray.h>
#include <Heap.h>
#include "LesserTriangle.h"
#include "Triangle.h"
#include <Vector.h>

namespace Prism
{
	namespace Navigation
	{
		class NavMesh;

		class PathFinderAStar
		{
		public:
			PathFinderAStar(NavMesh* aNavMesh);
			~PathFinderAStar();
			bool FindPath(const CU::Vector3<float>& aStart, const CU::Vector3<float>& anEnd
				, CU::GrowingArray<Triangle*>& someTrianglesOut);
			bool FindPath(const CU::Vector2<float>& aStart, const CU::Vector2<float>& anEnd
				, CU::GrowingArray<Triangle*>& someTrianglesOut);

			void Render();

			const CU::GrowingArray<Triangle*>& GetLatestPath();

			bool IsOutside(const CU::Vector2<float>& aPosition);
			Triangle* GetCurrentTriangle(const CU::Vector2<float>& aPosition);

		private:
			void operator=(PathFinderAStar&) = delete;
			bool FindStartAndEnd(const CU::Vector2<float>& aStart, const CU::Vector2<float>& anEnd);
			void ClearMesh();
			void AStar();
			void FillPath(CU::GrowingArray<Triangle*>& someTrianglesOut) const;
			float CostEstimate(Triangle* aStart, Triangle* anEnd) const;

			NavMesh* myNavMesh;

			Triangle* myStart;
			Triangle* myEnd;

			float myHeuristicMult;
			float myPathLength;

			CU::GrowingArray<Triangle*> myLatestPath;
		};

		inline float PathFinderAStar::CostEstimate(Triangle* aStart, Triangle* anEnd) const
		{
			return myHeuristicMult
				* (fabsf(aStart->myCenter.x - anEnd->myCenter.x)
				+ fabsf(aStart->myCenter.y - anEnd->myCenter.y));
		}

		inline const CU::GrowingArray<Triangle*>& PathFinderAStar::GetLatestPath()
		{
			return myLatestPath;
		}
	}
}