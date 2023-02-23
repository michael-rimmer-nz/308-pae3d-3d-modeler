#pragma once
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "define.h"

class Color {
public:
	Color(void (*r)(void), unsigned int, unsigned int, void (*d) (unsigned char, int,int), void (*u) (unsigned char, int,int));
	~Color(void);
	PAE3D_Material* GetMaterial(int);
	void Resize(int width, int height);
	int GetCurrentMaterial();
};
