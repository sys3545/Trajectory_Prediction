#pragma once
#include <afxwin.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>

class OPenGLRenderer : public CWnd
{
public:
	OPenGLRenderer();
	virtual ~OPenGLRenderer();
	bool CreateGLContext(CRect rect, CWnd* parent);
	void PrepareScene(int sx, int sy, int cx, int cy);				// Scene preparation
	void DestroyScene();
	int DrawGLScene();				//Here's where we do all the drawing
	bool initAi();

public:
	// ���� ȸ�� ����
	GLfloat		xrot = 0;
	GLfloat		yrot = 0;
	GLfloat		zrot = 0;
	GLfloat		GL_PI = (GLfloat)3.141592;

	// �� ȸ�� ����
	GLfloat		moon_xrot = 0;
	GLfloat		moon_yrot = 0;
	GLfloat		moon_zrot = 0;
	GLUquadricObj* earth; // ���� ��ü
	GLUquadricObj* moon; // �� ��ü
protected:
	bool InitContext(CWnd* parent);					 // Creates OpenGL Rendering Context

	HGLRC m_hrc;                        // OpenGL Rendering Context 
	HDC m_hdc;
	CRect m_rect;


public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

