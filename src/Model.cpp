/*******************************************************************************
 * Model.cpp : Model
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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include "Handle.h"
#include "ImageLoader.h"
#include "Model.h"

#include <iostream>

using namespace std;

/******************************************************************************/
/* Globals                                                                    */
/******************************************************************************/

int m_nListPoint;
int m_nListEdge;
int m_nListPolygon;
int numVert, numNorm, numUV, numFace;
GLUquadric* quadric = gluNewQuadric();

void Sort(unsigned int* num, int numLength);

/******************************************************************************/
/* Functions                                                                  */
/******************************************************************************/


/******************************************************************************/
/* Read object file and create model                                          */
/******************************************************************************/
void Model::ReadOBJ(const char *filename) {
	FILE* fp;
	char mode, vmode;
	char str[200];
	int v1, v2, v3, n1, n2, n3, t1, t2, t3;
	int numVert, numNorm, numUV, numFace;
	float x, y, z;
	float u, v;

	numVert = numNorm = numUV = numFace = 0;

	fp = fopen(filename, "r");
	if (fp == NULL)
		printf("Error reading %s file\n", filename);
	else
		printf("Reading %s file\n", filename);

	// Check number of vertex, normal, uvCoord, and Face
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
				numUV++;
			else if (vmode == 'n') // normal
				numNorm++;
			else if (vmode == ' ') // vertex
				numVert++;
			break;
		case 'f': /* faces */
			numFace++;
			break;
		}
	}

	m_nNumPoint = numVert;
	m_nNumPolygon = numFace;
	m_nNumEdge = 0;

	m_pVertexArray = new PAE3D_Point[m_nNumPoint];
	m_pEdgeArray = new PAE3D_Edge[m_nNumPolygon*3];
	m_pPolyArray = new PAE3D_Polygon[m_nNumPolygon];
	m_nListPoint = m_nNumPoint;
	m_nListEdge = m_nNumPolygon;
	m_nListPolygon = m_nNumPolygon;

	//-----------------------------------------------------------
	//	Read obj file
	//-----------------------------------------------------------
	numVert = numNorm = numUV = numFace = 0;

	fseek(fp, 0L, SEEK_SET);
	while (fgets(str, 200, fp) != NULL) {
		sscanf(str, "%c%c", &mode, &vmode);
		switch (mode) {
		case 'v': /* vertex, uv, normal */
			if (vmode == 't') // uv coordinate
					{
				sscanf(str, "vt %f %f", &u, &v);
				//m_pUVArray[numUV].u = u;
				//m_pUVArray[numUV].v = v;
				numUV++;
			} else if (vmode == 'n') // normal
					{
				sscanf(str, "vn %f %f %f", &x, &y, &z);
				/*m_pNormalArray[numNorm].x = x;
				m_pNormalArray[numNorm].y = y;
				m_pNormalArray[numNorm].z = z;*/
				numNorm++;
			} else if (vmode == ' ') // vertex
					{
				sscanf(str, "v %f %f %f", &x, &y, &z);
				m_pVertexArray[numVert].x = x;
				m_pVertexArray[numVert].y = y;
				m_pVertexArray[numVert].z = z;
				m_pVertexArray[numVert].selected = true;
				numVert++;
			}
			break;
		case 'f': /* faces : stored value is index - 1 since our index starts from 0, but obj starts from 1 */
			if (numNorm > 0 && numUV > 0) {
				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2, &v3, &t3, &n3);
			} else if(numNorm > 0 && numUV ==0){
				sscanf(str, "f %d//%d %d//%d %d//%d", &v1, &n1, &v2, &n2, &v3, &n3);
			} else if(numUV > 0 && numNorm==0){
				sscanf(str, "f %d/%d %d/%d %d/%d", &v1, &t1, &v2, &t2, &v3, &t3);
			} else if(numUV==0 && numNorm==0){
				sscanf(str, "f %d %d %d", &v1, &v2, &v3);
			}
			// Vertex indicies for triangle:
			if (numVert != 0) {
				m_pPolyArray[numFace].vertexCount = 3;
				m_pPolyArray[numFace].edges = new unsigned int[3];
				m_pPolyArray[numFace].vertices = new unsigned int[3];
				m_pPolyArray[numFace].mat = 0;
				m_pPolyArray[numFace].selected = true;

				m_pPolyArray[numFace].vertices[0] = v1 - 1;
				m_pPolyArray[numFace].vertices[1] = v2 - 1;
				m_pPolyArray[numFace].vertices[2] = v3 - 1;

				int e1 = FindEdge(v1-1, v2-1);
				if (e1 == -1) {
					PAE3D_Edge e;
					e.v1 = v1-1;
					e.v2 = v2-1;
					e.selected = true;
					e1 = m_nNumEdge;
					AddEdge(e);
				}
				int e2 = FindEdge(v2-1, v3-1);
				if (e2 == -1) {
					PAE3D_Edge e;
					e.v1 = v2-1;
					e.v2 = v3-1;
					e.selected = true;
					e2 = m_nNumEdge;
					AddEdge(e);
				}
				int e3 = FindEdge(v3-1, v1-1);
				if (e3 == -1) {
					PAE3D_Edge e;
					e.v1 = v3-1;
					e.v2 = v1-1;
					e.selected = true;
					e3 = m_nNumEdge;
					AddEdge(e);
				}
				m_pPolyArray[numFace].edges[0] = e1;
				m_pPolyArray[numFace].edges[1] = e2;
				m_pPolyArray[numFace].edges[2] = e3;
			}

			numFace++;
			break;
		default:
			break;
		}
	}

	fclose(fp);

	for (int i = 0; i < m_nNumPolygon; i++) {
		m_pPolyArray[i].n = PolyNormal(i);
	}

	/*for (int i = 0; i < m_nNumPoint; i++) {
		printf("%d\n", i);
		CalculateNormal(i);
	}*/

	PAE3D_Edge* temp = new PAE3D_Edge[m_nNumEdge];
	for (int i = 0; i < m_nNumEdge; i++) {
		temp[i] = m_pEdgeArray[i];
	}
	delete(m_pEdgeArray);
	m_pEdgeArray = temp;
	m_nListEdge = m_nNumEdge;
	AssignIntermediatePointers();
	for (int i = 0; i < m_nNumPoint; i++) {
		CalculateNormal(i);
	}
	printf("Reading OBJ file is DONE.\n");
	
} /* ReadOBJ */

/******************************************************************************/
/*  Refreshes face edges                                         */
/******************************************************************************/
void Model::AssignIntermediatePointers() {
	
	//rename function?
	
	for (int i = 0; i < m_nNumEdge; i++) {
		m_pEdgeArray[i].hasPoly1= false;
		m_pEdgeArray[i].hasPoly2= false;
	}

	// points each edge to the neighboring polygons
	for (int i = 0; i < m_nNumPolygon; i++) {
		
		//create new polygon edge array of vertex count size
		m_pPolyArray[i].edges = new unsigned int[m_pPolyArray[i].vertexCount];
		
		/* foreach polygon, get vertex and the next vertex find edge they make. 
		 * Assign found edges to the polygon. 
		 */
		for (int j = 0; j < m_pPolyArray[i].vertexCount; j++) {
			
			int v1 = m_pPolyArray[i].vertices[j];
			int v2 = m_pPolyArray[i].vertices[(j+1) % m_pPolyArray[i].vertexCount];
			// with modulo will also get last and first vertex pair 
			
			//cout << "| " << j << " | " << j+1 << " % " <<  m_pPolyArray[i].vertexCount << " = " << (j+1) % m_pPolyArray[i].vertexCount << " ";
			
			// Fix this, FindEdge() takes far to long
			int edge = FindEdge(v1, v2);
			m_pPolyArray[i].edges[j] = edge;
			
			
			if (m_pEdgeArray[edge].hasPoly1) {
				// if found edge already has a polygon assigned, assign this poly index to second slot
				m_pEdgeArray[edge].poly2 = i;
				m_pEdgeArray[edge].hasPoly2 = true;
			}
			else {
				//if edge has nopolygon assigned, assign this poly index
				m_pEdgeArray[edge].poly1 = i;
				m_pEdgeArray[edge].hasPoly1 = true;
			}
		}
	}


	//refresh vertex references to edges
	for (int i = 0 ; i < m_nNumPoint; i++) {
		m_pVertexArray[i].edgeCount = 0;
	}

	// points each vertex to the neighboring edges
	for (int i = 0; i < m_nNumEdge; i++) {
		
		//edge v1
		int v1 = m_pEdgeArray[i].v1;
		if (m_pVertexArray[v1].edgeCount == 0) {
			m_pVertexArray[v1].edges = new unsigned int[1];
		}
		else {
			unsigned int* old1 = m_pVertexArray[v1].edges;
			m_pVertexArray[v1].edges = new unsigned int[m_pVertexArray[v1].edgeCount+1];
			for (int j = 0; j < m_pVertexArray[v1].edgeCount; j++) {
				m_pVertexArray[v1].edges[j] = old1[j];
			}
			delete (old1);
		}
		m_pVertexArray[v1].edges[m_pVertexArray[v1].edgeCount] = i;
		m_pVertexArray[v1].edgeCount++;

		//edge v2
		int v2 = m_pEdgeArray[i].v2;
		if (m_pVertexArray[v2].edgeCount == 0) {
			m_pVertexArray[v2].edges = new unsigned int[1];
		}
		else {
			unsigned int* old2 = m_pVertexArray[v2].edges;
			m_pVertexArray[v2].edges = new unsigned int[m_pVertexArray[v2].edgeCount+1];
			for(int j = 0; j < m_pVertexArray[v2].edgeCount; j++) {
				m_pVertexArray[v2].edges[j] = old2[j];
			}
			delete(old2);
		}
		m_pVertexArray[v2].edges[m_pVertexArray[v2].edgeCount] = i;
		m_pVertexArray[v2].edgeCount++;
	}
} /* AssignIntermediatePointers */


/******************************************************************************/
/*  Model destructor. Frees up memory.									  	  */
/******************************************************************************/
Model::~Model(void) {
	if (m_pVertexArray != NULL)
		delete[] m_pVertexArray;
	if (m_pEdgeArray != NULL)
		delete[] m_pEdgeArray;
	if (m_pPolyArray != NULL)
		delete[] m_pPolyArray;
} /* ~Model */


