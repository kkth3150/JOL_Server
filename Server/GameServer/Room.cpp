#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "ClientSession.h"
#include "ServerPacketHandler.h"
#include "AbstractFactory.h"
#include "Tank.h"
#include "Normal_Potan.h"
#include "ObjectManager.h"
#include "Terrain_Manager.h"
#include "SGlobal.h"
#include "Collision_Manager.h"
#include "Drone.h"
#include "AirDrop_Bomb.h"

Room::Room() 
{
	SetMaxPlayer(8);
	isActive = false;
}

Room::Room(uint64 Max)
{

}

Room::~Room()
{
}

void Room::Initialize()
{
	CurState = ROOM_UNACTIVATE;
}

void Room::Update(float deltaTime)
{
	system("cls");

	switch (CurState) {

	case ROOM_UNACTIVATE:

		break;
	case ROOM_WAITTING:
	{
		if (Check_ClientLoading())
			ChangeRoomState(ROOM_INGAME);
	}
		break;
	case ROOM_INGAME:
	{
		Room_ObjectManager.Update(deltaTime);
		
		UpdateCaptureGauge(deltaTime);
		Detect_Bullet_Tank_Collisions();
		Detect_Bullet_Terrain_Collisions();
		
		Detect_Bomb_Terrain_Collisions();
		Detect_Bomb_Tank_Collisions();

	}
		break;
	default:
		break;
	}

}

void Room::LateUpdate()
{

	if (CurState == ROOM_INGAME)
	{
		Room_ObjectManager.Late_Update();
		Broadcast_All_TankStates();
		Broadcast_All_DroneState();

		auto now = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - _gameStartTime).count();

		std::cout << "[���� ��� �ð�] " << elapsed << "�� �����" << std::endl;
		// �� ������ ��ũ ��ǥ ���
		auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
		if (tankList)
		{
			std::cout << "---- ���� ������ ��ũ ��ġ ----" << std::endl;
			for (size_t i = 0; i < tankList->size(); ++i)
			{
				Tank* tank = dynamic_cast<Tank*>((*tankList)[i]);
				if (!tank) continue;

				//const Matrix4x4& mat = tank->GetTankState().TankTransform;
				Tank_INFO info = tank->GetTankState();
				std::cout << "��ũ �ε���: " << i
					<< " | X: " << info.TankTransform.m[3][0]
					<< " | Y: " << info.TankTransform.m[3][1]
					<< " | Z: " << info.TankTransform.m[3][2]
					<< " | ��ž ����: " << info.PotapAngle
					<< " | ���� ����: " << info.PosinAngle
					<<info.TankHP <<std::endl;
				
			}


		}

		auto droneList = Room_ObjectManager.Get_List(OBJ_DRONE);
		if (droneList)
		{
			std::cout << "---- ���� ������ ��� ��ġ ----" << std::endl;
			for (size_t i = 0; i < droneList->size(); ++i)
			{
				Drone* drone = dynamic_cast<Drone*>((*droneList)[i]);
				if (!drone) continue;

				Drone_INFO dInfo = drone->GetDroneState();
				std::cout << "��� �ε���: " << i
					<< " | X: " << dInfo.DroneTransform.X
					<< " | Y: " << dInfo.DroneTransform.Y
					<< " | Z: " << dInfo.DroneTransform.Z
					<< " | Yaw: " << dInfo.Yaw
					<< " | Roll: " << dInfo.Roll
					<< " | Pitch: " << dInfo.Pitch
					<< std::endl;
			}
		}
	}
}

void Room::Release()
{

}

#pragma region ForReady

void Room::Accept_Player(PlayerRef Player)
{
	
	WRITE_LOCK;
	_Players[Player->playerID] = Player;

	// ���� ���� �ο� �� ���
	int blueCount = 0;
	int redCount = 0;

	for (const auto& pair : _Player_States)
	{
		if (pair.second.Team) // true == Blue
			blueCount++;
		else
			redCount++;
	}

	// ������ �� ����
	bool assignedTeam = false; // false == Red, true == Blue
	if (_Player_States.empty())
	{
		// ù �����ڴ� ������ Blue
		assignedTeam = true;
	}
	else
	{
		assignedTeam = (blueCount <= redCount); // �ο��� ���� ��
	}

	// ������ �ĺ��� ����
	std::vector<uint8> usedPositions;
	for (const auto& pair : _Player_States)
		usedPositions.push_back(pair.second.Position);

	uint8 assignedPosition = 0;

	if (assignedTeam) // BLUE ��: 1 ~ 8
	{
		for (uint8 pos = 1; pos <= 8; ++pos)
		{
			if (std::find(usedPositions.begin(), usedPositions.end(), pos) == usedPositions.end())
			{
				assignedPosition = pos;
				break;
			}
		}
	}
	else // RED ��: 9 ~ 16
	{
		for (uint8 pos = 9; pos <= 16; ++pos)
		{
			if (std::find(usedPositions.begin(), usedPositions.end(), pos) == usedPositions.end())
			{
				assignedPosition = pos;
				break;
			}
		}
	}

	// ���� ������ ���� �� ����
	Room_Ready_Data playerData;
	playerData.PlayerID = static_cast<uint8>(Player->playerID);  // uint64 �� uint8 ĳ���� ����
	playerData.Team = assignedTeam;
	playerData.Position = assignedPosition;
	playerData.IsReady = false;

	_Player_States[Player->playerID] = playerData;
	++RoomCurPlayerCnt;
}

