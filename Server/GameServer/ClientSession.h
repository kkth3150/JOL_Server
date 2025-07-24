#pragma once
#include "Session.h"

class ClientSession : public PacketSession
{
public:
	~ClientSession()
	{
		cout << "~GameSession" << endl;
	}

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	Vector<PlayerRef> _players;
	//한 세션이 여러 플레이어를 가지고 있을 수 있음
	//TODO: DB 연결 시 필요
};