/******************************************************************************/
/*  Calculate face and return normal								  		  */
/******************************************************************************/
PAE3D_Normal Model::PolyNormal(int p) {
	PAE3D_Polygon poly = m_pPolyArray[p];
	PAE3D_Normal normal;
	normal.x = 0;
	normal.y = 0;
	normal.z = 0;
	for (int i = 0; i < poly.vertexCount; i++) {
		PAE3D_Point p1 = m_pVertexArray[poly.vertices[i]];
		PAE3D_Point p2 = m_pVertexArray[poly.vertices[(i+1)%poly.vertexCount]];
		PAE3D_Point p3 = m_pVertexArray[poly.vertices[(i+2)%poly.vertexCount]];
		PAE3D_Point v, u;
		v.x = p2.x - p1.x;
		v.y = p2.y - p1.y;
		v.z = p2.z - p1.z;
		u.x = p3.x - p1.x;
		u.y = p3.y - p1.y;
		u.z = p3.z - p1.z;
		PAE3D_Normal n;
		n.x = v.y * u.z - v.z * u.y;
		n.y = v.z * u.x - v.x * u.z;
		n.z = v.x * u.y - v.y * u.x;
		float m = sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
		if (m > 0) {
			n.x /= m;
			n.y /= m;
			n.z /= m;
			normal.x += n.x;
			normal.y += n.y;
			normal.z += n.z;
		}
	}
	float m = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
	normal.x /= m;
	normal.y /= m;
	normal.z /= m;
	return normal;
	
} /* PolyNormal */

/******************************************************************************/
/*  Calculate face center postion		  							  		  */
/******************************************************************************/
PAE3D_Point Model::PolyCenter(int p) {
	PAE3D_Polygon poly = m_pPolyArray[p];
	PAE3D_Point center;
	center.x = 0;
	center.y = 0;
	center.z = 0;
	for (int i = 0; i < poly.vertexCount; i++) {
		center.x += m_pVertexArray[poly.vertices[i]].x;
		center.y += m_pVertexArray[poly.vertices[i]].y;
		center.z += m_pVertexArray[poly.vertices[i]].z;
	}
	center.x /= poly.vertexCount;
	center.y /= poly.vertexCount;
	center.z /= poly.vertexCount;
	return center;
} /* PolyCenter */


/******************************************************************************/
/* 	Add edge to model				  								  		  */
/******************************************************************************/
void Model::AddEdge(PAE3D_Edge edge) {
	m_nNumEdge++;
	if (m_nNumEdge > m_nListEdge) {
		m_nListEdge = (m_nNumEdge-1) * 2;
		PAE3D_Edge* temp = new PAE3D_Edge[m_nListEdge];
		for (int i = 0; i < m_nNumEdge-1; i++) {
			temp[i] = m_pEdgeArray[i];
		}
		delete(m_pEdgeArray);
		m_pEdgeArray = temp;
	}
	m_pEdgeArray[m_nNumEdge-1] = edge;
} /* AddEdge */


/******************************************************************************/
/* 	Add vertex to model				  								  		  */
/******************************************************************************/
void Model::AddVertex(PAE3D_Point point) {
	m_nNumPoint++;
	if (m_nNumPoint > m_nListPoint) {
		m_nListPoint = (m_nNumPoint - 1) * 2;
		PAE3D_Point* temp = new PAE3D_Point[m_nListPoint];
		for (int i = 0; i < m_nNumPoint - 1; i++) {
			temp[i] = m_pVertexArray[i];
		}
		delete (m_pVertexArray);
		m_pVertexArray = temp;
	}
	m_pVertexArray[m_nNumPoint - 1] = point;
} /* AddVertex */


/******************************************************************************/
/* 	Add face to model				  								  		  */
/******************************************************************************/
void Model::AddPoly(PAE3D_Polygon quad) {
	m_nNumPolygon++;
	if (m_nNumPolygon > m_nListPolygon) {
		m_nListPolygon = (m_nNumPolygon - 1) * 2;
		PAE3D_Polygon* temp = new PAE3D_Polygon[m_nListPolygon];
		for (int i = 0; i < m_nNumPolygon - 1; i++) {
			temp[i] = m_pPolyArray[i];
		}
		delete (m_pPolyArray);
		m_pPolyArray = temp;
	}
	m_pPolyArray[m_nNumPolygon - 1] = quad;
} /* AddPoly */


/******************************************************************************/
/* 	Delete vertex from model				  								  */
/******************************************************************************/
void Model::DeleteVertex(int index){
	//shift down del.
	for(int i = index; i < m_nNumPoint-1;i++){
		m_pVertexArray[i] = m_pVertexArray[i+1];
	}
	//decrement egde references
	for (int i = 0; i < m_nNumEdge; i++) {
		if(m_pEdgeArray[i].v1 > index){
			m_pEdgeArray[i].v1 = m_pEdgeArray[i].v1-1;
		}
		if(m_pEdgeArray[i].v2 > index){
			m_pEdgeArray[i].v2 = m_pEdgeArray[i].v2-1;
		}
	}
	////decrement face references
	for (int i = 0; i < m_nNumPolygon; i++) {
		int count = 0;
		for (int k = 0; k < m_pPolyArray[i].vertexCount; k++){
			if(m_pPolyArray[i].vertices[k] == index){
				for(int j = k; j < m_pPolyArray[i].vertexCount-1; j++){
					m_pPolyArray[i].vertices[j] = m_pPolyArray[i].vertices[j+1];
				}
				count++;
			}
		}
		for (int k = 0; k < m_pPolyArray[i].vertexCount; k++){
			if(m_pPolyArray[i].vertices[k] > index){
				m_pPolyArray[i].vertices[k] = m_pPolyArray[i].vertices[k]-1;
			}
		}
		m_pPolyArray[i].vertexCount = m_pPolyArray[i].vertexCount - count;
	}

	m_nNumPoint = m_nNumPoint -1;
} /* DeleteVertex */


/******************************************************************************/
/* 	Delete edge from model				  									  */
/******************************************************************************/
void Model::DeleteEdge(int index) {

	for (int i = index; i < m_nNumEdge - 1; i++) {
		m_pEdgeArray[i] = m_pEdgeArray[i + 1];
	}

	for (int i = 0; i < m_nNumPolygon; i++) {

		for (int k = 0; k < m_pPolyArray[i].vertexCount; k++) {
			if (m_pPolyArray[i].edges[k] == index) {
				for (int j = k; j < m_pPolyArray[i].vertexCount - 1; j++) {
					m_pPolyArray[i].edges[j] = m_pPolyArray[i].edges[j+1];
				}
			}
		}
		for (int k = 0; k < m_pPolyArray[i].vertexCount; k++) {
			if (m_pPolyArray[i].edges[k] > index) {
				m_pPolyArray[i].edges[k] = m_pPolyArray[i].edges[k] - 1;
			}
		}
}
m_nNumEdge = m_nNumEdge-1;
}
void Model::DeletePoly(int index){
	for(int i = index; i < m_nNumPolygon-1;i++){
		m_pPolyArray[i] = m_pPolyArray[i+1];
	}
	m_nNumPolygon = m_nNumPolygon -1;
} /* DeleteEdge */


