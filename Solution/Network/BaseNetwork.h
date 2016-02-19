#pragma once
#include <vector>
class BaseNetwork
{
public:
	BaseNetwork();
	virtual ~BaseNetwork();

	virtual void StartNetwork() = 0;
	virtual void Send(const std::vector<char>& anArray) = 0;
	virtual int Receieve(char* aBuffer) = 0;

	/* NOTE: ** Not in use by the server */
	virtual void ConnectToServer(const char* anIP);

protected:


};

