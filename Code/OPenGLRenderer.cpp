
#include "pch.h"
#include "OPenGLRenderer.h"

BEGIN_MESSAGE_MAP(OPenGLRenderer, CWnd)
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
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

	glEnable(GL_DEPTH_TEST); // ���� ���۸� ����� ���� �˸�

	zoom = -30.0f; // �ʱ� �� ���� ����
	earth = gluNewQuadric(); // ���� ��ü �ν��Ͻ� ����
	moon = gluNewQuadric(); // �� ��ü �ν��Ͻ� ����
	LoadGLTextures();

	oldTime = glutGet(GLUT_ELAPSED_TIME); //oldTime ���� �ʱ�ȭ���� �� �������� ����
	wglMakeCurrent(m_hdc, NULL);
}

// �ʱ� ���� �۾�
bool OPenGLRenderer::initAi()
{
	wglMakeCurrent(m_hdc, m_hrc);

	glEnable(GL_LIGHT1);

	wglMakeCurrent(m_hdc, NULL);


	return true;
}

// �׸��� �޼ҵ�
int OPenGLRenderer::DrawGLScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //��� Ŭ����
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ���� ��Ʈ Ŭ���� (����)
	glLoadIdentity(); // (����)
	glTranslatef(0.0f, 0.0f, zoom); //������ �̵�
	glRotatef(zAngle, 0.0f, 1.0f, 0.0f); // z���� �������� ��ǥ�� ȸ��(ȭ�� ȸ����)

	////draw Earth
	glPushMatrix(); // �����߽���ǥ �߰�
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ������ ���߱�� )
	
	glRotatef(zrot, 0.0f, 0.0f, 1.0f); //ȸ�� ��ȯ (z��) (���� ����)
	gluQuadricDrawStyle(earth, GLU_FILL); // ��ü�� ä��� ���·� ����
	glColor3f(1.0f, 1.0f, 1.0f); // �� ����
	gluSphere(earth, radius_Earth, 24, 24); // ���� �׸�

	gluQuadricDrawStyle(earth, GLU_LINE); // ���� �ߴ� ���·� ����
	glColor3f(0.2f, 0.2f, 1.0f); // �� ����
	gluSphere(earth, radius_Earth + (GLfloat)0.05f, 24, 24); // ���� �׸�

	// Blue coordinate (z�� ��ǥ)
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0.0, 0.0, 40.0);
	glVertex3f(0.0, 0.0, -40.0);
	glEnd();
	glPopMatrix(); // ���� �߽���ǥ ����

	glPushMatrix(); ////// �� �׸��� ���� ��ǥ �߰�  /////////
	glRotatef(20.0f, 0.0f, 0.0f, 1.0f); // z���� �������� ��ǥ�� 20�� ȸ��( �˵��� ȸ�� )
	glRotatef(moon_zrot, 0.0f, 1.0f, 0.0f); //ȸ�� ��ȯ (z��) (�� ����) , �׸��� ȸ���� ����
	glTranslatef(7.0f, 0.0f, 0.0f); // �������� �Ÿ���ŭ x�࿡�� �̵�
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ȸ���� ���߱�� )

	gluQuadricDrawStyle(moon, GLU_FILL); // ��ü�� ä��� ���·� ����
	glColor3f(0.35f, 0.35f, 0.35f);
	gluSphere(moon, 0.3f, 12, 12);
	gluQuadricDrawStyle(moon, GLU_LINE); // ���� �ߴ� ���·� ����
	glColor3f(0.7f, 0.7f, 0.7f);
	gluSphere(moon, 0.32f, 12, 12);
	glPopMatrix(); // �� ��ǥ ����

	glPushMatrix();// �� �˵��� �׸��� ���� ��ǥ �߰�
	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef(20.0f, 0.0f, 0.0f, 1.0f); // z���� �������� ��ǥ�� 20�� ȸ��( �˵��� ȸ�� )
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ��ǥ�� ���߱�� )
	glBegin(GL_POINTS);
	for (GLfloat angle = 0; angle < 360; angle += 0.1f)
	{
		moon_xpos = (GLfloat)cos(angle) * 7.0f;
		moon_ypos = (GLfloat)sin(angle) * 7.0f;
		glVertex3f(moon_xpos, moon_ypos, moon_zpos);
	}
	glEnd();
	glPopMatrix(); /////// �� �˵� ��ǥ ���� ///////


	for (int n = 0; n <= numOfCraft; n++) {
		if (spaceCraft[n].craft != NULL) {
			glPushMatrix(); // �߰��� ���ֹ�ü �˵��� �׸��� ���� ��ǥ �߰�
			glColor3f(1.0f, 1.0f, 1.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ��ǥ�� ���߱�� )
			glRotatef(spaceCraft[n].omega, 0.0f, 0.0f, 1.0f); // ���� z���� �������� omega��ŭ ȸ�� (�±������� ����)
			glRotatef(spaceCraft[n].i, 1.0f, 0.0f, 0.0f); // ȸ���� ��ǥ�࿡�� x���� �������� i��ŭ ȸ�� (�˵���簢 ����)
			DrawTrajectory(n); // n��° ���ֹ�ü�� ������ �׸�
			glPopMatrix();// �߰��� ���ֹ�ü �˵��� �׸��� ���� ��ǥ ����
			
			glPushMatrix(); // ���ֹ�ü�� �׸������� ��ǥ �߰�
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ��ǥ�� ���߱�� )
			glRotatef(spaceCraft[n].omega, 0.0f, 0.0f, 1.0f); // ���� z���� �������� omega��ŭ ȸ�� (�±������� ����)
			glRotatef(spaceCraft[n].i, 1.0f, 0.0f, 0.0f); // ȸ���� ��ǥ�࿡�� x���� �������� i��ŭ ȸ�� (�˵���簢 ����)
			DrawSphere(n); // n��° ���ֹ�ü�� �׸�
			glPopMatrix();// �߰��� ���ֹ�ü�� �׸��� ���� ��ǥ ����

			glPushMatrix(); // ���� ��ġ�� ��ü�� �׸��� ���� ��ǥ �߰�
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // x���� �������� ��ǥ�� -90�� ȸ��( ��ǥ�� ���߱�� )
			glRotatef(spaceCraft[n].omega, 0.0f, 0.0f, 1.0f); // ���� z���� �������� omega��ŭ ȸ�� (�±������� ����)
			glRotatef(spaceCraft[n].i, 1.0f, 0.0f, 0.0f); // ȸ���� ��ǥ�࿡�� x���� �������� i��ŭ ȸ�� (�˵���簢 ����)
			glRotatef(spaceCraft[n].w, 0.0f, 0.0f, 1.0f);
			DrawPrediction(n); // ���� ��� ���̸� ���� ����ǥ�� ���� �׷���
			glPopMatrix(); // ���� ��ġ�� ��ü�� �׸��� ���� ��ǥ ����
		}
	}
	
	glFlush();
	
	// ���� ���� �ӵ�
	zrot += 360.0f / 87840.0f / timeScale;
	if (zrot > 360.0f) {
		zrot -= 360.0f;
		currentTime = glutGet(GLUT_ELAPSED_TIME);
		deltaTime = currentTime - oldTime;
		oldTime = currentTime;
	}

	moon_zrot += 1.0f;
	if (moon_zrot > 359.0f) {
		moon_zrot = 0.0f;
	}

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

