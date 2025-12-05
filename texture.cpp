#include <stdio.h>
#include <stdlib.h>

// Loads a 3D texture from a raw RGB file (width x height x depth x 3 bytes)
void load3DTexture(GLuint *textureID, const char *strFileName, int width, int height, int depth, int wrap) {
	FILE *pFile = NULL;
	fopen_s(&pFile, strFileName, "rb");
	if (!pFile) {
		MessageBoxA(NULL, "3D Texture file not found!", "Error!", MB_OK);
		exit(EXIT_FAILURE);
	}
	size_t dataSize = width * height * depth * 3;
	unsigned char *data = (unsigned char*)malloc(dataSize);
	fread(data, 1, dataSize, pFile);
	fclose(pFile);

	glGenTextures(1, textureID);
	glBindTexture(GL_TEXTURE_3D, *textureID);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap ? GL_REPEAT : GL_CLAMP);

	free(data);
}
#include "TextureBuilder.h"
#include <glut.h>

#define GLUT_KEY_ESCAPE 27

GLuint texID;

int rep = 1;

void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, texID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(100, 100, 0);
	glTexCoord2f(rep, 0.0f); glVertex3f(500, 100, 0);
	glTexCoord2f(rep, rep); glVertex3f(500, 500, 0);
	glTexCoord2f(0.0f, rep); glVertex3f(100, 500, 0);
	glEnd();
	glPopMatrix();

	glFlush();
}


void Keyboard(unsigned char key, int x, int y) {
	if (key == GLUT_KEY_ESCAPE)
		exit(EXIT_SUCCESS);
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);

	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("texture");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);

	gluOrtho2D(0, 600, 0, 600);

	loadBMP(&texID, "textures/box.bmp", false);
	//loadBMP(&texID, "textures/metal.bmp", true);
	//loadPPM(&texID, "textures/clouds.ppm", 200, 200, true);

	glutMainLoop();
}