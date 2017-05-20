#include "stdafx.h"
#include "TestDirection.h"
#include "TestDirectionDlg.h"

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
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestDirectionDlg::CTestDirectionDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestDirectionDlg::IDD, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDirectionDlg::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDirectionDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

BOOL CTestDirectionDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon
  
  return TRUE;
}

void CTestDirectionDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

void CTestDirectionDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CDialog::OnPaint();
  }
}

void CTestDirectionDlg::line(CPoint from, CPoint to, COLORREF color) {
  CClientDC dc(this);
  CPen pen;
  pen.CreatePen(PS_SOLID, 1, color);
  dc.SelectObject(&pen);
  dc.MoveTo(from);
  dc.LineTo(to);
}

HCURSOR CTestDirectionDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

#define WHITE RGB(255,255,255)
#define BLACK RGB(0,0,0)

void CTestDirectionDlg::setMessage(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  const String msg = vformat(format,argptr);
  va_end(argptr);
  setWindowText(this, IDC_STATICMSG,msg);
}

void CTestDirectionDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  m_mouseDown = point;
  m_lastMouse = point;
  CDialog::OnLButtonDown(nFlags, point);
}

void CTestDirectionDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  line(m_mouseDown, m_lastMouse, WHITE);
  CDialog::OnLButtonUp(nFlags, point);
}

String getArrowDirection(const Point2D &vector) {
  double theta = RAD2GRAD(atan2(vector.y, vector.x));
  if(theta >= 0) {
    if(theta <= 22.5) {
      return _T("E");
    } else if(theta <= 67.5 ) {
      return _T("SE");
    } else if(theta <= 112.5) {
      return _T("S");
    } else if(theta <= 157.5) {
      return _T("SW");
    } else {
      return _T("W");
    }
  } else { // theta < 0
    if(theta >= -22.5) {
      return _T("E");
    } else if(theta >= -67.5 ) {
      return _T("NE");
    } else if(theta >= -112.5) {
      return _T("N");
    } else if(theta >= -157.5) {
      return _T("NW");
    } else {
      return _T("W");
    }
  }
}

void CTestDirectionDlg::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON) {
    if(m_lastMouse != m_mouseDown) {
      line(m_mouseDown, m_lastMouse, WHITE);
    }
    m_lastMouse = point;
    if(m_lastMouse != m_mouseDown) {
      line(m_mouseDown, m_lastMouse, BLACK);
    }
  }
  Point2DP v = m_lastMouse - m_mouseDown;
  if(v.length() != 0) {
    double degree = RAD2GRAD(atan2(v.y,v.x));
    setMessage(_T("%.2lf degree %s"), degree, getArrowDirection(v).cstr());
  }
  CDialog::OnMouseMove(nFlags, point);
}

