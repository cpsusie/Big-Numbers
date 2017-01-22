#include "stdafx.h"
#include "asciiw.h"
#include "asciiwDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

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

CAsciiwDlg::CAsciiwDlg(CWnd* pParent /*=NULL*/) : CDialog(CAsciiwDlg::IDD, pParent) {
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsciiwDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAsciiwDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    ON_COMMAND(ID_VIEW_HEX, OnViewHex)
    ON_COMMAND(ID_VIEW_DEC, OnViewDec)
    ON_COMMAND(ID_VIEW_FONT, OnViewFont)
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

BOOL CAsciiwDlg::OnInitDialog() {
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

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon
  
  m_currentFont = NULL;
  m_defaultFont.CreateFont(10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH | FF_MODERN,
                         _T("Courier")
                        );
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1));

  setCurrentFont(&m_defaultFont);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDOK    , RELATIVE_POSITION);
  m_layoutManager.addControl(IDCANCEL, RELATIVE_POSITION);

//    memset(table,0,sizeof(table));
//    for(unsigned char ch = 0; ch < 128; ch++)
//      table[ch] = toupper(ch);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAsciiwDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  }
  else {
    CDialog::OnSysCommand(nID, lParam);
  }
}

String formatCh3(int ch) {
  if(ch > 255)
    return "";
  else {
    switch(ch) {

    case 0 : return " ";
//    case 7 : return "bel";
//    case 8 : return "bs ";
    case 9 : return "tab";
    case 10: return "lf ";
    case 13: return "cr ";

    default: 
      return format(_T("%c"), ch);
//      return isprint(ch) ? format("%c", ch) : ".";
    }
  }
}

#define BOXPOSX 10
#define BOXPOSY 20

#define LEFT 5
#define TOP  10
#define XPOS(x) (LEFT+((x)+1)*m_currentFontSize.cx)
#define YPOS(y) (TOP+(y)*m_currentFontSize.cy)
#define CHARSPERLINE(radix) ((radix == 16)?16:20)

#define POS0(r,c) XPOS(c),YPOS(r)
#define POS(r,c)  XPOS(c)+6,YPOS(r)

void CAsciiwDlg::printHeader(CDC &dc, int line, int radix) {
  int charactersPerLine = CHARSPERLINE(radix);
  dc.SelectObject(&m_defaultFont);
  for(int i = 0; i < charactersPerLine; i++) {
    String str;
    switch(radix) {
    case 10:
      str = format(_T("%d"),i % 10);
      break;
    case 16:
      str = format(_T("%X"),i);
      break;
    }
    dc.TextOut(POS(line,i),str.cstr());
  }
}

void CAsciiwDlg::printAscii(CDC &dc1, int radix) {

  CStatic *st = (CStatic*)GetDlgItem(IDC_STATIC);

  int charactersPerLine = CHARSPERLINE(radix);
  TCHAR *form =          (radix == 16) ? _T("%02X") : _T("%3d");

  WINDOWPLACEMENT wp;
  st->GetWindowPlacement(&wp);
  wp.rcNormalPosition.left   = BOXPOSX;
  wp.rcNormalPosition.top    = BOXPOSY;
  wp.rcNormalPosition.right  = BOXPOSX + XPOS(charactersPerLine+1) + 20;
  wp.rcNormalPosition.bottom = BOXPOSY + YPOS(256/(charactersPerLine)+3) + 4;
  st->SetWindowPlacement(&wp);

  int lineCount = 0;
  CClientDC dc(st);
  dc.SelectObject(m_currentFont);
  dc.SetBkColor(RGB(236,233,216));

  printHeader(dc,lineCount++,radix);

  for(int ch = 0; ch < 256;) {
    dc.SelectObject(&m_defaultFont);
    dc.TextOut(POS0(lineCount,-1), format(form, ch).cstr());
    dc.TextOut(POS(lineCount,charactersPerLine), format(form, ch+charactersPerLine-1).cstr());

    dc.SelectObject(m_currentFont);
    for(int j = 0; j < charactersPerLine; j++,ch++) {
      dc.TextOut(POS(lineCount,j),formatCh3(ch).cstr());
    }

    lineCount++;
  }
  printHeader(dc,lineCount++,radix);
}

void CAsciiwDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    CPaintDC dc(this);
    COLORREF color = dc.GetBkColor();
    if(isHexChecked()) {
      printAscii(dc, 16);
    } else {
      printAscii(dc, 10);
    }
    CDialog::OnPaint();
  }
}

HCURSOR CAsciiwDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

void CAsciiwDlg::OnFileExit() {
  exit(0);  
}

bool CAsciiwDlg::isHexChecked() {
  return isMenuItemChecked(this,ID_VIEW_HEX);
}

void CAsciiwDlg::OnViewHex() {
  checkMenuItem(this,ID_VIEW_DEC,false);
  checkMenuItem(this,ID_VIEW_HEX,true );
  Invalidate(true);
}

void CAsciiwDlg::OnViewDec() {
  checkMenuItem(this,ID_VIEW_DEC,true );
  checkMenuItem(this,ID_VIEW_HEX,false);
  Invalidate(true);
}

void CAsciiwDlg::setCurrentFont(CFont *newfont) {
  if(m_currentFont && m_currentFont != &m_defaultFont) {
    m_currentFont->DeleteObject();
    delete m_currentFont;
  }

  m_currentFont = newfont;
  CClientDC dc(this);
  dc.SelectObject(m_currentFont);
  int buf[256];
  dc.GetOutputCharWidth(0,255,buf);
  int m = buf[0];
  for(int i = 1; i < 256; i++)
    if(buf[i] > m) m = buf[i];

  CSize cs = dc.GetTextExtent(_T("bel"),3);
  if(cs.cx > m)
    m = cs.cx;
  cs = dc.GetTextExtent(_T("tab"),3);
  if(cs.cx > m) m = cs.cx;

  LOGFONT lf;
  newfont->GetLogFont(&lf);
  m_currentFontSize.cx = m + 5;
  m_currentFontSize.cy = abs(lf.lfHeight) + 3;

  TCHAR *s = lf.lfFaceName;
  TCHAR ch = lf.lfCharSet;

  SetWindowText(format(_T("Ascii (%s)"),s).cstr());
  Invalidate(true);
}

void CAsciiwDlg::OnViewFont() {
  LOGFONT curlf;
  m_currentFont->GetLogFont(&curlf);
  CFontDialog  dlg(&curlf);
  if(dlg.DoModal() == IDOK) {
    LOGFONT lf;
    dlg.GetCurrentFont(&lf);
    CFont *newfont = new CFont;
    newfont->CreateFontIndirect(&lf);

    setCurrentFont(newfont);
  }
}

BOOL CAsciiwDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg))
    return true;
    
  return CDialog::PreTranslateMessage(pMsg);
}

void CAsciiwDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType,cx,cy);    
  if(!IsWindowVisible()) {
    Invalidate();
  }
}
