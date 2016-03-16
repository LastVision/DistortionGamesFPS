#pragma once
#include <vector>
#include <NetworkMessageTypes.h>

namespace std
{
	class thread;
}

class NetMessage;
class NetMessageImportantReply;
class NetMessageConnectMessage;
class NetMessageOnJoin;
class NetMessageDisconnect;
class NetMessageRequestLevel;
class NetMessagePingRequest;
class NetMessagePingReply;
class NetMessagePosition;
class NetMessageAddEnemy;
class NetMessageOnHit;
class NetMessageOnDeath;
class NetMessageRequestStartGame;
class NetMessageStartGame;

class SharedNetworkManager
{
public:

	virtual void Initiate();
	virtual void StartNetwork(unsigned int aPortNum);
	virtual void Update(float aDelta);

	template<typename T>
	void AddMessage(T aMessage);
	template<typename T>
	void AddMessage(T aMessage, unsigned int aTargetID);
	
	eNetMessageType ReadType(const char* aBuffer);
	eNetMessageType ReadType(const std::vector<char>& aBuffer);
	unsigned short GetResponsTime() const;
	double GetDataSent() const;

	void SwapBuffer();

	void ReceieveIsDone();
	void MainIsDone();

	void WaitForMain();
	void WaitForReceieve();

protected:
	struct ImportantClient
	{
		float myTimer;
		bool myHasReplied;
		std::string myName;
		unsigned int myGID;
		sockaddr_in myNetworkAddress;
	};

	struct ImportantMessage
	{
		bool operator==(const ImportantMessage& aMessage) { return myImportantID == aMessage.myImportantID; };
		//Add Data
		std::vector<char> myData;
		unsigned int myImportantID;
		unsigned char myMessageType;
		CU::GrowingArray<ImportantClient> mySenders;
	};

	struct ImportantReceivedMessage
	{
		ImportantReceivedMessage() {}
		ImportantReceivedMessage(unsigned int anImportantID, unsigned int aSenderGID)
			: myImportantID(anImportantID)
			, mySenderGID(aSenderGID)
			, myTimer(0.f)
		{}

		unsigned int myImportantID;
		unsigned int mySenderGID;
		float myTimer;
	};

	struct SendBufferMessage
	{
		SendBufferMessage() {}
		SendBufferMessage(std::vector<char> aBuffer, unsigned int aTargetID) : myBuffer(aBuffer), myTargetID(aTargetID) {}

		std::vector<char> myBuffer;
		unsigned int myTargetID;
	};

	SharedNetworkManager();
	virtual ~SharedNetworkManager();

	virtual void UpdateImportantMessages(float aDeltaTime) = 0;

	void AddNetworkMessage(std::vector<char> aBuffer, unsigned int aTargetID);
	virtual void AddImportantMessage(std::vector<char> aBuffer, unsigned int aImportantID) = 0;

	virtual void SendThread() = 0;
	virtual void ReceieveThread() = 0;

	template<typename T> 
	void UnpackAndHandle(T aMessage, Buffer& aBuffer);
	template<typename T>
	bool CheckIfImportantMessage(T aMessage);
	template<typename T>
	T CreateMessage();
	

	void HandleMessage();

	virtual void HandleMessage(const NetMessageImportantReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageConnectMessage& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageOnJoin& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageDisconnect& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageRequestLevel& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePingRequest& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessagePosition& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageAddEnemy& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageOnHit& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageOnDeath& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageRequestStartGame& aMessage, const sockaddr_in& aSenderAddress);
	virtual void HandleMessage(const NetMessageStartGame& aMessage, const sockaddr_in& aSenderAddress);


	std::thread* myReceieveThread;
	std::thread* mySendThread;

	CU::StaticArray<CU::GrowingArray<Buffer>, 2> myReceieveBuffer;
	CU::StaticArray<CU::GrowingArray<SendBufferMessage>, 2> mySendBuffer;
	CU::GrowingArray<ImportantMessage> myImportantMessagesBuffer;
	CU::GrowingArray<ImportantReceivedMessage> myImportantReceivedMessages;

	bool myIsServer;
	bool myIsOnline;

	unsigned short myCurrentBuffer;
	unsigned short myCurrentSendBuffer;
	
	unsigned int myGID;

	volatile bool myIsRunning;
	volatile bool myReceieveIsDone;
	volatile bool myMainIsDone;
	
	float myPingTime;
	float myResponsTime;
	float myMS;

	double myDataSent;
	double myDataToPrint;

	unsigned int myImportantID;

private:
	bool AlreadyReceived(const NetMessage& aMessage);
	void UpdateImportantReceivedMessages(float aDelta);
};

template<typename T>
inline void SharedNetworkManager::AddMessage(T aMessage)
{
	AddMessage(aMessage, 0);
}

template<typename T>
inline void SharedNetworkManager::AddMessage(T aMessage, unsigned int aTargetID)
{
	aMessage.myTargetID = aTargetID;
	if (myIsServer == false)
	{
		aMessage.mySenderID = myGID;
	}
	bool isImportant = aMessage.GetIsImportant();
	unsigned int importantID = 0;
	if (isImportant == true)
	{
		aMessage.SetImportantID(myImportantID++);
		importantID = aMessage.GetImportantID();
	}

	aMessage.PackMessage();
	if (isImportant == true)
	{
		AddImportantMessage(aMessage.myStream, importantID);
	}
	myDataSent += aMessage.myStream.size() * sizeof(char);
	AddNetworkMessage(aMessage.myStream, aTargetID);
}

template<typename T>
T SharedNetworkManager::CreateMessage()
{
	T toReturn;
	toReturn.SetImportantID(myImportantID++);
	return toReturn;
}

template<typename T>
void SharedNetworkManager::UnpackAndHandle(T aMessage, Buffer& aBuffer)
{
	aMessage.UnPackMessage(aBuffer.myData, aBuffer.myLength);
	if (CheckIfImportantMessage(aMessage) == true)
	{
		AddMessage(NetMessageImportantReply(aMessage.GetImportantID()), aMessage.mySenderID);
	}
	if (AlreadyReceived(aMessage) == false)
	{
		HandleMessage(aMessage, aBuffer.mySenderAddress);
	}
}

template<typename T>
bool SharedNetworkManager::CheckIfImportantMessage(T aMessage)
{
	return aMessage.GetIsImportant();
}