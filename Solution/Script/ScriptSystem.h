#pragma once

#include <functional>
#include "LuaFiles\lua.hpp"
#include <unordered_map>
#include <vector>
#include "LuaArgs.h"
#include <FileWatcher.h>
#include <fstream>

namespace LUA
{
	enum class eFunctionStatus
	{
		NOT_FOUND,
		INVALID_ARGS,
		OK,
	};

	struct Documentation
	{
		std::string myFunction;
		std::string myFunctionLowerCase;
		std::string myArguments;
		std::string myHelpText;
	};

	class ScriptSystem
	{
	public:
		static void Create();
		static ScriptSystem* GetInstance();
		static void Destroy();

		void Init(const std::string& aLuaPath, const std::function<void()>& aRegisterFunction);

		void RegisterFunction(const std::string& aNameInLua, lua_CFunction aFunction
			, const std::string& aArguments, const std::string& aHelpText);

		eFunctionStatus CallFunction(const std::string& aFunctionName, const LuaArguments& someArgs);
		void RunLuaFromString(const std::string& aString);
		bool ValidateLuaString(const std::string& aString, std::string& aErrorOut) const;
		void UseFile(const std::string& aFileName);

		void Update();
		void PrintToFile(const std::string& aString);
		const std::vector<Documentation>& GetDocumentation() const;

		std::string FindClosestFunction(const std::string& aInput) const;

	private:
		ScriptSystem();
		~ScriptSystem();

		struct LuaFunction
		{
			lua_CFunction myFunction;
		};

		void ReInit(const std::string& aString);
		void CheckError(int aCode);
		void CheckFunctionError(int aCode);

		void PushArg(const Arg& aArg);
		void PushStringArg(const std::string& anArgAsString);
		void AddLuaFunction(const std::string& aNameInLua, int aNumberOfArgs);

		void PrintDocumentation();

		int GetLevenshteinDistance(const std::string &s1, const std::string &s2) const;
		float GetLevenshteinRatio(const std::string& aString, int aLevenshtienDistance) const;
		int GetSubstringBonus(const std::string& aInput, const std::string& aCorrectString, int aScore) const;
		

		//int levenshtein_distance(const std::string &s1, const std::string &s2);
		//std::string FindClosestFunction(const std::string& aFailedFunction);

		std::unordered_map<std::string, int> myArgumentsCount;
		std::unordered_map<std::string, LuaFunction> myLuaFunctions;
		std::function<void()> myCppRegisterFunction;
		std::vector<std::string> myActiveFiles;
		std::vector<Documentation> myDocumentation;

		lua_State* myLuaState;
		CU::FileWatcher myFileWatcher;
		std::fstream myOutputFile;
		static ScriptSystem* myInstance;
	};

	inline const std::vector<Documentation>& ScriptSystem::GetDocumentation() const
	{
		return myDocumentation;
	}
}