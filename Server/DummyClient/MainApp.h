#pragma once
class Main_App
{
public:
	Main_App();
	~Main_App();

public:
	void Initialize();
	void Update();
	void Late_Update();
	void Release();
public:
	std::atomic<unsigned int> MyClientID = 9999;
	unsigned int MyRoomID = 9999;


};

extern Main_App* GMainApp;