/******************************************************************************/
/* 	Provides both the subdivide operation and smooth operation				  */
/******************************************************************************/
void Model::Subdivide(bool smooth) {
	
	bool timedebug = false;
	// select all if none selected
	bool nofaces = true;
	
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			nofaces = false;
		}
	}
	if (nofaces) {
		SelectAll();
	}
	time_t start, end;
	if (timedebug) {
		printf("\n\nsmooth: %d\n", m_nNumPolygon);
		time(&start);
	}

	unsigned int points = m_nNumPoint;
	unsigned int edges = m_nNumEdge;
	unsigned int polys = m_nNumPolygon;

	// finding face points
	// adds new vertex at center
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			m_pPolyArray[i].c = m_nNumPoint;
			PAE3D_Point p = PolyCenter(i);
			p.selected = true;
			p.edgeCount = 0;
			p.edges = NULL;
			AddVertex(p);
		}
	}
	if (timedebug) {
		time(&end);
		printf("finding face points: %f\n", difftime(start, end));
		time(&start);
	}
	// finding edge points
	// adds vertices at center of edges (subdivide) or calcuated smooth center
	for (int i = 0; i < m_nNumEdge; i++) {
		if (m_pEdgeArray[i].selected) {
			if (smooth) {
				PAE3D_Point v1 = m_pVertexArray[m_pEdgeArray[i].v1];
				PAE3D_Point v2 = m_pVertexArray[m_pEdgeArray[i].v2];
				PAE3D_Point p;
				p.x = v1.x + v2.x;
				p.y = v1.y + v2.y;
				p.z = v1.z + v2.z;
				int count = 2;
				if (m_pEdgeArray[i].hasPoly1 && m_pPolyArray[m_pEdgeArray[i].poly1].selected) {
					PAE3D_Point v3 = m_pVertexArray[m_pPolyArray[m_pEdgeArray[i].poly1].c];
					p.x += v3.x;
					p.y += v3.y;
					p.z += v3.z;
					count++;
				}
				if (m_pEdgeArray[i].hasPoly2 && m_pPolyArray[m_pEdgeArray[i].poly2].selected) {
					PAE3D_Point v3 = m_pVertexArray[m_pPolyArray[m_pEdgeArray[i].poly2].c];
					p.x += v3.x;
					p.y += v3.y;
					p.z += v3.z;
					count++;
				}
				p.x /= count;
				p.y /= count;
				p.z /= count;
				p.edgeCount = 0;
				p.selected = true;
				p.edges = new unsigned int[4];
				m_pEdgeArray[i].c = m_nNumPoint;
				AddVertex(p);
			} else {
				PAE3D_Point v1 = m_pVertexArray[m_pEdgeArray[i].v1];
				PAE3D_Point v2 = m_pVertexArray[m_pEdgeArray[i].v2];
				PAE3D_Point p;
				p.x = (v1.x + v2.x) / 2;
				p.y = (v1.y + v2.y) / 2;
				p.z = (v1.z + v2.z) / 2;
				p.edgeCount = 0;
				p.edges = new unsigned int[4];
				p.selected = true;
				m_pEdgeArray[i].c = m_nNumPoint;
				AddVertex(p);
			}
		}
	}
	if (timedebug) {
		time(&end);
		printf("finding edge points: %f\n", difftime(start, end));
		time(&start);
	}
	
	// joining face points to edge points
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			for (int j = 0; j < m_pPolyArray[i].vertexCount; j++) {
				PAE3D_Edge e;
				e.v1 = m_pPolyArray[i].c;
				e.v2 = m_pEdgeArray[m_pPolyArray[i].edges[j]].c;
				e.hasPoly1 = false;
				e.hasPoly2 = false;
				e.selected = true;
				if (m_pVertexArray[m_pPolyArray[i].c].edgeCount == 0) {
					m_pVertexArray[m_pPolyArray[i].c].edgeCount = 1;
					m_pVertexArray[m_pPolyArray[i].c].edges = new unsigned int[1];
				} else {
					m_pVertexArray[m_pPolyArray[i].c].edgeCount++;
					unsigned int* temp = new unsigned int[m_pVertexArray[m_pPolyArray[i].c].edgeCount];
					for (int k = 0; k < m_pVertexArray[m_pPolyArray[i].c].edgeCount-1; k++) {
						temp[k] = m_pVertexArray[m_pPolyArray[i].c].edges[k];
					}
					delete(m_pVertexArray[m_pPolyArray[i].c].edges);
					m_pVertexArray[m_pPolyArray[i].c].edges = temp;
				}
				m_pVertexArray[m_pPolyArray[i].c].edges[m_pVertexArray[m_pPolyArray[i].c].edgeCount-1] = m_nNumEdge;

				m_pVertexArray[m_pEdgeArray[m_pPolyArray[i].edges[j]].c].edges[m_pVertexArray[m_pEdgeArray[m_pPolyArray[i].edges[j]].c].edgeCount] = m_nNumEdge;
				m_pVertexArray[m_pEdgeArray[m_pPolyArray[i].edges[j]].c].edgeCount++;

				AddEdge(e);
			}
		}
	}
	if (timedebug) {
		time(&end);
		printf("joining face points to edge points: %f\n", difftime(start, end));
		time(&start);
	}

	// adjusting original point positions
	if (smooth) {
		for (unsigned int i = 0; i < points; i++) {
			PAE3D_Point f;
			f.x = 0;
			f.y = 0;
			f.z = 0;
			int neighboringPolys = 0;
			for (int j = 0; j < m_nNumPolygon; j++) {
				bool touches = false;
				for (int k = 0; k < m_pPolyArray[j].vertexCount; k++) {
					if (m_pPolyArray[j].vertices[k] == i) {
						touches = true;
					}
				}
				if (touches) {
					PAE3D_Point c = PolyCenter(j);
					f.x += c.x;
					f.y += c.y;
					f.z += c.z;
					neighboringPolys++;
				}
			}
			if (neighboringPolys == 0) {
				continue;
			}
			f.x /= neighboringPolys;
			f.y /= neighboringPolys;
			f.z /= neighboringPolys;
			PAE3D_Point r;
			r.x = 0;
			r.y = 0;
			r.z = 0;
			int neighboringEdges = 0;
			for (int j = 0; j < m_nNumEdge; j++) {
				if (m_pEdgeArray[j].selected) {
					if (m_pEdgeArray[j].v1 == i || m_pEdgeArray[j].v2 == i) {
						r.x += m_pVertexArray[m_pEdgeArray[j].c].x;
						r.y += m_pVertexArray[m_pEdgeArray[j].c].y;
						r.z += m_pVertexArray[m_pEdgeArray[j].c].z;
						neighboringEdges++;
					}
				}
			}
			if (neighboringEdges == 0) {
				continue;
			}
			r.x /= neighboringEdges;
			r.y /= neighboringEdges;
			r.z /= neighboringEdges;
			int n = neighboringPolys;
			float w1 = (n - 2.0);
			float w2 = 1.0;
			float w3 = 1.0;
			m_pVertexArray[i].x = (w3 * f.x + w2 * r.x + w1 * m_pVertexArray[i].x) / n;
			m_pVertexArray[i].y = (w3 * f.y + w2 * r.y + w1 * m_pVertexArray[i].y) / n;
			m_pVertexArray[i].z = (w3 * f.z + w2 * r.z + w1 * m_pVertexArray[i].z) / n;
		}
	}
	if (timedebug) {
		time(&start);
		printf("adjusting original point positions: %f\n", difftime(start, end));
		time(&start);
	}
	// splitting original edges in half
	for (unsigned int i = 0; i < edges; i++) {
		if (m_pEdgeArray[i].selected) {
			int other = -1;
			if (m_pEdgeArray[i].hasPoly1
					&& !m_pPolyArray[m_pEdgeArray[i].poly1].selected) {
				other = m_pEdgeArray[i].poly1;
			} else if (m_pEdgeArray[i].hasPoly2
					&& !m_pPolyArray[m_pEdgeArray[i].poly2].selected) {
				other = m_pEdgeArray[i].poly2;
			}
			PAE3D_Edge e;
			e.v1 = m_pEdgeArray[i].c;
			e.v2 = m_pEdgeArray[i].v2;
			e.hasPoly1 = false;
			e.hasPoly2 = false;
			e.selected = true;
			int c = m_pEdgeArray[i].c;
			e.c = m_pEdgeArray[i].c;
			m_pEdgeArray[i].nextEdge = m_nNumEdge;
			m_pVertexArray[m_pEdgeArray[i].c].edges[m_pVertexArray[m_pEdgeArray[i].c].edgeCount] = i;
			m_pVertexArray[m_pEdgeArray[i].c].edgeCount++;

			m_pVertexArray[m_pEdgeArray[i].c].edges[m_pVertexArray[m_pEdgeArray[i].c].edgeCount] = m_nNumEdge;
			m_pVertexArray[m_pEdgeArray[i].c].edgeCount++;

			for (int j = 0; j < m_pVertexArray[m_pEdgeArray[i].v2].edgeCount; j++) {
				if (m_pVertexArray[m_pEdgeArray[i].v2].edges[j] == i) {
					m_pVertexArray[m_pEdgeArray[i].v2].edges[j] = m_nNumEdge;
				}
			}

			AddEdge(e);
			m_pEdgeArray[i].v2 = m_pEdgeArray[i].c;
			m_pEdgeArray[i].hasPoly1 = false;
			m_pEdgeArray[i].hasPoly2 = false;
			// fixing adding c to adjacent polygon
			if (other != -1) {
				m_pPolyArray[other].vertexCount++;
				unsigned int* oldVerts = m_pPolyArray[other].vertices;
				unsigned int* oldEdges = m_pPolyArray[other].edges;
				m_pPolyArray[other].vertices =
						new unsigned int[m_pPolyArray[other].vertexCount];
				m_pPolyArray[other].edges =
						new unsigned int[m_pPolyArray[other].vertexCount];
				int copied = 0;
				for (int j = 0; j < m_pPolyArray[other].vertexCount - 1; j++) {
					m_pPolyArray[other].vertices[j + copied] = oldVerts[j];
					if (-1 == FindEdge(oldVerts[j],	oldVerts[(j+1)%(m_pPolyArray[other].vertexCount-1)])) {
						m_pPolyArray[other].edges[j + copied] = FindEdge(oldVerts[j], c);
						copied++;
						m_pPolyArray[other].vertices[j + copied] = c;
						m_pPolyArray[other].edges[j + copied] =	FindEdge(oldVerts[(j+1)%(m_pPolyArray[other].vertexCount-1)], c);
					} else {
						m_pPolyArray[other].edges[j + copied] = oldEdges[j];
					}
				}
			}
		}
	}
	if (timedebug) {
		time(&start);
		printf("splitting original edges in half: %f\n", difftime(start, end));
		time(&start);
	}
	// creating new faces
	for (unsigned int i = 0; i < polys; i++) {
		if (m_pPolyArray[i].selected) {
			unsigned int* oldVertices = m_pPolyArray[i].vertices;
			unsigned int* oldEdges = m_pPolyArray[i].edges;
			int oldVertexCount = m_pPolyArray[i].vertexCount;
			m_pPolyArray[i].vertexCount = 4;
			m_pPolyArray[i].vertices = new unsigned int[4];
			m_pPolyArray[i].vertices[0] = oldVertices[0];
			m_pPolyArray[i].vertices[1] = m_pEdgeArray[oldEdges[0]].c;
			m_pPolyArray[i].vertices[2] = m_pPolyArray[i].c;
			m_pPolyArray[i].vertices[3] = m_pEdgeArray[oldEdges[oldVertexCount-1]].c;
			m_pPolyArray[i].edges = new unsigned int[4];

			if (m_pEdgeArray[oldEdges[0]].v1 == oldVertices[0] || m_pEdgeArray[oldEdges[0]].v2 == oldVertices[0])
				m_pPolyArray[i].edges[0] = oldEdges[0];
			else
				m_pPolyArray[i].edges[0] = m_pEdgeArray[oldEdges[0]].nextEdge;

			for (int j = 0; j < m_pVertexArray[m_pPolyArray[i].c].edgeCount; j++) {
				if (m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[j]].v1 == m_pEdgeArray[oldEdges[0]].c ||
						m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[j]].v2 == m_pEdgeArray[oldEdges[0]].c) {
					m_pPolyArray[i].edges[1] = m_pVertexArray[m_pPolyArray[i].c].edges[j];
				}
			}

			for (int j = 0; j < m_pVertexArray[m_pPolyArray[i].c].edgeCount; j++) {
				if (m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[j]].v1 == m_pEdgeArray[oldEdges[oldVertexCount-1]].c ||
						m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[j]].v2 == m_pEdgeArray[oldEdges[oldVertexCount-1]].c) {
					m_pPolyArray[i].edges[2] = m_pVertexArray[m_pPolyArray[i].c].edges[j];
				}
			}

			if (m_pEdgeArray[oldEdges[oldVertexCount-1]].v1 == oldVertices[0] || m_pEdgeArray[oldEdges[oldVertexCount-1]].v2 == oldVertices[0])
				m_pPolyArray[i].edges[3] = oldEdges[oldVertexCount-1];
			else
				m_pPolyArray[i].edges[3] = m_pEdgeArray[oldEdges[oldVertexCount-1]].nextEdge;

			for (int j = 1; j < oldVertexCount; j++) {
				PAE3D_Polygon poly2;
				poly2.vertexCount = 4;
				poly2.vertices = new unsigned int[4];
				poly2.vertices[0] = oldVertices[j];
				poly2.vertices[1] = m_pEdgeArray[oldEdges[j]].c;
				poly2.vertices[2] = m_pPolyArray[i].c;
				poly2.vertices[3] = m_pEdgeArray[oldEdges[j-1]].c;
				poly2.selected = m_pPolyArray[i].selected;
				poly2.edges = new unsigned int[4];

				if (m_pEdgeArray[oldEdges[j]].v1 == oldVertices[j] || m_pEdgeArray[oldEdges[j]].v2 == oldVertices[j])
					poly2.edges[0] = oldEdges[j];
				else
					poly2.edges[0] = m_pEdgeArray[oldEdges[j]].nextEdge;
				for (int k = 0; k < m_pVertexArray[m_pPolyArray[i].c].edgeCount; k++) {
					if (m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[k]].v1 == m_pEdgeArray[oldEdges[j]].c ||
							m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[k]].v2 == m_pEdgeArray[oldEdges[j]].c) {
						poly2.edges[1] = m_pVertexArray[m_pPolyArray[i].c].edges[k];
					}
				}
				for (int k = 0; k < m_pVertexArray[m_pPolyArray[i].c].edgeCount; k++) {
					if (m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[k]].v1 == m_pEdgeArray[oldEdges[j-1]].c ||
							m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].c].edges[k]].v2 == m_pEdgeArray[oldEdges[j-1]].c) {
						poly2.edges[2] = m_pVertexArray[m_pPolyArray[i].c].edges[k];
					}
				}
				if ((m_pEdgeArray[oldEdges[j-1]].v1 == oldVertices[j] && m_pEdgeArray[oldEdges[j-1]].v2 == m_pEdgeArray[oldEdges[j-1]].c) ||
						(m_pEdgeArray[oldEdges[j-1]].v2 == oldVertices[j] && m_pEdgeArray[oldEdges[j-1]].v1 == m_pEdgeArray[oldEdges[j-1]].c))
					poly2.edges[3] = oldEdges[j-1];
				else
					poly2.edges[3] = m_pEdgeArray[oldEdges[j-1]].nextEdge;

				poly2.c = 0;
				poly2.mat = m_pPolyArray[i].mat;
				poly2.selected = true;
				AddPoly(poly2);
				m_pPolyArray[m_nNumPolygon - 1].n = PolyNormal(
						m_nNumPolygon - 1);
			}
			delete (oldVertices);
			delete (oldEdges);
		}
	}
	if (timedebug) {
		time(&start);
		printf("creating new faces: %f\n", difftime(start, end));
		time(&start);
	}
	// clean dependencies
	for (int i = 0; i < m_nNumEdge; i++) {
		m_pEdgeArray[i].hasPoly1 = false;
		m_pEdgeArray[i].hasPoly2 = false;
	}
	for (int i = 0; i < m_nNumPolygon; i ++) {
		for (int j = 0; j < m_pPolyArray[i].vertexCount; j++) {
			for (int k = 0; k < m_pVertexArray[m_pPolyArray[i].vertices[j]].edgeCount; k++) {
				bool yes = false;
				if (m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].v1 == m_pPolyArray[i].vertices[j]) {
					for (int l = 0; l < m_pPolyArray[i].vertexCount; l++) {
						if (m_pPolyArray[i].vertices[l] == m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].v2) {
							yes = true;
							break;
						}
					}
				} else if (m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].v2 == m_pPolyArray[i].vertices[j]) {
					for (int l = 0; l < m_pPolyArray[i].vertexCount; l++) {
						if (m_pPolyArray[i].vertices[l] == m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].v1) {
							yes = true;
							break;
						}
					}
				}
				if (yes) {
					if (m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].hasPoly1) {
						m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].poly2 = i;
						m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].hasPoly2 = true;
					} else {
						m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].poly1 = i;
						m_pEdgeArray[m_pVertexArray[m_pPolyArray[i].vertices[j]].edges[k]].hasPoly1 = true;
					}
				}
			}
		}
	}
	if (timedebug ) {
		time(&start);
		printf("clean dependencies: %f\n", difftime(start, end));
		time(&start);
	}
	// compute normals
	for (int i = 0; i < m_nNumPoint; i++) {
		Sort(m_pVertexArray[i].edges, m_pVertexArray[i].edgeCount);
	}

	for (int i = 0; i < m_nNumPolygon; i++) {
		m_pPolyArray[i].n = PolyNormal(i);
	}
	for (int i = 0; i < m_nNumPoint; i++) {
		CalculateNormal(i);
	}
	if (timedebug) {
		printf("compute normals: %f\n", difftime(start, end));
		printf("done: %d\n\n\n", m_nNumPolygon);
	}

	if (nofaces) {
		DeselectEverything();
	}
} /* Subdivide */


