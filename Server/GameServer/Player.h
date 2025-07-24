#pragma once
class Player
{
public:

	uint64	playerID = 0;
	uint64	RoomNum = 9999;
	string	name;
	ClientSessionRef OwenerSession;

};

