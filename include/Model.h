#pragma once

#include "define.h"
#include "Color.h"
#include <GL/glut.h>

class Model
{
private:
	PAE3D_Point* m_pVertexArray;
	PAE3D_Polygon* m_pPolyArray;
	PAE3D_Edge* m_pEdgeArray;
	int m_nNumPoint;
	int m_nNumEdge;
	int m_nNumPolygon;
	PAE3D_Point m_selectedCenter;
	bool m_hasSelected;
	bool picking;
public:
	int m_SelectMode;
	int SelectedHandle;
	Model(char*);
	~Model(void);
	PAE3D_Normal PolyNormal(int);
	PAE3D_Point PolyCenter(int);
	void MoveSelected(float, float, float);
	void ScaleSelected(float, float, float);
	void DeselectEverything();
	void RenderVertices(float);
	void RenderEdges(float);
	void RenderFaces(Color*, bool);
	void RenderSelectedFacesHandle(float, int);
	void AddVertex(PAE3D_Point);
	void AddEdge(PAE3D_Edge);
	void AddPoly(PAE3D_Polygon);
	void DeletePoly(int);
	void DeleteVertex(int);
	void DeleteEdge(int);
	void Merge();
	void AddNormal();
	int FindEdge(unsigned int, unsigned int);
	void RenderPicker(float, int);
	void ProcessSelection(int, int, bool, bool, int);
	void SetSelectType(int);
	void CascadeFaceSelection(int);
	void CascadeEdgeSelection(int);
	void CascadeVertexSelection();
	void Extrude();
	void Duplicate();
	void DeleteSelection();
	void CalculateNormal(int);
	void AssignIntermediatePointers();
	void Subdivide(bool);
	void ResetHandlePosition();
	void ReadOBJ(const char*);
	void GenerateDefaultCube();
	void SelectAll();
};
