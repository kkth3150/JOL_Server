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



// �� ������ ����� �������� ���ϴ� �Լ� (XMPlaneFromPoints ��ü)
// ��� ������: Ax + By + Cz + D = 0
// ��ȯ��: Vector4 { A, B, C, D }
Vector4 Terrain_Manager::PlaneFromPoints(const Vector4& p1, const Vector4& p2, const Vector4& p3)
{
    // ��� ���� �� ���͸� �����մϴ� (p1->p2, p1->p3)
    Vector4 v1 = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z, 0.f };
    Vector4 v2 = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z, 0.f };

    // ����(Cross Product)�� ���� ����� ���� ���� N(A, B, C)�� ���մϴ�.
    Vector4 normal;
    normal.x = (v1.y * v2.z) - (v1.z * v2.y); // A
    normal.y = (v1.z * v2.x) - (v1.x * v2.z); // B
    normal.z = (v1.x * v2.y) - (v1.y * v2.x); // C

    // ���� ���͸� ����ȭ(Normalize)�մϴ�. (���̸� 1�� ����)
    float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
    if (length > 1e-6f) { // 0���� ������ ���� ����
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
    }

    // D ���� ����մϴ�: D = -(N �� p1) = -(A*x1 + B*y1 + C*z1)
    normal.w = -(normal.x * p1.x + normal.y * p1.y + normal.z * p1.z); // D

    return normal;
}

// ��û�Ͻ� �Լ��� ������ �����Դϴ�.
// (Ŭ���� ��� �Լ��̹Ƿ�, ���� ��� �ÿ��� Ŭ���� ���� �ȿ� �����ؾ� �մϴ�)
float Terrain_Manager::Get_Height(float x, float z)
{
    // NUMVERTERTICES�� Ŭ���� ��� ������� �����մϴ�.
    float half = NUMVERTERTICES / 2.f;

    int LX = static_cast<int>(x + half);
    int DZ = static_cast<int>(z + half);

    if (x < -half || z < -half || x > half || z > half)
        return 0.f;

    // _vector �迭�� Vector4 �迭�� ����
    Vector4 Positions[4];

    // XMVectorSet�� ����ü �ʱ�ȭ�� ����
    Positions[0] = { LX - half,       m_heightMap[(DZ + 1) * NUMVERTERTICES + LX],     DZ + 1 - half, 1.f };
    Positions[1] = { LX + 1 - half,   m_heightMap[(DZ + 1) * NUMVERTERTICES + LX + 1], DZ + 1 - half, 1.f };
    Positions[2] = { LX + 1 - half,   m_heightMap[DZ * NUMVERTERTICES + LX + 1],       DZ - half,     1.f };
    Positions[3] = { LX - half,       m_heightMap[DZ * NUMVERTERTICES + LX],           DZ - half,     1.f };

    float DeltaX = x - (LX - half);
    float DeltaZ = z - (DZ - half);

    // _vector�� Vector4�� ����
    Vector4 PlaneNormal;

    // ���� ������ PlaneFromPoints �Լ��� ȣ��
    if (DeltaX + DeltaZ <= 1.0f)
        PlaneNormal = PlaneFromPoints(Positions[0], Positions[2], Positions[3]);
    else
        PlaneNormal = PlaneFromPoints(Positions[0], Positions[1], Positions[2]);

    // XMVectorGetX/Y/Z/W�� ����ü ��� �������� ����
    // ��� ������(Ax + By + Cz + D = 0)�� y�� ���� ����: y = -(Ax + Cz + D) / B
    float B = PlaneNormal.y;

    return -(PlaneNormal.x * x + PlaneNormal.z * z + PlaneNormal.w) / B;
}