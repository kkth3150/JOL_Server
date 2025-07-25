#include "pch.h"
#include "Room_Manager.h"


Room_Manager* Room_Manager::m_pInstance = nullptr;

Room_Manager::Room_Manager()
{
    vRooms.resize(10);

    for (int i = 0; i < 10; ++i)
    {
        vRooms[i] = new Room();
        vRooms[i]->SetRoomID(i);

    }
}

Room_Manager::~Room_Manager()
{
    Release();
}

void Room_Manager::Initialize()
{
    for (auto& room : vRooms)
    {
        if (room->isActive)
            room->Initialize();
    }

}

int Room_Manager::Update(float DeltaTime)
{
    system("cls");
    for (auto& room : vRooms)
    {
        if (room->isActive)
            room->Update(DeltaTime); // ���÷� deltaTime 60fps ����
    }
    ShowRoomDataList();
    //ShowRoomData(0);

    return 0;
}

void Room_Manager::Late_Update(void)
{

    for (auto& room : vRooms)
    {
        if (room->isActive)
            room->LateUpdate();
    }
}

void Room_Manager::Release(void)
{

    for (auto& room : vRooms)
    {
        if (room)
        {
            room->Release();
            delete room;
            room = nullptr;
        }
    }
    vRooms.clear();
}

void Room_Manager::ActiveRoom(uint32 roomID)
{
    WRITE_LOCK;
    if (roomID >= vRooms.size())
        return;

    if (!vRooms[roomID]->GetRoomActivate())
    {
        vRooms[roomID]->RoomActivate();
        //vRooms[roomID]->Initialize();
        cout << "Room " << roomID << " Ȱ��ȭ" << endl;
    }
}

void Room_Manager::DeActiveRoom(uint32 roomID)
{

    if (roomID >= vRooms.size())
        return;

    if (vRooms[roomID])
    {
        vRooms[roomID]->RoomDeActivate();
        cout << "Room " << roomID << " ��Ȱ��ȭ" << endl;
    }
}

void Room_Manager::ShowRoomDataList()
{
    READ_LOCK;
    for (auto& room : vRooms) {
        cout << "ROOM(" << room->GetRoomID() << ")" << "\t";
        cout << "( " << room->GetRoomPlayerCnt() << " / " << room->GetRoomMaxPlayerCnt() << " )" << "\t";
        if (room->GetRoomActivate())
            cout << "Ȱ��ȭ";
        else
            cout << "��Ȱ��ȭ";
        cout << endl;
    }

}

void Room_Manager::ShowRoomData(uint32 RoomID)
{
    READ_LOCK;

    if (RoomID >= vRooms.size())
    {
        std::cout << "�������� �ʴ� RoomID �Դϴ�." << std::endl;
        return;
    }

    Room* room = vRooms[RoomID];
    if (room == nullptr || !room->GetRoomActivate())
    {
        std::cout << "Room " << RoomID << " �� ��Ȱ��ȭ �����Դϴ�." << std::endl;
        return;
    }

    //std::cout << "----- Room [" << RoomID << "] -----" << std::endl;
    room->ShowRoomData();
}

void Room_Manager::DeleteRoom(uint32 roomID)
{

    if (roomID >= vRooms.size())
        return;

    if (vRooms[roomID])
    {
        vRooms[roomID]->Release();
        delete vRooms[roomID];
        vRooms[roomID] = nullptr;
    }
}

int Room_Manager::Client_CreateRoom(PlayerRef player)
{

    for (auto& room : vRooms)
    {
        if (room && !room->GetRoomActivate())
        {
            room->RoomActivate();
            room->Accept_Player(player);
            std::cout << "Room ���� + ���� �Ϸ�" << std::endl;
            return room->GetRoomID();

        }
    }
    //std::cout << "Room ���� ���� (��� ��� ��)" << std::endl;
    return 999;

}

