#include "pch.h"
#include "Terrain_Manager.h"
#include "fstream"



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


float Terrain_Manager::Get_Height(float worldX, float worldZ) const
{
    int x = static_cast<int>((worldX / m_cellSpacing) + (m_width / 2));
    int z = static_cast<int>((worldZ / m_cellSpacing) + (m_height / 2));

    if (x < 0 || x >= m_width || z < 0 || z >= m_height)
        return 0.0f;

    return m_heightMap[Get_Index(x, z)];
}

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
