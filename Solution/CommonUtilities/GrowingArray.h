#pragma once
#include <cassert>
#include <cstring>
#include <DL_Debug.h>

#define GA_TEMPLATE template<typename ObjectType, typename SizeType = int>
#define GA_TYPE GrowingArray<ObjectType, SizeType>

namespace CU
{

	GA_TEMPLATE
	class GrowingArray
	{
	public:
		GrowingArray();
		GrowingArray(SizeType aNrOfRecommendedItems, bool aUseSafeModeFlag = true);
		GrowingArray(const GrowingArray& aGrowingArray);
		~GrowingArray();

		GrowingArray& operator=(const GrowingArray& aGrowingArray);

		void Init(SizeType aNrOfRecommendedItems, bool aUseSafeModeFlag = true);
		void ReInit(SizeType aNrOfRecommendedItems, bool aUseSafeModeFlag = true);
		void Reserve(SizeType aNrOfItems, bool aUseSafeModeFlag = true);

		inline ObjectType& operator[](const SizeType& aIndex);
		inline const ObjectType& operator[](const SizeType& aIndex) const;

		inline void Add(const ObjectType& aObject);
		inline void AddEmptyObject();
		inline void Insert(SizeType aIndex, const ObjectType& aObject);
		inline void InsertFirst(const ObjectType& aObject);
		inline void DeleteCyclic(ObjectType& aObject);
		inline void DeleteCyclicAtIndex(SizeType aItemNumber);
		inline void DeleteNonCyclicAtIndex(SizeType aItemNumber);
		inline void RemoveCyclic(const ObjectType& aObject);
		inline void RemoveCyclicAtIndex(SizeType aItemNumber);
		inline void RemoveNonCyclic(const ObjectType& aObject);
		inline void RemoveNonCyclicAtIndex(SizeType aItemNumber);
		inline SizeType Find(const ObjectType& aObject) const;

		inline ObjectType& GetLast();
		inline const ObjectType& GetLast() const;

		static const SizeType FoundNone = static_cast<SizeType>(-1);

		inline void RemoveAll();
		inline void DeleteAll();

		void Optimize();
		__forceinline SizeType Size() const;
		__forceinline SizeType GetCapacity() const;

		inline ObjectType* GetArrayAsPointer();

		typedef ObjectType* iterator;
		typedef const ObjectType* const_iterator;
		iterator begin() { return &myData[0]; }
		const_iterator begin() const { return &myData[0]; }
		iterator end() { return &myData[myCurrentSize]; }
		const_iterator end() const { return &myData[myCurrentSize]; }
	private:
		inline void Resize(int aNewSize);
		ObjectType* myData;
		SizeType myCurrentSize;
		SizeType myMaxSize;
		bool myUseSafeModeFlag;
		bool myIsInit;

	};
}

namespace CU
{
	GA_TEMPLATE
	inline GA_TYPE::GrowingArray()
		: myData(nullptr)
		, myCurrentSize(0)
		, myMaxSize(0)
		, myUseSafeModeFlag(true)
		, myIsInit(false)
	{
		//myData = nullptr;
		//myCurrentSize = 0;
		//myMaxSize = 0;
		//myUseSafeModeFlag = true;
		//myIsInit = false;
	}

	GA_TEMPLATE
	inline GA_TYPE::GrowingArray(SizeType aNrOfRecommendedItems, bool aUseSafeModeFlag = true)
	{
		DL_ASSERT_EXP(aNrOfRecommendedItems > 0, "Can't create GrowingArray smaller than 1.");

		myIsInit = false;
		Init(aNrOfRecommendedItems, aUseSafeModeFlag);
	}

	GA_TEMPLATE
	inline GA_TYPE::GrowingArray(const GrowingArray& aGrowingArray)
	{
		myData = nullptr;
		operator=(aGrowingArray);
	}

	GA_TEMPLATE
	inline GA_TYPE::~GrowingArray()
	{
		delete[] myData;
		myIsInit = false;
	}

