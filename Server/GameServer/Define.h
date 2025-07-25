#pragma once


#define PURE	= 0

#define ROBBY 9999
#define ROOM_ENTER_ERROR 9998
#define TEAM_BLUE	false
#define TEAM_RED	true
/*----------------
	For Object
----------------*/

enum ObjectID {

	OBJ_TANK, OBJ_WEAPON ,OBJ_END

};


enum RECV_Data {

	 DATA_TANK_MOVE, DATA_TANK_SHOT, DATA_TREE_DELETE 

};

struct Vec3
{
    float X;
    float Y;
    float Z;

    // 생성자
    Vec3() : X(0), Y(0), Z(0) {}
    Vec3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    // 연산자 오버로딩
    Vec3 operator+(const Vec3& other) const {
        return Vec3(X + other.X, Y + other.Y, Z + other.Z);
    }

    Vec3 operator-(const Vec3& other) const {
        return Vec3(X - other.X, Y - other.Y, Z - other.Z);
    }

    Vec3 operator*(float scalar) const {
        return Vec3(X * scalar, Y * scalar, Z * scalar);
    }

    Vec3 operator/(float scalar) const {
        return Vec3(X / scalar, Y / scalar, Z / scalar);
    }

    Vec3& operator+=(const Vec3& other) {
        X += other.X; Y += other.Y; Z += other.Z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other) {
        X -= other.X; Y -= other.Y; Z -= other.Z;
        return *this;
    }

    // 벡터 크기
    float Length() const {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    float LengthSq() const {
        return X * X + Y * Y + Z * Z;
    }

    // 정규화
    void Normalize() {
        float len = Length();
        if (len > 0.0f) {
            X /= len;
            Y /= len;
            Z /= len;
        }
    }

    Vec3 Normalized() const {
        float len = Length();
        if (len > 0.0f)
            return Vec3(X / len, Y / len, Z / len);
        return *this;
    }

    // 내적
    float Dot(const Vec3& rhs) const {
        return X * rhs.X + Y * rhs.Y + Z * rhs.Z;
    }

    static float Dot(const Vec3& a, const Vec3& b) {
        return a.X * b.X + a.Y * b.Y + a.Z * b.Z;
    }

    // 외적
    Vec3 Cross(const Vec3& rhs) const {
        return Vec3(
            Y * rhs.Z - Z * rhs.Y,
            Z * rhs.X - X * rhs.Z,
            X * rhs.Y - Y * rhs.X
        );
    }

    static Vec3 Cross(const Vec3& a, const Vec3& b) {
        return a.Cross(b);
    }
};

struct Vec2
{
	float X = 0;
	float Y = 0;

	Vec2() = default;
	Vec2(float x, float y) : X(x), Y(y) {}

	Vec2 operator+(const Vec2& rhs) const { return Vec2(X + rhs.X, Y + rhs.Y); }
	Vec2 operator-(const Vec2& rhs) const { return Vec2(X - rhs.X, Y - rhs.Y); }
	Vec2 operator*(float scalar) const { return Vec2(X * scalar, Y * scalar); }

	float Dot(const Vec2& rhs) const { return X * rhs.X + Y * rhs.Y; }

	float Length() const { return std::sqrt(X * X + Y * Y); }
	float LengthSq() const { return X * X + Y * Y; }

	void Normalize()
	{
		float len = Length();
		if (len != 0)
		{
			X /= len;
			Y /= len;
		}
	}

};

struct Size {
	float length_X;
	float length_Y;
	float length_Z;
};


struct Matrix4x4
{
	float m[4][4];
};


struct OBB
{
	Vec3 center;     
	Vec3 axis[3];    
	Vec3 halfSize;   
};


/*-----------------
	For Tank
-----------------*/


struct Tank_INFO {

	uint8		id;
	Matrix4x4	TankTransform;
	float		PosinAngle;
	float		PotapAngle;
	uint8		TankHP;

};

struct OBB2D
{
	Vec2 center;
	Vec2 axis[2];      // x축, z축 방향의 단위 벡터
	Vec2 halfSize;     // 반 너비, 반 높이
};


/*-----------------
	For Weapon
-----------------*/

enum WEAPON_ID {

	WEAPON_NPOTAN, WEAPON_NBULLET, WEAPON_END

};


/*------------------
	For Room
------------------*/

struct Room_Data {

	
	unsigned char	MaxPlayer;
	unsigned char	CurPlayer;
	bool			isActive;
	unsigned char	RoomID;

};

/*------------------
	팀 false = Red
	팀 True  = Blue

	포지션 False = 조종수
	포지션 True = 포수

	

-------------------*/

struct Room_Ready_Data {
	
	uint8	PlayerID;
	uint8   Position;
	bool	Team;
	bool	IsReady = false;
	
};





/*-----------------
	For Delete
-----------------*/




template<typename T>
void Safe_Delete(T& Temp)
{
	if (Temp)
	{
		delete Temp;
		Temp = nullptr;
	}
}






/*------------------
	For Protocol
------------------*/