void Room::Leave_Player(PlayerRef Player)
{
	WRITE_LOCK;

	const uint64 playerID = Player->playerID;

	// 1. �÷��̾� ��Ͽ��� ����
	auto playerIt = _Players.find(playerID);
	if (playerIt != _Players.end())
		_Players.erase(playerIt);

	// 2. ���� �����Ϳ��� ����
	auto stateIt = _Player_States.find(playerID);
	if (stateIt != _Player_States.end())
		_Player_States.erase(stateIt);

	// 3. ���� �ο� �� ����
	if (RoomCurPlayerCnt > 0)
		--RoomCurPlayerCnt;

	// 4. �÷��̾ ��� ���� ��� �� �� ��Ȱ��ȭ

}

bool Room::Change_Player_Info(uint64 playerID, const Room_Ready_Data& newData)
{
	WRITE_LOCK;

	// �÷��̾ �����ϴ��� ���� Ȯ��
	auto it = _Player_States.find(playerID);
	if (it == _Player_States.end())
		return false;

	// ��û�� �������� �̹� �ٸ� ����� ���� �ִ��� Ȯ��
	for (const auto& pair : _Player_States)
	{
		if (pair.first == playerID) continue; // ���� ����

		if (pair.second.Position == newData.Position)
			return false; // ������ �浹
	}

	// ��ȿ�ϴٸ� ������ ����
	Room_Ready_Data updated = newData;
	updated.PlayerID = static_cast<uint8>(playerID); // ID ����

	_Player_States[playerID] = updated;

	// ���� ���� ��ü ����
	//BroadCast_LobbyInfo();

	return true;
}

bool Room::Ready_Player(uint64 playerID)
{
	WRITE_LOCK;

	auto it = _Player_States.find(playerID);
	if (it == _Player_States.end())
		return false;

	// �̹� Ready ���¶�� false ����
	if (it->second.IsReady)
		return false;

	// Ready ���·� ���� �� true ����
	it->second.IsReady = true;

	return true;
}

void Room::Set_Player_Lobby_State(Room_Ready_Data data, uint64 PlayerID)
{
	WRITE_LOCK;
	_Player_States[PlayerID] = data;

}

bool Room::Check_ClientLoading()
{
	READ_LOCK;
	if (Wait_LoadingCnt >= RoomCurPlayerCnt) {
		return true;
	}
	return false;
}

void Room::Clinet_Loading_Finish()
{
	WRITE_LOCK;
	Wait_LoadingCnt++;
}

bool Room::CanStartGame()
{
	READ_LOCK;

	if (_Player_States.empty())
		return false;

	for (const auto& pair : _Player_States)
	{
		if (!pair.second.IsReady)
			return false;
	}

	return true;
}

bool Room::StartGame()
{
	WRITE_LOCK;

	if (CurState == ROOM_INGAME)
		return false;

	CurState = ROOM_INGAME;
	isStart = true;

	return true;

}

void Room::BroadCast_LobbyInfo()
{
	std::vector<Room_Ready_Data> playerStates;
	{
		READ_LOCK;

		for (const auto& pair : _Player_States)
		{
			uint64 playerID = pair.first;
			Room_Ready_Data data = pair.second;

			data.PlayerID = static_cast<uint8>(playerID);
			playerStates.push_back(data);
		}
	}

	SendBufferRef sendBuffer = ServerPacketHandler::Make_S_ROOM_PLAYER_STATES(playerStates);
	Broadcast(sendBuffer);
}


#pragma endregion Before Start


void Room::Broadcast_GameStart()
{

	SendBufferRef sendBuffer = ServerPacketHandler::Make_S_GAME_START(1);
	Broadcast(sendBuffer);
}



void Room::ChangeRoomState(ROOM_STATE state)
{
	if (state == ROOM_INGAME) {

		StartGame();
		SpawnTanks();
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_ALL_PLAYER_LOADING_FINISH(1);
		Broadcast(sendBuffer);

	}
}