/******************************************************************************/
/* 	Sorts vertex edges indices			  									  */
/******************************************************************************/
void Sort(unsigned int* num, int numLength) {
	int i, j, first, temp;
	for (i= numLength - 1; i > 0; i--)
		{
		first = 0;                 // initialize to subscript of first element
		for (j=1; j<=i; j++) {   // locate smallest between positions 1 and i.
			if (num[j] < num[first])
				first = j;
		}
		temp = num[first];   // Swap smallest found with element in position i.
		num[first] = num[i];
		num[i] = temp;
	}
} /*  */

/******************************************************************************/
/* 	Move selected based on mouse movement 	  								  */
/******************************************************************************/
void Model::SelectAll() {
	m_hasSelected = true;
	for (int i = 0 ; i < m_nNumPoint; i++) {
		m_pVertexArray[i].selected = true;
	}
	for (int i = 0 ; i < m_nNumPolygon; i++) {
		m_pPolyArray[i].selected = true;
	}
	for (int i = 0 ; i < m_nNumEdge; i++) {
		m_pEdgeArray[i].selected = true;
	}
	ResetHandlePosition();
}

/******************************************************************************/
/* 	Move selected based on mouse movement 	  								  */
/******************************************************************************/
void Model::MoveSelected(float dx, float dy, float dz) {
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {
			m_pVertexArray[i].x += dx;
			m_pVertexArray[i].y += dy;
			m_pVertexArray[i].z += dz;
		}
	}
	m_selectedCenter.x += dx;
	m_selectedCenter.y += dy;
	m_selectedCenter.z += dz;
	for (int i = 0; i < m_nNumPolygon; i++) {
		m_pPolyArray[i].n = PolyNormal(i);
	}
	for (int i = 0; i < m_nNumPoint; i++) {
		CalculateNormal(i);
	}
} /* MoveSelected */


/******************************************************************************/
/* 	Scale selected based on mouse movement 	  								  */
/******************************************************************************/
void Model::ScaleSelected(float dx, float dy, float dz) {
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {
			m_pVertexArray[i].x += dx * (m_pVertexArray[i].x - m_selectedCenter.x);
			m_pVertexArray[i].y += dy * (m_pVertexArray[i].y - m_selectedCenter.y);
			m_pVertexArray[i].z += dz * (m_pVertexArray[i].z - m_selectedCenter.z);
		}
	}
	for (int i = 0; i < m_nNumPolygon; i++) {
		m_pPolyArray[i].n = PolyNormal(i);
	}
	for (int i = 0; i < m_nNumPoint; i++) {
		CalculateNormal(i);
	}
} /* ScaleSelected */


/******************************************************************************/
/* 	Calculates normal for a vertex			  								  */
/******************************************************************************/
void Model::CalculateNormal(int id) {
	PAE3D_Normal n;
	n.x = 0;
	n.y = 0;
	n.z = 0;
	int count = 0;
	for (int i = 0; i < m_pVertexArray[id].edgeCount; i++) {
		int edge = m_pVertexArray[id].edges[i];
		if (m_pEdgeArray[edge].hasPoly1) {
			PAE3D_Normal n1 = m_pPolyArray[m_pEdgeArray[edge].poly1].n;
			n.x += n1.x;
			n.y += n1.y;
			n.z += n1.z;
			count++;
		}
		if (m_pEdgeArray[edge].hasPoly2) {
			PAE3D_Normal n2 = m_pPolyArray[m_pEdgeArray[edge].poly2].n;
			n.x += n2.x;
			n.y += n2.y;
			n.z += n2.z;
			count++;
		}
	}

	n.x /= count;
	n.y /= count;
	n.z /= count;
	m_pVertexArray[id].n = n;
	
} /* CalculateNormal */

/******************************************************************************/
/* 	Searches for edge using two vertices			  						  */
/******************************************************************************/
int Model::FindEdge(unsigned int v1, unsigned  int v2) {
	for (int i = 0; i < m_nNumEdge; i++) {
		if ((m_pEdgeArray[i].v1 == v1 && m_pEdgeArray[i].v2 == v2)
				|| (m_pEdgeArray[i].v2 == v1 && m_pEdgeArray[i].v1 == v2)) {
			return i;
		}
	}
	return -1;
} /* FindEdge */

/******************************************************************************/
/* 	Renders vertices							  							  */
/******************************************************************************/
void Model::RenderVertices(float zoom) {
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	for (int i = 0; i < m_nNumPoint; i++) {
		float radius = 1;
		int divs = 4;
		if(picking){
			int id = i + PAE3D_COLORPADDING;
			int r = (id & 0x000000FF) >> 0;
			int g = (id & 0x0000FF00) >> 8;
			int b = (id & 0x00FF0000) >> 16;
			glColor3f(r/255.0, g/255.0, b/255.0);
			radius = 10;
			divs = 10;
		}
		else if (m_hasSelected && m_pVertexArray[i].selected) {
			glColor3f(1.0, 0.0, 0.0);
		}
		else {
			glColor3f(0, 0, 0);
		}
		PAE3D_Point p = m_pVertexArray[i];
		glPushMatrix();
		glTranslatef(p.x, p.y, p.z);
		glutSolidSphere(radius/height * zoom, divs, divs);
		glPopMatrix();

	}
} /* RenderVertices */

/******************************************************************************/
/* 	Renders edges							  								  */
/******************************************************************************/
void Model::RenderEdges(float zoom) {
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	for (int i = 0; i < m_nNumEdge; i++) {
		if (picking) {
			int id = i + PAE3D_COLORPADDING;
			int r = (id & 0x000000FF) >> 0;
			int g = (id & 0x0000FF00) >> 8;
			int b = (id & 0x00FF0000) >> 16;
			glColor3f(r / 255.0, g / 255.0, b / 255.0);
			glLineWidth(6);
		} else if (m_hasSelected && m_pEdgeArray[i].selected) {
			glColor3f(1.0, 0.0, 0.0);
			glLineWidth(2);
		} else {
			glColor3f(0, 0, 0);
			glLineWidth(2);
		}
		PAE3D_Edge edge = m_pEdgeArray[i];
		PAE3D_Point p1 = m_pVertexArray[edge.v1];
		PAE3D_Point p2 = m_pVertexArray[edge.v2];
		glBegin(GL_LINES);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glEnd();
	}
} /* RenderEdges */

