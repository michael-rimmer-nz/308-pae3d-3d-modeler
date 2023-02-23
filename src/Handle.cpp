/*******************************************************************************
 * Handle.cpp : Transform tool drawn in context of 3D model.  
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

#include "Handle.h"

using namespace std;

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/

/******************************************************************************/
/* Render X handle with move arrow                                            */
/******************************************************************************/
void Handle::RenderXHandleMove(GLUquadric* quadric, float width, float length) {
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	gluCylinder(quadric, width, width, length, 20, 1);
	glTranslatef(0, 0, length);
	gluCylinder(quadric, width * 4, 0, width * 8, 20, 1);
	glPopMatrix();
	
} /* RenderXHandleMove */


/******************************************************************************/
/* Render Y handle with move arrow                                            */
/******************************************************************************/
void Handle::RenderYHandleMove(GLUquadric* quadric, float width, float length) {
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	gluCylinder(quadric, width, width, length, 20, 1);
	glTranslatef(0, 0, length);
	gluCylinder(quadric, width * 4, 0, width * 8, 20, 1);
	glPopMatrix();
	
} /* RenderYHandleMove */


/******************************************************************************/
/*  Render Z handle with move arrow                                           */
/******************************************************************************/
void Handle::RenderZHandleMove(GLUquadric* quadric, float width, float length) {
	glPushMatrix();
	gluCylinder(quadric, width, width, length, 20, 1);
	glTranslatef(0, 0, length);
	gluCylinder(quadric, width * 4, 0, width * 8, 20, 1);
	glPopMatrix();
	
} /* RenderZHandleMove */


/******************************************************************************/
/*  Render X handle with scale cube                                           */
/******************************************************************************/
void Handle::RenderXHandleScale(GLUquadric* quadric, float width, float length) {
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	gluCylinder(quadric, width, width, length, 20, 1);
	glTranslatef(0, 0, length);
	RenderCube(width);
	glPopMatrix();
	
} /* RenderXHandleScale */


/******************************************************************************/
/* Render Y handle with scale cube                                			  */
/******************************************************************************/
void Handle::RenderYHandleScale(GLUquadric* quadric, float width, float length) {
	glPushMatrix();
	glRotatef(90, -1, 0, 0);
	gluCylinder(quadric, width, width, length, 20, 1);
	glTranslatef(0, 0, length);
	RenderCube(width);
	glPopMatrix();
	
}  /* RenderYHandleScale */


/******************************************************************************/
/* Render Z handle with scale cube                                            */
/******************************************************************************/
void Handle::RenderZHandleScale(GLUquadric* quadric, float width, float length) {
	glPushMatrix();
	gluCylinder(quadric, width, width, length, 20, 1);
	glTranslatef(0, 0, length);
	RenderCube(width);
	glPopMatrix();
	
} /* RenderZHandleScale */

/******************************************************************************/
/* Render handle scale cube                                                   */
/******************************************************************************/
void Handle::RenderCube(float width) {
	glBegin(GL_QUADS);
	glVertex3f( width*4,  width*4, 0);
	glVertex3f(-width*4,  width*4, 0);
	glVertex3f(-width*4, -width*4, 0);
	glVertex3f( width*4, -width*4, 0);
	glVertex3f( width*4,  width*4, width*8);
	glVertex3f(-width*4,  width*4, width*8);
	glVertex3f(-width*4, -width*4, width*8);
	glVertex3f( width*4, -width*4, width*8);
	glVertex3f( width*4,  width*4, 0);
	glVertex3f(-width*4,  width*4, 0);
	glVertex3f(-width*4,  width*4, width*8);
	glVertex3f( width*4,  width*4, width*8);
	glVertex3f( width*4, -width*4, 0);
	glVertex3f(-width*4, -width*4, 0);
	glVertex3f(-width*4, -width*4, width*8);
	glVertex3f( width*4, -width*4, width*8);
	glVertex3f(-width*4,  width*4, 0);
	glVertex3f(-width*4, -width*4, 0);
	glVertex3f(-width*4, -width*4, width*8);
	glVertex3f(-width*4,  width*4, width*8);
	glVertex3f( width*4,  width*4, 0);
	glVertex3f( width*4, -width*4, 0);
	glVertex3f( width*4, -width*4, width*8);
	glVertex3f( width*4,  width*4, width*8);
	glEnd();
	
} /* RenderCube */
