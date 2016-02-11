#pragma once

namespace Prism
{
	namespace Navigation
	{
		class NavMesh;
		class PathFinderAStar;
		struct Portal;
		class Triangle;

		class PathFinderFunnel
		{
		public:
			PathFinderFunnel(NavMesh* aNavMesh);
			~PathFinderFunnel();

			bool FindPath(const CU::Vector3<float>& aStart
				, const CU::Vector3<float>& anEnd, CU::GrowingArray<CU::Vector3<float>>& someFunneledPointsOut) const;

			bool FindPath(const CU::Vector2<float>& aStart
				, const CU::Vector2<float>& anEnd, CU::GrowingArray<CU::Vector2<float>>& someFunneledPointsOut) const;

			PathFinderAStar* GetAStar() const;

			bool IsOutside(const CU::Vector2<float>& aPosition);
			Triangle* GetCurrentTriangle(const CU::Vector2<float>& aPosition);
		private:
			void CalcFunneledPath(const CU::Vector2<float>& aStart, const CU::Vector2<float>& anEnd
				, const CU::GrowingArray<Prism::Navigation::Triangle*>& someTriangles
				, CU::GrowingArray<CU::Vector2<float>>& someFunneledPointsOut) const;

			float TriArea2(const CU::Vector2<float>& aA, const CU::Vector2<float>& aB, const CU::Vector2<float>& aC) const;
			bool VEqual(const CU::Vector2<float>& aA, const CU::Vector2<float>& aB) const;
			int StringPull(const CU::GrowingArray<Portal*>& somePortals, CU::GrowingArray<CU::Vector2<float>>& someFunneledPointsOut) const;
			Portal* GetPortal(const Triangle* aTriangle0, const Triangle* aTriangle1) const;
			PathFinderAStar* myAStar;

			Portal* myPortalStart;
			Portal* myPortalEnd;
		};

		inline PathFinderAStar* PathFinderFunnel::GetAStar() const
		{
			return myAStar;
		}

		
	}
}