#pragma region ForGamePlay


void Room::SpawnTanks()
{
	std::map<int, std::vector<Room_Ready_Data>> tankMap;

	// ������ ������� �׷�ȭ (¦��: ��ž, Ȧ��: ������ �� ������ ���� �׷���)
	for (const auto& pair : _Player_States)
	{
		const Room_Ready_Data& player = pair.second;
		int key = (player.Position % 2 == 0) ? player.Position - 1 : player.Position;
		tankMap[key].push_back(player);
	}

	int tankIndex = 0;
	for (const auto& tankEntry : tankMap)
	{
		int driverPos = tankEntry.first;
		const std::vector<Room_Ready_Data>& passengers = tankEntry.second;

		bool isBlue = driverPos <= 8;

		float x = 50.f * tankIndex;
		float y = 40.f;
		float z = isBlue ? 50.f : 150.f;

		Matrix4x4 tankMat = Matrix4x4::CreateTranslation(x, y, z);
		Matrix4x4 droneMat = Matrix4x4::CreateTranslation(x, y + 50.f, z); // ��ũ ���� ������

		// ��ũ ����
		{
			GameObject* tankObj = CAbstractFactory<Tank>::Create();
			Tank* tank = dynamic_cast<Tank*>(tankObj);
			tank->SetBlueTeam(isBlue);

			if (!passengers.empty())
				tank->playerId = passengers[0].PlayerID; // ��ǥ��

			for (const auto& rider : passengers)
				tank->AddPassenger(rider);

			tank->SetTankState(tankMat, 0.f, 0.f); // ���� �ñ״�ó �״�� ���
			Room_ObjectManager.Add_Object(OBJ_TANK, tank);
		}

		// ���� ��� ����(��ũ�� ���� �ε����� 1:1 ��Ī) ������������������������������
		{
			GameObject* droneObj = CAbstractFactory<Drone>::Create();
			Drone* drone = dynamic_cast<Drone*>(droneObj);
			drone->SetBlueTeam(isBlue);

			for (const auto& rider : passengers)
				drone->AddPassenger(rider);

			// ��е� ��ũ�� ������ state �Լ��� �ִٰ� ������ �����ϰ� ȣ��
			Vec3 Temp = { x, y + 50.f, z };
			drone->SetDroneState(Temp,0,0,0);

			// (����) ��ȣ ���� ������ �ִٸ� �ε��� ���� ���
			// drone->SetOwnerTankIndex(tankIndex);
			// �Ǵ� tank->SetDroneIndex(tankIndex);

			Room_ObjectManager.Add_Object(OBJ_DRONE, drone);
		}
		tankIndex++;
	}
}


void Room::Change_Tank_INFO(int64 pID, const Matrix4x4& mat, const float& PotapAngle ,const float& PosinAngle)
{
	WRITE_LOCK;
	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))
		[pID])->SetTankState(mat, PotapAngle, PosinAngle);

}


void Room::Broadcast_All_TankStates()
{
	std::vector<Tank_INFO> tankStates;

	// 1. ��� ��ũ ���� ����
	{
		READ_LOCK;
		auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
		if (tankList)
		{
			for (size_t i = 0; i < tankList->size(); ++i)
			{
				Tank* tank = dynamic_cast<Tank*>((*tankList)[i]);
				if (!tank)
					continue;

				Tank_INFO info = tank->GetTankState();
				tankStates.push_back(info);
			}
		}
	}

	// 2. ��Ŷ ���� �� ��� �÷��̾�� ����
	if (!tankStates.empty())
	{
		auto sendBuffer = ServerPacketHandler::Make_S_ALL_TANK_STATE(tankStates);
		Broadcast(sendBuffer);
	}
}

void Room::Broadcast_All_DroneState()
{
	std::vector<Drone_INFO> DroneStates;

	// 1. ��� ��� ���� ����
	{
		READ_LOCK;
		auto DroneList = Room_ObjectManager.Get_List(OBJ_DRONE);
		if (DroneList)
		{
			for (size_t i = 0; i < DroneList->size(); ++i)
			{
				Drone* drone = dynamic_cast<Drone*>((*DroneList)[i]);
				if (!DroneList)
					continue;

				Drone_INFO info = drone->GetDroneState();
				DroneStates.push_back(info);
			}
		}
	}

	// 2. ��Ŷ ���� �� ��� �÷��̾�� ����
	if (!DroneStates.empty())
	{
		auto sendBuffer = ServerPacketHandler::Make_S_ALL_DRONE_STATE(DroneStates);
		Broadcast(sendBuffer);
	}
}

