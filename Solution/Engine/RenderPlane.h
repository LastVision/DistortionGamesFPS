#pragma once
#include "BaseModel.h"
#include "Vertices.h"

struct D3D11_MAPPED_SUBRESOURCE;

namespace Prism
{
	class Camera;
	class RenderPlane : public BaseModel
	{
	public:
		RenderPlane(int aSubDivisions, float aTileSize);
		~RenderPlane();

		void Render(const Camera& aCamera, const CU::Matrix44<float>& aOrientation);

		void StartModify();
		void EndModify();

		void SetVertexColor(int aIndex, const CU::Vector4<float>& aColor);
		int GetVertexCount() const;

	private:
		void CreateVertices();

		int mySubDivisions;
		float myTileSize;

		CU::GrowingArray<VertexPosColor> myVertices;
		D3D11_MAPPED_SUBRESOURCE* myMappedResource;
	};
}