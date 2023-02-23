#pragma once

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <jerror.h>
#include <png.h>
#include "string.h"

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#define png_voidp_NULL NULL
#define TEXTURE_NONE 0
#define TEXTURE_MATERIAL 1
#define TEXTURE_UV 2

typedef struct texInfo {
	char *filename;
	unsigned char* textureData;
	int format;
	int width;
	int height;
	bool hasAlpha;
} TextureInfo;

//static GLuint cubeMap;


int loadTextureFromJPEG(char*,TextureInfo*);
int loadTextureFromPNG(char*, TextureInfo*);
GLuint openTexture(const char* filename, GLuint);
GLuint openTexture(const char* filename);
TextureInfo openTextureInfo(const char* filename);
GLuint openCubeMap(const char*, const char*, const char*, const char*, const char*, const char*);

