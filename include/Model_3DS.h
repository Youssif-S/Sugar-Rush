#pragma once
#include <windows.h>
#include <GL/gl.h>
#include <glut.h>
#include <vector>
#include <string>
#include <map>

struct Vector3 {
    float x, y, z;
};

struct Vector2 {
    float x, y;
};

struct Face {
    int v[3];
    int vt[3];
    int vn[3];
    int materialIndex; // Index into materials vector
};

struct Material {
    std::string name;
    float Kd[3]; // Diffuse color
};

class Model_3DS {
public:
    Model_3DS();
    void Load(char* path);
    void LoadSTL(char* path);  // NEW: STL loader
    void Draw();

private:
    std::vector<Vector3> vertices;
    std::vector<Vector2> texCoords;
    std::vector<Vector3> normals;
    std::vector<Face> faces;
    std::vector<Material> materials;
    std::map<std::string, int> materialNameToIndex;
    void LoadMTL(const char* mtlPath);
};
