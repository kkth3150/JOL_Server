#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"
#include "iostream"

#include <cmath>
#include <algorithm>
#include <memory>

using ClientSessionRef = shared_ptr<class ClientSession>;
using PlayerRef = shared_ptr<class Player>;