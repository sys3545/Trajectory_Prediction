#pragma once
#include <afxwin.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/GLAUX.H>
#include <math.h>

#pragma comment(lib,"GLAUX.LIB")
#pragma comment(lib,"legacy_stdio_definitions.lib")

typedef struct vecInfo_t {
	GLfloat xpos = 0.0f, ypos = 0.0f, zpos = 0.0f;
	GLfloat xvel = 0.0f, yvel = 0.0f, zvel = 0.0f;
	GLfloat C1 = 0.0f, C2 = 0.0f, C3 = 0.0f, h = 0.0f;
	GLfloat i = 0.0f, omega = 0.0f, a = 0.0f, e = 0.0f, w = 0.0f, T = 0.0f; // �˵��� 6���
	GLfloat n = 0.0f, M = 0.0f, E = 0.0f, preRadius = 0.0f, preF = 0.0f, preTime = 0.0f; // n : mean motion, M : mean anomaly, E : eccentric anomaly
	GLfloat u = 0.0f, f = 0.0f, p = 0.0f; // u = w + f , f�� ���������̰� True anomaly , p�� ������
	GLfloat range = 0.0f, velocity = 0.0f; // �ʱ� �Ÿ�, �ʱ� �ӵ�
	GLfloat traj_range = 0.0f, traj_xpos = 0.0f, traj_ypos = 0.0f;  // �˵��� �׸��� ���� ���ϴ� ������
	GLfloat angleSpeed = 0.0f, angle = 0.0f, P = 0.0f; // ���ӵ�, ���ϴ� ��  (�ʱⰢ�� f) , �ֱ�
	GLfloat radius = 0.0f; // ���ϴ� �Ÿ� (�ʱ� �Ÿ����� ����)
	GLint isSelected = 0; // ������ �Ǿ��°�?
	GLint isStarted = 0; // ���� ��尡 ���� �Ǿ��°�?
	GLint type = 0; // �˵� ���� (1.Ÿ��, 2.�ְ)
	GLUquadricObj* craft = NULL;
}vecInfo_t;


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

	// �׸���� ����� ���� �Լ�
	void CreateCraft(int num); // ���ֹ�ü ��ü�� �����ϴ� �Լ�
	void DrawTrajectory(int num); // ���ֹ�ü ������ �׷��ִ� �Լ�
	void DrawSphere(int num); // ���ֹ�ü ���� �׷��ִ� �Լ�

	// �ؽ�ó ���� �Լ�
	AUX_RGBImageRec* LoadBMPFile(char* filename); // ������ �̹����� �ҷ����� �Լ�
	int LoadGLTextures(); //�ؽ��� �۾��� ���ִ� �Լ�

public:
	// ������ ����
	GLfloat		zrot = 0; // ���� ���� �ӵ�
	GLfloat     radius_Earth = 6.378f; ////////// 1.0f = 1000km //////////
	GLfloat     mass_Earth = (GLfloat)5.972 * (GLfloat)pow(10, 24); // kg
	double      G = 6.673 * pow(10, -11); // m^3/kg/s^2 (�߷»��)

	// �� ȸ�� ����
	GLfloat		moon_zrot = 0; // �� ���� �ӵ�
	GLfloat		moon_xpos = 7.0f; // ������ �ްŸ�
	GLfloat		moon_ypos = 0;
	GLfloat		moon_zpos = 0;

	GLUquadricObj* earth = NULL; // ���� ��ü
	GLUquadricObj* moon = NULL; // �� ��ü
	vecInfo_t spaceCraft[6]; // ���ֹ�ü ��ü
	int numOfCraft = 0; // ���ֹ�ü ����
	GLUquadricObj* preCraft[6]; // �����˵� �׸��ÿ� ����� ��ü

	/// ���Ŀ� �ʿ��� ��������
	GLfloat		GL_PI = (GLfloat)3.141592;
	GLfloat     mass_Sun = 2.0f * (GLfloat)pow(10, 30); // kg

	/// �ð����� ����
	GLfloat timeScale = 63.38f; // 1�ʴ� 63.38 tick �̴�. ���� ���ӵ��� �� ���� ������ �ȴ�. 
	int currentTime = 0;
	int oldTime = 0;
	int deltaTime = 0;
	int realTime = 1; // �����ð����� ����� ������� ���� �����ִ� ����

	CString test; // �� ����ũž �׽�Ʈ //
	GLdouble wx=0.0;
	GLdouble wy=0.0;
	GLdouble wz=0.0;

	// �ؽ�ó ���� ����
	AUX_RGBImageRec* pTextureImage[1]; // �̹��� ���Ͽ� ���� ������ ������ �ִ� �迭
	GLuint textureID[1]; // �ؽ�ó�� �ڵ��� �����ϴ� �迭

	// ���콺 �Է½� ȸ�� ����
	BOOL b_Rotate = FALSE; // ���콺 ���¿� ���� ȸ���� ����
	GLfloat mousePoint; // ���콺 ��ġ�� ��� ����
	GLfloat differ; // ���콺�� ������ ����
	GLfloat zAngle = 0.0f; // ȭ�� ȸ�� ����
	GLfloat zoom = 0.0f; // �� ����

	// �˵� ���� ���� �Լ�
	void PredictionPosition(int num, GLfloat time); // mean motion, E ���� �̿��Ͽ� �����ð� ���� ���õ� ��ü�� ��ġ�� ���ϴ� �Լ�
	void DrawPrediction(int num); // ���� ����ǥ ����� �׷��ִ� �Լ�
	void CalculateT(int num); // ������ ����ø� ���ϴ� �Լ� (�� ����)
	void CreatPreCraft(int num); // ���� ��ü�� ��ü�� �����ϴ� �Լ�
	void unProject(CPoint point); // ��ũ�� ��ǥ�� ������ǥ�� ��ȯ

protected:
	bool InitContext(CWnd* parent);					 // Creates OpenGL Rendering Context

	HGLRC m_hrc;                        // OpenGL Rendering Context 
	HDC m_hdc;
	CRect m_rect;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};