	GA_TEMPLATE
	inline GA_TYPE& GA_TYPE::operator=(const GrowingArray& aGrowingArray)
	{
		// if aGrowingArray.myMaxSize <= myMaxSize, I could just copy everything, don't need newData
		delete[] myData;
		myMaxSize = aGrowingArray.myMaxSize;
		myCurrentSize = aGrowingArray.myCurrentSize;
		myUseSafeModeFlag = aGrowingArray.myUseSafeModeFlag;
		myIsInit = aGrowingArray.myIsInit;

		ObjectType* newData = new ObjectType[aGrowingArray.myMaxSize];


		if (myUseSafeModeFlag == true)
		{
			for (SizeType i = 0; i < myCurrentSize; ++i)
			{
				newData[i] = aGrowingArray.myData[i];
			}
		}
		else
		{
			memcpy(newData, aGrowingArray.myData, sizeof(ObjectType) * aGrowingArray.myCurrentSize);
		}

		myData = newData;

		return *this;
	}

	GA_TEMPLATE
	inline void GA_TYPE::Init(SizeType aNrOfRecommendedItems, bool aUseSafeModeFlag = true)
	{
		DL_ASSERT_EXP(myIsInit == false, "Can't call Init twice, use ReInit() instead");
		DL_ASSERT_EXP(aNrOfRecommendedItems > 0, "Can't create GrowingArray smaller than 1.");

		myIsInit = true;
		myCurrentSize = 0;
		myMaxSize = aNrOfRecommendedItems;
		myUseSafeModeFlag = aUseSafeModeFlag;

		myData = new ObjectType[myMaxSize];
	}

	GA_TEMPLATE
	inline void GA_TYPE::ReInit(SizeType aNrOfRecommendedItems, bool aUseSafeModeFlag = true)
	{
		DL_ASSERT_EXP(myIsInit == true, "Can't call ReInit before Init.");
		delete[] myData;
		myIsInit = false;
		Init(aNrOfRecommendedItems, aUseSafeModeFlag);
	}

	GA_TEMPLATE
		inline void GA_TYPE::Reserve(SizeType aNrOfItems, bool aUseSafeModeFlag = true)
	{
		delete[] myData;
		myIsInit = false;
		Init(aNrOfItems, aUseSafeModeFlag);
		myCurrentSize = aNrOfItems;
	}

