#include "stdafx.h"
#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS //must be before cstdio, otherwise Error: C4996
#include <cstdio>
#include <cassert>
#include "Tga32.h"

namespace Prism
{
	Tga32::Image::~Image()
	{
		delete myImage;
	}


	Tga32::Image *Tga32::Load(const char* aName)
	{
		FILE *fp = NULL;
		Image *myImage;
		unsigned char *buffer;
		TgaHeader header;
		unsigned char *source;
		unsigned char *destination;

		myImage = new Image(); //allocates return object

		fp = fopen(aName, "rb");
		assert(fp != NULL);

		fread(&header, sizeof(header), 1, fp); // loads the header

		myImage->myWidth = header.myWidth;
		myImage->myHeight = header.myHeight;

		if (header.myBpp == 32)
		{
			myImage->myImage = new unsigned char[myImage->myHeight * myImage->myWidth * 4];
			buffer = new unsigned char[myImage->myHeight * myImage->myWidth * 4];

			fread(buffer, myImage->myHeight * myImage->myWidth * 4, 1, fp); // loads the image

			fclose(fp);

			source = buffer;

			destination = myImage->myImage;

			FlipImageData(header, source, destination);

			delete[](buffer);

			myImage->myBitDepth = 32;
			return(myImage);
		}

		fclose(fp);
		return(NULL);
	}


	void Tga32::FlipImageData(const TgaHeader& header, unsigned char* source, unsigned char* destination)
	{
		int x, y;
		for (y = 0; y < header.myHeight; y++)
		{
			for (x = 0; x < header.myWidth; x++)
			{
				destination[(y*header.myWidth + x) * 4 + 0] = source[((header.myHeight - y - 1)*header.myWidth + x) * 4 + 2]; // convert BGRA to RGBA
				destination[(y*header.myWidth + x) * 4 + 1] = source[((header.myHeight - y - 1)*header.myWidth + x) * 4 + 1];
				destination[(y*header.myWidth + x) * 4 + 2] = source[((header.myHeight - y - 1)*header.myWidth + x) * 4 + 0];
				destination[(y*header.myWidth + x) * 4 + 3] = source[((header.myHeight - y - 1)*header.myWidth + x) * 4 + 3];
			}
		}
	}
}