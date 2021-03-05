#include "pch.h"
#include "OPenGLRenderer.h"

BEGIN_MESSAGE_MAP(OPenGLRenderer, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void OPenGLRenderer::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	switch (nIDEvent)
	{
	case 1:
		wglMakeCurrent(m_hdc, m_hrc);
		DrawGLScene();
		SwapBuffers(m_hdc);
		wglMakeCurrent(m_hdc, NULL);
		break;
	default:
		break;
	}

	CWnd::OnTimer(nIDEvent);
}

OPenGLRenderer::OPenGLRenderer()
{
	m_hdc = NULL;
}

OPenGLRenderer::~OPenGLRenderer()
{

}

bool OPenGLRenderer::CreateGLContext(CRect rect, CWnd* parent)
{
	if (!InitContext(parent))
	{
		MessageBox(_T("ERROR Creating InitContext"));
		return false;
	};
	return true;
}

// ���ؽ�Ʈ �ʱ�ȭ
bool OPenGLRenderer::InitContext(CWnd* parent)
{
	int bits = 16;
	static	PIXELFORMATDESCRIPTOR pfd =					// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
		1,												// Version Number
		PFD_DRAW_TO_WINDOW |							// Format Must Support Window
		PFD_SUPPORT_OPENGL |							// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,								// Must Support Double Buffering
		PFD_TYPE_RGBA,									// Request An RGBA Format
		BYTE(bits),											// Select Our Color Depth
		0, 0, 0, 0, 0, 0,								// Color Bits Ignored
		0,												// No Alpha Buffer
		0,												// Shift Bit Ignored
		0,												// No Accumulation Buffer
		0, 0, 0, 0,										// Accumulation Bits Ignored
		16,												// 16Bit Z-Buffer (Depth Buffer)
		0,												// No Stencil Buffer
		0,												// No Auxiliary Buffer
		PFD_MAIN_PLANE,									// Main Drawing Layer
		0,												// Reserved
		0, 0, 0											// Layer Masks Ignored
	};

	m_hdc = ::GetDC(m_hWnd);
	int nPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
	if (nPixelFormat == 0) return false;
	BOOL bResult = SetPixelFormat(m_hdc, nPixelFormat, &pfd);
	if (!bResult) return false;

	m_hrc = wglCreateContext(m_hdc);
	wglMakeCurrent(m_hdc, m_hrc);

	if (!m_hrc)
		return false;

	CString str;
	str.Format(_T("OpenGL version: %s\n"), (CString)glGetString(GL_VERSION));
	TRACE(str);
	return true;
}

// �ʱ� �� �غ�
void OPenGLRenderer::PrepareScene(int sx, int sy, int cx, int cy)
{
	glClearColor(0.0, 0.0, 1.0, 0.0); //background to clear with.
	wglMakeCurrent(m_hdc, m_hrc);

	GLfloat fAspect;

	if (cy == 0) cy = 1;

	glViewport(sx, sy, cx, cy);

	fAspect = (GLfloat)cx / (GLfloat)cy;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0f, fAspect, 1.0f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	
	earth = gluNewQuadric(); // ���� ��ü �ν��Ͻ� ����
	moon = gluNewQuadric(); // �� ��ü �ν��Ͻ� ����

	wglMakeCurrent(m_hdc, NULL);
}

// �ʱ� ���� �۾�
bool OPenGLRenderer::initAi()
{
	wglMakeCurrent(m_hdc, m_hrc);

	glEnable(GL_LIGHT1);

	wglMakeCurrent(m_hdc, NULL);

	xrot = 0.0;
	yrot = 0.0;
	zrot = 0.0;
	

	return true;
}

// �׸��� �޼ҵ�
int OPenGLRenderer::DrawGLScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //��� Ŭ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���� ��Ʈ Ŭ���� (����)
	glLoadIdentity(); // (����)

	// draw
	glPushMatrix(); // �����߽���ǥ �߰�
	glTranslatef(0.0f, 0.0f, -20.0f); //������ �̵�
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ������ ���߱�� )
	glRotatef(-23.5f, 0.0f, 1.0f, 0.0f); // y���� �������� ��ǥ�� -23.5�� ȸ��( ������ ���߱�� )

	glRotatef(zrot, 0.0f, 0.0f, 1.0f); //ȸ�� ��ȯ (z��) (���� ����)
	gluQuadricDrawStyle(earth, GLU_FILL); // ��ü�� ä��� ���·� ����
	glColor3f(0.6f, 0.6f, 1.0f); // �� ����
	gluSphere(earth, 3.0f, 24, 24); // ���� �׸�

	gluQuadricDrawStyle(earth, GLU_LINE); // ���� �ߴ� ���·� ����
	glColor3f(0.0f, 0.0f, 1.0f);//�� ����
	gluSphere(earth, 3.1f, 24, 24); // ���� �׸�

	// Red coordinate (x�� ��ǥ)
	glColor3f(1, 0, 0);
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(20.0, 0.0, 0.0);
	glVertex3f(-20.0, 0.0, 0.0);
	glEnd();

	// Green coordinate (y�� ��ǥ)
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 20.0, 0.0);
	glVertex3f(0.0, -20.0, 0.0);
	glEnd();

	// Blue coordinate (z�� ��ǥ)
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 20.0);
	glVertex3f(0.0, 0.0, -20.0);
	glEnd();
	glPopMatrix(); // ���� �߽���ǥ ����

	glPushMatrix(); // �� �˵��� ���� ��ǥ �߰�
	glTranslatef(0.0f, 0.0f, -20.0f); // ���� ������ ������ ������
	glRotatef(moon_zrot, 0.0f, 1.0f, 0.0f); //ȸ�� ��ȯ (z��) (�� ����) , �׸��� ȸ���� ����
	glTranslatef(4.0f, 0.0f, 0.0f); // �������� �Ÿ���ŭ �̵�
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ȸ���� ���߱�� )
	
	gluQuadricDrawStyle(moon, GLU_FILL); // ��ü�� ä��� ���·� ����
	glColor3f(0.35f, 0.35f, 0.35f);
	gluSphere(moon, 0.7f, 12, 12);
	gluQuadricDrawStyle(moon, GLU_LINE); // ���� �ߴ� ���·� ����
	glColor3f(0.7f, 0.7f, 0.7f);
	gluSphere(moon, 0.75f, 12, 12);
	glPopMatrix();

	
	glFlush();

	zrot += 0.5f; 
	if (zrot > 359.5f) zrot = 0.0f;

	moon_zrot += 0.5f;
	if (moon_zrot > 359.5f) moon_zrot = 0.0f;

	return TRUE;
}

// �� ����
void OPenGLRenderer::DestroyScene()
{
	DestroyWindow();

	wglMakeCurrent(m_hdc, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}
