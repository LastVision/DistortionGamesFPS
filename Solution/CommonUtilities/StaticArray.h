#pragma once
#include <cassert>
#include "DL_Debug.h"
#define SA_TEMPLATE template<typename Type, int Size>
#define SA_TYPE StaticArray<Type, Size>

namespace CU
{
	SA_TEMPLATE
	class StaticArray
	{
	public:
		StaticArray();
		StaticArray(const StaticArray& aStaticArray);

		~StaticArray();

		StaticArray& operator=(const StaticArray& aStaticArray);

		inline const Type& operator[](const int& aIndex) const;
		inline Type& operator[](const int& aIndex);


		// Utility functions
		inline void Insert(int aIndex, Type& aObject);
		inline void DeleteAll();

		typedef Type* iterator;
		typedef const Type* const_iterator;
		iterator begin() { return &myData[0]; }
		const_iterator begin() const { return &myData[0]; }
		iterator end() { return &myData[Size]; }
		const_iterator end() const { return &myData[Size]; }
	private:
		Type myData[Size];
	};
}

namespace CU
{
	SA_TEMPLATE
	SA_TYPE::StaticArray()
	{

	}

	SA_TEMPLATE
	SA_TYPE::~StaticArray()
	{

	}

	SA_TEMPLATE
	SA_TYPE::StaticArray(const StaticArray& aStaticArray)
	{
		operator=(aStaticArray);
	}

	SA_TEMPLATE
	SA_TYPE& SA_TYPE::operator=(const StaticArray& aStaticArray)
	{
		for (int i = 0; i < Size; ++i)
		{
			myData[i] = aStaticArray.myData[i];
		}
		return *this;
	}

	SA_TEMPLATE
	inline const Type& SA_TYPE::operator[](const int& aIndex) const
	{
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < Size, "a index out of bounds!");
		return myData[aIndex];
	}

	SA_TEMPLATE
	inline Type& SA_TYPE::operator[](const int& aIndex)
	{
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < Size, "a index out of bounds!");
		return myData[aIndex];
	}

	SA_TEMPLATE
	inline void SA_TYPE::Insert(int aIndex, Type& aObject)
	{
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < Size, "a index out of bounds!");
		for (int i = Size - 2; i >= aIndex; --i)
		{
			myData[i + 1] = myData[i];
		}
		myData[aIndex] = aObject;
	}

	SA_TEMPLATE
	inline void SA_TYPE::DeleteAll()
	{
		for (int i = 0; i < Size; ++i)
		{
			delete myData[i];
			myData[i] = nullptr;
		}
	}
}

