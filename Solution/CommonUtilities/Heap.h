#pragma once
#include "GrowingArray.h"
#include <cassert>
#include "Greater.h"
#include "Lesser.h"

#define HEAP_TEMPLATE template<typename T, typename Comparer>

namespace CU
{
	HEAP_TEMPLATE
	class Heap
	{
	public:
		Heap();
		~Heap();
		Heap(GrowingArray<T> aGrowingArray);
		void Enqueue(const T& someData);
		T Dequeue();
		bool IsEmpty() const;
		void Heapify();

	private:
		void MoveDown(int aFirst, int aLast);
		void Swap(T& a, T& b);
		GrowingArray<T, int> myData;
		Comparer myCompare;
	};
}

namespace CU
{
	HEAP_TEMPLATE
	inline Heap<T, Comparer>::Heap()
	{
		myData.Init(16);
	}

	HEAP_TEMPLATE
	inline Heap<T, Comparer>::Heap(GrowingArray<T> aGrowingArray) : myData(aGrowingArray)
	{
		Heapify();
	}

	HEAP_TEMPLATE
	inline void Heap<T, Comparer>::Heapify()
	{
		int lastNonLeaf = myData.Size() / 2 - 1;
		for (int i = lastNonLeaf; i >= 0; --i)
		{
			MoveDown(i, myData.Size() - 1);
		}
	}

	HEAP_TEMPLATE
	inline Heap<T, Comparer>::~Heap()
	{
	}

	HEAP_TEMPLATE
	inline void Heap<T, Comparer>::Swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	HEAP_TEMPLATE
	inline void Heap<T, Comparer>::MoveDown(int aFirst, int aLast)
	{
		int first = aFirst;
		int largest = 2 * first + 1;

		while (largest <= aLast)
		{
			if (largest < aLast && myCompare(myData[largest + 1], myData[largest]))
			{
				++largest;
			}

			if (myCompare(myData[largest], myData[first]))
			{
				Swap(myData[largest], myData[first]);
				first = largest;
				largest = 2 * first + 1;
			}
			else
			{
				largest = aLast + 1;
			}
		}
	}

	HEAP_TEMPLATE
	inline void Heap<T, Comparer>::Enqueue(const T& someData)
	{
		myData.Add(someData);
		int pos = myData.Size() - 1;
		while (pos > 0 && myCompare(myData[pos], myData[(pos - 1) / 2]))
		{
			Swap(myData[(pos - 1) / 2], myData[pos]);
			pos = (pos - 1) / 2;
		}
	}

	HEAP_TEMPLATE
	inline T Heap<T, Comparer>::Dequeue()
	{
		T result = myData[0];
		myData[0] = myData[myData.Size() - 1];
		myData.RemoveCyclicAtIndex(myData.Size() - 1);
		int pos = 0;
		int lastNonLeaf = myData.Size() / 2 - 1;
		while (pos < lastNonLeaf
			&& (myCompare(myData[pos * 2 + 1], myData[pos]) || myCompare(myData[pos * 2 + 2], myData[pos])))
		{
			if (myCompare(myData[pos * 2 + 1], myData[pos * 2 + 2]))
			{
				Swap(myData[pos], myData[pos * 2 + 1]);
				pos = pos * 2 + 1;
			}
			else
			{
				Swap(myData[pos], myData[pos * 2 + 2]);
				pos = pos * 2 + 2;
			}
		}

		if (pos * 2 + 2 < myData.Size() && myCompare(myData[pos * 2 + 2], myData[pos]))
		{
			Swap(myData[pos], myData[pos * 2 + 2]);
		}

		if (pos * 2 + 1 < myData.Size() && myCompare(myData[pos * 2 + 1], myData[pos]))
		{
			Swap(myData[pos], myData[pos * 2 + 1]);
		}

		return result;
	}

	HEAP_TEMPLATE
	inline bool Heap<T, Comparer>::IsEmpty() const
	{
		return myData.Size() == 0;
	}
}