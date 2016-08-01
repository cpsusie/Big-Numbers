#include "stdafx.h"
#include "AvlTreeWnd.h"
#include "AvlTreeWndDlg.h"
#include "GetIntDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

  public:
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CAvlTreeWndDlg::CAvlTreeWndDlg(CWnd* pParent) : CDialog(CAvlTreeWndDlg::IDD, pParent) {
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAvlTreeWndDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAvlTreeWndDlg, CDialog)
  ON_WM_SIZE()
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_COMMAND(ID_EDIT_INSERT, OnEditInsert)
  ON_COMMAND(ID_EDIT_DELETEKEY, OnEditDeletekey)
  ON_COMMAND(ID_EDIT_SEARCHKEY, OnEditSearchkey)
  ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
  ON_COMMAND(ID_FILE_EXIT, OnFileExit)
  ON_COMMAND(ID_OPTIONS_TRACE, OnOptionsTrace)
  ON_COMMAND(ID_OPTIONS_SHOWBALANCE, OnOptionsShowbalance)
  ON_COMMAND(ID_EDIT_INSERTMANY, OnEditInsertmany)
  ON_COMMAND(ID_EDIT_FIBONACCITREE_TYPE1, OnEditFibonaccitreeType1)
  ON_COMMAND(ID_EDIT_FIBONACCITREE_TYPE2, OnEditFibonaccitreeType2)
  ON_WM_CLOSE()
  ON_COMMAND(ID_HELP_ABOUTAVTREE, OnHelpAboutavtree)
  ON_WM_CONTEXTMENU()
  ON_COMMAND(ID_MENU_DELETE, OnMenuDelete)
END_MESSAGE_MAP()

BOOL CAvlTreeWndDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
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

  SetIcon(m_hIcon, TRUE);       // Set big icon
  SetIcon(m_hIcon, FALSE);      // Set small icon

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_TREEFRAME, RELATIVE_SIZE                  );
  m_layoutManager.addControl(IDC_MESSAGE  , RELATIVE_Y_POS | RELATIVE_WIDTH);

  m_testTree.setAvlGraphics(this);
  m_itererator = new AvlIterator(m_testTree);
  
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_MAINDIALOG));
  m_font.CreateFont(10, 6, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                   ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                   ,DEFAULT_PITCH | FF_MODERN
                   ,_T("Courier")
                   );
  SetFont(&m_font);
  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAvlTreeWndDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)   {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAvlTreeWndDlg::OnPaint() {
  if(IsIconic())    {
    CPaintDC dc(this); // device context for painting

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
    CDialog::OnPaint();
    m_testTree.paint();
  }
}

void CAvlTreeWndDlg::vmessage(COLORREF color, const TCHAR *format, va_list argptr) {
  GetDlgItem(IDC_MESSAGE)->SetWindowText(vformat(format,argptr).cstr());
}

CRect CAvlTreeWndDlg::getTreeRect() {
  CRect r;
  GetDlgItem(IDC_TREEFRAME)->GetClientRect(r);
  return r;
}

CWnd *CAvlTreeWndDlg::getWindow() {
  return GetDlgItem(IDC_TREEFRAME);
}

CSize CAvlTreeWndDlg::getCharSize() {
  CSize size;
  size.cx = 8;
  size.cy = 10;
  return size;
}

HCURSOR CAvlTreeWndDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

void CAvlTreeWndDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType,cx,cy);
  CDialog::OnSize(nType, cx, cy);
}

BOOL CAvlTreeWndDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

void CAvlTreeWndDlg::OnOK() {
}

void CAvlTreeWndDlg::OnCancel() {
}

void CAvlTreeWndDlg::OnClose() {
  exit(0);
}

void CAvlTreeWndDlg::OnFileExit() {
  exit(0);
}

void CAvlTreeWndDlg::OnEditInsert() {
  CGetIntDlg dlg(_T("Insert key"), _T("Enter key:"));
  if(dlg.DoModal() == IDOK) {
    int key = dlg.getValue();
    m_testTree.insert(key);
    Invalidate();
  }
}

void CAvlTreeWndDlg::OnEditDeletekey() {
  CGetIntDlg dlg(_T("Remove key"), _T("Enter key:"));
  if(dlg.DoModal() == IDOK) {
    int key = dlg.getValue();
    m_testTree.remove(key);
    Invalidate();
  }
}

void CAvlTreeWndDlg::OnEditSearchkey() {
}

void CAvlTreeWndDlg::OnEditClear() {
  m_testTree.clear();
  Invalidate();
}

void CAvlTreeWndDlg::OnOptionsTrace() {
  m_testTree.setTracing(toggleMenuItem(this,ID_OPTIONS_TRACE));
  if(m_testTree.isTracing() && !isMenuItemChecked(this,ID_OPTIONS_SHOWBALANCE)) {
    OnOptionsShowbalance();
  }
}

void CAvlTreeWndDlg::OnOptionsShowbalance() {
  m_testTree.setBalanceVisible(toggleMenuItem(this,ID_OPTIONS_SHOWBALANCE));
  Invalidate(false);
}

void CAvlTreeWndDlg::OnEditInsertmany() {
  CGetIntDlg dlg(_T("Insert many"), _T("Enter number of keys:"), &IntInterval(0,300));
  if(dlg.DoModal() == IDOK) {
    int count = dlg.getValue();
    int start = (m_testTree.getLast() == NULL) ? 0 : *m_testTree.getLast();
    for(int key = (int)trunc(start + 10,-1); count >= 0; count--, key += 10) {
      m_testTree.insert(key);
    }
    Invalidate(false);
  }
}

void CAvlTreeWndDlg::OnEditFibonaccitreeType1() {
  CGetIntDlg dlg(_T("Fiboncci tree type 1"), _T("Enter height:"), &IntInterval(1,12), 2);
  if(dlg.DoModal() == IDOK) {
    int height = dlg.getValue();
    m_testTree.makeFibonacciTree(height,1);
    Invalidate();
  }
}

void CAvlTreeWndDlg::OnEditFibonaccitreeType2() {
  CGetIntDlg dlg(_T("Fiboncci tree type 2"), _T("Enter height:"), &IntInterval(1,12), 2);
  if(dlg.DoModal() == IDOK) {
    int height = dlg.getValue();
    m_testTree.makeFibonacciTree(height,2);
    Invalidate();
  }
}

void CAvlTreeWndDlg::OnHelpAboutavtree() {
  CAboutDlg dlg;
  dlg.DoModal();
}

void CAvlTreeWndDlg::OnContextMenu(CWnd* pWnd, CPoint point) {
  m_selectedNode = m_testTree.findNodeByPosition(point);
  if(m_selectedNode == NULL) {
    return;
  }
  CMenu menu;
  int ret = menu.LoadMenu(IDR_CONTEXTMENU);
  if(!ret) {
    AfxMessageBox(_T("Loadmenu failed"));
    return;
  }
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
}

void CAvlTreeWndDlg::OnMenuDelete() {
  m_testTree.remove(m_selectedNode->m_key);
  Invalidate();
}
