/*******************************************************************************
 * RenderHelper.cpp :   render utility 
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

#include "RenderHelper.h"


/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/*  Sky box render				                                              */
/******************************************************************************/
void PAE3D_RenderSkyBox(GLuint xp, GLuint xn, GLuint yp, GLuint yn, GLuint zp, GLuint zn) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float c = 0.001; // move faces closer by a 10th of a pixel to remove artifacts at seams

	glBindTexture(GL_TEXTURE_2D, xp);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(1 - c, -1, -1);
	glTexCoord2f(0, 1);
	glVertex3f(1 - c, 1, -1);
	glTexCoord2f(1, 1);
	glVertex3f(1 - c, 1, 1);
	glTexCoord2f(1, 0);
	glVertex3f(1 - c, -1, 1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, xn);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(c - 1, -1, 1);
	glTexCoord2f(0, 1);
	glVertex3f(c - 1, 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(c - 1, 1, -1);
	glTexCoord2f(1, 0);
	glVertex3f(c - 1, -1, -1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, zp);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(1, -1, 1 - c);
	glTexCoord2f(0, 1);
	glVertex3f(1, 1, 1 - c);
	glTexCoord2f(1, 1);
	glVertex3f(-1, 1, 1 - c);
	glTexCoord2f(1, 0);
	glVertex3f(-1, -1, 1 - c);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, zn);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-1, -1, c - 1);
	glTexCoord2f(0, 1);
	glVertex3f(-1, 1, c - 1);
	glTexCoord2f(1, 1);
	glVertex3f(1, 1, c - 1);
	glTexCoord2f(1, 0);
	glVertex3f(1, -1, c - 1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, yp);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(1, 1 - c, 1);
	glTexCoord2f(0, 1);
	glVertex3f(1, 1 - c, -1);
	glTexCoord2f(1, 1);
	glVertex3f(-1, 1 - c, -1);
	glTexCoord2f(1, 0);
	glVertex3f(-1, 1 - c, 1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, yn);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(1, c - 1, -1);
	glTexCoord2f(0, 1);
	glVertex3f(1, c - 1, 1);
	glTexCoord2f(1, 1);
	glVertex3f(-1, c - 1, 1);
	glTexCoord2f(1, 0);
	glVertex3f(-1, c - 1, -1);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glClear(GL_DEPTH_BUFFER_BIT);
	
} /* PAE3D_RenderSkyBox */

 
/******************************************************************************/
/* Reference xyz axes render                                                  */
/******************************************************************************/
void PAE3D_RenderAxes() {
	glLineWidth(1);
	glBegin(GL_LINES);
	// X-AXIS
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1000, 0, 0);
	// Y-AXIS
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1000, 0);
	// Z-AXIS
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1000);
	glEnd();
	
} /* PAE3D_RenderAxes */
 
/******************************************************************************/
/* Reference grid render                                	                  */
/******************************************************************************/
void PAE3D_RenderGrid(float zoom) {
	float level = pow(10, (int) log10(zoom) - 1);
	glLineWidth(1);
	glBegin(GL_LINES);
	glColor3f(0.3, 0.3, 0.3);
	for (int x = -10; x <= 10; x++) {
		if (x == 0) {
			glVertex3f(x * level, 0, 0);
			glVertex3f(x * level, 0, -10 * level);
		} else {
			glVertex3f(x * level, 0, 10 * level);
			glVertex3f(x * level, 0, -10 * level);
		}
	}
	for (int z = -10; z <= 10; z++) {
		if (z == 0) {
			glVertex3f(0, 0, z * level);
			glVertex3f(-10 * level, 0, z * level);
		} else {
			glVertex3f(10 * level, 0, z * level);
			glVertex3f(-10 * level, 0, z * level);
		}
	}
	glEnd();
	
} /* PAE3D_RenderGrid */
