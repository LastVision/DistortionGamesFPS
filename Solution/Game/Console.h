#pragma once
#include <Subscriber.h>


#ifdef RELEASE_BUILD
class Console : Subscriber
{
public:
	static Console* GetInstance();
	static void Destroy();

	void Update(){};
	//const std::string& GetInput() const;
	//ConsoleHistoryManager* GetConsoleHistory();
	//ConsoleHelp* GetConsoleHelp();

	//void ReceiveMessage(const RunScriptMessage& aMessage) override;

	void ClearInput(){};

	//void SetInput(const std::string& aString);
	void AddHistory(const std::string&, eHistoryType) {};
private:
	Console();
	~Console();
	static Console* myInstance;
};
#else

class ConsoleHelp;
class ConsoleHistoryManager;
class ConsoleBackspace;

class Console : Subscriber
{
public:
	static Console* GetInstance();
	static void Destroy();

	void Update();
	const std::string& GetInput() const;
	ConsoleHistoryManager* GetConsoleHistory();
	ConsoleHelp* GetConsoleHelp();

	void ReceiveMessage(const RunScriptMessage& aMessage) override;

	void ClearInput();

	void SetInput(const std::string& aString);
	void AddHistory(const std::string& aMessage, eHistoryType aType);
private:
	Console();
	~Console();
	void ReadInput();

	std::string myInput;

	static Console* myInstance;
	ConsoleHelp* myHelp;
	ConsoleHistoryManager* myHistory;
	ConsoleBackspace* myBackspace;
};

inline const std::string& Console::GetInput() const
{
	return myInput;
}

inline ConsoleHistoryManager* Console::GetConsoleHistory()
{
	return myHistory;
}

inline ConsoleHelp* Console::GetConsoleHelp() 
{
	return myHelp;
}

inline void Console::ClearInput()
{
	myInput = "";
}

inline void Console::SetInput(const std::string& aString)
{
	myInput = aString;
}
#endif