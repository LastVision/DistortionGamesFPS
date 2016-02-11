#pragma once
#include <DL_Debug.h>
#define VOS_TEMPLATE template <typename Type, int MaxSize, typename CountType = int, bool UseSafeModeFlag = true>
#define VOS_TYPE VectorOnStack<Type, MaxSize, CountType, UseSafeModeFlag>

namespace CU
{
	VOS_TEMPLATE
	class VectorOnStack
	{
	public:
		VectorOnStack();
		VectorOnStack(const VectorOnStack& aVectorOnStack);

		~VectorOnStack();

		VectorOnStack & operator=(const VectorOnStack & aVectorOnStack);

		inline const Type& operator[](const CountType & aIndex) const;
		inline Type& operator[](const CountType & aIndex);

		inline void Add(const Type& aObject);
		inline void Insert(CountType aIndex, Type& aObject);
		inline void DeleteCyclic(Type& aObject);
		inline void DeleteCyclicAtIndex(CountType aItemNumber);
		inline void RemoveCyclic(const Type& aObject);
		inline void RemoveCyclicAtIndex(CountType aItemNumber);

		inline void Clear();
		inline void DeleteAll();

		__forceinline CountType Size() const;
	private:
		Type myData[MaxSize];
		CountType myCurrentSize;
		bool myUseSafeModeFlag;
	};
}

namespace CU
{
	VOS_TEMPLATE
	VOS_TYPE::VectorOnStack()
	{
		myCurrentSize = 0;
		myUseSafeModeFlag = UseSafeModeFlag;
	}

	VOS_TEMPLATE
	VOS_TYPE::VectorOnStack(const VectorOnStack& aVectorOnStack)
	{
		operator=(aVectorOnStack);
	}

	VOS_TEMPLATE
	VOS_TYPE::~VectorOnStack()
	{

	}

	VOS_TEMPLATE
	VOS_TYPE& VOS_TYPE::operator=(const VectorOnStack& aVectorOnStack)
	{

		if (myUseSafeModeFlag == true)
		{
			for (int i = 0; i < MaxSize; ++i)
			{
				myData[i] = aVectorOnStack.myData[i];
			}
			myCurrentSize = aVectorOnStack.Size();
		}
		else
		{
			memcpy(this, &aVectorOnStack, sizeof(aVectorOnStack));
		}

		return *this;
	}

	VOS_TEMPLATE
	inline const Type& VOS_TYPE::operator[](const CountType & aIndex) const
	{
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < myCurrentSize, "a index out of bounds!");
		return myData[aIndex];
	}

	VOS_TEMPLATE
	inline Type& VOS_TYPE::operator[](const CountType & aIndex)
	{
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < myCurrentSize, "a index out of bounds!");
		return myData[aIndex];
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::Add(const Type& aObject)
	{
		DL_ASSERT_EXP(myCurrentSize < MaxSize, "Vector overflow (too many Add)");
		myData[myCurrentSize] = aObject;
		++myCurrentSize;
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::Insert(CountType aIndex, Type& aObject)
	{
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < myCurrentSize, "a index out of bounds!");
		for (int i = MaxSize - 2; i >= aIndex; --i)
		{
			myData[i + 1] = myData[i];
		}
		myData[aIndex] = aObject;
		++myCurrentSize;
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::DeleteCyclic(Type& aObject)
	{
		for (int i = 0; i < myCurrentSize; ++i)
		{
			if (myData[i] == aObject)
			{
				delete myData[i];
				myData[i] = myData[myCurrentSize - 1];
				--myCurrentSize;
				return;
			}
		}
		DL_ASSERT("Object not found.");
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::DeleteCyclicAtIndex(CountType aItemNumber)
	{
		DL_ASSERT_EXP(aItemNumber >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aItemNumber < myCurrentSize, "a index out of bounds!");
		delete myData[aItemNumber];
		myData[aItemNumber] = myData[myCurrentSize - 1];
		--myCurrentSize;
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::RemoveCyclic(const Type& aObject)
	{
		for (int i = 0; i < myCurrentSize; ++i)
		{
			if (myData[i] == aObject)
			{
				myData[i] = myData[myCurrentSize - 1];
				--myCurrentSize;
				return;
			}
		}
		DL_ASSERT("Object not found.");
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::RemoveCyclicAtIndex(CountType aItemNumber)
	{
		DL_ASSERT_EXP(aItemNumber >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aItemNumber < myCurrentSize, "a index out of bounds!");
		myData[aItemNumber] = myData[myCurrentSize - 1];
		--myCurrentSize;
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::Clear()
	{
		myCurrentSize = 0;
	}

	VOS_TEMPLATE
	inline void VOS_TYPE::DeleteAll()
	{
		for (int i = 0; i < myCurrentSize; ++i)
		{
			delete myData[i];
			myData[i] = nullptr;
		}
		myCurrentSize = 0;
	}

	VOS_TEMPLATE
	__forceinline CountType VOS_TYPE::Size() const
	{
		return myCurrentSize;
	}
}
