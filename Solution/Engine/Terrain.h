#pragma once

#include "BaseModel.h"
#include <Vector.h>
#include <Matrix.h>
#include <GrowingArray.h>

namespace Prism
{
	namespace Navigation
	{
		class NavMesh;
		class PathFinderFunnel;
	}

	class Camera;

	struct HeightMap;
	struct VertexIndexWrapper;
	struct VertexDataWrapper;
	struct VertexPosNormUVBiTang;
	class SplatMapContainer;
	class Ice;

	class Terrain : public BaseModel
	{
		friend class TerrainReader;
	public:
		Terrain(const std::string& aHeightMapPath, const std::string& aTexturePath, const std::string& aNormalMapPath
			, const CU::Vector2<float>& aSize, float aHeight, const CU::Matrix44<float>& aOrientation
			, const std::string& aIceInfluence);
		Terrain(const std::string& aHeightMapPath, const CU::Vector2<float>& aSize, float aHeight);
		Terrain(const std::string& aBinaryPath, const std::string& aTexturePath, const std::string& aNormalMapPath
			, const std::string& aIceInfluence);
		~Terrain();

		void SetupDirectXData(const std::string& aIceInfluence);

		void Render(const Camera& aCamera, bool aRenderNavMeshLines, bool aIsDepthRender);
		void CalcEntityHeight(CU::Matrix44<float>& anOrientation) const;
		CU::Vector3<float> GetHeight(const CU::Vector3<float>& aPosition, float aHeightOffset = 0.f) const;
		CU::Vector3<float> GetHeight(const CU::Vector2<float>& aPosition, float aHeightOffset = 0.f) const;

		CU::Vector3<float> CalcIntersection(const CU::Vector3<float>& aCameraPos
			, const CU::Vector3<float>& aRayCastToZero) const;

		void CreateNavMesh();
		void LoadNavMesh(const std::string& aBinaryPath);
		Navigation::NavMesh* GetNavMesh() const;
		Navigation::PathFinderFunnel* GetPathFinder() const;

		void CreatePathFinder();
		Ice* GetIce();

	private:
		void operator=(Terrain&) = delete;

		void CreateVertices();
		void CalcNormals(CU::GrowingArray<VertexPosNormUVBiTang>& someVertices) const;
		float GetHeight(unsigned int aX, unsigned int aY) const;
		float GetHeight(unsigned int aIndex) const;
		void GetPoint(CU::Vector3<float>& aPoint) const;
		int GetIndex(const CU::Vector3<float>& aPosition) const;
		bool GetAbove(const CU::Vector3<float>& aPosition) const;
		const HeightMap* myHeightMap;
		CU::Vector2<float> mySize;
		float myHeight;
		const float myIceHeight;
		const CU::Matrix44<float> myOrientation;

		VertexIndexWrapper* myIndexBaseData;
		VertexDataWrapper* myVertexBaseData;
		float myCellSize;
		int myVertexCount;

		CU::GrowingArray<VertexPosNormUVBiTang> myVertices;
		CU::GrowingArray<int> myIndices;

		Navigation::NavMesh* myNavMesh;
		Navigation::PathFinderFunnel* myPathFinder;

		SplatMapContainer* mySplatMapContainer;
		Ice* myIce;

		std::string myNormalMapFilePath;
	};

	inline Navigation::NavMesh* Terrain::GetNavMesh() const
	{
		return myNavMesh;
	}

	inline Navigation::PathFinderFunnel* Terrain::GetPathFinder() const
	{
		return myPathFinder;
	}
	
	inline Ice* Terrain::GetIce()
	{
		return myIce;
	}
}