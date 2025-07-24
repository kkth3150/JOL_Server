#include "pch.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "ServerPacketHandler.h"

void ClientSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32 len)
{

	PacketSessionRef session = GetPacketSessionRef();
	ServerPacketHandler::HandlePacket(session,buffer, len);

}

void ClientSession::OnSend(int32 len)
{
}