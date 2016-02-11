#pragma once

#include <D3D11.h>

#define MAXNROFLINES 65535

namespace Prism
{
	class Camera;
	class Effect;

	struct VertexBufferWrapper;
	class Line3DRenderer
	{
	public:
		Line3DRenderer();
		~Line3DRenderer();

		void Render(const CU::GrowingArray<Line3D>& someLines, const Camera& aCamera);

	private:
		VertexBufferWrapper* myVertexBuffer;
		D3D11_BUFFER_DESC* myVertexBufferDesc;
		D3D11_BUFFER_DESC* myIndexBufferDesc;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
		ID3D11InputLayout*	myInputLayout;
		Effect* myEffect;

		void UpdateVertexBuffer(const CU::GrowingArray<Line3D>& someLines);
		void CreateInputLayout();
		void InitVertexBuffer();
	};

}