/******************************************************************************/
/* 	Renders faces							  								  */
/******************************************************************************/
void Model::RenderFaces(Color* cols, bool phong) {
	for (int i = 0; i < m_nNumPolygon; i++) {
		
		PAE3D_Polygon poly = m_pPolyArray[i];
		PAE3D_Material* mat = cols->GetMaterial(poly.mat);
		if(picking){	
			int id = i + PAE3D_COLORPADDING;
			int r = (id & 0x000000FF) >> 0;
			int g = (id & 0x0000FF00) >> 8;
			int b = (id & 0x00FF0000) >> 16;
			glColor3f(r/255.0, g/255.0, b/255.0);
		}
		else if (m_hasSelected && poly.selected) {
			glColor3f(0.5, 0.3, 0.5);
		}
		else {
			glColor4f(mat->col.r, mat->col.g, mat->col.b, mat->col.a);
		}
		if (phong) {
			// cube mapping
			/*glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);//_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			//glBindTexture(GL_TEXTURE_2D, 1);
			glEnable(GL_TEXTURE_CUBE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);*/

			GLfloat* ambient = new GLfloat[4];
			ambient[0] = cols->GetMaterial(m_pPolyArray[i].mat)->col.r*0.2;
			ambient[1] = cols->GetMaterial(m_pPolyArray[i].mat)->col.g*0.2;
			ambient[2] = cols->GetMaterial(m_pPolyArray[i].mat)->col.b*0.2;
			ambient[3] = 1.0;
			GLfloat* diffuse = new GLfloat[4];
			diffuse[0] = cols->GetMaterial(m_pPolyArray[i].mat)->col.r*0.8;
			diffuse[1] = cols->GetMaterial(m_pPolyArray[i].mat)->col.g*0.8;
			diffuse[2] = cols->GetMaterial(m_pPolyArray[i].mat)->col.b*0.8;
			diffuse[3] = 1.0;
			GLfloat* specular = new GLfloat[4];
			specular[0] = cols->GetMaterial(m_pPolyArray[i].mat)->specular;
			specular[1] = cols->GetMaterial(m_pPolyArray[i].mat)->specular;
			specular[2] = cols->GetMaterial(m_pPolyArray[i].mat)->specular;
			specular[3] = 1;
			float shininess = cols->GetMaterial(m_pPolyArray[i].mat)->shininess +1;

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &shininess);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

			delete(ambient);
			delete(diffuse);
			delete(specular);
		} else {
			GLfloat* specular = new GLfloat[4];
			specular[0] = 0;
			specular[1] = 0;
			specular[2] = 0;
			specular[3] = 1;
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
			glNormal3f(poly.n.x, poly.n.y, poly.n.z);
		}
		glBegin(GL_POLYGON);
		for(int j = 0 ; j < poly.vertexCount; j++) {
			PAE3D_Point p = m_pVertexArray[poly.vertices[j]];
			if (phong) {

				glNormal3f(p.n.x, p.n.y, p.n.z);
			}
			glVertex3f(p.x, p.y, p.z);
		}
		glEnd();
		if (phong) {
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_CUBE_MAP);
		}
		glColor3f(1, 0, 0);
		// RENDER NORMALS
		if (PAE3D_RENDER_NORMALS) {
			glBegin(GL_LINES);
			PAE3D_Point p = PolyCenter(i);
			PAE3D_Normal n = poly.n;
			glVertex3f(p.x, p.y, p.z);
			glVertex3f(p.x + n.x, p.y + n.y, p.z + n.z);
			glEnd();
		}
	}
} /* RenderFaces */


/******************************************************************************/
/* 	Render control when a mouse is clicked.							  		  */
/******************************************************************************/
void Model::RenderPicker(float zoom, int handleMode) {
	
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	picking = true;
	
	// call render based on selection mode
	switch (m_SelectMode) {
		case PAE3D_SELECT_FACES:
			RenderFaces(NULL, false);
			break;
		case PAE3D_SELECT_EDGES:
			RenderEdges(zoom);
			break;
		case PAE3D_SELECT_VERTICES:
			RenderVertices(zoom);
			break;
	}
	
	if (m_hasSelected) {
		
		glClear(GL_DEPTH_BUFFER_BIT);
		float width = 0.5 / height * zoom;
		float length = 20.0 / height * zoom;
		glPushMatrix();
		glTranslatef(m_selectedCenter.x, m_selectedCenter.y, m_selectedCenter.z);
		
		//call handle render
		switch(handleMode){
			case PAE3D_HANDLE_MOVE:
				glColor3f(PAE3D_SELECT_X_HANDLE/255.0, 0, 0);
				Handle::RenderXHandleMove(quadric, width, length);
				glColor3f(PAE3D_SELECT_Y_HANDLE/255.0, 0, 0);
				Handle::RenderYHandleMove(quadric, width, length);
				glColor3f(PAE3D_SELECT_Z_HANDLE/255.0, 0, 0);
				Handle::RenderZHandleMove(quadric, width, length);
				break;
			case PAE3D_HANDLE_SCALE:
			
				glColor3f(PAE3D_SELECT_X_HANDLE / 255.0, 0, 0);
				Handle::RenderXHandleScale(quadric, width, length);
				glColor3f(PAE3D_SELECT_Y_HANDLE / 255.0, 0, 0);
				Handle::RenderYHandleScale(quadric, width, length);
				glColor3f(PAE3D_SELECT_Z_HANDLE / 255.0, 0, 0);
				Handle::RenderZHandleScale(quadric, width, length);
				break;
		}
		glPopMatrix();
	}
	picking = false;
	
} /* RenderPicker */

/******************************************************************************/
/* 	Clear face, edge, and vertex selection									  */
/******************************************************************************/
void Model::DeselectEverything() {
	m_hasSelected = false;
	for (int i = 0; i < m_nNumPolygon; i++) {
		m_pPolyArray[i].selected = false;
	}
	for (int i = 0; i < m_nNumEdge; i++) {
		m_pEdgeArray[i].selected = false;
	}
	for (int i = 0; i < m_nNumPoint; i++) {
		m_pVertexArray[i].selected = false;
	}
} /* DeselectEverything */


/******************************************************************************/
/* 	Processes display mouse click. 											  */
/* 	Called from display function when left or right mouse is clicked. 		  */
/******************************************************************************/
void Model::ProcessSelection(int cursorX, int cursorY, bool shift, bool leftClick, int mat){
	
	/* TODO:  
	* - Shift deselect on face removes selected unconnected edges and vertices. 
	* - Shift deselect on edge removes selected unconnected vertices. 
	*/
	
	SelectedHandle = PAE3D_SELECT_NO_HANDLE;
	
	GLint viewport[4];
	GLubyte pixel[3];

	glGetIntegerv(GL_VIEWPORT,viewport);
	glReadPixels(cursorX, viewport[3]-cursorY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void *)pixel);
	int id = (pixel[0] << 0) + (pixel[1] << 8) + (pixel[2] << 16) - PAE3D_COLORPADDING;
	
	if (leftClick && mat != -1 && id < m_nNumPolygon) {
		m_pPolyArray[id].mat = mat;
	}
	
	switch (m_SelectMode) 
	{
		case PAE3D_SELECT_FACES:
			if (id >=0 && id < m_nNumPolygon && !leftClick) {
				
				//face is already selected
				if (m_pPolyArray[id].selected) {
					if (shift) {
						/*deselects face from selection group*/
						m_pPolyArray[id].selected = false;
						m_hasSelected = false;
						//check if there is still any selected face
						for (int i = 0; i < m_nNumPolygon; i++) {
							if (m_pPolyArray[i].selected) {
								m_hasSelected = true;
								break;
							}
						}
						CascadeFaceSelection(id);
					}
					else {
						/*single select, empties selectiion group*/
						DeselectEverything();
						m_pPolyArray[id].selected = true;
						m_hasSelected = true;
						CascadeFaceSelection(id);
					}
				}
				//face is not selected
				else {
					if (shift) {
						/*adds to selection group*/
						m_pPolyArray[id].selected = true;
						m_hasSelected = true;
						CascadeFaceSelection(id);
					}
					else {
						/*single select, deselecting any group*/
						DeselectEverything();
						m_pPolyArray[id].selected = true;
						m_hasSelected = true;
						CascadeFaceSelection(id);
					}
				}
			}
			break;
		case PAE3D_SELECT_EDGES:
			if (id >= 0 && id < m_nNumEdge && !leftClick) {
				if (m_pEdgeArray[id].selected) {
					if (shift) {
						m_pEdgeArray[id].selected = false;
						m_hasSelected = false;
						for (int i = 0; i < m_nNumEdge; i++) {
							if (m_pEdgeArray[i].selected) {
								m_hasSelected = true;
							break;
							}
						}
						CascadeEdgeSelection(id);
					}
					else {
						for (int i = 0; i < m_nNumEdge; i++) {
							m_pEdgeArray[i].selected = false;
						}
						m_pEdgeArray[id].selected = true;
						m_hasSelected = true;
						CascadeEdgeSelection(id);
					}
				} else {
					if (shift) {
						m_pEdgeArray[id].selected = true;
						m_hasSelected = true;
						CascadeEdgeSelection(id);
					} else {
						for (int i = 0; i < m_nNumEdge; i++) {
							m_pEdgeArray[i].selected = false;
						}
						for (int i = 0; i < m_nNumPoint; i++) {
							m_pVertexArray[i].selected = false;
						}
						m_pEdgeArray[id].selected = true;
						m_hasSelected = true;
						CascadeEdgeSelection(id);
					}
				}
			}
			break;

		case PAE3D_SELECT_VERTICES:
			if (id >= 0 && id < m_nNumPoint && !leftClick) {
				if (m_pVertexArray[id].selected) {
					if (shift) {
						/*deselects face from group*/
						m_pVertexArray[id].selected = false;
						m_hasSelected = false;
						CascadeVertexSelection();
						for (int i = 0; i < m_nNumPoint; i++) {
							if (m_pVertexArray[i].selected) {
								m_hasSelected = true;

							break;
							}
						}
					} else {
						/*single select, deselecting any group*/
						for (int i = 0; i < m_nNumPoint; i++) {
							m_pVertexArray[i].selected = false;
						}
						m_pVertexArray[id].selected = true;
						m_hasSelected = false;
						CascadeVertexSelection();
					}
				} else {
					if (shift) {
						/*adds to selection group*/
						m_pVertexArray[id].selected = true;
						m_hasSelected = true;
						CascadeVertexSelection();
					} else {
						/*single select, deselecting any group*/
						for (int i = 0; i < m_nNumPoint; i++) {
							m_pVertexArray[i].selected = false;
						}
						m_pVertexArray[id].selected = true;
						m_hasSelected = true;
						CascadeVertexSelection();
					}
				}
			}
			break;
	}
	if (id < 0) {
		id += PAE3D_COLORPADDING;
		switch (id) {
		case PAE3D_SELECT_X_HANDLE:
		case PAE3D_SELECT_Y_HANDLE:
		case PAE3D_SELECT_Z_HANDLE:
			SelectedHandle = id;
			break;
		}
	}
	ResetHandlePosition();
	
} /* ProcessSelection */

/******************************************************************************/
/* 	Set Handle position to center of selected vertices						  */
/******************************************************************************/
void Model::ResetHandlePosition(){
	if(m_hasSelected){
		int count = 0;
		m_selectedCenter.x = 0;
		m_selectedCenter.y = 0;
		m_selectedCenter.z = 0;
		for (int i = 0; i < m_nNumPoint; i++) {
			PAE3D_Point p = m_pVertexArray[i];
			if (p.selected) {
				m_selectedCenter.x += p.x;
				m_selectedCenter.y += p.y;
				m_selectedCenter.z += p.z;
				count++;
			}
		}
		m_selectedCenter.x /= count;
		m_selectedCenter.y /= count;
		m_selectedCenter.z /= count;
	}
} /* ResetHandlePosition */ 


