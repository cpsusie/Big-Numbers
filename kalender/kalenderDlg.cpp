#include "stdafx.h"
#include "kalenderDlg.h"
#include "AfstandDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MIN_YEAR 1582
#define MAX_YEAR 4099

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }

protected:
  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CKalenderDlg::CKalenderDlg(CWnd *pParent) : CDialog(IDD, pParent) {
  m_year = 0;
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CKalenderDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITAAR, m_year);
  DDV_MinMaxInt(pDX, m_year, MIN_YEAR, MAX_YEAR);
}

BEGIN_MESSAGE_MAP(CKalenderDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINYEAR, OnDeltaposSpinyear)
  ON_COMMAND(ID_DAGE_AFSTAND, OnDageAfstand)
END_MESSAGE_MAP()

BOOL CKalenderDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty()) {
          pSysMenu->AppendMenu(MF_SEPARATOR);
          pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_PrintFont.CreateFont(12, 10, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                        ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                        ,DEFAULT_PITCH | FF_MODERN
                        ,_T("Courier New") );

  Date now;
  m_year = now.getYear();
  UpdateData(FALSE);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKalenderDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  }
  else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CKalenderDlg::OnPaint() {
    if(IsIconic()) {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
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
        CClientDC dc(this);
        dc.SelectObject(&m_PrintFont);
        if(m_year >= MIN_YEAR && m_year <= MAX_YEAR) {
          printhelligdage(dc,m_year);
        }
    }
}

HCURSOR CKalenderDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

static const TCHAR *dayname[] = {
  _T("mandag")
 ,_T("tirsdag")
 ,_T("onsdag")
 ,_T("torsdag")
 ,_T("fredag")
 ,_T("lørdag")
 ,_T("søndag")
};

void CKalenderDlg::printhelligdag(CClientDC &dc, int row, const String &name, const Date &d) {
  int dd,mm,yyyy;
  d.getDMY(dd,mm,yyyy);
  String tmp = format(_T("%-21s:%-7s - %02d/%02d/%d"),name.cstr(),dayname[d.getWeekDay()],dd,mm,yyyy);
  dc.TextOut(50,row*12+100,tmp.cstr());
}

void CKalenderDlg::printhelligdage(CClientDC &dc, int year) {
  Date paaske = Date::getEaster(year);

  printhelligdag(dc, 0,_T("Nytårsdag"           ), Date(1, 1 ,year ));
  printhelligdag(dc, 1,_T("Skærtorsdag"         ), Date(paaske - 3 ));
  printhelligdag(dc, 2,_T("Langfredag"          ), Date(paaske - 2 ));
  printhelligdag(dc, 3,_T("Påskedag"            ), paaske);
  printhelligdag(dc, 4,_T("2. påskedag"         ), Date(paaske + 1 ));
  printhelligdag(dc, 5,_T("Storebededag"        ), Date(paaske + 26));
  printhelligdag(dc, 6,_T("Kristihimmelfartsdag"), Date(paaske + 39));
  printhelligdag(dc, 7,_T("Pinsedag"            ), Date(paaske + 49));
  printhelligdag(dc, 8,_T("2. pinsedag"         ), Date(paaske + 50));
  printhelligdag(dc, 9,_T("Grundlovsdag"        ), Date(5 ,6 ,year ));
  printhelligdag(dc,10,_T("Juleaften"           ), Date(24,12,year ));
  printhelligdag(dc,11,_T("1. juledag"          ), Date(25,12,year ));
  printhelligdag(dc,12,_T("2. juledag"          ), Date(26,12,year ));
  printhelligdag(dc,13,_T("Nytårsaften"         ), Date(31,12,year ));
}

void CKalenderDlg::OnOK() {
  if(UpdateData()) {
    Invalidate();
  }
}

void CKalenderDlg::OnCancel() {
    __super::OnCancel();
}

void CKalenderDlg::OnDeltaposSpinyear(NMHDR *pNMHDR, LRESULT *pResult) {
    NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

    int u = pNMUpDown->iDelta;
    m_year -= u;
    if(m_year > MAX_YEAR) {
      m_year = MAX_YEAR;
    } else if(m_year < MIN_YEAR) {
      m_year = MIN_YEAR;
    }
    UpdateData(false);
    Invalidate();
    *pResult = 0;
}

void CKalenderDlg::OnDageAfstand() {
  CAfstandDlg dlg;
  dlg.DoModal();
}

BOOL CKalenderDlg::PreTranslateMessage(MSG *pMsg) {
  if(pMsg->message == WM_KEYDOWN) {
    switch(pMsg->wParam) {
      case VK_PRIOR:
        if(m_year <= MIN_YEAR) break;
        m_year--;
        UpdateData(false);
        Invalidate();
        break;
      case VK_NEXT:
        if(m_year >= MAX_YEAR) break;
        m_year++;
        UpdateData(false);
        Invalidate();
        break;
    }
  }

  return __super::PreTranslateMessage(pMsg);
}
