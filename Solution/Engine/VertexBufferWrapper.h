#pragma once

struct ID3D11Buffer;

namespace Prism
{
	struct VertexBufferWrapper
	{
		VertexBufferWrapper()
			: myByteOffset(0)
			, myNumberOfBuffers(0)
			, myStartSlot(0)
			, myStride(0)
			, myVertexBuffer(nullptr)
		{}
		unsigned int myByteOffset;
		int myNumberOfBuffers;
		int myStartSlot;
		unsigned int myStride;
		ID3D11Buffer* myVertexBuffer;
	};
}