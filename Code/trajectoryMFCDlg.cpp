
// trajectoryMFCDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "trajectoryMFC.h"
#include "trajectoryMFCDlg.h"
#include "afxdialogex.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CtrajectoryMFCDlg 대화 상자



CtrajectoryMFCDlg::CtrajectoryMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TRAJECTORYMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtrajectoryMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_pLeft);
}

BEGIN_MESSAGE_MAP(CtrajectoryMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CtrajectoryMFCDlg::OnBnClickedButtonAdd)
END_MESSAGE_MAP()


// CtrajectoryMFCDlg 메시지 처리기

BOOL CtrajectoryMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	SetWindowText(_T("Trajectory Prediction of Two-Body Problem - SHIM EUN SONG")); // Window title 설정
	SetBackgroundColor(RGB(255, 255, 255)); // Window 배경색 설정
	SetTimer(1000, 10, NULL);

	// OpenGL 생성 및 초기화 작업
	//CRect rectLeft;
	int iWidth, iHeight;

	m_pLeft.GetWindowRect(rectLeft);
	iWidth = rectLeft.Width();
	iHeight = rectLeft.Height();

	m_test = new OPenGLRenderer;
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL);
	m_test->CreateEx(0, className, _T("OpenGLCube"), WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, rectLeft, this, 0);
	m_test->ShowWindow(SW_SHOW);

	m_test->CreateGLContext(rectLeft, this);
	m_test->PrepareScene(0, 0, iWidth, iHeight); //will show without this but as black & white.
	m_test->SetTimer(1, 10, 0);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CtrajectoryMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CtrajectoryMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CtrajectoryMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtrajectoryMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CString str;
	if (m_test->numOfCraft >= 1) {
		str.Format(_T("%f"), m_test->spaceCraft[(m_test->numOfCraft) - 1].p); // float -> CString
		SetDlgItemText(IDC_EDIT0, str);

		str.Format(_T("%f"), m_test->spaceCraft[(m_test->numOfCraft)-1].omega); // float -> CString
		SetDlgItemText(IDC_EDIT1, str);

		str.Format(_T("%f"), m_test->spaceCraft[(m_test->numOfCraft) - 1].i); // float -> CString
		SetDlgItemText(IDC_EDIT2, str);

		str.Format(_T("%f"), m_test->spaceCraft[(m_test->numOfCraft) - 1].a); // float -> CString
		SetDlgItemText(IDC_EDIT3, str);

		str.Format(_T("%f"), m_test->spaceCraft[(m_test->numOfCraft) - 1].e); // float -> CString
		SetDlgItemText(IDC_EDIT4, str);

		str.Format(_T("%f"), m_test->spaceCraft[(m_test->numOfCraft) - 1].w); // float -> CString
		SetDlgItemText(IDC_EDIT5, str);

		str.Format(_T("%f"), m_test->spaceCraft[(m_test->numOfCraft) - 1].f); // float -> CString
		SetDlgItemText(IDC_EDIT6, str);
	}
	
	CDialogEx::OnTimer(nIDEvent);
}

