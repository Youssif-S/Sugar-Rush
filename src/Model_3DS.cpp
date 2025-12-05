#include "Model_3DS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <math.h>

Model_3DS::Model_3DS() {}

void Model_3DS::LoadMTL(const char* mtlPath) {
    std::ifstream mtlFile(mtlPath);
    if (!mtlFile.is_open()) {
        printf("Failed to open MTL file: %s\n", mtlPath);
        return;
    }
    std::string line;
    Material currentMat;
    bool inMaterial = false;
    while (std::getline(mtlFile, line)) {
        if (line.substr(0, 6) == "newmtl") {
            if (inMaterial) {
                materials.push_back(currentMat);
                materialNameToIndex[currentMat.name] = (int)materials.size() - 1;
            }
            currentMat = Material();
            std::istringstream iss(line);
            std::string dummy;
            iss >> dummy >> currentMat.name;
            currentMat.Kd[0] = currentMat.Kd[1] = currentMat.Kd[2] = 0.8f;
            inMaterial = true;
        } else if (line.substr(0, 2) == "Kd") {
            std::istringstream iss(line);
            std::string dummy;
            iss >> dummy >> currentMat.Kd[0] >> currentMat.Kd[1] >> currentMat.Kd[2];
        }
    }
    if (inMaterial) {
        materials.push_back(currentMat);
        materialNameToIndex[currentMat.name] = (int)materials.size() - 1;
    }
    mtlFile.close();
}

void Model_3DS::Load(char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        printf("Failed to open model file: %s\n", path);
        return;
    }

    char line[256];
    std::string currentMaterial = "";
    int currentMaterialIndex = -1;
    std::string objDir;
    {
        std::string p(path);
        size_t slash = p.find_last_of("/\\");
        if (slash != std::string::npos) objDir = p.substr(0, slash + 1);
        else objDir = "";
    }
    while (fgets(line, 256, file)) {
        if (strncmp(line, "mtllib", 6) == 0) {
            char mtlFile[256];
            sscanf(line, "mtllib %255s", mtlFile);
            std::string mtlPath = objDir + std::string(mtlFile);
            LoadMTL(mtlPath.c_str());
        } else if (strncmp(line, "usemtl", 6) == 0) {
            char matName[128];
            sscanf(line, "usemtl %127s", matName);
            currentMaterial = matName;
            auto it = materialNameToIndex.find(currentMaterial);
            currentMaterialIndex = (it != materialNameToIndex.end()) ? it->second : -1;
        } else if (strncmp(line, "v ", 2) == 0) {
            Vector3 v;
            sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z);
            vertices.push_back(v);
        } else if (strncmp(line, "vt ", 3) == 0) {
            Vector2 vt;
            sscanf(line, "vt %f %f", &vt.x, &vt.y);
            texCoords.push_back(vt);
        } else if (strncmp(line, "vn ", 3) == 0) {
            Vector3 vn;
            sscanf(line, "vn %f %f %f", &vn.x, &vn.y, &vn.z);
            normals.push_back(vn);
        } else if (strncmp(line, "f ", 2) == 0) {
            Face f;
            f.v[0] = f.v[1] = f.v[2] = 0;
            f.vt[0] = f.vt[1] = f.vt[2] = 0;
            f.vn[0] = f.vn[1] = f.vn[2] = 0;
            f.materialIndex = currentMaterialIndex;
            int matches = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                                 &f.v[0], &f.vt[0], &f.vn[0],
                                 &f.v[1], &f.vt[1], &f.vn[1],
                                 &f.v[2], &f.vt[2], &f.vn[2]);
            if (matches != 9) {
                matches = sscanf(line, "f %d//%d %d//%d %d//%d",
                                 &f.v[0], &f.vn[0],
                                 &f.v[1], &f.vn[1],
                                 &f.v[2], &f.vn[2]);
                if (matches == 6) {
                    f.vt[0] = f.vt[1] = f.vt[2] = 0;
                } else {
                    matches = sscanf(line, "f %d/%d %d/%d %d/%d",
                        &f.v[0], &f.vt[0],
                        &f.v[1], &f.vt[1],
                        &f.v[2], &f.vt[2]);
                    if (matches == 6) {
                        f.vn[0] = f.vn[1] = f.vn[2] = 0;
                    } else {
                        matches = sscanf(line, "f %d %d %d",
                            &f.v[0], &f.v[1], &f.v[2]);
                        if (matches == 3) {
                            f.vt[0] = f.vt[1] = f.vt[2] = 0;
                            f.vn[0] = f.vn[1] = f.vn[2] = 0;
                        }
                    }
                }
            }
            for (int j = 0; j < 3; j++) {
                if (f.v[j] > 0) f.v[j]--;
                if (f.vt[j] > 0) f.vt[j]--;
                if (f.vn[j] > 0) f.vn[j]--;
            }
            faces.push_back(f);
        }
    }
    fclose(file);
    printf("Model loaded: %d vertices, %d normals, %d texcoords, %d faces\n", 
           (int)vertices.size(), (int)normals.size(), (int)texCoords.size(), (int)faces.size());
    if (!vertices.empty()) {
        Vector3 minBounds = vertices[0];
        Vector3 maxBounds = vertices[0];
        for (const auto& v : vertices) {
            if (v.x < minBounds.x) minBounds.x = v.x;
            if (v.y < minBounds.y) minBounds.y = v.y;
            if (v.z < minBounds.z) minBounds.z = v.z;
            if (v.x > maxBounds.x) maxBounds.x = v.x;
            if (v.y > maxBounds.y) maxBounds.y = v.y;
            if (v.z > maxBounds.z) maxBounds.z = v.z;
        }
        printf("Bounding box: Min(%.2f, %.2f, %.2f) Max(%.2f, %.2f, %.2f)\n",
               minBounds.x, minBounds.y, minBounds.z,
               maxBounds.x, maxBounds.y, maxBounds.z);
        printf("Model size: %.2f x %.2f x %.2f\n",
               maxBounds.x - minBounds.x,
               maxBounds.y - minBounds.y,
               maxBounds.z - minBounds.z);
    }
}

