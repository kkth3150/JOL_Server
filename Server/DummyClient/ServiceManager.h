#pragma once
#include "Service.h"

class ServiceManager
{
public:
	static ServiceManager& GetInstace()
	{
		static ServiceManager instance;
		return instance;
	}

	void SetService(ClientServiceRef service) {
		_service = service;
	}

	ClientServiceRef GetService() {

		return _service;
	}

	void SetMyID(uint8 ID) { _myGlobalID = ID; }
	const uint16 GetMyID() { return _myGlobalID; }

private:

	ServiceManager() = default;
	~ServiceManager() = default;

	ServiceManager(const ServiceManager&) = delete;
	ServiceManager& operator=(const ServiceManager) = delete;




private:

	ClientServiceRef	_service;
	uint16				_myGlobalID;



#pragma	region
	uint8				_myRoomID;
	uint8				_myID_in_Room;
#pragma endregion TODO : ·ë ´Ã¸®¸é ·ë°ü·Ã º¯¼ö ¼¼ÆÃ  

};

