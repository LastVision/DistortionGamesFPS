#pragma once

#include "BaseModel.h"
#include "Vertices.h"
class Terrain;

namespace Prism
{
	class Camera;
	struct VertexIndexWrapper;
	struct VertexDataWrapper;
	struct VertexPosNormUVBiTang;
	class Terrain;

	class SelectionCircle : public BaseModel
	{
	public:
		SelectionCircle(Effect* aEffect, const CU::Vector2<float>& aSize, float aHeight, const std::string& aFilePath
			, const Terrain& aTerrain);
		~SelectionCircle();

		void Render(const Camera& aCamera, const CU::Matrix44<float>& aOrientation);

		void SetTextures();
		const float GetHeight() const;

	private:
		void operator=(SelectionCircle&) = delete;
		void CreateVertices();

		const CU::Vector2<float> mySize;
		const float myHeight;

		VertexIndexWrapper* myIndexBaseData;
		VertexDataWrapper* myVertexBaseData;
		CU::GrowingArray<IceVertex> myVertices;

		const Terrain& myTerrain;
	};

	inline const float SelectionCircle::GetHeight() const
	{
		return myHeight;
	}

}