void Room::Broadcast_Hit_Weapon(Vec3 Pos)
{
	for (const auto& iter : _Players)
	{
		uint64 playerID = iter.first;
		PlayerRef player = iter.second;

		auto sendBuffer = ServerPacketHandler::Make_S_WEAPON_HIT(Pos.X, Pos.Y, Pos.Z);
		player->OwenerSession->Send(sendBuffer);
	}
}


#pragma endregion ForGamePlay


void Room::Broadcast(SendBufferRef sendBuffer)
{
	WRITE_LOCK;
	for (const auto& iter : _Players)
	{
		const PlayerRef& player = iter.second;
		if (player && player->OwenerSession)
			player->OwenerSession->Send(sendBuffer);
	}
}



#pragma region function

void Room::ShowRoomData()
{
	std::cout << "======== ROOM INFO ========" << std::endl;
	std::cout << "RoomID: " << (int)RoomID << " | Current Players: " << RoomCurPlayerCnt << " / " << RoomMaxPlayerCnt << std::endl;
	std::cout << "Active: " << (isActive ? "True" : "False") << " | State: ";

	switch (CurState)
	{
	case ROOM_UNACTIVATE:
		break;

	case ROOM_WAITTING: {
		std::cout << "WAITING";
		std::cout << std::endl;
		std::cout << "============================" << std::endl;

		std::cout << "Players in Room:" << std::endl;

		for (const auto& iter : _Players)
		{
			PlayerRef player = iter.second;
			if (player)
			{
				Room_Ready_Data& data = _Player_States[player->playerID];

				std::cout << "PlayerID: " << player->playerID
					<< " | Team: " << (data.Team ? "Blue" : "Red")
					<< " | Position: " << (int)data.Position
					<< " | Loaded: " << (data.IsReady ? "Yes" : "No")
					<< std::endl;
			}
		}
		std::cout << "============================" << std::endl;
	}
		break;
	case ROOM_INGAME:
		std::cout << "INGAME";
		break;
	case ROOM_END:
		std::cout << "END";
		break;
	}
	
}



void Room::ShowTankState(uint8 Id)
{
	READ_LOCK;
	Tank_INFO Tank0state = GetTankState(Id);
	std::cout << "Tank  "<<  Id  <<"  ����" << std::endl;
	std::cout << "X: " << Tank0state.TankTransform.m[3][0] << std::endl;  // _41
	std::cout << "Y: " << Tank0state.TankTransform.m[3][1] << std::endl;  // _42
	std::cout << "Z: " << Tank0state.TankTransform.m[3][2] << std::endl;  // _43
	std::cout << "��ž ����: " << Tank0state.PotapAngle << std::endl;
	std::cout << "���� ����: " << Tank0state.PosinAngle << std::endl;
	std::cout << "HP: " << static_cast<int>(Tank0state.TankHP) << std::endl;
}

void Room::ShowBulletCnt()
{

	int BulletCnt = 0;
	if (Room_ObjectManager.Get_List(OBJ_WEAPON)) {
		READ_LOCK;
		BulletCnt = Room_ObjectManager.Get_List(OBJ_WEAPON)->size();
	}
	cout << "������ �Ѿ� : " << BulletCnt << " ��" << endl;
}

bool Room::Wait_Full(uint16 MaxPlayer)
{
	READ_LOCK;
	int playerCnt = (int)_Players.size();

	std::cout << "�÷��̾� ���� ��� ��" << std::endl;
	std::cout << "���� �÷��̾� (" << playerCnt << ") ��" << std::endl;
	std::cout << "�ִ� �÷��̾� (" << MaxPlayer << ") ��" << std::endl;

	
	if (playerCnt >= MaxPlayer)
		return true;

	return false;
		
}

#pragma endregion ForDebug

void Room::SetTankState(int64 index, const Matrix4x4& mat, const float& PotapAngle, const float& PosinAngle)
{
	WRITE_LOCK;
	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[index])->SetTankState(mat, PotapAngle, PosinAngle);

}

void Room::SetTankPosin(int64 index, const float& PotapAngle, const float& PosinAngle) {
	WRITE_LOCK;
	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[index])->SetTankOnlyPosin(PosinAngle, PotapAngle);
}

void Room::SetTankPos(int64 index, const Matrix4x4& mat) {

	WRITE_LOCK;
	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[index])->SetTankOnlyPos(mat);
}

void Room::SetDroneState(int64 DroneIndex, const Vec3 Pos, float Yaw, float Roll, float Pitch)
{

	WRITE_LOCK;
	dynamic_cast<Drone*>((*Room_ObjectManager.Get_List(OBJ_DRONE))[DroneIndex])->SetDroneState(Pos, Yaw, Roll, Pitch);
}