/******************************************************************************/
/* 	Re-render function for the handle when selection changes				  */
/******************************************************************************/
void Model::RenderSelectedFacesHandle(float zoom, int handleMode) {
	
	if (m_hasSelected) {
		// only renders handler when there is a selection. 
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		float width = 0.5/height* zoom;
		float length = 20.0/height* zoom;
		glPushMatrix();
		
		switch(handleMode) {
		case PAE3D_HANDLE_MOVE:
			//render xyz handles with move arrow
			glTranslatef(m_selectedCenter.x, m_selectedCenter.y, m_selectedCenter.z);
			glColor3f(1, 0, 0);
			Handle::RenderXHandleMove(quadric, width, length);
			glColor3f(0, 1, 0);
			Handle::RenderYHandleMove(quadric, width, length);
			glColor3f(0, 0, 1);
			Handle::RenderZHandleMove(quadric, width, length);
			break;
		case PAE3D_HANDLE_SCALE:
			//render xyz handles with scale cube
			glTranslatef(m_selectedCenter.x, m_selectedCenter.y, m_selectedCenter.z);
			glColor3f(1, 0, 0);
			Handle::RenderXHandleScale(quadric, width, length);
			glColor3f(0, 1, 0);
			Handle::RenderYHandleScale(quadric, width, length);
			glColor3f(0, 0, 1);
			Handle::RenderZHandleScale(quadric, width, length);
			break;
		}
		
		glPopMatrix();
	}
} /* RenderSelectedFacesHandle */

/******************************************************************************/
/* 	Setter for select mode type												  */
/******************************************************************************/
void Model::SetSelectType(int type){
	m_SelectMode = type;
} /* SetSelectType*/


/******************************************************************************/
/* 	Cascades a face selection or deslection to the face's edges and vertices  */
/******************************************************************************/
void Model::CascadeFaceSelection(int face){
	
	PAE3D_Polygon poly = m_pPolyArray[face];
	//PAE3D_PrintPoly(poly);
	
	if (poly.selected) {
		//set polygon vertices and edges to selected
		for (int i = 0; i < poly.vertexCount; i++) {
			m_pVertexArray[poly.vertices[i]].selected = true;
		}
		for (int i = 0; i < poly.vertexCount; i++) {
			m_pEdgeArray[poly.edges[i]].selected = true;
		}
	}
	else {
		//deselect edges, vertices and recurse on any selected face
		for (int i = 0; i < m_nNumEdge; i++) {
			m_pEdgeArray[i].selected = false;
		}
		for (int i = 0; i < m_nNumPoint; i++) {
			m_pVertexArray[i].selected = false;
		}
		for (int i = 0 ; i < m_nNumPolygon; i++) {
			if (m_pPolyArray[i].selected) {
				CascadeFaceSelection(i);
			}
		}
	}
	
	CascadeVertexSelection();
}

/******************************************************************************/
/* 	Cascades an edge selection or deslection to the edges's vertices		  */
/******************************************************************************/
void Model::CascadeEdgeSelection(int edge){
	
	PAE3D_Edge currentEdge = m_pEdgeArray[edge];

	if (currentEdge.selected) {
		
		// select edge vertex
		m_pVertexArray[currentEdge.v1].selected = true;
		m_pVertexArray[currentEdge.v2].selected = true;

		for (int i = 0; i < m_nNumPolygon; i++) {
			
			//check if current poly does not have a selected vertex 
			PAE3D_Polygon poly = m_pPolyArray[i];
			bool hasVerts = true;
			if (poly.vertexCount > 2) {
				for (int k = 0; k < poly.vertexCount; k++) {
					if (!m_pEdgeArray[poly.edges[k]].selected) {
						hasVerts = false;
						break;
					}
				}
			}
			
			// polygon is only selected if all vertices are selected 
			m_pPolyArray[i].selected = hasVerts;
		}


	} else {
		
		//deselect edge vertices and recurse on any selected edge
		m_pVertexArray[currentEdge.v1].selected = false;
		m_pVertexArray[currentEdge.v2].selected = false;

		for (int i = 0 ; i < m_nNumEdge; i++) {
			if (m_pEdgeArray[i].selected) {
				CascadeEdgeSelection(i);
			}
		}
	}
	
	CascadeVertexSelection();
}

/******************************************************************************/
/* Refreshes edge and face selection based on current selected vertices		  */
/******************************************************************************/
void Model::CascadeVertexSelection(){
	
	for (int i = 0; i < m_nNumPolygon; i++) {
		
		//check if current poly does not have a selected vertex 
		PAE3D_Polygon poly = m_pPolyArray[i];
		bool hasVerts = true;
		
		for (int k = 0; k < poly.vertexCount; k++) {
			if (!m_pVertexArray[poly.vertices[k]].selected) {
				hasVerts = false;
				break;
			}
		}
		
		// polygon is only selected if all vertices are selected 
		m_pPolyArray[i].selected = hasVerts;
	}
	for(int i = 0; i < m_nNumEdge; i++){
		
		PAE3D_Edge currentEdge = m_pEdgeArray[i];
		
		if(m_pVertexArray[currentEdge.v1].selected && m_pVertexArray[currentEdge.v2].selected){
			m_pEdgeArray[i].selected = true;
		}else{
			m_pEdgeArray[i].selected = false;
		}
	}
} /* CascadeVertexSelection */

/******************************************************************************/
/*   Model extrude transformation				                              */
/******************************************************************************/
void Model::Extrude(){
	int innerVertCount = 0;int outerVertCount = 0;
	int innerEdgeCount = 0;int outerEdgeCount = 0;
	int faceCount = 0;
	
	//deselect erroneous
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {
			m_pVertexArray[i].selected = false;
		}
	}
	for (int i = 0; i < m_nNumEdge; i++) {
		if (m_pEdgeArray[i].selected) {
			m_pEdgeArray[i].selected = false;
		}
	}
	/* Count selected faces and refresh edge and vertex selection by selected faces */
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			faceCount++;
			CascadeFaceSelection(i);
		}

	}
	if(faceCount ==0 ){	return;}  //cant extrude, exit

	/* count inner edges selected between 2 selected faces and count remaining outer edges of faces  */
	for (int i = 0; i < m_nNumEdge; i++) {
		
		if (m_pEdgeArray[i].selected) {
			
			if(m_pPolyArray[m_pEdgeArray[i].poly1].selected && m_pPolyArray[m_pEdgeArray[i].poly2].selected){
				innerEdgeCount++;
			}else{
				outerEdgeCount++;
			}
		}
	}
	
	// create arrays for inner and outer edges
	unsigned int* outerEdgeInd = new unsigned int[outerEdgeCount];
	unsigned int* innerEdgeInd = new unsigned int[innerEdgeCount];
	int outerPosition = 0; int innerPosition = 0;

	for (int i = 0; i < m_nNumEdge; i++) {
		if (m_pEdgeArray[i].selected) {
			if(m_pPolyArray[m_pEdgeArray[i].poly1].selected && m_pPolyArray[m_pEdgeArray[i].poly2].selected){
				innerEdgeInd[innerPosition] = i;
				innerPosition++;
			}else{
				outerEdgeInd[outerPosition] = i;
				outerPosition++;
			}
		}
	}

	/* count inner vertices and outer vertices using outer edges */
	for (unsigned int i = 0; i < (unsigned int)m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {
			
			//if selected vertex is a vertex of an outer edge, it is an outer vertex
			bool outer = false;
			for (int k = 0; k < outerEdgeCount; k++) {
				if (m_pEdgeArray[outerEdgeInd[k]].v1 == i || m_pEdgeArray[outerEdgeInd[k]].v2 == i){
					outer = true;
					break;
				}
			}
			if(outer){
				outerVertCount++;
			}else{
				innerVertCount++;
			}
		}
	}

	// create arrays for inner and outer vertices
	unsigned int* outerVertsInd = new unsigned int[outerVertCount];
	unsigned int* innerVertsInd = new unsigned int[innerVertCount];
	outerPosition = 0; innerPosition = 0;

	for (unsigned int i = 0; i < (unsigned int)m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {
			bool outer = false;
			for (int k = 0; k < outerEdgeCount; k++) {
				if (m_pEdgeArray[outerEdgeInd[k]].v1 == i || m_pEdgeArray[outerEdgeInd[k]].v2 == i){
					outer = true;
					break;
				}
			}
			if(outer){
				outerVertsInd[outerPosition] = i;
				outerPosition++;
			}else{
				innerVertsInd[innerPosition] = i;
				innerPosition++;
			}
		}
	}

	PAE3D_Point* tempVerts = new PAE3D_Point[outerVertCount];
	PAE3D_Edge* tempEdges = new PAE3D_Edge[outerEdgeCount];
	//PAE3D_Polygon* tempFaces = new PAE3D_Polygon[faceCount];

	//vertices add
	for (int i = 0; i < outerVertCount; i++) {
			PAE3D_Point temp;
			temp.x = m_pVertexArray[outerVertsInd[i]].x;
			temp.y = m_pVertexArray[outerVertsInd[i]].y;
			temp.z = m_pVertexArray[outerVertsInd[i]].z;
			m_pVertexArray[outerVertsInd[i]].selected = false;
			temp.selected = true;
			tempVerts[i] = temp;
	}

	for(int i = 0; i < outerVertCount; i++){
		AddVertex(tempVerts[i]);
	}

	//add edges
	for(int i = 0; i < outerEdgeCount;i++){
		PAE3D_Edge temp;
		temp.selected = true;
		for(int k = 0; k < outerVertCount; k++){
			if(outerVertsInd[k]== m_pEdgeArray[outerEdgeInd[i]].v1){
				temp.v1 = (m_nNumPoint - outerVertCount) + k;
			}
			if(outerVertsInd[k]== m_pEdgeArray[outerEdgeInd[i]].v2){
				temp.v2 = (m_nNumPoint - outerVertCount) + k;
			}

		}
		m_pEdgeArray[outerEdgeInd[i]].selected = false;
		tempEdges[i] = temp;
	}

	for(int i = 0; i < outerEdgeCount; i++){
		AddEdge(tempEdges[i]);
	}


	int pos = 0;
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			for (int k = 0; k < m_pPolyArray[i].vertexCount; k++) {
				for (int j = 0; j < outerVertCount; j++) {
					if (outerVertsInd[j] == m_pPolyArray[i].vertices[k]) {
						m_pPolyArray[i].vertices[k] = (m_nNumPoint- outerVertCount) + j;
						break;
					}
				}
				for (int j = 0; j < outerEdgeCount; j++) {
					if (outerEdgeInd[j] == m_pPolyArray[i].edges[k]) {
						m_pPolyArray[i].edges[k] = (m_nNumEdge - outerEdgeCount)+ j;
						break;
					}
				}
			}
			m_pPolyArray[i].selected = true;
			pos++;
		}
	}
	for(int i = 0; i < innerEdgeCount;i++){
		for(int k = 0; k < outerVertCount; k++){

			if(outerVertsInd[k]== m_pEdgeArray[innerEdgeInd[i]].v1){
				m_pEdgeArray[innerEdgeInd[i]].v1 = (m_nNumPoint - outerVertCount) + k;
			}
			if(outerVertsInd[k]== m_pEdgeArray[innerEdgeInd[i]].v2){
				m_pEdgeArray[innerEdgeInd[i]].v2 = (m_nNumPoint - outerVertCount) + k;
			}
		}
	}
	/*NEW FACES*/
	for(int i = 0; i < outerEdgeCount; i++){
		PAE3D_Polygon left = m_pPolyArray[m_pEdgeArray[outerEdgeInd[i]].poly1];
		PAE3D_Polygon right = m_pPolyArray[m_pEdgeArray[outerEdgeInd[i]].poly2];

		unsigned int v1 = m_pEdgeArray[outerEdgeInd[i]].v1;
		unsigned int v2 = m_pEdgeArray[outerEdgeInd[i]].v2;
		unsigned int v1s = m_pEdgeArray[(m_nNumEdge - outerEdgeCount) + i].v1;
		unsigned int v2s = m_pEdgeArray[(m_nNumEdge - outerEdgeCount) + i].v2;

		int v1lp = -1;int v2lp = -1;
		int v1rp = -1; int v2rp = -1;

		for (int k = 0; k < left.vertexCount; k++) {
			if (left.vertices[k] == v1s) {
				v1lp = k;
			}
			if (left.vertices[k] == v2s) {
				v2lp = k;
			}
			if (left.vertices[k] == v1) {
				v1lp = k;
			}
			if (left.vertices[k] == v2) {
				v2lp = k;
			}
		}
		for (int k = 0; k < right.vertexCount; k++) {
			if (right.vertices[k] == v1s) {
				v1rp = k;
			}
			if (right.vertices[k] == v2s) {
				v2rp = k;
			}
			if (right.vertices[k] == v1) {
				v1rp = k;
			}
			if (right.vertices[k] == v2) {
				v2rp = k;
			}
		}

		PAE3D_Polygon temp;
		temp.vertices = new unsigned int[4];
		temp.vertexCount = 4;

		if (left.selected) {
			if ((v1lp == 0 && v2lp == left.vertexCount-1) || (v2lp == 0 && v1lp == left.vertexCount-1)) {
				if (v1lp == 0 && v2lp == left.vertexCount-1) {
					temp.vertices[0] = v2;
					temp.vertices[1] = v1;
					temp.vertices[2] = v1s;
					temp.vertices[3] = v2s;
				} else {
					temp.vertices[0] = v1;
					temp.vertices[1] = v2;
					temp.vertices[2] = v2s;
					temp.vertices[3] = v1s;
				}
			} else if (v1lp < v2lp) {
				temp.vertices[0] = v1;
				temp.vertices[1] = v2;
				temp.vertices[2] = v2s;
				temp.vertices[3] = v1s;
			} else if (v1lp > v2lp) {
				temp.vertices[0] = v2;
				temp.vertices[1] = v1;
				temp.vertices[2] = v1s;
				temp.vertices[3] = v2s;
			}
			temp.selected = false;
			AddPoly(temp);
			m_pPolyArray[m_nNumPolygon-1].n = PolyNormal(m_nNumPolygon-1);
		}
		if (right.selected) {
			if ((v1lp == 0 && v2lp == left.vertexCount-1) || (v2lp == 0 && v1lp == left.vertexCount-1)) {
				if (v1lp == 0 && v2lp == left.vertexCount-1) {
					temp.vertices[0] = v2s;
					temp.vertices[1] = v1s;
					temp.vertices[2] = v1;
					temp.vertices[3] = v2;
				} else {
					temp.vertices[0] = v1s;
					temp.vertices[1] = v2s;
					temp.vertices[2] = v2;
					temp.vertices[3] = v1;
				}
			} else if (v1lp < v2lp) {
				temp.vertices[0] = v1s;
				temp.vertices[1] = v2s;
				temp.vertices[2] = v2;
				temp.vertices[3] = v1;
			} else if (v1lp > v2lp) {
				temp.vertices[0] = v2s;
				temp.vertices[1] = v1s;
				temp.vertices[2] = v1;
				temp.vertices[3] = v2;
			}
			temp.selected = false;
			AddPoly(temp);
			m_pPolyArray[m_nNumPolygon-1].n = PolyNormal(m_nNumPolygon-1);
		}
	}

	for(int i = 0; i < outerVertCount; i++){
		PAE3D_Edge temp;
		temp.selected = false;
		temp.v1 = outerVertsInd[i];
		temp.v2 = (m_nNumPoint - outerVertCount) + i;
		AddEdge(temp);
	}

	AssignIntermediatePointers();
	ResetHandlePosition();
} /* Extrude */