void CtrajectoryMFCDlg::OnBnClickedButtonAdd() // ADD 버튼이 클릭되면
{
	GM = m_test->G * m_test->mass_Earth; // 기준을 지구? 태양? 으로 할지 설정

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_test->numOfCraft <= 4) { // 물체는 5개까지 추가가능
		GetDlgItemText(IDC_EDIT_XPOS, xpos); // XPOS 텍스트박스에 적힌 값을 xpos에 저장
		GetDlgItemText(IDC_EDIT_YPOS, ypos);
		GetDlgItemText(IDC_EDIT_ZPOS, zpos);
		GetDlgItemText(IDC_EDIT_XVEL, xvel); // XVEL 텍스트박스에 적힌 값을 xvel에 저장
		GetDlgItemText(IDC_EDIT_YVEL, yvel);
		GetDlgItemText(IDC_EDIT_ZVEL, zvel);

		m_test->spaceCraft[m_test->numOfCraft].xpos = (GLfloat)atof((CStringA)xpos); // CString의 xpos를 GLfloat으로 변환하여 저장
		m_test->spaceCraft[m_test->numOfCraft].ypos = (GLfloat)atof((CStringA)ypos);
		m_test->spaceCraft[m_test->numOfCraft].zpos = (GLfloat)atof((CStringA)zpos);
		m_test->spaceCraft[m_test->numOfCraft].xvel = (GLfloat)atof((CStringA)xvel); // Cstring의 xvel을 GLfloat으로 변환하여 저장
		m_test->spaceCraft[m_test->numOfCraft].yvel = (GLfloat)atof((CStringA)yvel);
		m_test->spaceCraft[m_test->numOfCraft].zvel = (GLfloat)atof((CStringA)zvel);

		CalculateCAndH(m_test->numOfCraft); // C1, C2, C3, h 구하기

		CalculateOmegaAndI(m_test->numOfCraft); // 경사각 i와 승교점 적경 omege 구하기  ////// ***** omega에 심각한 문제가 있나? 어느정도 해결

		CalculateAAndE(m_test->numOfCraft); // 장반경 a와 이심률 e 구하기
		
		CalculateWAndF(m_test->numOfCraft); // 근지점인수 w와 진근지점이각 f 구하기
		
		m_test->CreateCraft(m_test->numOfCraft); // 우주물체 생성
		m_test->numOfCraft++; // 개체수 증가
	}
}

void CtrajectoryMFCDlg::CalculateCAndH(int n) {
	m_test->spaceCraft[n].C1 = (m_test->spaceCraft[n].ypos * m_test->spaceCraft[n].zvel) - (m_test->spaceCraft[n].zpos * m_test->spaceCraft[n].yvel);
	m_test->spaceCraft[n].C2 = (m_test->spaceCraft[n].zpos * m_test->spaceCraft[n].xvel) - (m_test->spaceCraft[n].xpos * m_test->spaceCraft[n].zvel);
	m_test->spaceCraft[n].C3 = (m_test->spaceCraft[n].xpos * m_test->spaceCraft[n].yvel) - (m_test->spaceCraft[n].ypos * m_test->spaceCraft[n].xvel);
	m_test->spaceCraft[n].h = (GLfloat)sqrt(pow(m_test->spaceCraft[n].C1, 2) + pow(m_test->spaceCraft[n].C2, 2) + pow(m_test->spaceCraft[n].C3, 2));
}

void CtrajectoryMFCDlg::CalculateOmegaAndI(int n) {
	radianI = (GLfloat)acos(m_test->spaceCraft[n].C3 / m_test->spaceCraft[n].h);
	m_test->spaceCraft[n].i = (radianI * 180.0f) / m_test->GL_PI;

	if (m_test->spaceCraft[n].C1 != 0) { // C1이 0이 아닌 경우
		temp = (double)m_test->spaceCraft[n].C1 / ((double)m_test->spaceCraft[n].h * sin(radianI));
		if (temp >= 1.0) temp = 1.0; // 여기 보정필요
		radianOmega = asinf(temp);
	}
	else if(m_test->spaceCraft[n].C1 == 0 && m_test->spaceCraft[n].C2 != 0){ //  C2가 0이 아닌 경우
		temp = (double)m_test->spaceCraft[n].C2 / (-(double)m_test->spaceCraft[n].h * sin(radianI));
		if (temp >= 1.0) temp = 1.0; // 여기 보정필요
		radianOmega = acosf(temp);
	}
	else if (m_test->spaceCraft[n].C1 == 0 && m_test->spaceCraft[n].C2 == 0) { // C1, C2 둘다 0인 경우
		radianOmega = 0.0;
	}
	m_test->spaceCraft[n].omega = (GLfloat)((radianOmega)*180.0f/ m_test->GL_PI);
}

