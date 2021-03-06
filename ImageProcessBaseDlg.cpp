
// ImageProcessBaseDlg.cpp : 구현 파일
//


#include "stdafx.h"
#include "ImageProcessBase.h"
#include "ImageProcessBaseDlg.h"
#include "afxdialogex.h"

#define RED  0
#define GREEN 1
#define BLUE 2
#define CLIP(x) (((x) <0)?0:(((x)>255)?255:(x)))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BITMAPINFO BmInfo;
LPBYTE pImgBuffer;


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


// CImageProcessBaseDlg 대화 상자



CImageProcessBaseDlg::CImageProcessBaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_IMAGEPROCESSBASE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImageProcessBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageProcessBaseDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CImageProcessBaseDlg 메시지 처리기

BOOL CImageProcessBaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	RECT m_Rect = { 0, 0, 640, 480 };     // 생성하고 싶은 사이즈를 RECT 변수에 초기화. 

	AdjustWindowRect(&m_Rect, WS_OVERLAPPEDWINDOW, FALSE);
	int width = m_Rect.right - m_Rect.left;
	int height = m_Rect.bottom - m_Rect.top;
	this->SetWindowPos(NULL, 0, 0, width, height, SWP_NOSIZE);

	// 윈도우 생성
	m_Cap = capCreateCaptureWindow(TEXT("Image Test"), WS_CHILD
		| WS_VISIBLE, 0, 0, 640, 480, this->m_hWnd, NULL);

	// 콜백함수 지정
	if (capSetCallbackOnFrame(m_Cap, CallbackOnFrame) == FALSE) {
		return FALSE;
	}

	// 카메라 드라이버와 연결
	if (capDriverConnect(m_Cap, 0) == FALSE) {
		return FALSE;
	}

	capPreviewRate(m_Cap, 33);    // 초당 프레임 지정
	capOverlay(m_Cap, false);
	capPreview(m_Cap, true);        // 미리보기 기능 설정

	if (BmInfo.bmiHeader.biBitCount != 24) {

		BmInfo.bmiHeader.biBitCount = 24;
		BmInfo.bmiHeader.biCompression = 0;
		BmInfo.bmiHeader.biSizeImage = BmInfo.bmiHeader.biWidth * BmInfo.bmiHeader.biHeight * 3;

	
		capGetVideoFormat(m_Cap, &BmInfo, sizeof(BITMAPINFO));
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CImageProcessBaseDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CImageProcessBaseDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CImageProcessBaseDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImageProcessBaseDlg::OnDestroy()
{
	CDialog::OnDestroy();

	capDriverDisconnect(m_Cap);

	if (pImgBuffer != NULL) {
		delete[] pImgBuffer;
	}
}

LRESULT CALLBACK CallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr)
{

	unsigned int uiBuflen = lpVHdr->dwBufferLength;
	unsigned char RGB[485][645][3] = { 0, };
	unsigned int nWidth, nHeight;
	unsigned int i, j;
	int Y0, U, Y1, V;

	nWidth = 640;
	nHeight = 480;


	// YUY2 ---> RGB
	for (j = 0; j < nHeight; j++) { // height
		for (i = 0; i < nWidth; i += 2) { //width
			Y0 = lpVHdr->lpData[(nWidth*j + i) * 2];
			U = lpVHdr->lpData[(nWidth*j + i) * 2 + 1];
			Y1 = lpVHdr->lpData[(nWidth*j + i) * 2 + 2];
			V = lpVHdr->lpData[(nWidth*j + i) * 2 + 3];

			RGB[j][i][RED] = (int)CLIP(Y0 + (1.4075*(V - 128)));
			RGB[j][i][GREEN] = (int)CLIP(Y0 - 0.3455*(U - 128) - 0.7169*(V - 128));
			RGB[j][i][BLUE] = (int)CLIP(Y0 + 1.7790*(U - 128));
			RGB[j][i + 1][RED] = (int)CLIP(Y1 + (1.4075*(V - 128)));
			RGB[j][i + 1][GREEN] = (int)CLIP(Y1 - 0.3455*(U - 128) - 0.7169*(V - 128));
			RGB[j][i + 1][BLUE] = (int)CLIP(Y1 + 1.7790*(U - 128));
		}
	}

/////////////////////////////////////////////////////////////////////////////////
	// RGB영상처리부분

	for (j = 0; j < nHeight; j++) { 
		for (i = 0; i < nWidth; i++) { 
			if (RGB[j][i][RED] < 50 && RGB[j][i][BLUE] < 50 && RGB[j][i][GREEN] < 50)
			RGB[j][i][RED] = RGB[j][i][GREEN] = RGB[j][i][BLUE] = 255;
			}
		}



//////////////////////////////////////////////////////////////////////////////////

// RGB ---> YUY2 

	for (j = 0; j < nHeight; j++) { // height
		for (i = 0; i < nWidth; i += 2) { //width

			Y0 = (int)CLIP(0.2999*RGB[j][i][RED] + 0.587*RGB[j][i][GREEN] + 0.114*RGB[j][i][BLUE]);
			Y1 = (int)CLIP(0.2999*RGB[j][i + 1][RED] + 0.587*RGB[j][i + 1][GREEN] + 0.114*RGB[j][i + 1][BLUE]);
			U = (int)CLIP(-0.1687*RGB[j][i][RED] - 0.3313*RGB[j][i][GREEN] + 0.5*RGB[j][i][BLUE] + 128.0);
			V = (int)CLIP(0.5*RGB[j][i][RED] - 0.4187*RGB[j][i][GREEN] - 0.0813*RGB[j][i][BLUE] + 128.0);

			lpVHdr->lpData[(nWidth*j + i) * 2] = Y0;
			lpVHdr->lpData[(nWidth*j + i) * 2 + 1] = U;
			lpVHdr->lpData[(nWidth*j + i) * 2 + 2] = Y1;
			lpVHdr->lpData[(nWidth*j + i) * 2 + 3] = V;

		}
	}
	return (LRESULT)true;
}


