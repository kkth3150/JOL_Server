#include "pch.h"
#include "Room_Manager.h"
#include "SGlobal.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "ClientSession.h"
#include "Player.h"
#include "Room.h"

static Atomic<uint64> ClientID = 0;

void ServerPacketHandler::HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br.Peek(&header);

	switch (header.id)
	{
	case C_LOGIN:
		Handle_C_LOGIN(session, buffer, len);
		break;

	case C_KEYINPUT:
		Handle_C_KEYINPUT(session, buffer, len);
		break;

	case C_MOVEMENT:
		Handle_C_MOVEMENT(session, buffer, len);
		break;

	case C_SHOT:
		Handle_C_SHOT(session, buffer, len);
		break;

	case C_RESPAWN_TANK:
		Handle_C_TANK_RESPAWN(session, buffer, len);
		break;
	case C_SHOW_ROOM:
		Handle_C_SHOW_ROOM(session, buffer, len);
		break;

	case C_CREATE_ROOM:
		Handle_C_CREATE_ROOM(session, buffer, len);
		break;

	case C_JOIN_ROOM:
		Handle_C_JOIN_ROOM(session, buffer, len);
		break;

	case C_CHANGE_INFO:
		Handle_C_CHANGE_INFO(session, buffer, len);
		break;
	case C_EXIT_ROOM:
		Handle_C_EXIT_ROOM(session, buffer, len);
		break;
	case C_READY:
		Handle_C_READY(session, buffer, len);
		break;
	case C_START:
		Handle_C_GAMESTART(session, buffer, len);
		break;
	case C_FINISH_LOADING:
		Handle_C_LOADING_FINISH(session, buffer, len);
		break;
	default:
		break;
	}
}

void ServerPacketHandler::Handle_C_LOGIN(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);


	uint64 newClientID = ClientID.fetch_add(1);

	PlayerRef playerRef = MakeShared<Player>();
	playerRef->playerID = newClientID;
	playerRef->OwenerSession = C_Session;
	C_Session->_players.push_back(playerRef);

	//GRoom.Accept_Player(playerRef);
	//cout << "Player" << playerRef->playerID << "EnterRoom" << endl;

	auto sendbuffer = Make_S_SUCCESS_LOGIN((uint16)newClientID);
	session->Send(sendbuffer);


	//�α���


}


// -> �� ť => �븶�� ���� �������� �����͸� �׾Ƶд�.
//���� �����ӵ��� ù �����ӿ� => ť�� ��� ������ ó���� ��ü�� ������Ʈ -> ��ε�ĳ��Ʈ
//Ű��ǲ->

void ServerPacketHandler::Handle_C_KEYINPUT(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);

	int64 id;
	if (!C_Session->_players.empty())
		id = C_Session->_players[0]->playerID;

	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 key;

	br >> key;

	cout << "Player(" << id << ") Moved" << endl;


	//GRoom.MovePlayer(id);


	// �ӵ� ����.
	// �������� ���Ͻô°� ��ũ �˵�.. ���� -> �������� �����Ѵ� ?
	//
	// ���� ������ �ϳ� ���� -> firstDown

}

void ServerPacketHandler::Handle_C_MOVEMENT(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 tankIndex;
	br >> tankIndex;

	Matrix4x4 mat;
	float potapRotation;
	float posinRotation;

	// ��� ������ �б�
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			br >> mat.m[i][j];
		}
	}

	br >> potapRotation >> posinRotation;

	if (!C_Session->_players.empty()) {
		uint64 playerID = C_Session->_players[0]->playerID;
		int roomID = C_Session->_players[0]->RoomNum;

		// �ε����� ������ ��ũ ���� ����
		Room_Manager::Get_Instance()->Get_Room(roomID)->SetTankState(tankIndex, mat, potapRotation, posinRotation);
	}

}

void ServerPacketHandler::Handle_C_SHOT(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;
	WEAPON_ID WeaponID = WEAPON_NPOTAN;
	Vec3 InitPos;
	Vec3 Normalized_Dir;

	br >> InitPos.X >> InitPos.Y >> InitPos.Z
		>> Normalized_Dir.X >> Normalized_Dir.Y >> Normalized_Dir.Z;

	uint8 ID = C_Session->_players[0]->playerID;
	uint8 RoomNum = C_Session->_players[0]->RoomNum;
	Room_Manager::Get_Instance()->Get_Room(RoomNum)->CreateBullet(ID, WeaponID, Normalized_Dir, InitPos);
	//GRoom.CreateBullet(ID, WeaponID, Normalized_Dir, InitPos);

}

