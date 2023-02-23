/*******************************************************************************
 * Color.cpp : Generates and control colour selection window
 *******************************************************************************
 *
 *  Author(s):      Henry P, Michael R
 *  Project:		Comp 308
 *  Date:			2014
 *
 ******************************************************************************/
 
/******************************************************************************/
/* Header files                                                               */
/******************************************************************************/

#include "Color.h"
#include <GL/glut.h>
#include <stdio.h>
#include "ImageLoader.h"
#include <iostream>

using namespace std;

/******************************************************************************/
/* Globals                                                                    */
/******************************************************************************/

void (*repostMain)(void);
void Display(void);
void Click(int, int, int, int);
void Move(int, int);
void leftf();
void rightf();
void addf();
void clip();

PAE3D_Material* mats;
int matCount;
GLint wind;
int cur = 0;
bool leftDown = false;

GLuint red;
GLuint green;
GLuint blue;
GLuint specular;
GLuint shininess;
GLuint add;
GLuint leftArrow;
GLuint rightArrow;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* 	Init																	  */
/******************************************************************************/
Color::Color(void (*r)(void), unsigned int mainWin, unsigned int height, void (*d) (unsigned char, int,int), void (*u) (unsigned char, int,int)) {
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	wind = glutCreateSubWindow(mainWin,0,height -6*32,255,6*32);
	repostMain = r;
	
	matCount = 1;
	mats = new PAE3D_Material[matCount];
	PAE3D_Material mat;
	mat.col.r = mat.col.g = 0.77;
	mat.col.b = 0.85;
	mat.col.a = 1;
	mat.shininess = 1;
	mat.specular = 0;
	mats[0] = mat;
	
	red = openTexture("textures/RED.png");
	green = openTexture("textures/GRE.png");
	blue = openTexture("textures/BLU.png");
	specular = openTexture("textures/SPE.png");
	shininess = openTexture("textures/SHI.png");
	add = openTexture("textures/add.png");
	leftArrow = openTexture("textures/leftarrow.png");
	rightArrow = openTexture("textures/rightarrow.png");
	
	glutDisplayFunc(Display);
	glutMouseFunc(Click);
	glutMotionFunc(Move);
	glutKeyboardFunc(d);
	glutKeyboardUpFunc(u);
	glClearColor(1, 1, 1, 1);
} /* Color */

/******************************************************************************/
/* 	Destructor																  */
/******************************************************************************/
Color::~Color() {
	delete(mats);
} /*  */

/******************************************************************************/
/* 	Get material															  */
/******************************************************************************/
PAE3D_Material* Color::GetMaterial(int i) {
	if (i < 0 || i > matCount) return &mats[0];
	return &mats[i];
} /*  */

/******************************************************************************/
/* 	Resize sub window														  */
/******************************************************************************/
void Color::Resize(int width, int height) {
	(void)width;
	glutSetWindow(wind);
	glutPositionWindow (0, height-6*32);
} /*  */

/******************************************************************************/
/* 	Getter for the current material											  */
/******************************************************************************/
int Color::GetCurrentMaterial() {
	return cur;
} /*  */

/******************************************************************************/
/* 	Process subwindow click													  */
/******************************************************************************/
void Click(int button, int state, int x, int y) {
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	float newy = y * 6.0/10;
	float percY = -(newy / (height / 2.0) - 1);
	
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			leftDown = true;
			if (percY > 0.8) {
				mats[cur].col.r = (x - 1) / 256.0;
			}
			else if (percY > 0.6) {
				mats[cur].col.g = (x - 1) / 256.0;
			}
			else if (percY > 0.4) {
				mats[cur].col.b = (x - 1) / 256.0;
			}
			else if (percY > 0.2) {
				mats[cur].specular = (x - 1) / 256.0;
			}
			else if (percY > 0.0) {
				mats[cur].shininess = (x - 1) / 2.0;
			}
			else if (percY > -0.2) {
				if (x < 32) leftf();
				else if (x > 256-32) addf();
				else if (x > 256-64) rightf();
			}
		}
		else {
			leftDown = false;
		}
	}
	clip();
	glutPostRedisplay();
	repostMain();
} /*  */