AUX_RGBImageRec* OPenGLRenderer::LoadBMPFile(char* filename)
{
	FILE* hFile = NULL;
	if (!filename) return NULL;

	hFile = fopen(filename, "r");
	wchar_t wtext[20];
	mbstowcs(wtext, filename, strlen(filename) + 1);//Plus null
	LPWSTR ptr = wtext;

	if (hFile) {
		fclose(hFile);
		return auxDIBImageLoad(ptr);
	}

	return NULL;
}

int OPenGLRenderer::LoadGLTextures()
{
	int Status = FALSE;

	gluQuadricTexture(earth, GL_TRUE); // �ؽ�ó ���� ���
	memset(pTextureImage, 0, sizeof(void*) * 1); // ������ �ʱ�ȭ

	if (pTextureImage[0] = LoadBMPFile("earth.bmp")) {
		Status = TRUE;
		glGenTextures(1, &textureID[0]); // �ؽ��� ��ü ����
		glBindTexture(GL_TEXTURE_2D, textureID[0]); // ���°����ڿ��� [0]��° �ؽ�ó�� ���ε�
		glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage[0]->sizeX, pTextureImage[0]->sizeY, 0,
			GL_RGB, GL_UNSIGNED_BYTE, pTextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // �̹��� ���ϰ� ��ü�� ũ�⸦ �����ش�
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}

	//�ؽ�ó �����ݳ�
	if (pTextureImage[0]) {
		if (pTextureImage[0]->data) {
			free(pTextureImage[0]->data);
		}
		free(pTextureImage[0]);
	}
	return Status;
}