void ServerPacketHandler::Handle_C_TANK_RESPAWN(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 tankIndex;
	br >> tankIndex;

	Matrix4x4 mat;
	float potapRotation;
	float posinRotation;

	// ��� ������ �б�
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			br >> mat.m[i][j];
		}
	}

	br >> potapRotation >> posinRotation;

	if (!C_Session->_players.empty()) {
		uint64 playerID = C_Session->_players[0]->playerID;
		int roomID = C_Session->_players[0]->RoomNum;

		// �ε����� ������ ��ũ ���� ����
		Room_Manager::Get_Instance()->Get_Room(roomID)->SetTankRespawn(tankIndex, mat, potapRotation, posinRotation);

	}

}

void ServerPacketHandler::Handle_C_SHOW_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	auto sendbuffer = Make_S_ROOM_DATA(0);
	session->Send(sendbuffer);

}

void ServerPacketHandler::Handle_C_CREATE_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;
	int RoomID = Room_Manager::Get_Instance()->Client_CreateRoom(C_Session->_players[0]);
	C_Session->_players[0]->RoomNum = RoomID;

	Room_Manager::Get_Instance()->BroadCast_LobbyState(RoomID);
}

void ServerPacketHandler::Handle_C_JOIN_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	uint32 RoomID;

	br >> header;
	br >> RoomID;

	Room_Manager::Get_Instance()->Client_EnterRoom(RoomID, C_Session->_players[0]);
	C_Session->_players[0]->RoomNum = RoomID;

	Room_Manager::Get_Instance()->BroadCast_LobbyState(RoomID);


}

void ServerPacketHandler::Handle_C_EXIT_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	uint32 RoomID;

	br >> header;
	br >> RoomID;


	Room_Manager::Get_Instance()->Client_LeaveRoom(C_Session->_players[0]->RoomNum, C_Session->_players[0]);
	Room_Manager::Get_Instance()->BroadCast_LobbyState(C_Session->_players[0]->RoomNum);
	C_Session->_players[0]->RoomNum = ROBBY;

}

void ServerPacketHandler::Handle_C_CHANGE_INFO(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 RoomID = C_Session->_players[0]->RoomNum;
	uint8 ID = C_Session->_players[0]->playerID;

	Room_Ready_Data Temp;
	br >> Temp.PlayerID >> Temp.Position >> Temp.Team >> Temp.IsReady;

	if (Room_Manager::Get_Instance()->Client_ChangeINFO(RoomID, ID, Temp)) {

		Room_Manager::Get_Instance()->BroadCast_LobbyState(RoomID);
	}



}

void ServerPacketHandler::Handle_C_READY(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	uint8 RoomID = C_Session->_players[0]->RoomNum;
	uint8 ID = C_Session->_players[0]->playerID;

	if (Room_Manager::Get_Instance()->Ready_Player(RoomID, ID))
		Room_Manager::Get_Instance()->BroadCast_LobbyState(RoomID);
}


void ServerPacketHandler::Handle_C_GAMESTART(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	uint8 RoomID = C_Session->_players[0]->RoomNum;

	if (Room_Manager::Get_Instance()->Check_StartGame(RoomID))
		Room_Manager::Get_Instance()->BroadCast_Game_Start(RoomID);


}

void ServerPacketHandler::Handle_C_LOADING_FINISH(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	uint8 RoomID = C_Session->_players[0]->RoomNum;
	
	Room_Manager::Get_Instance()->Client_LOADING_FINISH(RoomID);

}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id << hp << attack;



	header->size = bw.WriteSize();
	header->id = S_TEST; // 1 : Test Msg

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}


