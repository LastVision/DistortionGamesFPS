#include "stdafx.h"
#include "HeightMap.h"
#include "HeightMapFactory.h"
#include "Tga32.h"

namespace Prism
{
	HeightMap* HeightMapFactory::Create(const char* aFilePath)
	{
		Tga32::Image* image = Tga32::Load(aFilePath);

		int width = image->myWidth;
		int depth = image->myHeight;

		unsigned char* data = new unsigned char[width * depth];

		for (int i = 0; i < width * depth; ++i)
		{
			data[i] = image->myImage[i * 4];
		}

		SAFE_DELETE(image);

		return new HeightMap(width, depth, data);
	}
}