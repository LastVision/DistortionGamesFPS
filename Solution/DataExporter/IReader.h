#pragma once
#include <string>
class IReader
{
public:
	virtual ~IReader();
	virtual void ReadFile(const std::string& aFile) = 0;

protected:
	IReader();
};