SendBufferRef ServerPacketHandler::Make_S_SUCCESS_LOGIN(uint16 id)
{

	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;						// ���� �����͸� ����
	header->size = bw.WriteSize();	// �״��� ��Ȯ�� ��ü ũ�⸦ �����Ѵ�
	header->id = S_SUCCESS_LOGIN;



	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_SUCCESS_ENTER_ROOM(uint16 id)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;

	header->size = bw.WriteSize();
	header->id = S_SUCCESS_ENTER_ROOM;

	cout << "SEND_SUCCESS_ENTER_MESSAGE" << endl;
	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_GAME_START(uint8 dummy)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << dummy;

	header->size = bw.WriteSize();
	header->id = S_GAME_START;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_ALL_PLAYER_LOADING_FINISH(uint8 Dummy)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Dummy;

	header->size = bw.WriteSize();
	header->id = S_ROOM_ALL_PLAYER_FINISH_LOADING;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_GAME_WIN(uint8 Dummy)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Dummy;

	header->size = bw.WriteSize();
	header->id = S_GAME_WIN;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_GAME_LOSE(uint8 Dummy)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Dummy;

	header->size = bw.WriteSize();
	header->id = S_GAME_LOSE;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_WEAPON_HIT(float x, float y, float z)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << x << y << z;

	header->size = bw.WriteSize();
	header->id = S_WEAPON_HIT;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_ALL_TANK_STATE(std::vector<Tank_INFO>& tanks)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	header->id = S_ALL_TANK_STATE;

	bw << static_cast<uint16>(tanks.size());

	for (Tank_INFO& tank : tanks)
	{
		bw << tank.id;

		for (int i = 0; i < 4; ++i)
		{
			bw << tank.TankTransform.m[i][0];
			bw << tank.TankTransform.m[i][1];
			bw << tank.TankTransform.m[i][2];
			bw << tank.TankTransform.m[i][3];
		}

		bw << tank.PotapAngle;
		bw << tank.PosinAngle;
		bw << tank.TankHP;
	}

	header->size = bw.WriteSize();
	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_PLAYER_MOVED(uint8 Id, Matrix4x4 mat, float PotapAngle, float PosinAngle)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Id;

	for (int i = 0; i < 4; ++i)
	{
		bw << mat.m[i][0];  // row i, col 0
		bw << mat.m[i][1];  // row i, col 1
		bw << mat.m[i][2];  // row i, col 2
		bw << mat.m[i][3];  // row i, col 3
	}
	bw << PotapAngle;
	bw << PosinAngle;

	header->size = bw.WriteSize();
	header->id = S_ALL_TANK_STATE;

	sendBuffer->Close(bw.WriteSize());
	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_TANK_HIT(uint8 id)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;

	header->size = bw.WriteSize();
	header->id = S_TANK_HIT;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_TANK_DAMAGED(uint8 id)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;

	header->size = bw.WriteSize();
	header->id = S_TANK_DAMAGED;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_TANK_DEAD(uint8 id)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;

	header->size = bw.WriteSize();
	header->id = S_TANK_DEAD;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_TANK_KILL(uint8 id)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << id;

	header->size = bw.WriteSize();
	header->id = S_TANK_KILL;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}


SendBufferRef ServerPacketHandler::Make_S_ROOM_PLAYER_STATES(const std::vector<Room_Ready_Data>& dataList)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	// ��� ����
	PacketHeader* header = bw.Reserve<PacketHeader>();

	// ������ ���� ���� ����
	bw << static_cast<uint16>(dataList.size());

	// �� Room_Ready_Data ����ȭ
	for (auto data : dataList)
	{
		bw << data.PlayerID << data.Position << data.Team << data.IsReady;
	}


	// ��� ���� ä���
	header->size = bw.WriteSize();
	header->id = S_ROOM_PLAYER_STATES;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_ROOM_DATA(uint8 id)
{

	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	std::vector<Room_Data> temp = Room_Manager::Get_Instance()->Client_ShowRoom();

	bw << (uint32)temp.size();

	for (auto& data : temp)
		bw << data;

	header->size = bw.WriteSize();
	header->id = S_ROOM_DATA;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;

}

SendBufferRef ServerPacketHandler::Make_S_ROOM_ENTER(uint8 RoomNum, uint8 CurPlayerCnt)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << RoomNum;
	bw << CurPlayerCnt;


	header->size = bw.WriteSize();
	header->id = S_ROOM_ENTER;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}