/******************************************************************************/
/* 	Mouse click and move processing on colour bar. Changes settings. 		  */
/******************************************************************************/
void Move(int x, int y) {
	
	float newy = y * 6.0/10;
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	float percY = -(newy / (height / 2.0) - 1);
	
	//change settings
	if (leftDown) {
		if (percY > 0.8) {
			mats[cur].col.r = (x - 1) / 256.0;
		}
		else if (percY > 0.6) {
			mats[cur].col.g = (x - 1) / 256.0;
		}
		else if (percY > 0.4) {
			mats[cur].col.b = (x - 1) / 256.0;
		}
		else if (percY > 0.2) {
			mats[cur].specular = (x - 1) / 256.0;
		}
		else if (percY > 0.0) {
			mats[cur].shininess = (x - 1) / 2.0;
		}
	}
	clip();
	glutPostRedisplay();
	repostMain();
} /*  */

/******************************************************************************/
/* 	Keep rgb, specular andshiniess between boundaries						  */
/******************************************************************************/
void clip() {
	mats[cur].col.r = mats[cur].col.r > 1 ? 1 : mats[cur].col.r < 0 ? 0 : mats[cur].col.r;
	mats[cur].col.g = mats[cur].col.g > 1 ? 1 : mats[cur].col.g < 0 ? 0 : mats[cur].col.g;
	mats[cur].col.b = mats[cur].col.b > 1 ? 1 : mats[cur].col.b < 0 ? 0 : mats[cur].col.b;
	mats[cur].specular = mats[cur].specular > 1 ? 1 : mats[cur].specular < 0 ? 0 : mats[cur].specular;
	mats[cur].shininess = mats[cur].shininess > 128 ? 128 : mats[cur].shininess < 0 ? 0 : mats[cur].shininess;
} /*  */

/******************************************************************************/
/* 	Move left button operation												  */
/******************************************************************************/
void leftf() {
	if (cur > 0) cur--;
} /* leftf */

/******************************************************************************/
/* 	Move right button operation												  */
/******************************************************************************/
void rightf() {
	if (cur < matCount-1) cur++;
} /* rightf */

/******************************************************************************/
/* 	Add another colour materal to palette 									  */
/******************************************************************************/
void addf() {
	PAE3D_Material* temp = new PAE3D_Material[matCount+1];
	for (int i = 0; i < matCount; i++) {
		temp[i] = mats[i];
	}
	delete(mats);
	mats = temp;
	cur = matCount;
	mats[cur].col.r = mats[cur].col.g = 0.77;
	mats[cur].col.b = 0.5;
	mats[cur].col.a = 1;
	mats[cur].shininess = 1;
	mats[cur].specular = 0;
	matCount++;
} /*  */

