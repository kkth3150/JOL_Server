#include "pch.h"
#include "Terrain_Manager.h"
#include "fstream"

#define NUMVERTERTICES 4096

bool Terrain_Manager::Read_Map(const std::string& filePath, int width, int height, float cellSpacing)
{
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open())
        return false;

    m_width = width;
    m_height = height;
    m_cellSpacing = cellSpacing;

    m_heightMap.resize(width * height);

    float fx, y, fz;
    for (int z = 0; z < height; ++z)
    {
        for (int x = 0; x < width; ++x)
        {
            inFile.read(reinterpret_cast<char*>(&fx), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&y), sizeof(float));
            inFile.read(reinterpret_cast<char*>(&fz), sizeof(float));

            int idx = Get_Index(x, z);
            m_heightMap[idx] = y;

        } 
    }

    inFile.close();
    return true;
}


//float Terrain_Manager::Get_Height(float worldX, float worldZ) const
//{
//    int x = static_cast<int>((worldX / m_cellSpacing) + (m_width / 2));
//    int z = static_cast<int>((worldZ / m_cellSpacing) + (m_height / 2));
//
//    if (x < 0 || x >= m_width || z < 0 || z >= m_height)
//        return 0.0f;
//
//    return m_heightMap[Get_Index(x, z)];
//}

void Terrain_Manager::Show_MapData()
{

    for (int z = 0; z < 4096; ++z)
    {
        for (int x = 0; x < 4096; ++x)
        {
            int idx = Get_Index(x, z);
            float height = m_heightMap[idx];
            std::cout << "X : " << x << "      Z: " << z << "      Y: " << height << endl;
        }
    }
}



// 세 점으로 평면의 방정식을 구하는 함수 (XMPlaneFromPoints 대체)
// 평면 방정식: Ax + By + Cz + D = 0
// 반환값: Vector4 { A, B, C, D }
Vector4 Terrain_Manager::PlaneFromPoints(const Vector4& p1, const Vector4& p2, const Vector4& p3)
{
    // 평면 위의 두 벡터를 생성합니다 (p1->p2, p1->p3)
    Vector4 v1 = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z, 0.f };
    Vector4 v2 = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z, 0.f };

    // 외적(Cross Product)을 통해 평면의 법선 벡터 N(A, B, C)를 구합니다.
    Vector4 normal;
    normal.x = (v1.y * v2.z) - (v1.z * v2.y); // A
    normal.y = (v1.z * v2.x) - (v1.x * v2.z); // B
    normal.z = (v1.x * v2.y) - (v1.y * v2.x); // C

    // 법선 벡터를 정규화(Normalize)합니다. (길이를 1로 만듦)
    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (length > 1e-6f) { // 0으로 나누는 것을 방지
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }

    // D 값을 계산합니다: D = -(N · p1) = -(A*x1 + B*y1 + C*z1)
    normal.w = -(normal.x * p1.x + normal.y * p1.y + normal.z * p1.z); // D

    return normal;
}

// 요청하신 함수를 수정한 버전입니다.
// (클래스 멤버 함수이므로, 실제 사용 시에는 클래스 정의 안에 포함해야 합니다)
float Terrain_Manager::Get_Height(float x, float z)
{
    // NUMVERTERTICES가 클래스 멤버 변수라고 가정합니다.
    float half = NUMVERTERTICES / 2.f;

    int LX = static_cast<int>(x + half);
    int DZ = static_cast<int>(z + half);

    if (x < -half || z < -half || x > half || z > half)
        return 0.f;

    // _vector 배열을 Vector4 배열로 변경
    Vector4 Positions[4];

    // XMVectorSet을 구조체 초기화로 변경
    Positions[0] = { LX - half,       m_heightMap[(DZ + 1) * NUMVERTERTICES + LX],     DZ + 1 - half, 1.f };
    Positions[1] = { LX + 1 - half,   m_heightMap[(DZ + 1) * NUMVERTERTICES + LX + 1], DZ + 1 - half, 1.f };
    Positions[2] = { LX + 1 - half,   m_heightMap[DZ * NUMVERTERTICES + LX + 1],       DZ - half,     1.f };
    Positions[3] = { LX - half,       m_heightMap[DZ * NUMVERTERTICES + LX],           DZ - half,     1.f };

    float DeltaX = x - (LX - half);
    float DeltaZ = z - (DZ - half);

    // _vector를 Vector4로 변경
    Vector4 PlaneNormal;

    // 직접 구현한 PlaneFromPoints 함수를 호출
    if (DeltaX + DeltaZ <= 1.0f)
        PlaneNormal = PlaneFromPoints(Positions[0], Positions[2], Positions[3]);
    else
        PlaneNormal = PlaneFromPoints(Positions[0], Positions[1], Positions[2]);

    // XMVectorGetX/Y/Z/W를 구조체 멤버 접근으로 변경
    // 평면 방정식(Ax + By + Cz + D = 0)을 y에 대해 정리: y = -(Ax + Cz + D) / B
    float B = PlaneNormal.y;

    return -(PlaneNormal.x * x + PlaneNormal.z * z + PlaneNormal.w) / B;
}