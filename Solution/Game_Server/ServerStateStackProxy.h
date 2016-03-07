#pragma once
class ServerStateStack;
class ServerState;

class ServerStateStackProxy
{
public:
	ServerStateStackProxy(ServerStateStack& aStateStack);
	
	void PushSubState(ServerState* aState);
	void PushMainState(ServerState* aState);
private:
	ServerStateStackProxy& operator=(const ServerStateStackProxy&);
	ServerStateStack& myStateStack;
};

