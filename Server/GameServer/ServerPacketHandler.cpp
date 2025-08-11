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
	case C_MYPOS:
		Handle_C_POS_MOVE(session, buffer, len);
		break;
	case C_MYPOSIN: 
		Handle_C_POSIN_MOVE(session, buffer, len);
		break;
	case C_MYDRONEMOVE:
		Handle_C_DRONE_MOVE(session, buffer, len);
		break;
	case C_AIRDROP:
		Handle_C_AIRDROP(session, buffer, len);
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


	//로그인


}


// -> 또 큐 => 룸마다 관리 같은데에 데이터를 쌓아둔다.
//서버 프레임돌때 첫 프레임에 => 큐에 모든 데이터 처리후 객체들 업데이트 -> 브로드캐스트
//키인풋->

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

	// 행렬 데이터 읽기
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

		// 인덱스로 지정된 탱크 상태 갱신
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
	uint8 TankIndex;
	br >> TankIndex >>InitPos.X >> InitPos.Y >> InitPos.Z
		>> Normalized_Dir.X >> Normalized_Dir.Y >> Normalized_Dir.Z;

	uint8 ID = C_Session->_players[0]->playerID;
	uint8 RoomNum = C_Session->_players[0]->RoomNum;
	Room_Manager::Get_Instance()->Get_Room(RoomNum)->CreateBullet(ID, TankIndex,WeaponID, Normalized_Dir, InitPos);
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

	// 행렬 데이터 읽기
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

		// 인덱스로 지정된 탱크 상태 갱신
		Room_Manager::Get_Instance()->Get_Room(roomID)->SetTankRespawn(tankIndex, mat, potapRotation, posinRotation);

	}

}

void ServerPacketHandler::Handle_C_DRONE_MOVE(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 Droneindex;
	br >> Droneindex;

	float PosX, PosY, PosZ, Yaw, Roll, Pitch;

	br >> PosX >> PosY  >> PosZ >> Yaw >> Roll >> Pitch;
	Vec3 Pos = { PosX ,PosY, PosZ };


	if (!C_Session->_players.empty()) {
		uint64 playerID = C_Session->_players[0]->playerID;
		int roomID = C_Session->_players[0]->RoomNum;
		Room_Manager::Get_Instance()->Get_Room(roomID)->SetDroneState(Droneindex, Pos, Yaw, Roll, Pitch);

	}
}

void ServerPacketHandler::Handle_C_AIRDROP(PacketSessionRef& session, BYTE* buffer, int32 len)
{

	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 TankIndex;
	uint8 AreaIndex;
	br >> TankIndex;
	br >> AreaIndex;


	if (!C_Session->_players.empty()) {
		uint64 playerID = C_Session->_players[0]->playerID;
		int roomID = C_Session->_players[0]->RoomNum;

		Room_Manager::Get_Instance()->Get_Room(roomID)->CreateBomb(playerID, TankIndex, AreaIndex);
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

void ServerPacketHandler::Handle_C_POSIN_MOVE(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 tankIndex;
	br >> tankIndex;

	float potapRotation;
	float posinRotation;

	// 행렬 데이터 읽기

	br >> potapRotation >> posinRotation;

	if (!C_Session->_players.empty()) {
		uint64 playerID = C_Session->_players[0]->playerID;
		int roomID = C_Session->_players[0]->RoomNum;

		// 인덱스로 지정된 탱크 상태 갱신
		Room_Manager::Get_Instance()->Get_Room(roomID)->SetTankPosin(tankIndex,  potapRotation, posinRotation);
	}
}

void ServerPacketHandler::Handle_C_POS_MOVE(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	ClientSessionRef C_Session = static_pointer_cast<ClientSession>(session);
	BufferReader br(buffer, len);
	PacketHeader header;
	br >> header;

	uint8 tankIndex;
	br >> tankIndex;
	Matrix4x4 mat;

	// 행렬 데이터 읽기
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			br >> mat.m[i][j];
		}
	}



	if (!C_Session->_players.empty()) {
		uint64 playerID = C_Session->_players[0]->playerID;
		int roomID = C_Session->_players[0]->RoomNum;

		// 인덱스로 지정된 탱크 상태 갱신
		Room_Manager::Get_Instance()->Get_Room(roomID)->SetTankPos(tankIndex, mat);
	}

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

	bw << id;						// 먼저 데이터를 쓴다
	header->size = bw.WriteSize();	// 그다음 정확한 전체 크기를 설정한다
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

SendBufferRef ServerPacketHandler::Make_S_CAPTURE(uint8 BULE, uint8 RED)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << BULE << RED;

	header->size = bw.WriteSize();
	header->id = S_CAPTURE;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_BULLETADD(float DirX, float DirY, float DirZ, float PosX, float PosY, float PosZ)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << DirX << DirY << DirZ << PosX << PosY << PosZ;
	header->size = bw.WriteSize();
	header->id = S_BULLET_ADD;

	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_AIRDROP(uint8 AreaIndex)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << AreaIndex;

	header->size = bw.WriteSize();
	header->id = S_AIRDROP_INDEX;

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

	bw << static_cast<uint16>(tanks.size());

	uint8 Tankindex = 0;
	for (Tank_INFO& tank : tanks)
	{
		bw << Tankindex++;

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
	header->id = S_ALL_TANK_STATE;
	sendBuffer->Close(bw.WriteSize());
	
	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_ALL_DRONE_STATE(std::vector<Drone_INFO>& drones)
{

	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();
	bw << static_cast<uint16>(drones.size());
	uint8 Droneindex = 0;

	for (Drone_INFO& drone : drones)
	{
		bw << Droneindex++;

		bw << drone.DroneTransform.X << drone.DroneTransform.Y << drone.DroneTransform.Z;
		bw << drone.Yaw << drone.Roll << drone.Pitch;
		bw << drone.DroneHP;

	}

	header->size = bw.WriteSize();
	header->id = S_ALL_DRONE_STATE;
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

SendBufferRef ServerPacketHandler::Make_S_TANK_HIT(uint8 Damaged_Tank_Index)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Damaged_Tank_Index;

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

SendBufferRef ServerPacketHandler::Make_S_TANK_DEAD(uint8 Dead_Tank_index)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Dead_Tank_index;

	header->size = bw.WriteSize();
	header->id = S_TANK_DEAD;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}

SendBufferRef ServerPacketHandler::Make_S_TANK_KILL(uint8 Dead_Tank_Index)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);

	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	PacketHeader* header = bw.Reserve<PacketHeader>();

	bw << Dead_Tank_Index;

	header->size = bw.WriteSize();
	header->id = S_TANK_KILL;


	sendBuffer->Close(bw.WriteSize());

	return sendBuffer;
}


SendBufferRef ServerPacketHandler::Make_S_ROOM_PLAYER_STATES(const std::vector<Room_Ready_Data>& dataList)
{
	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	BufferWriter bw(sendBuffer->Buffer(), sendBuffer->AllocSize());

	// 헤더 예약
	PacketHeader* header = bw.Reserve<PacketHeader>();

	// 데이터 개수 먼저 쓰기
	bw << static_cast<uint16>(dataList.size());

	// 각 Room_Ready_Data 직렬화
	for (auto data : dataList)
	{
		bw << data.PlayerID << data.Position << data.Team << data.IsReady;
	}


	// 헤더 정보 채우기
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
