/*******************************************************************************
 * Toolbar.cpp :  Toolbar with various editing tool buttons
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

#include "Toolbar.h"
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "ImageLoader.h"

using namespace std;

/******************************************************************************/
/* Globals                                                                    */
/******************************************************************************/

void BarDisplay(void);
void BarClick(int, int, int, int);
void BarSetCamera(void);
void DrawButtons(bool);
void ProcessButton(int x, int y);
void (*startId)(int);

GLint barWind;
GLint barToMainWind;
int barMode = 0;
bool barLeftDown = false;
int mouseX =0;
int mouseY =0;
int currentButton = 0;

int buttonCount = 12;

int (*getParami)(int);

GLuint buttons;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* Init                                                                		  */
/******************************************************************************/
Toolbar::Toolbar(void (*i)(int), unsigned int mainWin, void (*d) (unsigned char, int,int), void (*u) (unsigned char, int,int), int (*pi)(int)) {
	startId = i;
	currentButton = 0;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	barToMainWind = mainWin;
	barWind = glutCreateSubWindow(mainWin,0,0,32*buttonCount,32);
	glClearColor(0,0,0,0);
	glutDisplayFunc(BarDisplay);
	BarSetCamera();
	buttons = openTexture("textures/toolbar.jpg");
	glutMouseFunc(BarClick);
	glutKeyboardFunc(d);
	glutKeyboardUpFunc(u);
	glClearColor(1, 1, 1, 1);
	getParami = pi;
	
} /* Toolbar */

/******************************************************************************/
/* Destructor                                                          		  */
/******************************************************************************/
Toolbar::~Toolbar() {

} /* ~Toolbar */

/******************************************************************************/
/* Redisplay                                                           		  */
/******************************************************************************/
void Toolbar::repaint() {
	glutSetWindow(barWind);
	glutPostRedisplay();
	
} /* repaint */


/******************************************************************************/
/* 	Process click                                                      		  */
/******************************************************************************/
void BarClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			barLeftDown = true;
			barMode = 1;
		}
		else {
			barLeftDown = false;
			barMode = 0;
		}
	}
	mouseX = x;
	mouseY = y;
	glutPostRedisplay();
	
} /* BarClick */


/******************************************************************************/
/* 	Returns current button                                             		  */
/******************************************************************************/
int Toolbar::getCurrentButton(){
	return currentButton;
} /* getCurrentButton */


/******************************************************************************/
/* 	 Toolbar display control                                           		  */
/******************************************************************************/
void BarDisplay() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(barMode == 0)
	{	
		DrawButtons(false);
	}
	
	else if(barMode == 1)
	{
		DrawButtons(true);
		ProcessButton(mouseX,mouseY);
	}
	
	if(barMode != 0) 
	{
		barMode = 0;
		currentButton = 0;
		glutPostRedisplay();
	} 
	else 
	{
		glutSwapBuffers();
	}
	
} /* BarDisplay */

/******************************************************************************/
/* 	 Process toolbar click                                       			  */
/******************************************************************************/
void ProcessButton(int x, int y){
	GLint viewport[4];
	GLubyte pixel[3];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glReadPixels(x,viewport[3]-y,1,1,GL_RGB,GL_UNSIGNED_BYTE,(void *)pixel);
	int id = (pixel[0] << 0) + (pixel[1] << 8) + (pixel[2] << 16);

	currentButton = id;

	startId(id);
	
} /* ProcessButton */

/******************************************************************************/
/* 	 Draw toolbar buttons                                  					  */
/******************************************************************************/
void DrawButtons(bool picking){
	
	if (!picking) {
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, buttons);
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(-1, -1, 0);
		glTexCoord2f(1, 0);
		glVertex3f(1, -1, 0);
		glTexCoord2f(1, 1);
		glVertex3f(1, 1, 0);
		glTexCoord2f(0, 1);
		glVertex3f(-1, 1, 0);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		bool highlighted[12] = {
				getParami(PAE3D_RIGHT_CLICK) == PAE3D_SELECT_FACES,
				getParami(PAE3D_RIGHT_CLICK) == PAE3D_SELECT_EDGES,
				getParami(PAE3D_RIGHT_CLICK) == PAE3D_SELECT_VERTICES,
				false,
				getParami(PAE3D_HANDLE_MODE) == PAE3D_HANDLE_MOVE,
				getParami(PAE3D_HANDLE_MODE) == PAE3D_HANDLE_SCALE,
				false, false, false, false,
				getParami(PAE3D_LEFT_CLICK) == PAE3D_LEFTCLICK_COLOR,
				getParami(PAE3D_RENDER_MODE) == PAE3D_PHONG_MODE};

		glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0, 1.0, 1.0, 0.3);

		for (int i = 0; i < 12; i++) {
			if (highlighted[i]) {
			glBegin(GL_QUADS);
			glVertex3f(-1+i*1.0/6, -1, 0);
			glVertex3f(-1+(i+1)*1.0/6, -1, 0);
			glVertex3f(-1+(i+1)*1.0/6, 1, 0);
			glVertex3f(-1+i*1.0/6, 1, 0);
			glEnd();
			}
		}


		glDisable(GL_BLEND);
	} else {
		for (int i = 0; i < 12; i++) {

			int id = i + 1;
			int r = (id & 0x000000FF) >> 0;
			int g = (id & 0x0000FF00) >> 8;
			int b = (id & 0x00FF0000) >> 16;
			glColor3f(r / 255.0, g / 255.0, b / 255.0);

			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(-1 + i * 2.0 / buttonCount, -1, 0);
			glTexCoord2f(1, 0);
			glVertex3f(-1 + (i + 1) * 2.0 / buttonCount, -1, 0);
			glTexCoord2f(1, 1);
			glVertex3f(-1 + (i + 1) * 2.0 / buttonCount, 1, 0);
			glTexCoord2f(0, 1);
			glVertex3f(-1 + i * 2.0 / buttonCount, 1, 0);
			glEnd();
		}
	}
	
} /* DrawButtons */

/******************************************************************************/
/* 	Generating fixed toolbar camera 					                	  */
/******************************************************************************/
void BarSetCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(PAE3D_FOVY, (double) 640 / (double) (35), PAE3D_ZNEAR_3D, PAE3D_ZFAR_3D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	PAE3D_Point newCenter;
	newCenter.x =0;newCenter.y =0;newCenter.z =0;
	gluLookAt(0.0, 0.0, 1.0, newCenter.x, newCenter.y, newCenter.z, 0, 1, 0);
}
