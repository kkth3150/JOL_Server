#pragma once
#include "ObjectManager.h"

enum ROOM_STATE {

	ROOM_UNACTIVATE, ROOM_WAITTING, ROOM_INGAME, ROOM_END

};


class Room 
{
public:
	Room();
	Room(uint64 Max);
	~Room();

public:
	void Initialize();
	void Update(float deltaTime);
	void LateUpdate();
	void Release();

public:
	//for PlayerManagement
	void Accept_Player(PlayerRef Player);
	void Leave_Player(PlayerRef Player);
	bool Change_Player_Info(uint64 playerID, const Room_Ready_Data& newData);

	bool Ready_Player(uint64 playerID);

public:

	void Broadcast_GameStart();

public:

	bool CollisionTest();
	
	
public:
	//for data
	void Broadcast_PlayerData(Vec3 p1, Vec3 p2);
	void Broadcast_All_TankState(uint8 PlayersCnt);
	void Broadcast_Hit_Weapon(Vec3 Pos);
	void Broadcast(SendBufferRef sendBuffer);

public:
	//for Debug
	void ShowRoomData();
	void ShowTankState(uint8 Id);
	void ShowBulletCnt();

public:
	//for PlayerSet
	void SetTankState(int64 pID, const Matrix4x4& mat, const float& PosinAngle, const float& PotapAngl);
	Tank_INFO GetTankState(int64 pID);

public:
	//for GamePlay
	void CreateBullet(int8 playerID, WEAPON_ID WeaponID, Vec3 Dir, Vec3 Pos);
	void Check_Bullet_Collision();
	void Detect_Bullet_Tank_Collisions();
	bool Check_OBB_Collision(const Vec3& point, const OBB& obb);

public:
	
	ObjectManager Room_ObjectManager;
	

public:

	void RoomActivate() {

		isActive = true;
	}

	void RoomDeActivate() {

		isActive = false;
	}

	bool GetRoomActivate(){
		return isActive;
	}
	

	int GetRoomPlayerCnt(){
		READ_LOCK;
		return RoomCurPlayerCnt;
	}

	int GetRoomMaxPlayerCnt() {
		READ_LOCK;
		return RoomMaxPlayerCnt;
	}

	void SetRoomID(int id){

		RoomID = id;

	}
	
	int GetRoomID(){

		return RoomID;

	}

	void BroadCast_LobbyInfo();


	bool isStart = false;
	bool isMax = false;
	bool isActive = false;

	unsigned char RoomID;

private:
	uint8 my_RoomID;
	uint8 RoomMaxPlayerCnt;
	uint8 RoomCurPlayerCnt;

	uint8 Wait_LoadingCnt;

	bool waitStartDelay = false;    // 2초 딜레이 시작 여부
	float waitStartTimer = 0.0f;    // 딜레이 타이머 (초)

	//queue<>
private:

	USE_LOCK;
	map<uint64, PlayerRef>			_Players;
	map<uint64, Room_Ready_Data>	_Player_States;


public:

	ROOM_STATE CurState = ROOM_WAITTING;


	/*------------------
	*	For Lobby
	------------------*/

public:
	//for ReadyGame
	bool Check_ClientLoading();
	void Clinet_Loading_Finish();

	uint32 GetPlayers() {
		READ_LOCK;
		return (int)_Players.size();
	};

	bool isFull() {
		READ_LOCK;
		int playerCnt = (int)_Players.size();

		if (playerCnt >= RoomMaxPlayerCnt)
			return true;
		return false;
	}

	void SetMaxPlayer(uint8 maxPlayer) {
		RoomMaxPlayerCnt = maxPlayer;

	}

	bool Wait_Full(uint16 MaxPlayer);
	void Set_Player_Lobby_State(Room_Ready_Data data, uint64 PlayerID);
	void Show_Room_Data();

	bool CanStartGame();
	bool StartGame();

private:


	uint8 RedTeam_MaxCount = 4;
	uint8 BlueTeam_MaxCount = 4;

	uint8 RedTeam_CurCount = 0;
	uint8 BlueTeam_CurCount = 0;



};