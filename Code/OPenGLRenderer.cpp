#include "pch.h"
#include "OPenGLRenderer.h"

BEGIN_MESSAGE_MAP(OPenGLRenderer, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()


void OPenGLRenderer::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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

// 컨텍스트 초기화
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

// 초기 씬 준비
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

	
	earth = gluNewQuadric(); // 지구 객체 인스턴스 생성
	moon = gluNewQuadric(); // 달 객체 인스턴스 생성

	wglMakeCurrent(m_hdc, NULL);
}

// 초기 설정 작업
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

// 그리기 메소드
int OPenGLRenderer::DrawGLScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //배경 클리어
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 버퍼 비트 클리어 (고정)
	glLoadIdentity(); // (고정)

	// draw
	glPushMatrix(); // 지구중심좌표 추가
	glTranslatef(0.0f, 0.0f, -20.0f); //원점의 이동
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 자전축 맞추기용 )
	glRotatef(-23.5f, 0.0f, 1.0f, 0.0f); // y축을 기준으로 좌표축 -23.5도 회전( 자전축 맞추기용 )

	glRotatef(zrot, 0.0f, 0.0f, 1.0f); //회전 변환 (z축) (지구 자전)
	gluQuadricDrawStyle(earth, GLU_FILL); // 객체를 채우는 형태로 설정
	glColor3f(0.6f, 0.6f, 1.0f); // 색 지정
	gluSphere(earth, 3.0f, 24, 24); // 구를 그림

	gluQuadricDrawStyle(earth, GLU_LINE); // 선을 긋는 형태로 설정
	glColor3f(0.0f, 0.0f, 1.0f);//색 지정
	gluSphere(earth, 3.1f, 24, 24); // 구를 그림

	// Red coordinate (x축 좌표)
	glColor3f(1, 0, 0);
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(20.0, 0.0, 0.0);
	glVertex3f(-20.0, 0.0, 0.0);
	glEnd();

	// Green coordinate (y축 좌표)
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 20.0, 0.0);
	glVertex3f(0.0, -20.0, 0.0);
	glEnd();

	// Blue coordinate (z축 좌표)
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 20.0);
	glVertex3f(0.0, 0.0, -20.0);
	glEnd();
	glPopMatrix(); // 지구 중심좌표 제거

	glPushMatrix(); // 달 궤도를 위한 좌표 추가
	glTranslatef(0.0f, 0.0f, -20.0f); // 먼저 원점을 지구와 맞춰줌
	glRotatef(moon_zrot, 0.0f, 1.0f, 0.0f); //회전 변환 (z축) (달 공전) , 그리고 회전을 적용
	glTranslatef(4.0f, 0.0f, 0.0f); // 지구와의 거리만큼 이동
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x축을 기준으로 좌표축 -90도 회전( 회전축 맞추기용 )
	
	gluQuadricDrawStyle(moon, GLU_FILL); // 객체를 채우는 형태로 설정
	glColor3f(0.35f, 0.35f, 0.35f);
	gluSphere(moon, 0.7f, 12, 12);
	gluQuadricDrawStyle(moon, GLU_LINE); // 선을 긋는 형태로 설정
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

// 씬 삭제
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
