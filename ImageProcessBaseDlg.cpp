
// ImageProcessBaseDlg.cpp : ���� ����
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


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CImageProcessBaseDlg ��ȭ ����



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


// CImageProcessBaseDlg �޽��� ó����

BOOL CImageProcessBaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ���� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	RECT m_Rect = { 0, 0, 640, 480 };     // �����ϰ� ���� ����� RECT ������ �ʱ�ȭ. 

	AdjustWindowRect(&m_Rect, WS_OVERLAPPEDWINDOW, FALSE);
	int width = m_Rect.right - m_Rect.left;
	int height = m_Rect.bottom - m_Rect.top;
	this->SetWindowPos(NULL, 0, 0, width, height, SWP_NOSIZE);

	// ������ ����
	m_Cap = capCreateCaptureWindow(TEXT("Image Test"), WS_CHILD
		| WS_VISIBLE, 0, 0, 640, 480, this->m_hWnd, NULL);

	// �ݹ��Լ� ����
	if (capSetCallbackOnFrame(m_Cap, CallbackOnFrame) == FALSE) {
		return FALSE;
	}

	// ī�޶� ����̹��� ����
	if (capDriverConnect(m_Cap, 0) == FALSE) {
		return FALSE;
	}

	capPreviewRate(m_Cap, 33);    // �ʴ� ������ ����
	capOverlay(m_Cap, false);
	capPreview(m_Cap, true);        // �̸����� ��� ����

	if (BmInfo.bmiHeader.biBitCount != 24) {

		BmInfo.bmiHeader.biBitCount = 24;
		BmInfo.bmiHeader.biCompression = 0;
		BmInfo.bmiHeader.biSizeImage = BmInfo.bmiHeader.biWidth * BmInfo.bmiHeader.biHeight * 3;

	
		capGetVideoFormat(m_Cap, &BmInfo, sizeof(BITMAPINFO));
	}

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CImageProcessBaseDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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
	// RGB����ó���κ�

	int gray;
	for (j = 0; j < nHeight; j++) {
		for (i = 0; i < nWidth; i++) {
			gray = (RGB[j][i][RED] + RGB[j][i][BLUE] + RGB[j][i][GREEN]) / 3;
			RGB[j][i][RED] = RGB[j][i][BLUE] = RGB[j][i][GREEN] = gray;
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

