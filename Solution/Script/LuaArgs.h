#pragma once
#include <string>

namespace LUA
{
#define MAX_NUM_ARGS 3
	struct Arg
	{
		enum class eType
		{
			NOT_USED,
			STRING,
			FLOAT,
			BOOL,
		};

		Arg()
			: myType(eType::NOT_USED){}
		Arg(float aFloat)
			: myType(eType::FLOAT), myFloat(aFloat){}
		Arg(const char* aString)
			: myType(eType::STRING), myString(aString){}
		Arg(const std::string& aString)
			: myType(eType::STRING), myString(aString.c_str()){}
		Arg(bool aBool)
			: myType(eType::BOOL), myBool(aBool){}

		friend class ScriptSystem;
	private:

		eType myType = eType::NOT_USED;

		union
		{
			float myFloat;
			const char* myString;
			bool myBool;
		};
	};

	struct LuaArguments
	{
		LuaArguments() : myCount(0){}
		LuaArguments(Arg aArg1);
		LuaArguments(Arg aArg1, Arg aArg2);
		LuaArguments(Arg aArg1, Arg aArg2, Arg aArg3);

		friend class ScriptSystem;
	private:

		int myCount;
		Arg myArgs[MAX_NUM_ARGS];
	};

	inline LuaArguments::LuaArguments(Arg aArg1)
	{
		myCount = 1;
		myArgs[0] = aArg1;
	}

	inline LuaArguments::LuaArguments(Arg aArg1, Arg aArg2)
	{
		myCount = 2;
		myArgs[0] = aArg1;
		myArgs[1] = aArg2;
	}

	inline LuaArguments::LuaArguments(Arg aArg1, Arg aArg2, Arg aArg3)
	{
		myCount = 3;
		myArgs[0] = aArg1;
		myArgs[1] = aArg2;
		myArgs[2] = aArg3;
	}
}