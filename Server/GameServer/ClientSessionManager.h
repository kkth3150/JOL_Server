#pragma once

class ClientSession;

using ClientSessionRef = shared_ptr<ClientSession>;

class ClientSessionManager
{
public:
	void Add(ClientSessionRef session);
	void Remove(ClientSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);

private:
	USE_LOCK;
	Set<ClientSessionRef> _sessions;
};

extern ClientSessionManager GSessionManager;
