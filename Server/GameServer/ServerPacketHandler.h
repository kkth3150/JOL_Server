#pragma once
#include "Define.h"

struct Matrix4x4;
enum
{
	S_TEST = 1,//for Dummy
	S_SUCCESS_LOGIN = 2,
	S_SUCCESS_ENTER_ROOM = 3,
	S_GAME_START = 4,
	S_PLAYER_MOVE = 5,
	S_WEAPON_HIT = 6,
	S_ROOM_DATA = 7,
	S_ROOM_ENTER = 8,
	S_ROOM_PLAYER_STATES = 9,
	C_LOGIN = 1001,
	C_FINISH_LOADING = 1002,
	C_KEYINPUT = 1003,
	C_MOVEMENT = 1004,
	C_SHOT = 1005,
	C_SHOW_ROOM = 1006,
	C_CREATE_ROOM = 1007,
	C_JOIN_ROOM = 1008,
	C_EXIT_ROOM = 1009,
	C_CHANGE_INFO = 1010,
	C_READY = 1011,
	C_START = 1012
};

struct BuffData
{
	uint64 buffId;
	float remainTime;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len);

	//For Recv
	static void Handle_C_LOGIN(PacketSessionRef& session,BYTE* buffer, int32 len);
	static void Handle_C_KEYINPUT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_MOVEMENT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_SHOT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_FINISH_LOADING(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_SHOW_ROOM(PacketSessionRef& session, BYTE* buffer,int32 len);
	static void Handle_C_CREATE_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_JOIN_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_EXIT_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_CHANGE_INFO(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_READY(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_GAMESTART(PacketSessionRef& session, BYTE* buffer, int32 len);

	//For Send
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack);
	static SendBufferRef Make_S_SUCCESS_ENTER_ROOM(uint16 id);
	static SendBufferRef Make_S_WEAPON_HIT(float x, float y, float z);
	static SendBufferRef Make_S_PLAYER_MOVED(uint8 id, Matrix4x4 mat, float PotapAngle, float PosinAngle);
	
	//ForLogin
	static SendBufferRef Make_S_SUCCESS_LOGIN(uint16 id);
	//For RoomList
	static SendBufferRef Make_S_ROOM_DATA(uint8 id);
	//For InRoom
	static SendBufferRef Make_S_ROOM_ENTER(uint8 RoomNum, uint8 GamePosNum);
	static SendBufferRef Make_S_ROOM_PLAYER_STATES(const std::vector<Room_Ready_Data>& dataList);
	static SendBufferRef Make_S_GAME_START(uint8 Dummy);


};