// ���ֹ�ü�� ���������� �����ϴ� �Լ�
void OPenGLRenderer::CreateCraft(int num) 
{
	spaceCraft[num].craft = gluNewQuadric(); // ��ü ��ü �ν��Ͻ� ����
}

// ���ֹ�ü ������ �׷��ִ� �Լ�
void OPenGLRenderer::DrawTrajectory(int num) 
{
	glRotatef(spaceCraft[num].w, 0.0f, 0.0f, 1.0f);
	
	if (spaceCraft[num].type == 1) {
		glBegin(GL_POINTS);
		for (GLfloat angle_a = 0; angle_a < 360; angle_a += 0.1f)
		{
			spaceCraft[num].traj_range = (GLfloat)(spaceCraft[num].p / (1 + spaceCraft[num].e * cos(angle_a)));
			spaceCraft[num].traj_xpos = (GLfloat)cos(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			spaceCraft[num].traj_ypos = (GLfloat)sin(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			glVertex3f(spaceCraft[num].traj_xpos, spaceCraft[num].traj_ypos, 0.0f);
		}
		glEnd();
	}
	else {
		GLfloat bound = acosf(1 / spaceCraft[num].e);
		glBegin(GL_POINTS);
		for (GLfloat angle_a = 0; angle_a < 360; angle_a += 0.1f)
		{
			spaceCraft[num].traj_range = (GLfloat)(spaceCraft[num].p / (1 + spaceCraft[num].e * cos(angle_a)));
			spaceCraft[num].traj_xpos = (GLfloat)cos(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			spaceCraft[num].traj_ypos = (GLfloat)sin(angle_a) * spaceCraft[num].traj_range / 1000.0f;
			glVertex3f(spaceCraft[num].traj_xpos, spaceCraft[num].traj_ypos, 0.0f); 
		}
		glEnd();
	}
}

// ���ֹ�ü ���� �׷��ִ� �Լ�
void OPenGLRenderer::DrawSphere(int num) 
{
	glRotatef(spaceCraft[num].w, 0.0f, 0.0f, 1.0f); // w ��ŭ �׸��� ���ظ� ȸ��
	// ���⼭ f��ŭ ȸ���� ��ġ���� �ʱ� ��ü ����
	spaceCraft[num].xpos = (GLfloat)cos(spaceCraft[num].angle * GL_PI / 180.0f) * spaceCraft[num].radius / 1000.0f;
	spaceCraft[num].ypos = (GLfloat)sin(spaceCraft[num].angle * GL_PI / 180.0f) * spaceCraft[num].radius / 1000.0f;

	// drawing
	glTranslatef(spaceCraft[num].xpos, spaceCraft[num].ypos, 0.0f); // ��ü �̵� (�����Ŀ� ���� �� �������� �ٲ��)
	gluQuadricDrawStyle(spaceCraft[num].craft, GLU_FILL); // ��ü�� ä��� ���·� ����
	glColor3f(0.35f, 0.35f, 0.35f);
	gluSphere(spaceCraft[num].craft, 0.3f, 12, 12);

	gluQuadricDrawStyle(spaceCraft[num].craft, GLU_LINE); // ���� �ߴ� ���·� ����
	if(spaceCraft[num].isSelected == 0)
		glColor3f(0.4f, 0.4f, 0.8f);
	else
		glColor3f(0.9f, 0.0f, 0.0f);
	gluSphere(spaceCraft[num].craft, 0.32f, 12, 12);

	/// Update values ///
	spaceCraft[num].angleSpeed = spaceCraft[num].h / (GLfloat)pow((double)spaceCraft[num].radius, 2); // ���ӵ� ���ϱ�
	spaceCraft[num].angle += (spaceCraft[num].angleSpeed * 180.0f) / GL_PI / timeScale; // ���ӵ� ����
	
	spaceCraft[num].radius = (GLfloat)(spaceCraft[num].p / (1 + spaceCraft[num].e * cos(spaceCraft[num].angle * GL_PI / 180.0f)));
	if (spaceCraft[num].angle > 360.0f) {
		spaceCraft[num].angle -= 360.0f;
	}

	CalculateT(num); // ������ ����� ������Ʈ
	//// Update End ////

	PredictionPosition(num, spaceCraft[num].preTime); //
}

// ������ ��� �ð��� ���ϴ� �Լ�
void OPenGLRenderer::CalculateT(int num) 
{ 
	GLfloat cosE;
	GLfloat initialM;
	GLfloat initialE;

	cosE = (1.0f - (spaceCraft[num].radius / spaceCraft[num].a)) / spaceCraft[num].e;
	initialE = acosf(cosE);

	initialM = initialE - (spaceCraft[num].e * sinf(initialE));

	spaceCraft[num].T = -initialM / spaceCraft[num].n;

	if (spaceCraft[num].angle >= 0.0f && spaceCraft[num].angle <= 180.0f)
		spaceCraft[num].T = spaceCraft[num].P + spaceCraft[num].T;
	else
		spaceCraft[num].T = -spaceCraft[num].T;
}

// ���� ����ǥ�� ���ϴ� �Լ�
void OPenGLRenderer::PredictionPosition(int num, GLfloat time) 
{
	const int repeat = 4;
	GLfloat M[repeat]; // M0, M1, M2, M3 
	GLfloat E[repeat]; // E0, E1, E2, E3
	GLfloat deltaE;
	GLfloat tanHalf_F;

	if (time >= spaceCraft[num].T)
		spaceCraft[num].M = spaceCraft[num].n * (time - spaceCraft[num].T);
	else
		spaceCraft[num].M = spaceCraft[num].n * (time + spaceCraft[num].P - spaceCraft[num].T);

	E[0] = spaceCraft[num].M; // E0 = M0 �ٻ�
	for (int i = 0; i < repeat; i++) {
		M[i] = E[i] - (spaceCraft[num].e * sinf(E[i]));
		deltaE = (spaceCraft[num].M - M[i]) / (1 - spaceCraft[num].e * cosf(E[0]));
		
		if(i < repeat-1)
			E[i + 1] = E[i] + deltaE;
	}
	spaceCraft[num].M = M[repeat - 1];
	spaceCraft[num].E = E[repeat - 1];

	spaceCraft[num].preRadius = spaceCraft[num].a * (1 - (spaceCraft[num].e * cosf(spaceCraft[num].E))); // r
	tanHalf_F = sqrtf((1 + spaceCraft[num].e) / (1 - spaceCraft[num].e)) * tanf(spaceCraft[num].E / 2.0f); // tan(f/2)
	spaceCraft[num].preF = 2.0f * atanf(tanHalf_F) * 180.0f / GL_PI; // f
	
	if (spaceCraft[num].preF < 0) {
		spaceCraft[num].preF = 360.0f + spaceCraft[num].preF;
	}
}

void OPenGLRenderer::CreatPreCraft(int num)
{
	preCraft[num] = gluNewQuadric(); // ��ü ��ü �ν��Ͻ� ����
}

// ���� ����ǥ�� �׷��ִ� �Լ�
void OPenGLRenderer::DrawPrediction(int num)
{
	GLfloat preX;
	GLfloat preY;
	
	if (spaceCraft[num].isSelected == 1 && spaceCraft[num].isStarted == 1) {
		if (preCraft[num] != NULL) {
			preX = (GLfloat)cos(spaceCraft[num].preF * GL_PI / 180.0f) * spaceCraft[num].preRadius / 1000.0f;
			preY = (GLfloat)sin(spaceCraft[num].preF * GL_PI / 180.0f) * spaceCraft[num].preRadius / 1000.0f;

			// drawing
			glTranslatef(preX, preY, 0.0f); // ��ü �̵� (�����Ŀ� ���� �� �������� �ٲ��)
			gluQuadricDrawStyle(preCraft[num], GLU_FILL); // ��ü�� ä��� ���·� ����
			glColor3f(0.0f, 0.0f, 0.0f);
			gluSphere(preCraft[num], 0.3f, 12, 12);

			gluQuadricDrawStyle(preCraft[num], GLU_LINE); // ���� �ߴ� ���·� ����
			glColor3f(0.7f, 0.7f, 0.7f);
			gluSphere(preCraft[num], 0.32f, 12, 12);
		}
	}
}

void OPenGLRenderer::unProject(CPoint point) {
	int xCursor, yCursor;
	GLdouble projection[16];
	GLdouble modelView[16];
	GLint viewPort[4];

	xCursor = (int)point.x;
	yCursor = (int)point.y;
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelView);
	glGetIntegerv(GL_VIEWPORT, viewPort);

	GLfloat zCursor, winX, winY;
	winX = (float)xCursor;
	winY = (float)viewPort[3] - (float)yCursor;
	glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &zCursor);
	
	gluUnProject(winX, winY, zCursor, modelView, projection, viewPort, &wx, &wy, &wz);
}


///// user input functions /////
void OPenGLRenderer::OnLButtonDown(UINT nFlags, CPoint point) // Ŭ���ϸ� Ŭ�� ���� ���콺 ��ġ�� ����ȴ�.
{
	GLfloat x;

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	b_Rotate = TRUE; // ȸ����� ����
	x = (GLfloat)point.x;
	mousePoint = x;

	unProject(point);

	CWnd::OnLButtonDown(nFlags, point);
}


void OPenGLRenderer::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	GLfloat x;
	
	x = (GLfloat)point.x;
	if (b_Rotate) {
		differ = x - mousePoint;
	}
	mousePoint = x;
	differ *= 40.0f; // ȸ�� �ΰ���
	zAngle += differ/100.0f;

	if (zAngle > 359.0f || zAngle < -359.0f) zAngle = 0.0f;

	CWnd::OnMouseMove(nFlags, point);
}


void OPenGLRenderer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	b_Rotate = FALSE;
	differ = 0.0f;
	CWnd::OnLButtonUp(nFlags, point);
}


BOOL OPenGLRenderer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) // ȭ�� �ܾƿ�
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	GLfloat temp_zoom;
	temp_zoom = ((GLfloat)zDelta) / 100.0f;

	zoom += temp_zoom;

	if (zoom < -80.0f) zoom = -80.0f;
	if (zoom > -20.0f) zoom = -20.0f;

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
