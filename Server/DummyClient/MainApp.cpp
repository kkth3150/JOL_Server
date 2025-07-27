#include "pch.h"
#include "MainApp.h"

Main_App* GMainApp = nullptr;

Main_App::Main_App()
{
}

Main_App::~Main_App()
{
}

void Main_App::Initialize()
{
}

void Main_App::Update()
{
	system("cls");
	cout << "MY CLIENT ID  (" << MyClientID.load() << ")" << "\t";
}

void Main_App::Late_Update()
{
} 

void Main_App::Release()
{
}
