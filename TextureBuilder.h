
#pragma once
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <glut.h>
#include <stdio.h>
#include <stdlib.h>

// Loads a PPM texture (2D)
void loadPPM(GLuint *textureID, const char *strFileName, int width, int height, int wrap);
// Loads a BMP texture (2D) - expects 24-bit uncompressed BMP
void loadBMP(GLuint *textureID, const char *strFileName, int wrap);
// Loads a 3D texture from raw RGB data (width x height x depth x 3 bytes)
void load3DTexture(GLuint *textureID, const char *strFileName, int width, int height, int depth, int wrap);