void Room::SetDroneRespawn(int64 index, const Matrix4x4& mat)
{
	WRITE_LOCK;
	dynamic_cast<Drone*>((*Room_ObjectManager.Get_List(OBJ_DRONE))[index])->SetSpawn(mat);
}

void Room::SetTankRespawn(int64 index, const Matrix4x4& mat, const float& PotapAngle, const float& PosinAngle)
{
	WRITE_LOCK;
	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[index])->SetSpawn(mat, PotapAngle,PosinAngle);
}

Tank_INFO Room::GetTankState(int64 index)
{
	READ_LOCK;
	return dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[index])->GetTankState();

}

void Room::CreateBullet(int8 pID, uint8 tankindex,WEAPON_ID ID, Vec3 Dir, Vec3 Pos)
{
	WRITE_LOCK;

	switch (ID) {

	case WEAPON_NPOTAN:
	{
		bool isBlueTeam = dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[tankindex])->isBlueTeam();
		GameObject* TempBullet = CAbstractFactory<Normal_Potan>::Create();
		dynamic_cast<Normal_Potan*>(TempBullet)->SetInitData(Dir, Pos, tankindex ,pID, isBlueTeam);
		Room_ObjectManager.Add_Object(OBJ_WEAPON, TempBullet);

		auto sendBuffer = ServerPacketHandler::Make_S_BULLETADD(Dir.X, Dir.Y, Dir.Z, Pos.X,Pos.Y,Pos.Z);
		Broadcast(sendBuffer);

	}
	break;

	case WEAPON_NBULLET:
		break;

	default:
		break;


	}
}

void Room::CreateBomb(uint8 playerID, uint8 TankIndex, uint8 AreaNum)
{
	WRITE_LOCK;

	if (AreaNum < 1 || AreaNum > 9) return;

	// ���� X ����(���� ����)
	constexpr float WORLD_MIN_X = -500.f;
	constexpr float WORLD_MAX_X = 500.f;

	// Area ���
	float minX, maxX, minZ, maxZ;
	switch (AreaNum)
	{
	case 1: minX = -500; maxX = -167; minZ = -500; maxZ = -167; break;
	case 2: minX = -167; maxX = 167; minZ = -500; maxZ = -167; break;
	case 3: minX = 167; maxX = 500; minZ = -500; maxZ = -167; break;

	case 4: minX = -500; maxX = -167; minZ = -167; maxZ = 167; break;
	case 5: minX = -167; maxX = 167; minZ = -167; maxZ = 167; break;
	case 6: minX = 167; maxX = 500; minZ = -167; maxZ = 167; break;

	case 7: minX = -500; maxX = -167; minZ = 167; maxZ = 500; break;
	case 8: minX = -167; maxX = 167; minZ = 167; maxZ = 500; break;
	case 9: minX = 167; maxX = 500; minZ = 167; maxZ = 500; break;
	default: return;
	}

	// ��ġ �Ķ����
	constexpr int   bombsPerLine = 6;   // �ٴ� 6�� �� �� 12��
	constexpr float PAD = 10.f;
	constexpr float Z_OFFSET = 40.f; // �� �� ����
	constexpr float BASE_ALT = 300.f; // ���� ���� �������� �⺻ ��
	constexpr float RAMP_ALT = 300.f; // ���� ���� ������ �߰��Ǵ� ��
	const bool leftToRight = true;       // ���� ����(�¡��). �ݴ�� false

	const float leftX = minX + PAD;
	const float rightX = maxX - PAD;
	const float centerZ = (minZ + maxZ) * 0.5f;

	for (int line = 0; line < 2; ++line) // 0: ����, 1: �Ʒ���
	{
		const float zLine = centerZ + (line == 0 ? -Z_OFFSET : Z_OFFSET);

		for (int i = 0; i < bombsPerLine; ++i)
		{
			// Area ���ο��� X ���(���� ��ġ)
			const float tLocal = (bombsPerLine == 1) ? 0.f : float(i) / float(bombsPerLine - 1);
			const float x = leftX + (rightX - leftX) * tLocal;

			// ���� X ���� �� ����(���� ����)
			float tGlobal = (x - WORLD_MIN_X) / (WORLD_MAX_X - WORLD_MIN_X); // 01
			tGlobal = std::clamp(tGlobal, 0.f, 1.f);
			if (!leftToRight) tGlobal = 1.f - tGlobal;

			const float y = BASE_ALT + RAMP_ALT * tGlobal;

			Vec3 pos{ x, y, zLine };

			GameObject* obj = CAbstractFactory<AirDrop_Bomb>::Create();
			if (auto* bomb = dynamic_cast<AirDrop_Bomb*>(obj))
				bomb->SetInitData(playerID, TankIndex, pos);

			Room_ObjectManager.Add_Object(OBJ_BOMB, obj); // �ʿ�� ī�װ� ����
		}
	}

	auto sendBuffer = ServerPacketHandler::Make_S_AIRDROP(AreaNum);
	Broadcast(sendBuffer);

}