int Room_Manager::Client_EnterRoom(uint32 RoomID, PlayerRef player)
{
    if (RoomID >= vRooms.size())
        return ROOM_ENTER_ERROR;

    Room* room = vRooms[RoomID];
    if (!room || !room->GetRoomActivate())
        return ROOM_ENTER_ERROR;

    if (room->isFull())
    {
        std::cout << "Room " << RoomID << " �ο��� ���� á���ϴ�. ���� �Ұ�." << std::endl;
        return ROOM_ENTER_ERROR;
    }

    room->Accept_Player(player);
    std::cout << "Room " << RoomID << " ���� �Ϸ�" << std::endl;
    return RoomID;
}

bool Room_Manager::Client_LeaveRoom(uint32 ID, PlayerRef player)
{
    if (player == nullptr)
        return false;

    uint32 RoomID = ID;  // �Ǵ� player->roomID ��

    if (RoomID >= vRooms.size())
        return false;

    Room* room = vRooms[RoomID];
    if (!room || !room->GetRoomActivate())
        return false;

    room->Leave_Player(player);

    if (room->GetRoomPlayerCnt() == 0)
    {
        DeActiveRoom(RoomID);
    }

    return true;
}

bool Room_Manager::Client_ChangeINFO(uint32 ROOMID, uint64 PlayerID, Room_Ready_Data data)
{
    /*   if (vRooms[ROOMID]->isActive) {
           vRooms[ROOMID]->Change_Player_Info(PlayerID, data);
       }*/


    if (ROOMID >= vRooms.size())
        return false;

    if (vRooms[ROOMID]->isActive)
    {
        if (vRooms[ROOMID]->Change_Player_Info(PlayerID, data))
            return true;
    }

    return false;
}

bool Room_Manager::Ready_Player(uint32 RoomID, uint64 PlayerID)
{
    if (RoomID >= vRooms.size())
        return false;

    if (vRooms[RoomID]->isActive)
    {
        if (vRooms[RoomID]->Ready_Player(PlayerID))
            return true;
    }

    return false;

}

void Room_Manager::BroadCast_LobbyState(uint32 roomID)
{
    READ_LOCK;

    if (roomID >= vRooms.size() || vRooms[roomID] == nullptr || !vRooms[roomID]->GetRoomActivate())
        return;

    vRooms[roomID]->BroadCast_LobbyInfo();
}

void Room_Manager::BroadCast_Game_Start(uint32 roomID)
{
    READ_LOCK;

    if (roomID >= vRooms.size() || vRooms[roomID] == nullptr || !vRooms[roomID]->GetRoomActivate())
        return;

    vRooms[roomID]->Broadcast_GameStart();
}



std::vector<Room_Data> Room_Manager::Client_ShowRoom()
{

    std::vector<Room_Data> vRoom_Data;

    for (auto& room : vRooms) {

        //if (room->GetRoomActivate()) {
        Room_Data temp;
        temp.MaxPlayer = room->GetRoomMaxPlayerCnt();
        temp.CurPlayer = room->GetRoomPlayerCnt();
        temp.RoomID = room->GetRoomID();
        temp.isActive = room->GetRoomActivate();
        vRoom_Data.push_back(temp);
        // }

    }


    return vRoom_Data;
}

bool Room_Manager::Check_StartGame(uint32 RoomID)
{
    if (RoomID >= vRooms.size())
        return false;

    Room* room = vRooms[RoomID];
    if (!room || !room->GetRoomActivate())
        return false;

    if (room->CanStartGame())
    {
        return room->StartGame();
    }

    return false;
}


void Room_Manager::Process_Objectdata(RECV_Data input, int RoomID, int PlayerID)
{

    if (RoomID >= vRooms.size())
        return;

    if (!vRooms[RoomID]->GetRoomActivate())
    {
        switch (input) {
        case DATA_TANK_MOVE:

            break;

        case DATA_TANK_SHOT:

            break;

        case DATA_TREE_DELETE:

            break;

        default:
            break;
        }

    }

}