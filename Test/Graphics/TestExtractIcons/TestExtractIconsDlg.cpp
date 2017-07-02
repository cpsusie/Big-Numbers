#include "stdafx.h"
#include <FileNameSplitter.h>
#include "TestExtractIconsDlg.h"

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

CTestExtractIconsDlg::CTestExtractIconsDlg(CWnd *pParent) : CDialog(CTestExtractIconsDlg::IDD, pParent) {
    m_showLargeIcons = FALSE;
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestExtractIconsDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK_SHOW_LARGE, m_showLargeIcons);
}

BEGIN_MESSAGE_MAP(CTestExtractIconsDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_OPEN     , OnButtonOpen    )
    ON_BN_CLICKED(IDC_CHECK_SHOW_LARGE, OnCheckShowLarge)
END_MESSAGE_MAP()

BOOL CTestExtractIconsDlg::OnInitDialog() {
    __super::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu *pSysMenu = GetSystemMenu(FALSE);
    if(pSysMenu != NULL) {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if(!strAboutMenu.IsEmpty()) {
        pSysMenu->AppendMenu(MF_SEPARATOR);
        pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_layoutManager.OnInitDialog(this);
    m_layoutManager.addControl(IDC_STATIC_ICONFRAME, RELATIVE_SIZE    );
    m_layoutManager.addControl(IDC_BUTTON_OPEN     , RELATIVE_POSITION);
    m_layoutManager.addControl(IDCANCEL            , RELATIVE_POSITION);

    return TRUE;
}

void CTestExtractIconsDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestExtractIconsDlg::OnPaint()  {
  if (IsIconic()){
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    CWnd *frame = GetDlgItem(IDC_STATIC_ICONFRAME);
    CClientDC dc(frame);
    const CRect cl = getClientRect(frame);

    CPoint p(0, 0);
    int maxHeight = 0;
    const CompactArray<HICON> &iconArray = m_showLargeIcons ? m_largeIcons :m_smallIcons;

    for(size_t i = 0; i < iconArray .size(); i++) {
      HICON       icon = iconArray[i];
      const CSize sz   = getIconSize(icon);
      if(p.x + sz.cx > cl.Width()) {
        p.y      += maxHeight + 3;
        p.x       = 0;
        maxHeight = 0;
      }
      DrawIconEx(dc, p.x, p.y, icon, 0, 0, 0, NULL, DI_NORMAL);
      p.x += sz.cx + 3;
      maxHeight = max(sz.cy, maxHeight);
    }
  }
}

HCURSOR CTestExtractIconsDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

static StringArray getFileNames(const TCHAR *fileNames) {
  StringArray result;
  const TCHAR *dir = fileNames;
  if(*dir == 0) {
    return result;
  }
  bool addCount = 0;
  for(const TCHAR *cp = dir + _tcslen(dir)+1; *cp; cp += _tcslen(cp) + 1) {
    result.add(FileNameSplitter::getChildName(dir, cp));
    addCount++;
  }
  if(addCount == 0) {
    result.add(dir);
  }
  return result;
}

void CTestExtractIconsDlg::OnButtonOpen() {
  static const TCHAR *loadFileDialogExtensions = _T("Image files\0*.exe;*.dll;*.ico;\0"
                                                    "EXE files\0*.exe;\0"
                                                    "DLL-Files (*.dll)\0*.dll;\0"
                                                    "ICO-files (*.ico)\0*.ico;\0"
                                                    "All files (*.*)\0*.*\0\0");
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = loadFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");
  dlg.m_ofn.Flags |=  OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;
  TCHAR fileNameBuffer[0x10000];
  memset(fileNameBuffer,0,sizeof(fileNameBuffer));
  fileNameBuffer[ARRAYSIZE(fileNameBuffer)-1] = 0;
  dlg.m_ofn.lpstrFile = fileNameBuffer;
  dlg.m_ofn.nMaxFile  = ARRAYSIZE(fileNameBuffer);

  if((dlg.DoModal() != IDOK) || (_tcslen(fileNameBuffer) == 0) ) {
    return;
  }
  destroyAllIcons();
  bool              anyAdded  = false;
  const StringArray fileNames = getFileNames(fileNameBuffer);
  for(size_t i = 0; i < fileNames.size(); i++) {
    const String &name = fileNames[i];
    int n = ExtractIconEx(name.cstr(), -1, NULL, NULL, 0);
    if(n > 0) {
      CompactArray<HICON> largeIcons, smallIcons;
      for(int k = 0; k < n; k++) {
        largeIcons.add(NULL);
        smallIcons.add(NULL);
      }
      if(n + m_largeIcons.size() + m_smallIcons.size() > 8000) {
        MessageBox(format(_T("Too many icons to load. max is 9000")).cstr(), _T("Warning"), MB_ICONWARNING);
        break;
      }
      n = ExtractIconEx(name.cstr(), 0, &largeIcons[0], &smallIcons[0], n);
      if(n > 0) {
        m_largeIcons.addAll(largeIcons);
        m_smallIcons.addAll(smallIcons);
        anyAdded = true;
      }
    }
  }
  if(anyAdded) {
    Invalidate();
  }
}

void CTestExtractIconsDlg::destroyAllIcons() {
  for(size_t i = 0; i < m_largeIcons.size(); i++) {
    DestroyIcon(m_largeIcons[i]);
  }
  for(size_t i = 0; i < m_smallIcons.size(); i++) {
    DestroyIcon(m_smallIcons[i]);
  }
  m_largeIcons.clear();
  m_smallIcons.clear();
}

void CTestExtractIconsDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CTestExtractIconsDlg::OnCheckShowLarge() {
  UpdateData();
  Invalidate();
}