/******************************************************************************/
/* 	Display	colour bar														  */
/******************************************************************************/
void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glScalef(1, 10.0/6.0, 1);
	glTranslatef(0, -0.4, 0);

	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glBegin(GL_LINES);
	// red
	for (int i = 0; i < 256; i++) {
		glColor3f(i / 256.0, mats[cur].col.g, mats[cur].col.b);
		glVertex2f(i / 256.0 * 2 - 1, 1);
		glVertex2f(i / 256.0 * 2 - 1, 0.8);
	}
	glColor3f(1 - mats[cur].col.r, 1 - mats[cur].col.g, 1 - mats[cur].col.b);
	glVertex2f((mats[cur].col.r * 256 + 1) / 129.0 - 1, 1);
	glVertex2f((mats[cur].col.r * 256 + 1) / 129.0 - 1, 0.8);
	// green
	for (int i = 0; i < 256; i++) {
		glColor3f(mats[0].col.r, i / 256.0, mats[cur].col.b);
		glVertex2f(i / 256.0 * 2 - 1, 0.8);
		glVertex2f(i / 256.0 * 2 - 1, 0.6);
	}
	glColor3f(1 - mats[cur].col.r, 1 - mats[cur].col.g, 1 - mats[cur].col.b);
	glVertex2f((mats[cur].col.g * 256 + 1) / 129.0 - 1, 0.8);
	glVertex2f((mats[cur].col.g * 256 + 1) / 129.0 - 1, 0.6);
	// blue
	for (int i = 0; i < 256; i++) {
		glColor3f(mats[cur].col.r, mats[cur].col.b, i / 256.0);
		glVertex2f(i / 256.0 * 2 - 1, 0.6);
		glVertex2f(i / 256.0 * 2 - 1, 0.4);
	}
	glColor3f(1 - mats[cur].col.r, 1 - mats[cur].col.g, 1 - mats[cur].col.b);
	glVertex2f((mats[cur].col.b * 256 + 1) / 129.0 - 1, 0.6);
	glVertex2f((mats[cur].col.b * 256 + 1) / 129.0 - 1, 0.4);
	// specular
	for (int i = 0; i < 256; i++) {
		glColor3f(i / 256.0, i / 256.0, i / 256.0);
		glVertex2f(i / 256.0 * 2 - 1, 0.4);
		glVertex2f(i / 256.0 * 2 - 1, 0.2);
	}
	glColor3f(1 - mats[cur].specular, 1 - mats[cur].specular, 1 - mats[cur].specular);
	glVertex2f((mats[cur].specular * 256 + 1) / 129.0 - 1, 0.4);
	glVertex2f((mats[cur].specular * 256 + 1) / 129.0 - 1, 0.2);
	// shininess
	for (int i = 0; i < 256; i++) {
		glColor3f(1 - i / 256.0, 1 - i / 256.0, 1 - i / 256.0);
		glVertex2f(i / 256.0 * 2 - 1, 0.2);
		glVertex2f(i / 256.0 * 2 - 1, 0.0);
	}
	glColor3f(mats[cur].shininess/128, mats[cur].shininess/128, mats[cur].shininess/128);
	glVertex2f((mats[cur].shininess * 2 + 1) / 129.0 - 1, 0.2);
	glVertex2f((mats[cur].shininess * 2 + 1) / 129.0 - 1, 0.0);
	glEnd();

	glBegin(GL_QUADS);
	for (int i = 0; i < matCount; i++) {
		glColor3f(mats[i].col.r, mats[i].col.g, mats[i].col.b);
		glVertex3f(-0.75 + 1.25/matCount * (i+1), -0, 0);
		glVertex3f(-0.75 + 1.25/matCount * (i+1), -0.2, 0);
		glVertex3f(-0.75 + 1.25/matCount * i, -0.2, 0);
		glVertex3f(-0.75 + 1.25/matCount * i, -0, 0);
	}
	glEnd();
	glBegin(GL_LINE_STRIP);
	glColor3f(1-mats[cur].col.r, 1-mats[cur].col.g, 1-mats[cur].col.b);
	float pixel = 0.002;
	glVertex3f(-0.75 + 1.25/matCount * (cur+1)-pixel, -0-pixel, 0);
	glVertex3f(-0.75 + 1.25/matCount * (cur+1)-pixel, -0.2, 0);
	glVertex3f(-0.75 + 1.25/matCount * cur+pixel, -0.2, 0);
	glVertex3f(-0.75 + 1.25/matCount * cur+pixel, -0-pixel, 0);
	glVertex3f(-0.75 + 1.25/matCount * (cur+1)-pixel, -0-pixel, 0);
	glEnd();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glColor3f(1, 0, 0);
	glBindTexture(GL_TEXTURE_2D, red);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 0.8, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, 0.8, 0);
	glEnd();

	glColor3f(0, 1, 0);
	glBindTexture(GL_TEXTURE_2D, green);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 0.6, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 0.8, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 0.8, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, 0.6, 0);
	glEnd();

	glColor3f(0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, blue);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 0.4, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 0.6, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 0.6, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, 0.4, 0);
	glEnd();

	glColor3f(1, 1, 1);
	glBindTexture(GL_TEXTURE_2D, specular);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 0.2, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 0.4, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 0.4, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, 0.2, 0);
	glEnd();

	glColor3f(0, 0, 0);
	glBindTexture(GL_TEXTURE_2D, shininess);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, 0.0, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 0.2, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, 0.2, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, 0.0, 0);
	glEnd();

	glColor3f(1, 1, 1);
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA);

	glBindTexture(GL_TEXTURE_2D, leftArrow);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -0, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-1, -0.2, 0);
	glTexCoord2f(1, 1);
	glVertex3f(-0.75, -0.2, 0);
	glTexCoord2f(1, 0);
	glVertex3f(-0.75, -0, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, rightArrow);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0.5, -0, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0.5, -0.2, 0);
	glTexCoord2f(1, 1);
	glVertex3f(0.75, -0.2, 0);
	glTexCoord2f(1, 0);
	glVertex3f(0.75, -0, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, add);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0.75, -0, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0.75, -0.2, 0);
	glTexCoord2f(1, 1);
	glVertex3f(1, -0.2, 0);
	glTexCoord2f(1, 0);
	glVertex3f(1, -0, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glutSwapBuffers();
	
} /* Display */

