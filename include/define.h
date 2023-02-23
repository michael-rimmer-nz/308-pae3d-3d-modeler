/******************************************************************************/
/* Definitions                                                                */
/******************************************************************************/

#pragma once

// Default Window
#define PAE3D_WIN_WIDTH	1280
#define PAE3D_WIN_HEIGHT 720

// Projection parameters
#define PAE3D_FOVY		20.0
#define PAE3D_ZNEAR_3D	1
#define PAE3D_ZFAR_3D	1000.0
#define PAE3D_ZNEAR_2D	-50.0
#define PAE3D_ZFAR_2D	50.0

#define PAE3D_HANDLE_MOVE 5
#define PAE3D_HANDLE_SCALE 6

#define PAE3D_RENDER 1
#define PAE3D_SELECT_RIGHT 2
#define PAE3D_SELECT_LEFT 3
#define PAE3D_LEFTCLICK_NOTHING 0
#define PAE3D_LEFTCLICK_COLOR 1
#define PAE3D_EDIT_MODE 0
#define PAE3D_PHONG_MODE 1

#define PAE3D_LEFT_CLICK 0
#define PAE3D_RIGHT_CLICK 1
#define PAE3D_RENDER_MODE 2
#define PAE3D_HANDLE_MODE 3

#define PAE3D_SELECT_VERTICES 1
#define PAE3D_SELECT_EDGES 2
#define PAE3D_SELECT_FACES 3
#define PAE3D_SELECT_NO_HANDLE -1
#define PAE3D_SELECT_X_HANDLE 0
#define PAE3D_SELECT_Y_HANDLE 1
#define PAE3D_SELECT_Z_HANDLE 2
#define PAE3D_COLORPADDING 3
#define PAE3D_RENDER_NORMALS false

struct PAE3D_Normal {
	float x;
	float y;
	float z;
};

struct PAE3D_Point {
	float x;
	float y;
	float z;
	bool selected;
	unsigned int* edges;
	int edgeCount;
	PAE3D_Normal n;
};

struct PAE3D_Vertex {
	float x;
	float y;
	float z;
	bool selected;
};

struct PAE3D_Edge {
	unsigned int v1;
	unsigned int v2;
	unsigned int c;			// used for subdivision
	bool hasPoly1;			// used for initialization
	bool hasPoly2;			// used for initialization
	unsigned int poly1;
	unsigned int poly2;
	bool selected;
	unsigned int nextEdge;	// used for subdivision
};

struct PAE3D_Polygon {
	bool selected;
	PAE3D_Normal n;
	int vertexCount;
	unsigned int* vertices;
	unsigned int* edges;
	unsigned int c;					// used for subdivision
	int mat;
};

struct PAE3D_RGBA {
        float r;
        float g;
        float b;
        float a;
};

struct PAE3D_Material {
	PAE3D_RGBA col;
	float specular;
	float shininess;
};
