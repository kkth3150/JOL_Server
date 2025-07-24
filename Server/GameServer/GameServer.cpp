#include "pch.h"
#include "SGlobal.h"
#include "ThreadManager.h"
#include "Terrain_Manager.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "Room_Manager.h"
#include "BufferWriter.h"
#include "ServerPacketHandler.h"
#include <chrono>
#include <thread>

using namespace std::chrono;

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"0.0.0.0", 7777),
		MakeShared<IocpCore>(),
		MakeShared<ClientSession>,
		100);

	ASSERT_CRASH(service->Start());
	int32 threadCount = std::thread::hardware_concurrency();


	
	for (int32 i = 0; i < threadCount; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}				
			});
	}	

	constexpr int64_t FRAME_DURATION_MS = 33;


	//std::cout << "맵 데이터 생성 중" << endl;
	//bool success = Terrain_Manager::GetInstance().Read_Map("Terrain4096Map.bin", 4096, 4096, 1.0f);

	//if (!success)
	//{
	//	std::cout << "맵 읽기 오류!" << std::endl;
	//}

	//Terrain_Manager::GetInstance().Show_MapData();

	//Room_Manager::Get_Instance()->Initialize();

	using clock = std::chrono::high_resolution_clock;
	auto previousTime = clock::now();

	while (true)
	{
		auto currentTime = clock::now();
		auto deltaTime = std::chrono::duration<float>(currentTime - previousTime).count(); // seconds
		previousTime = currentTime;

		Room_Manager::Get_Instance()->Update(deltaTime);
		Room_Manager::Get_Instance()->Late_Update();

		constexpr float targetFrameTime = 1.0f / 60.0f; // 60 FPS
		float sleepTime = targetFrameTime - deltaTime;

		if (sleepTime > 0)
			std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
	}

	GThreadManager->Join();
}