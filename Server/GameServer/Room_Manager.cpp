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
            room->Update(DeltaTime); // 예시로 deltaTime 60fps 기준
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
        cout << "Room " << roomID << " 활성화" << endl;
    }
}

void Room_Manager::DeActiveRoom(uint32 roomID)
{

    if (roomID >= vRooms.size())
        return;

    if (vRooms[roomID])
    {
        vRooms[roomID]->RoomDeActivate();
        cout << "Room " << roomID << " 비활성화" << endl;
    }
}

void Room_Manager::ShowRoomDataList()
{
    READ_LOCK;
    for (auto& room : vRooms) {
        cout << "ROOM(" << room->GetRoomID() << ")" << "\t";
        cout << "( " << room->GetRoomPlayerCnt() << " / " << room->GetRoomMaxPlayerCnt() << " )" << "\t";
        if (room->GetRoomActivate())
            cout << "활성화";
        else
            cout << "비활성화";
        cout << endl;
    }

}

void Room_Manager::ShowRoomData(uint32 RoomID)
{
    READ_LOCK;

    if (RoomID >= vRooms.size())
    {
        std::cout << "존재하지 않는 RoomID 입니다." << std::endl;
        return;
    }

    Room* room = vRooms[RoomID];
    if (room == nullptr || !room->GetRoomActivate())
    {
        std::cout << "Room " << RoomID << " 은 비활성화 상태입니다." << std::endl;
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
            std::cout << "Room 생성 + 입장 완료" << std::endl;
            return room->GetRoomID();

        }
    }
    //std::cout << "Room 생성 실패 (모두 사용 중)" << std::endl;
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
        std::cout << "Room " << RoomID << " 인원이 가득 찼습니다. 입장 불가." << std::endl;
        return ROOM_ENTER_ERROR;
    }

    room->Accept_Player(player);
    std::cout << "Room " << RoomID << " 입장 완료" << std::endl;
    return RoomID;
}

bool Room_Manager::Client_LeaveRoom(uint32 ID, PlayerRef player)
{
    if (player == nullptr)
        return false;

    uint32 RoomID = ID;  // 또는 player->roomID 등

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

void Room_Manager::Client_LOADING_FINISH(uint32 ROOMID)
{

    if (ROOMID >= vRooms.size())
        return;

    if (vRooms[ROOMID]->isActive)
    {
        vRooms[ROOMID]->Clinet_Loading_Finish();
    }


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

void Room_Manager::SetTankByRoomIndex(int RoomID, int64 pID, const Matrix4x4& mat, const float& PosinAngle, const float& PotapAngl)
{
    if (RoomID >= vRooms.size())
        return;

    Room* room = vRooms[RoomID];
    if (!room || !room->GetRoomActivate())
        return;

    room->SetTankState(pID,mat,PosinAngle,PotapAngl);
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
        return true;
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