#pragma region function


void Room::Detect_Bullet_Tank_Collisions()
{
	auto bulletList = Room_ObjectManager.Get_List(OBJ_WEAPON);
	auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
	if (!bulletList || !tankList) return;

	for (GameObject* objBullet : *bulletList)
	{
		if (!objBullet) continue;

		Normal_Potan* bullet = dynamic_cast<Normal_Potan*>(objBullet);
		if (!bullet || bullet->isHit()) continue;

		Vec3 bulletPos = bullet->GetPos();
		uint8 shooterPlayerID = bullet->GetOwnerID();
		bool shooterTeam = bullet->isBlueTeam();

		for (size_t i = 0; i < tankList->size(); ++i)
		{
			Tank* targetTank = dynamic_cast<Tank*>((*tankList)[i]);
			if (!targetTank) continue;

			// �Ʊ��̸� ����
			if (targetTank->isBlueTeam() == shooterTeam)
				continue;

			if (!targetTank->isSpawned()) continue;

			// �浹 ����
			if (!CollisionManager::GetInstance()->CheckCollision_Point_Sphere(bulletPos, targetTank->GetPos(), 5.0f))
				continue;

			// �浹 �� ó��
			auto effectBuffer = ServerPacketHandler::Make_S_WEAPON_HIT(bulletPos.X, bulletPos.Y, bulletPos.Z);
			Broadcast(effectBuffer);

			bullet->SetDead();
			targetTank->Damage(25);

			// �ǰ��ڿ��� TANK_DAMAGED
			for (const Room_Ready_Data& damagedRider : targetTank->GetPassengers())
			{
				auto it = _Players.find(damagedRider.PlayerID);
				if (it != _Players.end() && it->second && it->second->OwenerSession)
				{
					auto buffer = ServerPacketHandler::Make_S_TANK_DAMAGED((uint8)i);
					it->second->OwenerSession->Send(buffer);
				}
			}

			Tank* shooterTank = nullptr;
			{
				const uint8 ownerIdx = bullet->GetOwnerTankIndex();
				if (ownerIdx < tankList->size())
					shooterTank = dynamic_cast<Tank*>((*tankList)[ownerIdx]);
			}

			// �����ڿ��� TANK_HIT
			if (shooterTank)
			{
				for (const Room_Ready_Data& shooter : shooterTank->GetPassengers())
				{
					auto it = _Players.find(shooter.PlayerID);
					if (it != _Players.end() && it->second && it->second->OwenerSession)
					{
						auto buffer = ServerPacketHandler::Make_S_TANK_HIT((uint8)i);
						it->second->OwenerSession->Send(buffer);
					}
				}
			}

			// ��� ����
			if (targetTank->IsDead())
			{
				targetTank->SetUnSpawn();

				auto bufferDead = ServerPacketHandler::Make_S_TANK_DEAD((uint8)i);
				Broadcast(bufferDead);

				if (shooterTank)
				{
					auto bufferKill = ServerPacketHandler::Make_S_TANK_KILL((uint8)i);
					for (const Room_Ready_Data& killer : shooterTank->GetPassengers())
					{
						auto it = _Players.find(killer.PlayerID);
						if (it != _Players.end() && it->second && it->second->OwenerSession)
						{
							it->second->OwenerSession->Send(bufferKill);
						}
					}
				}
			}
			break;
		}
	}
}