	GA_TEMPLATE
	inline ObjectType& GA_TYPE::operator[](const SizeType& aIndex)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < myCurrentSize, "a index out of bounds!");
		return myData[aIndex];
	}

	GA_TEMPLATE
	inline const ObjectType& GA_TYPE::operator[](const SizeType& aIndex) const
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < myCurrentSize, "a index out of bounds!");
		return myData[aIndex];
	}

	GA_TEMPLATE
	inline void GA_TYPE::Add(const ObjectType& aObject)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		if (myCurrentSize >= myMaxSize)
		{
			Resize(myMaxSize * 2);
		}
		myData[myCurrentSize++] = aObject;
	}

	GA_TEMPLATE
	inline void GA_TYPE::AddEmptyObject()
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");

		if (myCurrentSize == myMaxSize)
		{
			Resize(myMaxSize * 2);
		}

		myCurrentSize++;
	}

	GA_TEMPLATE
	inline void GA_TYPE::Insert(SizeType aIndex, const ObjectType& aObject)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aIndex >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aIndex < myCurrentSize, "a index out of bounds!");
		if (myCurrentSize >= myMaxSize)
		{
			Resize(myMaxSize * 2);
		}
		for (SizeType i = myCurrentSize - 1; i >= aIndex; --i)
		{
			if (i == SizeType(-1))
			{
				break;
			}
			myData[i + 1] = myData[i];
		}
		myData[aIndex] = aObject;
		++myCurrentSize;
	}

	GA_TEMPLATE
	inline void GA_TYPE::InsertFirst(const ObjectType& aObject)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		
		if (myCurrentSize == 0)
		{
			Add(aObject);
		}
		else
		{
			Insert(0, aObject);
		}
	}

	GA_TEMPLATE
	inline void GA_TYPE::DeleteCyclic(ObjectType& aObject)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		for (SizeType i = 0; i < myCurrentSize; ++i)
		{
			if (myData[i] == aObject)
			{
				DeleteCyclicAtIndex(i);
				return;
			}
		}
		DL_ASSERT("Object not found.");
	}

	GA_TEMPLATE
	inline void GA_TYPE::DeleteCyclicAtIndex(SizeType aItemNumber)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aItemNumber >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aItemNumber < myCurrentSize, "a index out of bounds!");
		delete myData[aItemNumber];
		myData[aItemNumber] = nullptr;
		myData[aItemNumber] = myData[--myCurrentSize];
	}

	GA_TEMPLATE
	inline void GA_TYPE::DeleteNonCyclicAtIndex(SizeType aItemNumber)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aItemNumber >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aItemNumber < myCurrentSize, "a index out of bounds!");
		delete myData[aItemNumber];
		myData[aItemNumber] = nullptr;

		for (SizeType i = aItemNumber; i < myCurrentSize - 1; ++i)
		{
			myData[i] = myData[i + 1];
		}
		--myCurrentSize;
	}


	GA_TEMPLATE
	inline void GA_TYPE::RemoveCyclic(const ObjectType& aObject)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");

		for (SizeType i = 0; i < myCurrentSize; ++i)
		{
			if (myData[i] == aObject)
			{
				RemoveCyclicAtIndex(i);
				return;
			}
		}
		DL_ASSERT("Object not found.");
	}

	GA_TEMPLATE
	inline void GA_TYPE::RemoveCyclicAtIndex(SizeType aItemNumber)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aItemNumber >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aItemNumber < myCurrentSize, "a index out of bounds!");
		myData[aItemNumber] = myData[--myCurrentSize];
	}

	GA_TEMPLATE
	inline void GA_TYPE::RemoveNonCyclic(const ObjectType& aObject)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");

		for (SizeType i = 0; i < myCurrentSize; ++i)
		{
			if (myData[i] == aObject)
			{
				RemoveNonCyclicAtIndex(i);
				return;
			}
		}
		DL_ASSERT("Object not found.");
	}


	GA_TEMPLATE
	inline void GA_TYPE::RemoveNonCyclicAtIndex(SizeType aItemNumber)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aItemNumber >= 0, "Index has to be 0 or more.");
		DL_ASSERT_EXP(aItemNumber < myCurrentSize, "a index out of bounds!");

		for (SizeType i = aItemNumber; i < myCurrentSize - 1; ++i)
		{
			myData[i] = myData[i + 1];
		}
		--myCurrentSize;
	}

	GA_TEMPLATE
	inline SizeType GA_TYPE::Find(const ObjectType& aObject) const
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");

		for (SizeType i = 0; i < myCurrentSize; ++i)
		{
			if (myData[i] == aObject)
			{
				return i;
			}
		}
		return FoundNone;
	}

	GA_TEMPLATE
	inline ObjectType& GA_TYPE::GetLast()
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		return myData[myCurrentSize - 1];
	}

	GA_TEMPLATE
	inline const ObjectType& GA_TYPE::GetLast() const
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		return myData[myCurrentSize - 1];
	}

	GA_TEMPLATE
	inline void GA_TYPE::RemoveAll()
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		myCurrentSize = 0;
	}

	GA_TEMPLATE
	inline void GA_TYPE::DeleteAll()
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		for (SizeType i = 0; i < myCurrentSize; ++i)
		{
			delete myData[i];
			myData[i] = nullptr;
		}
		myCurrentSize = 0;
	}

	GA_TEMPLATE
	inline void GA_TYPE::Optimize()
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		myMaxSize = myCurrentSize;
		if (myMaxSize < 1)
		{
			myMaxSize = 1;
		}
		Resize(myMaxSize);
	}

	GA_TEMPLATE
	__forceinline SizeType GA_TYPE::Size() const
	{
		return myCurrentSize;
	}

	GA_TEMPLATE
	__forceinline SizeType GA_TYPE::GetCapacity() const
	{
		return myMaxSize;
	}

	GA_TEMPLATE
	inline void GA_TYPE::Resize(int aNewSize)
	{
		DL_ASSERT_EXP(myIsInit == true, "Not initialized, run Init first.");
		DL_ASSERT_EXP(aNewSize > 0, "Can't make GrowingArray smaller than 1.");

		ObjectType* newData = new ObjectType[aNewSize];
		if (myUseSafeModeFlag == true)
		{
			for (SizeType i = 0; i < myCurrentSize; ++i)
			{
				newData[i] = myData[i];
			}
		}
		else
		{
			memcpy(newData, myData, sizeof(ObjectType) * myCurrentSize);
		}
		delete[] myData;
		myData = newData;
		myMaxSize = static_cast<SizeType>(aNewSize);
	}

	GA_TEMPLATE
	inline ObjectType* GA_TYPE::GetArrayAsPointer()
	{
		return myData;
	}
}