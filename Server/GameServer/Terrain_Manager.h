#pragma once

struct Vector4 {
    float x, y, z, w;
};


class Terrain_Manager
{
public:
    static Terrain_Manager& GetInstance()
    {
        static Terrain_Manager instance;
        return instance;
    }

private:
    Terrain_Manager() = default;
    ~Terrain_Manager() = default;

public:

	bool Read_Map(const std::string& filePath, int width, int height, float cellSpacing);
    float Get_Height(float worldX, float worldZ);
    void Show_MapData();

    Vector4 PlaneFromPoints(const Vector4& p1, const Vector4& p2, const Vector4& p3);

    //float Get_Terrain_Heights(float x, float z);

private:

    std::vector<float> m_heightMap; 
    int m_width = 0;
    int m_height = 0;
    float m_cellSpacing = 1.0f;

    inline int Get_Index(int x, int z) const {
        return z * m_width + x;
    }
};



