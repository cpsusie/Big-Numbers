#include "stdafx.h"
#include "TestDirect3D.h"
#include "TestDirect3DDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestDirect3DDlg::CTestDirect3DDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestDirect3DDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestDirect3DDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDirect3DDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZING()
END_MESSAGE_MAP()

BOOL CTestDirect3DDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  Image::init(*this);
  m_boardImage       = new Image(IDB_BITMAP_BOARD);
  m_blackKnightImage = new Image(IDB_BITMAP_BLACKKNIGHT, RESOURCE_BITMAP, true);

  return TRUE;
}

void CTestDirect3DDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestDirect3DDlg::OnPaint()  {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();

    Image::beginScene();

    Image tmpImage(m_boardImage->getSize(), true);
    m_boardImage->paint(tmpImage, ORIGIN);
    tmpImage.paintSolidRect(CRect(0,0,100,100), D3DCOLOR_ARGB(0,255,0,0));
    m_blackKnightImage->paint(tmpImage, CPoint(100,100));

    Image::endScene();

    tmpImage.render();
  }
}

HCURSOR CTestDirect3DDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestDirect3DDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  __super::OnSizing(fwSide, pRect);
  Invalidate(FALSE);
}