void CtrajectoryMFCDlg::CalculateAAndE(int n) { // a e p 구함
	// 초기거리와 초기속도를 구함
	m_test->spaceCraft[n].range = (GLfloat)sqrt(pow(m_test->spaceCraft[n].xpos, 2) + pow(m_test->spaceCraft[n].ypos, 2) + pow(m_test->spaceCraft[n].zpos, 2));
	m_test->spaceCraft[n].velocity = (GLfloat)sqrt(pow(m_test->spaceCraft[n].xvel, 2) + pow(m_test->spaceCraft[n].yvel, 2) + pow(m_test->spaceCraft[n].zvel, 2));
	// 속도벡터의 크기를 이용해서 장반경을 구하기 전에 타원궤도인지 쌍곡선궤도인지 알아야함
	int type = 0;
	type = CheckTrajShape(n);
	if (type == 1) { // 타원궤도
		reciprocal_a = (2.0 / (1000.0 * (double)(m_test->spaceCraft[n].range))) - ((double)pow(1000.0 * m_test->spaceCraft[n].velocity, 2) / GM);// 태양으로 임시설정(지구예제는 나중에) (km->m)
	}
	else { // 쌍곡선궤도
		reciprocal_a = ((double)pow(1000.0 * m_test->spaceCraft[n].velocity, 2) / GM) - (2.0 / (1000.0 * (double)(m_test->spaceCraft[n].range)));// 태양으로 임시설정(지구예제는 나중에) (km->m)
	}

	m_test->spaceCraft[n].a = (GLfloat)(1.0 / reciprocal_a) / 1000.0f; // (m->km)
	squarh = pow((double)m_test->spaceCraft[n].h, 2); // h^2
	square = 1 - (squarh / (GM * pow(10, -9) * m_test->spaceCraft[n].a)); // GM을 pow(10,-9)를 통해 km로 맞춰줌
	m_test->spaceCraft[n].e = (GLfloat)sqrt(square);

	if (type == 1) { // 타원궤도
		m_test->spaceCraft[n].p = m_test->spaceCraft[n].a * (1 - (GLfloat)pow(m_test->spaceCraft[n].e, 2));
	}
	else { // 쌍곡선궤도
		m_test->spaceCraft[n].p = m_test->spaceCraft[n].a * ((GLfloat)pow(m_test->spaceCraft[n].e, 2) - 1);
	}
}

int CtrajectoryMFCDlg::CheckTrajShape(int n) {
	GLfloat escapevelocity = 0.0f;
	int ellipse = 1;
	int hyperbola = 2;

	escapevelocity = (GLfloat)sqrt((2.0 * GM) / (1000.0 * m_test->spaceCraft[n].range));
	escapevelocity /= 1000.0f;

	if (escapevelocity <= m_test->spaceCraft[n].velocity)
		return hyperbola;
	else
		return ellipse;
}

void CtrajectoryMFCDlg::CalculateWAndF(int n) {
	// f 구하기 (f의 사분면을 아직 고려해야함)
	posXvel = m_test->spaceCraft[n].xpos * m_test->spaceCraft[n].xvel + m_test->spaceCraft[n].ypos * m_test->spaceCraft[n].yvel + m_test->spaceCraft[n].zpos * m_test->spaceCraft[n].zvel;
	tanf = (m_test->spaceCraft[n].p / m_test->spaceCraft[n].h) * (posXvel / (m_test->spaceCraft[n].p - m_test->spaceCraft[n].range));
	radianF = atanf(tanf);
	m_test->spaceCraft[n].f = (GLfloat)(radianF * 180.0 / (double)m_test->GL_PI);

	if (m_test->spaceCraft[n].f < 0.0f) { // f가 음수이면 양수화
		m_test->spaceCraft[n].f = 360.0f - m_test->spaceCraft[n].f;
	}

	// u 구하기
	cosu = (m_test->spaceCraft[n].xpos * cos(radianOmega) + m_test->spaceCraft[n].ypos * sin(radianOmega)) / m_test->spaceCraft[n].range;
	radianU = acosf(cosu);
	m_test->spaceCraft[n].u = (GLfloat)(radianU * 180.0 / (double)m_test->GL_PI);

	if (m_test->spaceCraft[n].u < m_test->spaceCraft[n].f) {
		m_test->spaceCraft[n].u += 360.0f;
	}
	// w 구하기
	m_test->spaceCraft[n].w = m_test->spaceCraft[n].u - m_test->spaceCraft[n].f;
}
