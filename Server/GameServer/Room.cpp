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
	//SetMaxPlayer(8);
	//WRITE_LOCK;
	//Room_ObjectManager.Add_Object(OBJ_TANK, CAbstractFactory<Tank>::Create(0, 0, 0));
	//Room_ObjectManager.Add_Object(OBJ_TANK, CAbstractFactory<Tank>::Create(10, 10, 10));

}

void Room::Update(float deltaTime)
{
	system("cls");

	switch (CurState) {

	case ROOM_UNACTIVATE:
		break;
	case ROOM_WAITTING:
	{

	}
		break;
	case ROOM_INGAME:
	{

		ShowTankState(0);
		ShowTankState(1);
		ShowBulletCnt();
		Broadcast_All_TankState(2);
		Room_ObjectManager.Update(deltaTime);
		Detect_Bullet_Tank_Collisions();

	}
		break;
	default:
		break;
	}
	//if (!isStart) {
	//	if (Check_ClientLoading()) {

	//		isStart = true;
	//	}
	//}

}

void Room::LateUpdate()
{

	if (isStart) {
		Room_ObjectManager.Late_Update();
		Check_Bullet_Collision();
	}
}

void Room::Release()
{

}

void Room::Accept_Player(PlayerRef Player)
{
	
	WRITE_LOCK;
	_Players[Player->playerID] = Player;

	// 현재 팀별 인원 수 계산
	int blueCount = 0;
	int redCount = 0;

	for (const auto& pair : _Player_States)
	{
		if (pair.second.Team) // true == Blue
			blueCount++;
		else
			redCount++;
	}

	// 배정할 팀 결정
	bool assignedTeam = false; // false == Red, true == Blue
	if (_Player_States.empty())
	{
		// 첫 입장자는 무조건 Blue
		assignedTeam = true;
	}
	else
	{
		assignedTeam = (blueCount <= redCount); // 인원이 적은 쪽
	}

	// 포지션 후보군 생성
	std::vector<uint8> usedPositions;
	for (const auto& pair : _Player_States)
		usedPositions.push_back(pair.second.Position);

	uint8 assignedPosition = 0;

	if (assignedTeam) // BLUE 팀: 1 ~ 8
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
	else // RED 팀: 9 ~ 16
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

	// 최종 데이터 구성 및 저장
	Room_Ready_Data playerData;
	playerData.PlayerID = static_cast<uint8>(Player->playerID);  // uint64 → uint8 캐스팅 주의
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

	// 1. 플레이어 목록에서 제거
	auto playerIt = _Players.find(playerID);
	if (playerIt != _Players.end())
		_Players.erase(playerIt);

	// 2. 상태 데이터에서 제거
	auto stateIt = _Player_States.find(playerID);
	if (stateIt != _Player_States.end())
		_Player_States.erase(stateIt);

	// 3. 현재 인원 수 감소
	if (RoomCurPlayerCnt > 0)
		--RoomCurPlayerCnt;

	// 4. 플레이어가 모두 나간 경우 → 룸 비활성화

}

bool Room::Change_Player_Info(uint64 playerID, const Room_Ready_Data& newData)
{
	WRITE_LOCK;

	// 플레이어가 존재하는지 먼저 확인
	auto it = _Player_States.find(playerID);
	if (it == _Player_States.end())
		return false;

	// 요청된 포지션이 이미 다른 사람이 쓰고 있는지 확인
	for (const auto& pair : _Player_States)
	{
		if (pair.first == playerID) continue; // 본인 제외

		if (pair.second.Position == newData.Position)
			return false; // 포지션 충돌
	}

	// 유효하다면 데이터 갱신
	Room_Ready_Data updated = newData;
	updated.PlayerID = static_cast<uint8>(playerID); // ID 보존

	_Player_States[playerID] = updated;

	// 변경 사항 전체 전파
	//BroadCast_LobbyInfo();

	return true;
}

