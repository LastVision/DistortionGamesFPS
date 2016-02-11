#pragma once
#include <D3D11.h>

namespace Prism
{
	struct IndexBufferWrapper
	{
		IndexBufferWrapper()
			: myByteOffset(0)
			, myIndexBuffer(nullptr)
			, myIndexBufferFormat(DXGI_FORMAT::DXGI_FORMAT_A8_UNORM)
		{}
		unsigned int myByteOffset;
		ID3D11Buffer* myIndexBuffer;
		DXGI_FORMAT myIndexBufferFormat;
	};
}