void Model_3DS::LoadSTL(char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        printf("Failed to open STL file: %s\n", path);
        return;
    }

    // Read 80-byte header (we don't use it)
    char header[80];
    fread(header, 80, 1, file);

    // Read number of triangles
    unsigned int numTriangles;
    fread(&numTriangles, sizeof(unsigned int), 1, file);

    printf("Loading STL file with %u triangles...\n", numTriangles);

    // Read each triangle
    for (unsigned int i = 0; i < numTriangles; i++) {
        // Read normal (3 floats)
        Vector3 normal;
        fread(&normal.x, sizeof(float), 1, file);
        fread(&normal.y, sizeof(float), 1, file);
        fread(&normal.z, sizeof(float), 1, file);

        // Read 3 vertices (3 floats each)
        Vector3 v1, v2, v3;
        fread(&v1.x, sizeof(float), 1, file);
        fread(&v1.y, sizeof(float), 1, file);
        fread(&v1.z, sizeof(float), 1, file);

        fread(&v2.x, sizeof(float), 1, file);
        fread(&v2.y, sizeof(float), 1, file);
        fread(&v2.z, sizeof(float), 1, file);

        fread(&v3.x, sizeof(float), 1, file);
        fread(&v3.y, sizeof(float), 1, file);
        fread(&v3.z, sizeof(float), 1, file);

        // Read attribute byte count (usually 0)
        unsigned short attributeByteCount;
        fread(&attributeByteCount, sizeof(unsigned short), 1, file);

        // Add vertices
        int idx1 = vertices.size();
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        // Check if normal is zero
        if (normal.x == 0 && normal.y == 0 && normal.z == 0) {
            // Calculate normal
            Vector3 edge1 = {v2.x - v1.x, v2.y - v1.y, v2.z - v1.z};
            Vector3 edge2 = {v3.x - v1.x, v3.y - v1.y, v3.z - v1.z};
            
            normal.x = edge1.y * edge2.z - edge1.z * edge2.y;
            normal.y = edge1.z * edge2.x - edge1.x * edge2.z;
            normal.z = edge1.x * edge2.y - edge1.y * edge2.x;
            
            // Normalize
            float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            if (length > 0) {
                normal.x /= length;
                normal.y /= length;
                normal.z /= length;
            }
        }

        // Add normal (same for all 3 vertices of the triangle)
        normals.push_back(normal);
        normals.push_back(normal);
        normals.push_back(normal);

        // Create face with direct indices
        Face f;
        f.v[0] = idx1;
        f.v[1] = idx1 + 1;
        f.v[2] = idx1 + 2;
        f.vn[0] = idx1;
        f.vn[1] = idx1 + 1;
        f.vn[2] = idx1 + 2;
        f.vt[0] = f.vt[1] = f.vt[2] = 0;

        faces.push_back(f);
    }

    fclose(file);

    printf("STL loaded: %d vertices, %d normals, %d faces\n",
           (int)vertices.size(), (int)normals.size(), (int)faces.size());

    // Calculate bounding box
    if (!vertices.empty()) {
        Vector3 minBounds = vertices[0];
        Vector3 maxBounds = vertices[0];

        for (const auto& v : vertices) {
            if (v.x < minBounds.x) minBounds.x = v.x;
            if (v.y < minBounds.y) minBounds.y = v.y;
            if (v.z < minBounds.z) minBounds.z = v.z;
            if (v.x > maxBounds.x) maxBounds.x = v.x;
            if (v.y > maxBounds.y) maxBounds.y = v.y;
            if (v.z > maxBounds.z) maxBounds.z = v.z;
        }

        printf("Bounding box: Min(%.2f, %.2f, %.2f) Max(%.2f, %.2f, %.2f)\n",
               minBounds.x, minBounds.y, minBounds.z,
               maxBounds.x, maxBounds.y, maxBounds.z);
        printf("Model size: %.2f x %.2f x %.2f\n",
               maxBounds.x - minBounds.x,
               maxBounds.y - minBounds.y,
               maxBounds.z - minBounds.z);
    }
}

void Model_3DS::Draw() {
    if (faces.empty()) {
        // Only print warning if no faces
        printf("Warning: No faces to draw!\n");
        return;
    }

    // Use GL_COLOR_MATERIAL to allow glColor3f to work
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glBegin(GL_TRIANGLES);
    for (const auto& face : faces) {
        for (int i = 0; i < 3; i++) {
            // Apply texture coordinates if available
            if (face.vt[i] >= 0 && face.vt[i] < texCoords.size()) {
                glTexCoord2f(texCoords[face.vt[i]].x, texCoords[face.vt[i]].y);
            }
            
            // Use normals for lighting
            if (face.vn[i] >= 0 && face.vn[i] < normals.size()) {
                glNormal3f(normals[face.vn[i]].x, normals[face.vn[i]].y, normals[face.vn[i]].z);
            }
            
            // Draw vertex
            if (face.v[i] >= 0 && face.v[i] < vertices.size()) {
                glVertex3f(vertices[face.v[i]].x, vertices[face.v[i]].y, vertices[face.v[i]].z);
            }
        }
    }
    glEnd();
}
