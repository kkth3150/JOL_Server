#pragma once
#include "Room.h"

class Room_Manager
{
private:
	Room_Manager();
	~Room_Manager();

public:

	void		Initialize();
	int			Update(float DeltaTime);
	void		Late_Update(void);
	void		Release(void);

public:

	void ActiveRoom(uint32 roomID);
	void DeActiveRoom(uint32 roomID);

	void ShowRoomDataList();
	void ShowRoomData(uint32 RoomID);

	Room* Get_Room(uint32 roomID) {
		if (!vRooms.empty())
			return vRooms[roomID];
	}

	void		DeleteRoom(uint32 roomID);



public:

	int Client_CreateRoom(PlayerRef player);
	int Client_EnterRoom(uint32 RoomID, PlayerRef player);
	bool Client_LeaveRoom(uint32 RoomID, PlayerRef player);


	bool Client_ChangeINFO(uint32 ROOMID, uint64 PlayerID,Room_Ready_Data data);
	void BroadCast_LobbyState(uint32 roomID);



	std::vector<Room_Data> Client_ShowRoom();


public:

	int GetCurPlayer(int RoomID) {
		return vRooms[RoomID]->GetRoomPlayerCnt();

	}

private:

	std::vector<Room*>	vRooms;

public:
	static	Room_Manager* Get_Instance()
	{
		if (!m_pInstance)
		{
			m_pInstance = new Room_Manager;
		}
		return m_pInstance;
	}

	static void			Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
private:
	static	Room_Manager* m_pInstance;
	USE_LOCK;

};

