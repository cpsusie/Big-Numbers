#include "stdafx.h"
#include "testlockrect.h"
#include "testlockrectDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <MFCUtil/WinTools.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx {
public:
  CAboutDlg();

#if defined(AFX_DESIGN_TIME)
  enum { IDD = IDD_ABOUTBOX };
#endif

  protected:
  virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


CtestlockrectDlg::CtestlockrectDlg(CWnd *pParent /*=nullptr*/)
  : CDialogEx(IDD_TESTLOCKRECT_DIALOG, pParent)
{
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CtestlockrectDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CtestlockrectDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_CLOSE()
  ON_BN_CLICKED(IDC_BUTTONSTART, &CtestlockrectDlg::OnClickedButtonstart)
END_MESSAGE_MAP()

BOOL CtestlockrectDlg::OnInitDialog() {
  __super::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != nullptr) {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon

  CWnd *frame = GetDlgItem(IDC_STATICFRAME);
  m_device.attach(*frame);

  return TRUE;
}

void CtestlockrectDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  }
  else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CtestlockrectDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else {
    __super::OnPaint();
  }
}

HCURSOR CtestlockrectDlg::OnQueryDragIcon() {
  return static_cast<HCURSOR>(m_hIcon);
}

void CtestlockrectDlg::OnCancel() {
}

void CtestlockrectDlg::OnClose() {
  OnOK();
}

typedef struct {
  PixRectType type;
  const TCHAR *name;
} TypeName;

typedef struct {
  D3DPOOL      pool;
  const TCHAR *name;
} PoolName;

typedef struct {
  DWORD        flag;
  const TCHAR *name;
} FlagName;

#define ENUMNAME(e) e, _T(#e)

void CtestlockrectDlg::OnClickedButtonstart() {
  const TypeName typeArray[] = {
    ENUMNAME(PIXRECT_PLAINSURFACE)
   ,ENUMNAME(PIXRECT_TEXTURE)
  };

  const PoolName poolArray[] = {
    ENUMNAME(D3DPOOL_DEFAULT)
   ,ENUMNAME(D3DPOOL_MANAGED)
   ,ENUMNAME(D3DPOOL_SYSTEMMEM)
   ,ENUMNAME(D3DPOOL_SCRATCH)
  };

  const FlagName lockFlags[]{
    ENUMNAME(D3DLOCK_READONLY)
   ,ENUMNAME(D3DLOCK_DISCARD)
   ,ENUMNAME(D3DLOCK_NOOVERWRITE)
   ,ENUMNAME(D3DLOCK_NOSYSLOCK)
   ,ENUMNAME(D3DLOCK_DONOTWAIT)
   ,ENUMNAME(D3DLOCK_NO_DIRTY_UPDATE)
  };

  for (int t = 0; t < ARRAYSIZE(typeArray); t++) {
    const TypeName &tn = typeArray[t];
    for (int p = 0; p < ARRAYSIZE(poolArray); p++) {
      const PoolName &pn = poolArray[p];
      try {
        PixRect pr(m_device, tn.type, 100, 100, pn.pool);
        for (int lock = 0; lock < 32; lock++) {
          DWORD flags = 0;
          for (int f = 0; f < ARRAYSIZE(lockFlags); f++) {
            if (lock & (1 << f)) {
              flags |= lockFlags[f].flag;
            }
          }
          try {
            PixelAccessor *pa = pr.getPixelAccessor(flags);
            String fs;
            TCHAR *delim = nullptr;
            for (int f = 0; f < ARRAYSIZE(lockFlags); f++) {
              if (lock & (1 << f)) {
                if (delim) fs += delim; else delim = _T(" ");
                fs += lockFlags[f].name;
              }
            }
            log(_T("success. type=%s, pool=%s, flags=[%s]"), tn.name, pn.name, fs.cstr());
            pr.releasePixelAccessor();
          }
          catch (...) {
          }
        }
      }
      catch (Exception e) {
        log(_T("Create PixRect failed. %s. type=%s, pool=%s"), e.what(), tn.name, pn.name);
      }
    }
  }
}

void CtestlockrectDlg::log(TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String msg = vformat(format, argptr);
  va_end(argptr);
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LIST1);
  lb->AddString(msg.cstr());
}
