#include "TextureBuilder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loadBMP(GLuint *textureID, const char *strFileName, int wrap) {
    FILE *file = fopen(strFileName, "rb");
    if (!file) {
        printf("Image could not be opened: %s\n", strFileName);
        return;
    }

    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return;
    }

    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file\n");
        fclose(file);
        return;
    }

    unsigned int dataPos = *(int*)&header[0x0A];
    unsigned int imageSize = *(int*)&header[0x22];
    unsigned int width = *(int*)&header[0x12];
    unsigned int height = *(int*)&header[0x16];

    if (imageSize == 0) imageSize = width * height * 3;
    if (dataPos == 0) dataPos = 54;

    unsigned char *data = (unsigned char*)malloc(imageSize);
    fread(data, 1, imageSize, file);
    fclose(file);

    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);

    free(data);
}

void loadPPM(GLuint *textureID, const char *strFileName, int width, int height, int wrap) {
    // Basic implementation or placeholder
}

