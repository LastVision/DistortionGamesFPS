#pragma once

#include <D3D11.h>

namespace Prism
{
	class Effect;
	class Cube3D
	{
	public:
		Cube3D();
		~Cube3D();

		void SetPosition(const CU::Vector3<float>& aPosition);
		void SetSizeAndColor(float aSideLength, const CU::Vector4<float>& aColor);
		void SetWireFrame(bool aWireFrame);
		void Render(const Camera& aCamera);

	private:
		Effect* myEffect;
		VertexIndexWrapper* myIndexBaseData;
		IndexBufferWrapper* myIndexBuffer;
		VertexDataWrapper* myVertexBaseData;
		VertexBufferWrapper* myVertexBuffer;
		D3D11_PRIMITIVE_TOPOLOGY myPrimitiveTopology;
		ID3D11InputLayout*	myInputLayout;

		void CreateVertexBuffer(float aSideLength);
		void InitVertexBuffer();
		void InitIndexBuffer();

		CU::Matrix44<float> myOrientation;
		bool myWireFrame;
		CU::Vector3<float> mySize;
		CU::Vector4<float> myColor;
	};

	inline void Cube3D::SetPosition(const CU::Vector3<float>& aPosition)
	{
		myOrientation.SetPos(aPosition);
	}

	inline void Cube3D::SetWireFrame(bool aWireFrame)
	{
		myWireFrame = aWireFrame;
	}

}