void Room::Detect_Bullet_Terrain_Collisions()
{
	auto bulletList = Room_ObjectManager.Get_List(OBJ_WEAPON);
	if (!bulletList) return;

	for (GameObject* objBullet : *bulletList)
	{
		if (!objBullet) continue;

		Normal_Potan* bullet = dynamic_cast<Normal_Potan*>(objBullet);
		if (!bullet || bullet->isHit()) continue;

		if (CollisionManager::GetInstance()->Check_Terrain_Collision(bullet))
		{

		

			Vec3 hitPos = bullet->GetPos();
			bullet->SetDead(); // �Ѿ� ����

			std::cout << "\n=== ���� �浹 �߻� ===" << std::endl;
			std::cout << "�Ѿ� ��ġ: X=" << hitPos.X << " Y=" << hitPos.Y << " Z=" << hitPos.Z << std::endl;

			auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
			if (tankList)
			{
				std::cout << "--- ��ũ OBB ���� ---" << std::endl;
				for (size_t i = 0; i < tankList->size(); ++i)
				{
					Tank* tank = dynamic_cast<Tank*>((*tankList)[i]);
					if (!tank) continue;

					OBB obb = tank->Get_OBB();

					std::cout << "[��ũ " << i << "]" << std::endl;
					std::cout << "Center: X=" << obb.center.X << " Y=" << obb.center.Y << " Z=" << obb.center.Z << std::endl;
					std::cout << "HalfSize: X=" << obb.halfSize.X << " Y=" << obb.halfSize.Y << " Z=" << obb.halfSize.Z << std::endl;

					std::cout << "Axis[0] (Right): X=" << obb.axis[0].X << " Y=" << obb.axis[0].Y << " Z=" << obb.axis[0].Z << std::endl;
					std::cout << "Axis[1] (Up):    X=" << obb.axis[1].X << " Y=" << obb.axis[1].Y << " Z=" << obb.axis[1].Z << std::endl;
					std::cout << "Axis[2] (Look):  X=" << obb.axis[2].X << " Y=" << obb.axis[2].Y << " Z=" << obb.axis[2].Z << std::endl;
				}
			}
			auto sendBuffer = ServerPacketHandler::Make_S_WEAPON_HIT(hitPos.X, hitPos.Y, hitPos.Z);
			Broadcast(sendBuffer);
		}
	}
}


#pragma endregion for_Collision



Tank* Room::FindTankByPlayerId(uint8 playerId)
{
	auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
	if (!tankList) return nullptr;

	for (GameObject* obj : *tankList)
	{
		Tank* tank = dynamic_cast<Tank*>(obj);
		if (tank && tank->playerId == playerId)
			return tank;
	}
	return nullptr;
}

void Room::UpdateCaptureGauge(float deltaTime)
{
	int blueCount = 0;
	int redCount = 0;

	Vec2 center(0.f, 0.f);
	auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
	if (!tankList) return;

	for (GameObject* obj : *tankList)
	{
		Tank* tank = dynamic_cast<Tank*>(obj);
		if (!tank || !tank->isSpawned()) continue;

		Vec3 pos3D = tank->GetPos();
		Vec2 pos2D(pos3D.X, pos3D.Z);

		Vec2 offset = pos2D - center;
		if (offset.LengthSq() <= captureRadius * captureRadius)
		{
			if (tank->isBlueTeam()) blueCount++;
			else redCount++;
		}
	}

	// ���ɷ� ����
	blueGauge += blueCount * gaugePerTankPerSecond * deltaTime;
	redGauge += redCount * gaugePerTankPerSecond * deltaTime;

	//���� ������ �����ߴ��� ����
	int currBlueInt = static_cast<int>(blueGauge);
	int currRedInt = static_cast<int>(redGauge);

	bool shouldBroadcast = false;

	if (currBlueInt > lastSentBlueGauge)
	{
		lastSentBlueGauge = currBlueInt;
		shouldBroadcast = true;
	}

	if (currRedInt > lastSentRedGauge)
	{
		lastSentRedGauge = currRedInt;
		shouldBroadcast = true;
	}

	if (shouldBroadcast)
	{
		auto buffer = ServerPacketHandler::Make_S_CAPTURE(blueGauge, redGauge);
		Broadcast(buffer);
	}

	// ����� ���
	std::cout << "���ɷ� �� BLUE: " << blueGauge << " / RED: " << redGauge << std::endl;

	if (blueGauge >= 100.f) OnTeamWin(true);
	else if (redGauge >= 100.f) OnTeamWin(false);
}

void Room::ResetRoom()
{
	WRITE_LOCK;

	CurState = ROOM_WAITTING;
	blueGauge = 0.f;
	redGauge = 0.f;
	isGameEnded = false;
	Wait_LoadingCnt = 0;

	Room_ObjectManager.Release(); // ��� ������Ʈ ����
	for (auto& tankState : _Player_States)
		tankState.second.IsReady = false;

	/*BroadCast_LobbyInfo();*/
}

void Room::OnTeamWin(bool isBlueWinner)
{
	isGameEnded = true;

	for (const auto& pair : _Players)
	{
		PlayerRef player = pair.second;
		if (!player || !player->OwenerSession) continue;

		const Room_Ready_Data& data = _Player_States[player->playerID];
		if (data.Team == isBlueWinner)
		{
			auto winMsg = ServerPacketHandler::Make_S_GAME_WIN(1);   // �¸� ��Ŷ
			player->OwenerSession->Send(winMsg);
		}
		else
		{
			auto loseMsg = ServerPacketHandler::Make_S_GAME_LOSE(1); // �й� ��Ŷ
			player->OwenerSession->Send(loseMsg);
		}
	}
	system("PAUSE");
	//ShowMessageForFrames(isBlueWinner ? "������� �¸��߽��ϴ�!" : "�������� �¸��߽��ϴ�!", 300);

	// 3�� �� �ʱ�ȭ ����
	std::thread([this]()
		{
			std::this_thread::sleep_for(std::chrono::seconds(3));
			this->ResetRoom();
		}).detach();
}


