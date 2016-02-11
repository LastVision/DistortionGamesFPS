#pragma once
#include <fstream>
namespace Prism
{
	struct HeightMap
	{
		HeightMap(int aWidth, int aDepth, unsigned char* const someData) : myWidth(aWidth), myDepth(aDepth), myData(someData){}
		HeightMap(std::fstream& aStream);
		~HeightMap(){ delete[] myData; }
		void operator=(HeightMap&) = delete;

		int myWidth;
		int myDepth;
		unsigned char* myData;
	};

	inline HeightMap::HeightMap(std::fstream& aStream)
		: myWidth(0), myDepth(0), myData(nullptr)

	{
		aStream.read((char*)&myWidth, sizeof(int));
		aStream.read((char*)&myDepth, sizeof(int));
		myData = new unsigned char[myWidth * myDepth];
		aStream.read((char*)myData, sizeof(char) * myWidth * myDepth);
	}
}