#pragma once
#include "ObjectManager.h"

class Tank;

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
	
	void SpawnTanks();

	void Change_Tank_INFO(int64 pID, const Matrix4x4& mat,
		const float& PosinAngle, const float& PotapAngl);

public:

	bool CollisionTest();
	
	
public:
	//for data
	void Broadcast_PlayerData(Vec3 p1, Vec3 p2);
	void Broadcast_All_TankState(uint8 PlayersCnt);
	void Broadcast_All_DroneState();
	void Broadcast_Hit_Weapon(Vec3 Pos);
	void Broadcast(SendBufferRef sendBuffer);
	void Broadcast_Tank_Data();



public:
	//for Debug
	void ShowRoomData();
	void ShowTankState(uint8 Id);
	void ShowBulletCnt();

public:
	//for PlayerSet
	void SetTankState(int64 Tankindex, const Matrix4x4& mat, const float& PosinAngle, const float& PotapAngl);
	void SetTankPosin(int64 index, const float& PotapAngle, const float& PosinAngle);
	void SetTankPos(int64 index, const Matrix4x4& mat);

	void SetDroneState(int64 DroneIndex, const Vec3 Pos, float Yaw, float Roll, float Pitch);

	void SetDroneRespawn(int64 index, const Matrix4x4& mat);

	Tank_INFO GetTankState(int64 pID);

	void SetTankRespawn(int64 index, const Matrix4x4& mat, const float& PotapAngle, const float& PosinAngle);

public:
	//for GamePlay
	void CreateBullet(int8 playerID,uint8 TankIndex, WEAPON_ID WeaponID, Vec3 Dir, Vec3 Pos);
	void CreateBomb(uint8 playerID, uint8 TankIndex, uint8 AreaNum);
	void Check_Bullet_Collision();
	Tank* FindTankByPlayerId(uint8 playerId);
	void UpdateCaptureGauge(float deltaTime);
	void ResetRoom();
	void OnTeamWin(bool isBlueWinner);
	void Detect_Bomb_Tank_Collisions();
	void Detect_Bomb_Terrain_Collisions();
	void Broadcast_All_TankStates();
	void Detect_Bullet_Tank_Collisions();
	void HandleTankHit(Tank* tank, uint8 shooterPlayerID);
	bool Check_OBB_Collision(const Vec3& point, const OBB& obb);
	void Send_RespawnPacket(uint8 tankIndex);
	void Send_SoundData(uint8 tnakIndex, float engvol,float engpit, float trkvol,float trkpit);
	void Send_PingData(uint8 tankIndex, float engvol, float engpit, float trkvol);
	void Detect_Bullet_Terrain_Collisions();


public:
	
	ObjectManager Room_ObjectManager;
	std::chrono::steady_clock::time_point _gameStartTime;

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


	void ChangeRoomState(ROOM_STATE state);

	bool isStart = false;
	bool isMax = false;
	bool isActive = false;

	int lastSentBlueGauge = 0;
	int lastSentRedGauge = 0;

	unsigned char RoomID;

private:
	uint8 my_RoomID;
	uint8 RoomMaxPlayerCnt;
	uint8 RoomCurPlayerCnt;

	uint8 Wait_LoadingCnt = 0;

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


	float blueGauge = 0.f;
	float redGauge = 0.f;

	const float captureRadius = 300.f;
	const float gaugePerTankPerSecond = 100.f / 300.f; // = 0.333f
	bool isGameEnded = false;
};