#pragma once

#include "Vertices.h"

namespace Prism
{
	struct VertexDataWrapper
	{
		~VertexDataWrapper()
		{
			delete myVertexData;
		}
		int myNumberOfVertices;
		int mySize;
		int myStride;
		VertexType myType;
		char* myVertexData;
	};
}