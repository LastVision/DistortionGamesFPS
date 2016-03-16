#pragma once
#include <vector>
#include <NetworkMessageTypes.h>
#include <NetworkSubscriber.h>

namespace std
{
	class thread;
}

class NetMessage;
class NetMessageImportantReply;

class NetworkSubscriber;

struct NetworkSubscriberInfo
{
	NetworkSubscriber* myNetworkSubscriber;
};

class SharedNetworkManager : public NetworkSubscriber
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

	void Subscribe(const eNetMessageType aMessageType, NetworkSubscriber* aSubscriber);
	void UnSubscribe(const eNetMessageType aMessageType, NetworkSubscriber* aSubscriber);
	void UnSubscribe(NetworkSubscriber* aSubscriber);
	bool IsSubscribed(const eNetMessageType aMessageType, NetworkSubscriber* aSubscriber);

	void ReceiveNetworkMessage(const NetMessageImportantReply& aMessage, const sockaddr_in& aSenderAddress) override;
	void ReceiveNetworkMessage(const NetMessagePingReply& aMessage, const sockaddr_in& aSenderAddress) override;

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
	void SendToSubscriber(const T& aMessage);
	void HandleMessage();

	std::thread* myReceieveThread;
	std::thread* mySendThread;

	CU::StaticArray<CU::GrowingArray<Buffer>, 2> myReceieveBuffer;
	CU::StaticArray<CU::GrowingArray<SendBufferMessage>, 2> mySendBuffer;
	CU::GrowingArray<ImportantMessage> myImportantMessagesBuffer;
	CU::GrowingArray<ImportantReceivedMessage> myImportantReceivedMessages;

	CU::StaticArray<CU::GrowingArray<NetworkSubscriberInfo>, static_cast<int>(eNetMessageType::_COUNT)> mySubscribers;

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
		importantID = myImportantID;
		aMessage.SetImportantID(myImportantID++);
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
void SharedNetworkManager::UnpackAndHandle(T aMessage, Buffer& aBuffer)
{
	aMessage.UnPackMessage(aBuffer.myData, aBuffer.myLength);
	if (CheckIfImportantMessage(aMessage) == true)
	{
		AddMessage(NetMessageImportantReply(aMessage.GetImportantID()), aMessage.mySenderID);
	}
	if (AlreadyReceived(aMessage) == false)
	{
		SendToSubscriber(aMessage)
	}
}

template<typename T>
bool SharedNetworkManager::CheckIfImportantMessage(T aMessage)
{
	return aMessage.GetIsImportant();
}

template<typename T>
void SharedNetworkManager::SendToSubscriber(const T& aMessage)
{
	CU::GrowingArray<NetworkSubscriberInfo>& subscribers = mySubscribers[static_cast<int>(aMessage.myID)];

	if (subscribers.Size() > 0)
	{
		for (int i = 0; i < subscribers.Size(); ++i)
		{
			subscribers[i].myNetworkSubscriber->RecieveNetworkMessage(aMessage);
		}
	}
	else
	{
		DL_ASSERT("Network message sent without subscriber.");
	}
}