#ifndef CommonLib_GROWINGARRAY_HEADER
#define CommonLib_GROWINGARRAY_HEADER

#include "assert.h"
#include <memory.h>

//////////////////////////////////////////////////////////////////////////
//																		//
//		Instructions!													//
//																		//
//		Open 'autoexp.dat' found from the filepath below as a textfile	//
//		Copy the code segment seen below and paste it CountTypeo the	//
//		file below the header '[Visualizer]'. This will allow you to	//
//		access better debug info from the typename.						//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////

//C:\Program Files\Microsoft Visual Studio 9.0\Common7\Packages\Debugger\autoexp.dat
/*
;------------------------------------------------------------------------------
;  CommonLib::GrowingArray
;------------------------------------------------------------------------------
CommonLib::GrowingArray<*>{
	children
	(
		#(
			[Details]: [$c,!],
			#array
			(
				expr :		($e.myItemList)[$i],
				size :		$e.myCurNrOfItems
			)
		)
	)
	preview
	(
		#(
			"[", $e.myCurNrOfItems , "](",
				#array
				(
					expr :	($e.myItemList)[$i],
					size :	$e.myCurNrOfItems
				), 
			")"
		)
	)
}
*/

#define NULL 0

namespace CommonLib
{
	template <class Type,class CountType =int>
	class GrowingArray
	{
	public:
		GrowingArray();
		GrowingArray(int aNrOfRecomendedItems,bool aShouldGrowFlag = true , bool aUseSafeModeFlag=true);
		GrowingArray(const GrowingArray& aGrowingArray);
		~GrowingArray();

		GrowingArray& operator=(const GrowingArray& aGrowingArray);
		
		void Init(int aNrOfRecomendedItems,bool aShouldGrowFlag = true ,bool aUseSafeModeFlag=true);
		void Init(int aNrOfRecomendedItems,bool aShouldGrowFlag = true ,bool aUseSafeModeFlag=true) volatile;
		void Reset(int aNrOfRecomendedItems);//Dont use unless you want to reset the data to default will leak
		void ReInit(int aNrOfRecomendedItems,bool aShouldGrowFlag = true ,bool aUseSafeModeFlag=true);

		inline Type& operator[](const CountType& aIndex) const;

		Type& GetVolatile(const CountType& aIndex) volatile;

		inline void Add(const Type& aObject);
		inline void AddVolatile(const Type& aObject) volatile;
		inline void AddEmptyObject();
		inline void Insert(CountType aIndex,const Type& aObject);
		inline void DeleteCyclic(const Type& aObject);
		inline void DeleteCyclicAtIndex(CountType aItemNumber);
		inline void RemoveCyclic(const Type& aObject);
		inline void RemoveCyclicAtIndex(CountType aItemNumber);
		inline void RemoveCyclicAtIndexVolatile(CountType aItemNumber) volatile;
		inline void Remove(CountType aItemNumber);
		inline void Remove(const Type& aObject);

		void Swap(GrowingArray& aGrowingArray);

		CountType GetCapacity() volatile { return myMaxNrOfItems; }

		CountType Find(const Type& aObject);
		void RemoveLast();

		const Type& GetLast() const;
		Type& GetLast();

		Type& GetLastVolatile() volatile;

		inline void RemoveAll();
		void RemoveAllVolatile() volatile;
		void RemoveAndDestroyAll();
		inline void DeleteAll();

		void Optimize();
		__forceinline CountType Count() const;
		inline CountType CountVolatile() volatile;

		void SetSize(int aSize);
		inline void ReSize(CountType aNewSize,int currentSize);
		inline void ReSizeVolatile(CountType aNewSize,int currentSize) volatile;
	protected:

		Type* myItemList;
		CountType myCurNrOfItems;
		CountType myMaxNrOfItems;
		bool myShouldGrowFlag;
		bool myUseSafeModeFlag;
	};

	template <class Type,class CountType>
	GrowingArray<Type,CountType>::GrowingArray()
	{
		myItemList=NULL;
		myCurNrOfItems=0;
		myMaxNrOfItems=0;
		myUseSafeModeFlag=true;
	}

