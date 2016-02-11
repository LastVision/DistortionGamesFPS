#pragma once
#include "GrowingArray.h"
#include "Line.h"
#include <vector>

namespace CU
{
	template <typename T>
	class LineVolume
	{
	public:
		LineVolume(GrowingArray<Line<T>> aLineList);
		LineVolume(std::vector<Line<T>> aLineList);
		~LineVolume();
		void AddLine(Line<T > aLine);
		void RemoveLine(Line<T> aLine);
		bool Inside(Vector2<T> aPosition);

		GrowingArray<Line<T>> myLines;
	};
}

namespace CU
{
	template <typename T>
	LineVolume<T>::LineVolume(GrowingArray<Line<T>> aLineList)
	{
		myLines = aLineList;
	}

	template <typename T>
	LineVolume<T>::LineVolume(std::vector<Line<T>> aLineList)
	{
		myLines.Init(aLineList.size() + 1);
		for (unsigned int i = 0; i < aLineList.size(); ++i)
		{
			myLines.Add(aLineList[i]);
		}
	}

	template <typename T>
	LineVolume<T>::~LineVolume()
	{
	}

	template <typename T>
	void LineVolume<T>::AddLine(Line<T> aLine)
	{
		myLines.Add(aLine);
	}

	template <typename T>
	void LineVolume<T>::RemoveLine(Line<T> aLine)
	{
		myLines.RemoveCyclic(aLine);
	}

	template <typename T>
	bool LineVolume<T>::Inside(Vector2<T> aPosition)
	{
		for (unsigned short i = 0; i < myLines.Size(); ++i)
		{
			if (myLines[i].Inside(aPosition) == false)
			{
				return false;
			}
		}
		return true;
	}
}