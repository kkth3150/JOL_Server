#include "pch.h"
#include "ClientSessionManager.h"
#include "ClientSession.h"

ClientSessionManager GSessionManager;

void ClientSessionManager::Add(ClientSessionRef session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void ClientSessionManager::Remove(ClientSessionRef session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void ClientSessionManager::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (ClientSessionRef session : _sessions)
	{
		session->Send(sendBuffer);
	}
}