	template <class Type,class CountType>
	GrowingArray<Type,CountType>::GrowingArray(int aNrOfRecomendedItems,bool aShouldGrowFlag ,bool aUseSafeModeFlag)
	{
		myItemList=NULL;
		myCurNrOfItems=0;
		myUseSafeModeFlag=aUseSafeModeFlag;
		myShouldGrowFlag = aShouldGrowFlag;
		Init(aNrOfRecomendedItems,aShouldGrowFlag);
	}

	template <class Type,class CountType>
	GrowingArray<Type,CountType>::GrowingArray(const GrowingArray<Type,CountType>& aGrowingArray)
	{
		myItemList=NULL;
		myCurNrOfItems=0;
		myMaxNrOfItems=0;
		myUseSafeModeFlag=true;
		Init(aGrowingArray.myMaxNrOfItems,aGrowingArray.myShouldGrowFlag ,aGrowingArray.myUseSafeModeFlag);
		*this=aGrowingArray;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Reset(int aNrOfRecomendedItems)
	{
		myItemList=NULL;
		Init(aNrOfRecomendedItems);
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Swap(GrowingArray& aGrowingArray)
	{
		Type* oldList = myItemList;
		myItemList = aGrowingArray.myItemList;
		aGrowingArray.myItemList = oldList;
		myCurNrOfItems = aGrowingArray.myCurNrOfItems;
		aGrowingArray.RemoveAll();
		CountType maxNr = myMaxNrOfItems;
		myMaxNrOfItems = aGrowingArray.myMaxNrOfItems;
		aGrowingArray.myMaxNrOfItems = maxNr;
	}

	template <class Type,class CountType>
	GrowingArray<Type,CountType>& GrowingArray<Type,CountType>::operator=(const GrowingArray<Type,CountType>& aGrowingArray)
	{
		ReInit(aGrowingArray.myMaxNrOfItems,aGrowingArray.myShouldGrowFlag,aGrowingArray.myUseSafeModeFlag);

		if(myUseSafeModeFlag==true)
		{
			for(CountType i=0;i<aGrowingArray.myCurNrOfItems;i++)
			{
				myItemList[i]=aGrowingArray[i];
			}
		}
		else
		{
			memcpy(myItemList,&aGrowingArray[0],aGrowingArray.Count()*sizeof(Type));
		}
		myShouldGrowFlag = aGrowingArray.myShouldGrowFlag;
		myCurNrOfItems=aGrowingArray.Count();
		return(*this);
	}

	template <class Type,class CountType>
	GrowingArray<Type,CountType>::~GrowingArray()
	{
		if(myItemList!=NULL) delete [] (myItemList);
		myItemList=NULL;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Init(int aNrOfRecomendedItems,bool aShouldGrowFlag,bool aUseSafeModeFlag)
	{
		assert((myMaxNrOfItems == 0 || myItemList==NULL) && "CALLED INIT ON AN ALREADY INITED GROWINGARRAY PLEASE USE REINIT");

		myShouldGrowFlag = aShouldGrowFlag;
		myMaxNrOfItems=aNrOfRecomendedItems;
		myUseSafeModeFlag=aUseSafeModeFlag;
		myCurNrOfItems=0;

		myItemList = new Type[myMaxNrOfItems];
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Init(int aNrOfRecomendedItems,bool aShouldGrowFlag,bool aUseSafeModeFlag) volatile
	{
		assert((myMaxNrOfItems == 0 || myItemList==NULL) && "CALLED INIT ON AN ALREADY INITED GROWINGARRAY PLEASE USE REINIT");

		myShouldGrowFlag = aShouldGrowFlag;
		myMaxNrOfItems=aNrOfRecomendedItems;
		myUseSafeModeFlag=aUseSafeModeFlag;
		myCurNrOfItems=0;

		myItemList = new Type[myMaxNrOfItems];
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::ReInit(int aNrOfRecomendedItems,bool aShouldGrowFlag,bool aUseSafeModeFlag)
	{
		if(myMaxNrOfItems > 0)
		{
			delete [] (myItemList);
		}

		myShouldGrowFlag = aShouldGrowFlag;
		myMaxNrOfItems=aNrOfRecomendedItems;
		myUseSafeModeFlag=aUseSafeModeFlag;
		myCurNrOfItems=0;

		myItemList = new Type [myMaxNrOfItems];
	}



	template <class Type,class CountType>
	Type& GrowingArray<Type,CountType>::operator[](const CountType& aIndex) const 
	{
		assert((aIndex>=0) && (aIndex<myCurNrOfItems) && "OUT OF BOUNDS ERROR");
		return(myItemList[aIndex]);
	}

	template <class Type,class CountType>
	Type& GrowingArray<Type,CountType>::GetVolatile(const CountType& aIndex) volatile 
	{
		assert((aIndex>=0) && (aIndex<myCurNrOfItems) && "OUT OF BOUNDS ERROR");
		return(myItemList[aIndex]);
	}
	
	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Add(const Type& aItem)
	{
		assert(myItemList!=NULL && "USE INIT BEFORE USE!");

		if(myCurNrOfItems==myMaxNrOfItems) ReSize(myMaxNrOfItems*2,myCurNrOfItems);
		myItemList[myCurNrOfItems]=aItem;
		myCurNrOfItems++;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::AddVolatile(const Type& aItem) volatile
	{
		assert(myItemList!=NULL && "USE INIT BEFORE USE!");

		if(myCurNrOfItems==myMaxNrOfItems) ReSizeVolatile(myMaxNrOfItems*2,myCurNrOfItems);
		myItemList[myCurNrOfItems]=aItem;
		myCurNrOfItems++;
	}
	
	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::AddEmptyObject()
	{
		assert(myItemList!=NULL && "USE INIT BEFORE USE!");

		if(myCurNrOfItems==myMaxNrOfItems) ReSize(myMaxNrOfItems*2,myCurNrOfItems);
		myCurNrOfItems++;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::DeleteCyclic(const Type& aItem)
	{
		For_Count_i(myCurNrOfItems)
		{
			if(myItemList[i]==aItem)
			{
				delete(myItemList[i]);
				myItemList[i]=myItemList[myCurNrOfItems-1];
				myCurNrOfItems--;
				return;
			}
		}
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::DeleteCyclicAtIndex(CountType aItemNumber)
	{
		delete(myItemList[aItemNumber]);
		myItemList[aItemNumber]=myItemList[myCurNrOfItems-1];
		myCurNrOfItems--;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveCyclic(const Type& aItem) 
	{
		for(CountType i = 0;i < myCurNrOfItems;i++)
		{
			if(myItemList[i]==aItem)
			{
				myItemList[i]=myItemList[myCurNrOfItems-1];
				myCurNrOfItems--;
				return;
			}
		}
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveCyclicAtIndex(CountType aItemNumber)
	{
		assert(aItemNumber >= 0 && aItemNumber < myCurNrOfItems && "USE INIT BEFORE USE!");
		myItemList[aItemNumber]=myItemList[myCurNrOfItems-1];
		myCurNrOfItems--;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveCyclicAtIndexVolatile(CountType aItemNumber) volatile
	{
		assert(aItemNumber >= 0 && aItemNumber < myCurNrOfItems && "USE INIT BEFORE USE!");
		myItemList[aItemNumber]=myItemList[myCurNrOfItems-1];
		myCurNrOfItems--;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Insert(CountType aIndex,const Type& aObject)
	{
		if(myCurNrOfItems==myMaxNrOfItems) ReSize(myMaxNrOfItems*2,myCurNrOfItems);
		for(CountType i=myCurNrOfItems-1;i>=aIndex;i--)
		{
			myItemList[i+1]=myItemList[i];
		}
		myItemList[aIndex]=aObject;
		myCurNrOfItems++;
	}

	template <class Type,class CountType>
	const Type& GrowingArray<Type,CountType>::GetLast() const
	{
		assert(myCurNrOfItems > 0);
		return myItemList[myCurNrOfItems-1];
	}
	
	template <class Type,class CountType>
	Type& GrowingArray<Type,CountType>::GetLast()
	{
		assert(myCurNrOfItems > 0);
		return myItemList[myCurNrOfItems-1];
	}


	template <class Type,class CountType>
	Type& GrowingArray<Type,CountType>::GetLastVolatile() volatile
	{
		assert(myCurNrOfItems > 0);
		return myItemList[myCurNrOfItems-1];
	}

	template <class Type,class CountType>
	CountType GrowingArray<Type,CountType>::Find(const Type& aObject)
	{
		CountType i = 0;
		for(;i < myCurNrOfItems;i++)
		{
			if(aObject == myItemList[i])
			{
				break;
			}
		}
		return i;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveLast()
	{
		assert(myCurNrOfItems > 0 && "List is Already empty!");
		myCurNrOfItems--;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Remove(CountType aItemNumber)
	{
		for(CountType i=aItemNumber;i<myCurNrOfItems;i++)
		{
			myItemList[i]=myItemList[i+1];
		}
		myCurNrOfItems--;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveAll()
	{
		myCurNrOfItems=0;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveAllVolatile() volatile
	{
		myCurNrOfItems=0;
	}
	

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::RemoveAndDestroyAll()
	{
		for(CountType i = 0;i < myCurNrOfItems;i++)
		{
			(&myItemList[i])->~Type();
		}
		myCurNrOfItems = 0;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::DeleteAll()
	{
		for(CountType i = 0;i < myCurNrOfItems;i++)
		{
			SAFE_DELETE(myItemList[i]);
		}
		myCurNrOfItems=0;
		
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::Optimize()
	{
		if(myMaxNrOfItems != myCurNrOfItems)
		{
			ReSize(myCurNrOfItems,myCurNrOfItems);
		}
	}

	template <class Type,class CountType>
	CountType GrowingArray<Type,CountType>::Count() const
	{
		return(myCurNrOfItems);
	}

	template <class Type,class CountType>
	CountType GrowingArray<Type,CountType>::CountVolatile() volatile
	{
		return(myCurNrOfItems);
	}

	

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::ReSizeVolatile(CountType aNewSize,int currentSize) volatile
	{
		assert((myShouldGrowFlag || aNewSize < myMaxNrOfItems) && "this array can't grow!");
		assert((aNewSize!=currentSize || aNewSize < myMaxNrOfItems) && "This list can't grow!");
		Type* tempList=new Type [aNewSize];
		if(myUseSafeModeFlag==true)
		{
			for(CountType i=0;i<myCurNrOfItems && i < aNewSize;i++)
			{
				tempList[i]=myItemList[i];
			}
		}
		else
		{
			memcpy(tempList,myItemList,myCurNrOfItems*sizeof(Type*));
		}
		if(myItemList)
		{
			delete [] (myItemList);
		}
		myMaxNrOfItems=aNewSize;
		myCurNrOfItems = currentSize;
		myItemList=tempList;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::SetSize(int aSize)
	{
		if(aSize > myMaxNrOfItems)
		{
			ReSize(aSize,myCurNrOfItems);
		}
		myCurNrOfItems = aSize;
	}

	template <class Type,class CountType>
	void GrowingArray<Type,CountType>::ReSize(CountType aNewSize,int currentSize)
	{
		assert((myShouldGrowFlag || aNewSize < myMaxNrOfItems) && "this array can't grow!");
		assert((aNewSize!=currentSize || aNewSize < myMaxNrOfItems) && "This list can't grow!");
		Type* tempList=new Type [aNewSize];
		if(myUseSafeModeFlag==true)
		{
			for(CountType i=0;i<myCurNrOfItems && i < aNewSize;i++)
			{
				tempList[i]=myItemList[i];
			}
		}
		else
		{
			memcpy(tempList,myItemList,myCurNrOfItems*sizeof(Type*));
		}
		if(myItemList)
		{
			delete [] (myItemList);
		}
		myMaxNrOfItems=aNewSize;
		myCurNrOfItems = currentSize;
		myItemList=tempList;
	}

}

namespace CL = CommonLib;
		


#endif