void Room::Detect_Bomb_Tank_Collisions()
{
	auto bombList = Room_ObjectManager.Get_List(OBJ_BOMB); // Bomb ���� �����̳� ����
	auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
	if (!bombList || !tankList) return;

	for (GameObject* objBomb : *bombList)
	{
		if (!objBomb) continue;

		AirDrop_Bomb* bomb = dynamic_cast<AirDrop_Bomb*>(objBomb);
		if (!bomb || bomb->isHit()) continue;

		Vec3 bombPos = bomb->GetPos();

		// Bomb�� ���� ���� ��� (�Ʊ�/���� ���� Ÿ��)
		const uint8 ownerPlayerID = bomb->GetOwnerID();
		const uint8 ownerTankIdx = bomb->GetOwnerTankIndex();

		// (����) ������ ��ũ ������ �̸� Ȯ��
		Tank* shooterTank = nullptr;
		if (ownerTankIdx < (uint8)tankList->size())
			shooterTank = dynamic_cast<Tank*>((*tankList)[ownerTankIdx]);

		for (size_t i = 0; i < tankList->size(); ++i)
		{
			Tank* targetTank = dynamic_cast<Tank*>((*tankList)[i]);
			if (!targetTank) continue;
			if (!targetTank->isSpawned()) continue;

			// �� ���� ����: ����Ʈ-���Ǿ� ���� �浹
			if (!CollisionManager::GetInstance()->CheckCollision_Point_Sphere(bombPos, targetTank->GetPos(), 5.0f))
				continue;

			// 1) ����Ʈ ��ε�ĳ��Ʈ
			{
				auto effectBuffer = ServerPacketHandler::Make_S_WEAPON_HIT(bombPos.X, bombPos.Y, bombPos.Z);
				Broadcast(effectBuffer);
			}

			// 2) Bomb ����(�� �� ������ ��)
			bomb->SetDead();

			// 3) ������ ���� (���ϴ� ��ġ��)
			targetTank->Damage(25);

			// 4) �ǰ���(ž���� ����)���� TANK_DAMAGED
			for (const Room_Ready_Data& damagedRider : targetTank->GetPassengers())
			{
				auto it = _Players.find(damagedRider.PlayerID);
				if (it != _Players.end() && it->second && it->second->OwenerSession)
				{
					auto buffer = ServerPacketHandler::Make_S_TANK_DAMAGED((uint8)i); // i = Ÿ�� ��ũ �ε���
					it->second->OwenerSession->Send(buffer);
				}
			}

			// 5) ��� ��: DEAD ��ε�ĳ��Ʈ + (�ִٸ�) ���ʿ��� KILL
			if (targetTank->IsDead())
			{
				targetTank->SetUnSpawn();

				auto bufferDead = ServerPacketHandler::Make_S_TANK_DEAD((uint8)i);
				Broadcast(bufferDead);

				if (shooterTank)
				{
					auto bufferKill = ServerPacketHandler::Make_S_TANK_KILL((uint8)i);
					for (const Room_Ready_Data& killer : shooterTank->GetPassengers())
					{
						auto it = _Players.find(killer.PlayerID);
						if (it != _Players.end() && it->second && it->second->OwenerSession)
						{
							it->second->OwenerSession->Send(bufferKill);
						}
					}
				}
			}

			break; // �� ��ź���� �ϳ� �������� ����
		}
	}
}


void Room::Detect_Bomb_Terrain_Collisions()
{
	auto bombList = Room_ObjectManager.Get_List(OBJ_BOMB);
	if (!bombList) return;

	for (GameObject* objBomb : *bombList)
	{
		if (!objBomb) continue;

		AirDrop_Bomb* bomb = dynamic_cast<AirDrop_Bomb*>(objBomb);
		if (!bomb || bomb->isHit()) continue;

		if (CollisionManager::GetInstance()->Check_Terrain_Collision(bomb))
		{
			Vec3 hitPos = bomb->GetPos();
			bomb->SetDead();

			auto sendBuffer = ServerPacketHandler::Make_S_WEAPON_HIT(hitPos.X, hitPos.Y, hitPos.Z);
			Broadcast(sendBuffer);
		}
	}
}