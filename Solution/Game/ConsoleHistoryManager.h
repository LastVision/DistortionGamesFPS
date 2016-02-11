#pragma once
#include <GrowingArray.h>
#undef ERROR

namespace Prism
{
	class Text;
}



struct History
{
	History();
	~History();
	
	eHistoryType myType;
	std::string myMessage;
	Prism::Text* myRenderText;
};

class ConsoleHistoryManager
{
public:
	ConsoleHistoryManager();
	~ConsoleHistoryManager();

	const std::string& GetNext(eHistoryType aType = eHistoryType::HISTORY);
	const std::string& GetPrevious(eHistoryType aType = eHistoryType::HISTORY);
	const std::string& GetCurrent(eHistoryType aType = eHistoryType::HISTORY);

	void AddHistory(const std::string& aCommand, eHistoryType anEnum = eHistoryType::HISTORY);
	void Save();
	void Load();
	const CU::GrowingArray<History*>& GetHistoryArray();

	bool IsInHistory(const std::string& aString) const;

	void ResetHistoryCounter();
private:
	bool CheckType(eHistoryType aType, bool aShouldGoBackwards);
	void SplitCommandToMulitpleLines(const std::string& aCommand, eHistoryType anEnum);
	std::string RemoveTabFromString(const std::string& aCommand);

	std::string myEmptyString;
	CU::GrowingArray<History*> myHistory;
	const char* myHistoryFile;
	int myCurrentIndex;
	int myInsertIndex;
	bool myHasWrapped;

};

inline const CU::GrowingArray<History*>& ConsoleHistoryManager::GetHistoryArray()
{
	return myHistory;
}

inline void ConsoleHistoryManager::ResetHistoryCounter()
{
	myCurrentIndex = 0;
}