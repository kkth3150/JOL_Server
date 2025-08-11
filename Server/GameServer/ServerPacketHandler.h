#pragma once
#include "Define.h"

struct Matrix4x4;
enum
{
	S_TEST = 1,//for Dummy
	S_SUCCESS_LOGIN = 2,
	S_SUCCESS_ENTER_ROOM = 3,
	S_GAME_START = 4,
	S_ALL_TANK_STATE = 5,
	S_WEAPON_HIT = 6,
	S_ROOM_DATA = 7,
	S_ROOM_ENTER = 8,
	S_ROOM_PLAYER_STATES = 9,
	S_ROOM_ALL_PLAYER_FINISH_LOADING = 10,
	S_TANK_HIT = 11,
	S_TANK_DAMAGED = 12,
	S_TANK_DEAD = 13,
	S_TANK_KILL = 14,
	S_GAME_WIN = 15,
	S_GAME_LOSE = 16,
	S_CAPTURE = 17,
	S_ALL_DRONE_STATE = 18,
	S_BULLET_ADD = 19,
	S_AIRDROP_INDEX = 20,

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
	C_START = 1012,
	C_RESPAWN_TANK = 1013,
	C_MYPOS = 1014,
	C_MYPOSIN = 1015,
	C_MYDRONEMOVE = 1016,
	C_AIRDROP = 1017

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
	static void Handle_C_LOGIN(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_KEYINPUT(PacketSessionRef& session, BYTE* buffer, int32 len);

	//For Room
	static void Handle_C_SHOW_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_CREATE_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_JOIN_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_EXIT_ROOM(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_CHANGE_INFO(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_READY(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_GAMESTART(PacketSessionRef& session, BYTE* buffer, int32 len);
	
	//For GamePlay
	static void Handle_C_LOADING_FINISH(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_MOVEMENT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_POS_MOVE(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_POSIN_MOVE(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_SHOT(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_TANK_RESPAWN(PacketSessionRef& session, BYTE* buffer, int32 len);
	
	static void Handle_C_DRONE_MOVE(PacketSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_AIRDROP(PacketSessionRef& session, BYTE* buffer, int32 len);





	/*--------------
		For Send
	--------------*/

	//For GamePlay
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack);
	static SendBufferRef Make_S_SUCCESS_ENTER_ROOM(uint16 id);
	static SendBufferRef Make_S_WEAPON_HIT(float x, float y, float z);
	static SendBufferRef Make_S_ALL_TANK_STATE(std::vector<Tank_INFO>& tanks);
	static SendBufferRef Make_S_ALL_DRONE_STATE(std::vector<Drone_INFO>& drones);

	static SendBufferRef Make_S_PLAYER_MOVED(uint8 id, Matrix4x4 mat, float PotapAngle, float PosinAngle);
	static SendBufferRef Make_S_TANK_HIT(uint8 id);
	static SendBufferRef Make_S_TANK_DAMAGED(uint8 id);
	static SendBufferRef Make_S_TANK_DEAD(uint8 id);
	static SendBufferRef Make_S_TANK_KILL(uint8 id);

	//ForLogin
	static SendBufferRef Make_S_SUCCESS_LOGIN(uint16 id);
	//For RoomList
	static SendBufferRef Make_S_ROOM_DATA(uint8 id);
	//For InRoom
	static SendBufferRef Make_S_ROOM_ENTER(uint8 RoomNum, uint8 GamePosNum);
	static SendBufferRef Make_S_ROOM_PLAYER_STATES(const std::vector<Room_Ready_Data>& dataList);
	static SendBufferRef Make_S_GAME_START(uint8 Dummy);
	static SendBufferRef Make_S_ALL_PLAYER_LOADING_FINISH(uint8 Dummy);
	static SendBufferRef Make_S_GAME_WIN(uint8 Dummy);
	static SendBufferRef Make_S_GAME_LOSE(uint8 Dummy);
	static SendBufferRef Make_S_CAPTURE(uint8 BULE, uint8  Red);
	static SendBufferRef Make_S_BULLETADD(float DirX, float DirY, float DirZ, float PosX,float PosY, float PosZ);
	static SendBufferRef Make_S_AIRDROP(uint8 AreaIndex);

};