/******************************************************************************/
/*   Delete selection operation					                              */
/******************************************************************************/
void Model::Duplicate(){
	
	int vertCount = 0;
	int edgeCount = 0;
	int faceCount =0;
	
	/* Count selected vertices, edges, faces
	*/
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {vertCount++;}
	}
	for (int i = 0; i < m_nNumEdge; i++) {
		if (m_pEdgeArray[i].selected) {edgeCount++;}
	}
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {faceCount++;}
	}
	
	/* Store indexes of selected
	 * Create copy array of vertices, edges and faces and add them.
	 */
	unsigned int* tempVertsInd = new unsigned int[vertCount];
	unsigned int* tempEdgeInd = new unsigned int[edgeCount];
	PAE3D_Point* tempVerts = new PAE3D_Point[vertCount];
	PAE3D_Edge* tempEdges = new PAE3D_Edge[edgeCount];
	PAE3D_Polygon* tempFaces = new PAE3D_Polygon[faceCount];

	/* Vertices */
	int pos = 0;
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected == true) {
			PAE3D_Point temp;
			temp.x = m_pVertexArray[i].x;
			temp.y = m_pVertexArray[i].y;
			temp.z = m_pVertexArray[i].z;
			m_pVertexArray[i].selected = false;
			temp.selected = true;
			tempVertsInd[pos]= i;
			tempVerts[pos] = temp;
			pos++;
		}
	}
	
	for(int i = 0; i < vertCount; i++){
		AddVertex(tempVerts[i]);
	}
	
	/* Edges */
	pos = 0;
	for(int i = 0; i< m_nNumEdge;i++){
		if (m_pEdgeArray[i].selected) {

			PAE3D_Edge temp;
			for(int k = 0; k< vertCount; k++){
				if(tempVertsInd[k]== m_pEdgeArray[i].v1){
					temp.v1 = (m_nNumPoint - vertCount) + k;
				}
				if(tempVertsInd[k]== m_pEdgeArray[i].v2){
					temp.v2 = (m_nNumPoint - vertCount) + k;
				}
			}
			m_pEdgeArray[i].selected = false;
			temp.selected = true;
			tempEdgeInd[pos]= i;
			tempEdges[pos] = temp;
			pos++;
		}
	}
	
	for(int i = 0; i < edgeCount; i++){
		AddEdge(tempEdges[i]);
	}
	
	/* Faces */
	pos = 0;
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			PAE3D_Polygon temp;
			temp.vertices = new unsigned int[m_pPolyArray[i].vertexCount];
			temp.vertexCount = m_pPolyArray[i].vertexCount;
			for(int k =0; k < m_pPolyArray[i].vertexCount; k++){
					for(int j= 0; j< vertCount;j++){
						if(tempVertsInd[j]==m_pPolyArray[i].vertices[k]){
							temp.vertices[k] = (m_nNumPoint - vertCount)+j;
							break;
						}
					}
			}
			m_pPolyArray[i].selected = false;
			temp.selected = true;
			tempFaces[pos] = temp;
			pos++;
		}
	}
	
	for(int i = 0; i < faceCount; i++){
		AddPoly(tempFaces[i]);
	}
	
	AssignIntermediatePointers();
	ResetHandlePosition();
	delete(tempVertsInd);
	delete(tempEdgeInd);
	delete(tempVerts);
	delete(tempEdges);
	delete(tempFaces);
	
} /* Duplicate */

/******************************************************************************/
/*   Delete selection operation					                              */
/******************************************************************************/
void Model::DeleteSelection(){
	
	int vertCount = 0;
	int edgeCount = 0;
	int faceCount =0;
	
	/* Count selected vertices, edges, faces
	*/
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {vertCount++;}
	}
	for (int i = 0; i < m_nNumEdge; i++) {
		if (m_pEdgeArray[i].selected) {edgeCount++;}
	}
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {faceCount++;}
	}
	
	/* Store indexes of selected 
	*/
	unsigned int* tempVertsInd = new unsigned int[vertCount];
	unsigned int* tempEdgeInd = new unsigned int[edgeCount];
	unsigned int* tempFaceInd = new unsigned int[faceCount];

	int pos = 0;
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected == true) {
			tempVertsInd[pos]= i;
			pos++;
		}
	}
	pos = 0;
	for(int i = 0; i< m_nNumEdge;i++){
		if (m_pEdgeArray[i].selected) {
			tempEdgeInd[pos]= i;
			pos++;
		}
	}
	pos = 0;
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			tempFaceInd[pos] = i;
			pos++;
		}
	}
	
	/* Edges or faces with all vertices selected will also be set as selected. 
	 * Edges or faces with only some selected vertices will be incomplete if the selected vertices are deleted.
	 * Edges, faces with at least one selected vertex will be need to be selected as well for deletion. 
	 */
	for (int i = 0; i < vertCount; i++) {
		// if edge has selected vertex, add to selection 
		for (int k = 0; k < m_nNumEdge; k++) {
			if (m_pEdgeArray[k].v1 == tempVertsInd[i]
				|| m_pEdgeArray[k].v2 == tempVertsInd[i]) {
				m_pEdgeArray[k].selected = true;
			}
		}
		// if poly has selected vertex, add to selection 
		for (int k = 0; k < m_nNumPolygon; k++) {
			for (int l = 0; l < m_pPolyArray[k].vertexCount; l++) {
				if (m_pPolyArray[k].vertices[l] == tempVertsInd[i]) {
					m_pPolyArray[k].selected = true;
				}
			}
		}
	}

	/* Deletions
	*/
	for (int k = 0; k < m_nNumPolygon; k++) {
			if (m_pPolyArray[k].selected) {
				DeletePoly(k); k--;
			}
	}
	for (int k = 0; k < m_nNumEdge; k++) {
		if (m_pEdgeArray[k].selected) {
			DeleteEdge(k);k--;
		}
	}
	for (int k = 0; k < m_nNumPoint; k++) {
		if (m_pVertexArray[k].selected) {
			DeleteVertex(k);k--;
		}
	}

	AssignIntermediatePointers();
	ResetHandlePosition();
	delete(tempVertsInd);
	delete(tempEdgeInd);
	delete(tempFaceInd);
	
} /* DeleteSelection */