void Room::Change_Tank_INFO(int64 pID, const Matrix4x4& mat, const float& PotapAngle ,const float& PosinAngle)
{
	WRITE_LOCK;
	//터질 가능성 존재. 인덱스 오버런

	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))
		[pID])->SetTankState(mat, PotapAngle, PosinAngle);

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

bool Room::Check_ClientLoading()
{
	if (Wait_LoadingCnt >= RoomMaxPlayerCnt) {
		return true;
	}
	return false;
}

void Room::Clinet_Loading_Finish()
{
	Wait_LoadingCnt++;
}

bool Room::CollisionTest()
{
	WRITE_LOCK;
	

	return false;
}

void Room::Broadcast_PlayerData(Vec3 p1, Vec3 p2)
{

}

void Room::Broadcast_All_TankState(uint8 PlayersCnt)
{
	std::vector<GameObject*> tankListCopy;
	{
		READ_LOCK;
		auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
		if (tankList)
			tankListCopy = *tankList;
	}

	WRITE_LOCK;
	for (const auto& iter : _Players)
	{
		uint64 playerID = iter.first;
		PlayerRef player = iter.second;

		if (player == nullptr || player->OwenerSession == nullptr)
			continue;

		uint64 otherID = (playerID == 0) ? 1 : 0;

		if (otherID >= tankListCopy.size())
			continue;

		Tank* otherTank = dynamic_cast<Tank*>(tankListCopy[otherID]);
		if (otherTank == nullptr)
			continue;

		Tank_INFO info = otherTank->GetTankState();

		auto sendBuffer = ServerPacketHandler::Make_S_PLAYER_MOVED(
			(uint8)otherID,
			info.TankTransform,
			info.PotapAngle,
			info.PosinAngle
		);

		player->OwenerSession->Send(sendBuffer);
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

void Room::Broadcast_Tank_Data()
{

	std::vector<Tank_INFO> vTankInfo;

	{
		READ_LOCK;

		auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);
		if (tankList)
		{
			for (size_t i = 0; i < tankList->size(); ++i)
			{
				GameObject* obj = (*tankList)[i];
				if (!obj)
					continue;

				Tank* tank = dynamic_cast<Tank*>(obj);
				if (!tank)
					continue;

				Tank_INFO info = tank->GetTankState();
				info.id = static_cast<uint8>(i); // or PlayerID if mapped
				vTankInfo.push_back(info);
			}
		}
	}

	SendBufferRef sendBuffer = ServerPacketHandler::Make_S_ALL_TANK_STATE(vTankInfo);
	Broadcast(sendBuffer);
}

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
	std::cout << "Tank  "<<  Id  <<"  상태" << std::endl;
	std::cout << "X: " << Tank0state.TankTransform.m[3][0] << std::endl;  // _41
	std::cout << "Y: " << Tank0state.TankTransform.m[3][1] << std::endl;  // _42
	std::cout << "Z: " << Tank0state.TankTransform.m[3][2] << std::endl;  // _43
	std::cout << "포탑 각도: " << Tank0state.PotapAngle << std::endl;
	std::cout << "포신 각도: " << Tank0state.PosinAngle << std::endl;
	std::cout << "HP: " << static_cast<int>(Tank0state.TankHP) << std::endl;
}

void Room::ShowBulletCnt()
{

	int BulletCnt = 0;
	if (Room_ObjectManager.Get_List(OBJ_WEAPON)) {
		READ_LOCK;
		BulletCnt = Room_ObjectManager.Get_List(OBJ_WEAPON)->size();
	}
	cout << "생성된 총알 : " << BulletCnt << " 개" << endl;
}

bool Room::Wait_Full(uint16 MaxPlayer)
{
	READ_LOCK;
	int playerCnt = (int)_Players.size();

	std::cout << "플레이어 접속 대기 중" << std::endl;
	std::cout << "접속 플레이어 (" << playerCnt << ") 명" << std::endl;
	std::cout << "최대 플레이어 (" << MaxPlayer << ") 명" << std::endl;

	
	if (playerCnt >= MaxPlayer)
		return true;

	return false;
		
}