/******************************************************************************/
/*   Merge operation										                  */
/******************************************************************************/
void Model::Merge(){
	
	int vertCount = 0;
	int edgeCount = 0;
	int faceCount = 0;
	
	/* Get selection count
	 */
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {
			vertCount++;
		}
	}
	
	if(vertCount <= 1){return;} // cant merge
	
	for (int i = 0; i < m_nNumEdge; i++) {
		if (m_pEdgeArray[i].selected) {
			edgeCount++;
		}
	}
	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			faceCount++;
		}
	}

	/* create array of selected faces
	 */
	unsigned int* facesInd = new unsigned int[faceCount];
	int position = 0;

	for (int i = 0; i < m_nNumPolygon; i++) {
		if (m_pPolyArray[i].selected) {
			facesInd[position] = i;
			m_pPolyArray[i].selected = false;
			position++;
		}
	}

	/* create array of selected edges
	 */
	unsigned int* edgesInd = new unsigned int[edgeCount];
	position = 0;

	for (int i = 0; i < m_nNumEdge; i++) {
		if (m_pEdgeArray[i].selected) {
			edgesInd[position] = i;
			m_pEdgeArray[i].selected = false;
			position++;
		}
	}

	/* create array of selected vertices and get average position of selected vertices
	 */
	unsigned int* vertsInd = new unsigned int[vertCount];
	position = 0;

	PAE3D_Point mergeCenter;
	mergeCenter.x = 0;
	mergeCenter.y = 0;
	mergeCenter.z = 0;
	
	for (int i = 0; i < m_nNumPoint; i++) {
		if (m_pVertexArray[i].selected) {	
			vertsInd[position] = i;
			position++;
			mergeCenter.x += m_pVertexArray[i].x;
			mergeCenter.y += m_pVertexArray[i].y;
			mergeCenter.z += m_pVertexArray[i].z;
			m_pVertexArray[i].selected = false;
		}
	}
	
	//average position
	mergeCenter.x = mergeCenter.x/vertCount;
	mergeCenter.y = mergeCenter.y/vertCount;
	mergeCenter.z = mergeCenter.z/vertCount;
	
	//add merged vertex, new endpoint
	mergeCenter.selected = true;
	AddVertex(mergeCenter);

	/* Edge assignment to end point */
	for (int i = 0; i < m_nNumEdge; i++) {
		for (int k = 0; k < vertCount; k++) {
			if (m_pEdgeArray[i].v1 == vertsInd[k]) {
				m_pEdgeArray[i].v1 = m_nNumPoint-1;
			}
			if(m_pEdgeArray[i].v2 == vertsInd[k]){
				m_pEdgeArray[i].v2 = m_nNumPoint-1;
			}
		}
	}

	/* Face assignment to end point */
	for (int i = 0; i < m_nNumPolygon; i++) {
		for(int j =0; j < m_pPolyArray[i].vertexCount; j++){

			for (int k = 0; k < vertCount; k++) {
				
				// match polygon vertices to selected and assign to end point
				if( m_pPolyArray[i].vertices[j] == vertsInd[k]) {
						 m_pPolyArray[i].vertices[j] = m_nNumPoint-1;
				}
			}
		}
	}
	
	/* Poly merge of poly vertex references */
	for (int i = 0; i < m_nNumPolygon; i++) {

		int delcount = 0;
		for(int j =0; j < m_pPolyArray[i].vertexCount; j++){

			if(m_pPolyArray[i].vertices[j]== m_nNumPoint-1
					&& m_pPolyArray[i].vertices[(j+1)% m_pPolyArray[i].vertexCount] == m_nNumPoint-1
			){
				
				/* If polygon vertex equals endpoint && the next vertex equals endpoint then the next vertex is redundant. 
				*  Note: index+1 modulo vertex count will return either index+1 or 0 when index+1 equals vertex count
				*/
				delcount++;
				for (int l = j; l < m_pPolyArray[i].vertexCount-1; l++) {
					m_pPolyArray[i].vertices[l] = m_pPolyArray[i].vertices[l+1];
				}
			}
		}
		/* set new vectex count and delete if there is not enough vertices for a polygon */
		m_pPolyArray[i].vertexCount = m_pPolyArray[i].vertexCount - delcount;
		if(m_pPolyArray[i].vertexCount < 3){
			DeletePoly(i);
		}
	}
	// delete old edges
	for (int i = 0; i < edgeCount; i++) {
		DeleteEdge(edgesInd[i]-i);
	}
	// delete old vertices
	for (int i = 0; i < vertCount; i++) {
		DeleteVertex(vertsInd[i] - i);
	}

	AssignIntermediatePointers();
	ResetHandlePosition();
	delete(vertsInd);
	delete(edgesInd);
	delete(facesInd);

} /* Merge */

/******************************************************************************/
/*   Generate a default 1 * 1 cube 				                              */
/******************************************************************************/
void Model::GenerateDefaultCube(){
	
	SelectedHandle = PAE3D_SELECT_NO_HANDLE;
		m_SelectMode = PAE3D_SELECT_FACES;
		m_hasSelected = false;
		picking = false;

		m_pVertexArray = NULL;
		m_pEdgeArray = NULL;
		m_pPolyArray= NULL;

		/* Define Vertices
		 */
		m_nNumPoint = 8;
		m_pVertexArray = new PAE3D_Point[m_nNumPoint];
		m_pVertexArray[0].x = 0.5;
		m_pVertexArray[0].y = 0.5;
		m_pVertexArray[0].z = 0.5;
		m_pVertexArray[1].x = -0.5;
		m_pVertexArray[1].y = 0.5;
		m_pVertexArray[1].z = 0.5;
		m_pVertexArray[2].x = -0.5;
		m_pVertexArray[2].y = -0.5;
		m_pVertexArray[2].z = 0.5;
		m_pVertexArray[3].x = 0.5;
		m_pVertexArray[3].y = -0.5;
		m_pVertexArray[3].z = 0.5;
		m_pVertexArray[4].x = 0.5;
		m_pVertexArray[4].y = 0.5;
		m_pVertexArray[4].z = -0.5;
		m_pVertexArray[5].x = -0.5;
		m_pVertexArray[5].y = 0.5;
		m_pVertexArray[5].z = -0.5;
		m_pVertexArray[6].x = -0.5;
		m_pVertexArray[6].y = -0.5;
		m_pVertexArray[6].z = -0.5;
		m_pVertexArray[7].x = 0.5;
		m_pVertexArray[7].y = -0.5;
		m_pVertexArray[7].z = -0.5;

		/* Define Faces
		 */
		m_nNumPolygon = 6;
		m_pPolyArray = new PAE3D_Polygon[m_nNumPolygon];

		unsigned int* points = new unsigned int[4];
		points[0] = 0;
		points[1] = 1;
		points[2] = 2;
		points[3] = 3;
		m_pPolyArray[0].vertices = points;
		points = new unsigned int[4];
		points[0] = 7;
		points[1] = 6;
		points[2] = 5;
		points[3] = 4;
		m_pPolyArray[1].vertices = points;
		points = new unsigned int[4];
		points[0] = 1;
		points[1] = 5;
		points[2] = 6;
		points[3] = 2;
		m_pPolyArray[2].vertices = points;
		points = new unsigned int[4];
		points[0] = 4;
		points[1] = 0;
		points[2] = 3;
		points[3] = 7;
		m_pPolyArray[3].vertices = points;
		points = new unsigned int[4];
		points[0] = 4;
		points[1] = 5;
		points[2] = 1;
		points[3] = 0;
		m_pPolyArray[4].vertices = points;
		points = new unsigned int[4];
		points[0] = 3;
		points[1] = 2;
		points[2] = 6;
		points[3] = 7;
		m_pPolyArray[5].vertices = points;

		for (int i = 0 ; i < m_nNumPoint; i++) {
			m_pVertexArray[i].edgeCount = 0;
		}
		for (int i = 0; i < m_nNumPolygon; i++) {
			m_pPolyArray[i].selected = false;
			m_pPolyArray[i].vertexCount = 4;
			m_pPolyArray[i].n = PolyNormal(i);
			m_pPolyArray[i].mat = 0;
		}

		/* Define edges
		 */
		m_nNumEdge = 12;
		m_pEdgeArray = new PAE3D_Edge[m_nNumEdge];
		PAE3D_Edge e0;
		e0.v1 = 0;
		e0.v2 = 1;
		m_pEdgeArray[0] = e0;
		PAE3D_Edge e1;
		e1.v1 = 1;
		e1.v2 = 2;
		m_pEdgeArray[1] = e1;
		PAE3D_Edge e2;
		e2.v1 = 2;
		e2.v2 = 3;
		m_pEdgeArray[2] = e2;
		PAE3D_Edge e3;
		e3.v1 = 3;
		e3.v2 = 0;
		m_pEdgeArray[3] = e3;
		PAE3D_Edge e4;
		e4.v1 = 4;
		e4.v2 = 5;
		m_pEdgeArray[4] = e4;
		PAE3D_Edge e5;
		e5.v1 = 5;
		e5.v2 = 6;
		m_pEdgeArray[5] = e5;
		PAE3D_Edge e6;
		e6.v1 = 6;
		e6.v2 = 7;
		m_pEdgeArray[6] = e6;
		PAE3D_Edge e7;
		e7.v1 = 7;
		e7.v2 = 4;
		m_pEdgeArray[7] = e7;
		PAE3D_Edge e8;
		e8.v1 = 0;
		e8.v2 = 4;
		m_pEdgeArray[8] = e8;
		PAE3D_Edge e9;
		e9.v1 = 1;
		e9.v2 = 5;
		m_pEdgeArray[9] = e9;
		PAE3D_Edge e10;
		e10.v1 = 2;
		e10.v2 = 6;
		m_pEdgeArray[10] = e10;
		PAE3D_Edge e11;
		e11.v1 = 3;
		e11.v2 = 7;
		m_pEdgeArray[11] = e11;
		AssignIntermediatePointers();
		m_nListPoint = m_nNumPoint;
		m_nListEdge = m_nNumEdge;
		m_nListPolygon = m_nNumPolygon;
}

/******************************************************************************/
/*   Init									                                  */
/******************************************************************************/
Model::Model(char* filename) {

	m_hasSelected = false;
	picking = false;
	m_SelectMode = PAE3D_SELECT_FACES;
	SelectedHandle = PAE3D_HANDLE_MOVE;
	
	/* Construct basic default cube when no object file is being loaded. 
	*/
	if (filename == NULL) {
		GenerateDefaultCube();
	}
	else {
		ReadOBJ(filename);
	}
	DeselectEverything();
	for (int i = 0; i < m_nNumPolygon; i++) {
		m_pPolyArray[i].n = PolyNormal(i);
	}
	for (int i = 0; i < m_nNumPoint; i++) {
		CalculateNormal(i);
	}
} /* Model */