void Room::SetTankState(int64 pID, const Matrix4x4& mat, const float& PotapAngle, const float& PosinAngle)
{
	WRITE_LOCK;
	dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[pID])->SetTankState(mat, PotapAngle, PosinAngle);

}

Tank_INFO Room::GetTankState(int64 pID)
{
	READ_LOCK;
	return dynamic_cast<Tank*>((*Room_ObjectManager.Get_List(OBJ_TANK))[pID])->GetTankState();
	
}

void Room::CreateBullet(int8 pID, WEAPON_ID ID, Vec3 Dir,Vec3 Pos)
{
	WRITE_LOCK;
	switch (ID) {

	case WEAPON_NPOTAN:
	{
		GameObject* TempBullet = CAbstractFactory<Normal_Potan>::Create();
		dynamic_cast<Normal_Potan*>(TempBullet)->SetInitData(Dir, Pos, pID);
		Room_ObjectManager.Add_Object(OBJ_WEAPON, TempBullet);
	}
		break;

	case WEAPON_NBULLET:
		break;

	default:
		break;


	}
}

void Room::Detect_Bullet_Tank_Collisions()
{
	auto bulletList = Room_ObjectManager.Get_List(OBJ_WEAPON);
	auto tankList = Room_ObjectManager.Get_List(OBJ_TANK);

	if (bulletList == nullptr || tankList == nullptr)
		return;

	WRITE_LOCK;

	for (GameObject* objBullet : *bulletList)
	{
		if (!objBullet) continue;

		Normal_Potan* bullet = dynamic_cast<Normal_Potan*>(objBullet);
		if (!bullet) continue;

		if (bullet->isHit())  
			continue;

		Vec3 bulletPos = bullet->GetPos();
		int bulletOwnerID = bullet->GetOwnerID();

		for (size_t tankIndex = 0; tankIndex < tankList->size(); ++tankIndex)
		{
			if (!(*tankList)[tankIndex]) continue;

			if ((int)tankIndex == bulletOwnerID)
				continue;

			Tank* tank = dynamic_cast<Tank*>((*tankList)[tankIndex]);
			if (!tank) continue;

			const OBB& tankOBB = tank->Get_OBB();

			if (Check_OBB_Collision(bulletPos, tankOBB))
			{
				int a = tankIndex;

				bullet->SetDead();
				break; // 한 총알은 한 탱크만 충돌처리
			}
		}
	}
}

bool Room::Check_OBB_Collision(const Vec3& point, const OBB& obb)
{
	Vec3 dir = { point.X - obb.center.X, point.Y - obb.center.Y, point.Z - obb.center.Z };

	for (int i = 0; i < 3; ++i)
	{
		float dist = dir.X * obb.axis[i].X + dir.Y * obb.axis[i].Y + dir.Z * obb.axis[i].Z;

		float halfSize = (i == 0) ? obb.halfSize.X : (i == 1) ? obb.halfSize.Y : obb.halfSize.Z;

		if (fabs(dist) > halfSize)
			return false;
	}

	return true;
}

void Room::Set_Player_Lobby_State(Room_Ready_Data data, uint64 PlayerID)
{
	WRITE_LOCK;
	_Player_States[PlayerID] = data;

}


void Room::Check_Bullet_Collision()
{
	auto bulletList = Room_ObjectManager.Get_List(OBJ_WEAPON);
	if (bulletList == nullptr)
		return;

	READ_LOCK;

	for (GameObject* obj : *bulletList)
	{
		if (!obj) continue;

		Normal_Potan* bullet = dynamic_cast<Normal_Potan*>(obj);
		if (bullet && bullet->isHit())
		{
			Vec3 hitPos = bullet->GetPos();
			Broadcast_Hit_Weapon(hitPos);
		}
	}

}
