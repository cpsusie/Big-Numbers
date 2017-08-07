#include "stdafx.h"
#include <MFCUtil/ColorSpace.h>
#include <ExternProcess.h>
#include "BigText.h"
#include "BigTextDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx {
public:
  CAboutDlg();

#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CBigTextDlg::CBigTextDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(IDD_BIGTEXT_DIALOG, pParent)
  , m_text(_T(""))
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBigTextDlg::DoDataExchange(CDataExchange* pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITTEXT, m_text);
}

BEGIN_MESSAGE_MAP(CBigTextDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_MAKEBIGTEXT, OnClickedMakeBigText)
  ON_WM_CLOSE()
END_MESSAGE_MAP()


BOOL CBigTextDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL)	{
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE );
  SetIcon(m_hIcon, FALSE);

  gotoEditBox(this, IDC_EDITTEXT);
  return FALSE;
}

void CBigTextDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)	{
    CAboutDlg().DoModal();
  }	else {
    __super::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBigTextDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    // Center icon in client rectangle
    int   cxIcon = GetSystemMetrics(SM_CXICON);
    int   cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect   = getClientRect(this);
    int   x      = (rect.Width() - cxIcon + 1) / 2;
    int   y      = (rect.Height() - cyIcon + 1) / 2;
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CBigTextDlg::OnQueryDragIcon() {
  return static_cast<HCURSOR>(m_hIcon);
}

void CBigTextDlg::OnClickedMakeBigText() {
  UpdateData();
  String text = m_text;
  if(text.length() == 0) {
    gotoEditBox(this, IDC_EDITTEXT);
    Message(_T("Must fill text"));
    return;
  }
  CWnd       *e = GetDlgItem(IDC_EDITTEXT);
  CClientDC   dc(e);
  CFont      *font      = GetFont();
  HGDIOBJ     oldFont   = dc.SelectObject(*font);
  StringArray textLines(Tokenizer(text, _T("\n\r")));
  const CSize textSize = getTextExtent1(dc, textLines);
  dc.SelectObject(oldFont);
  HDC         hdc   = CreateCompatibleDC(NULL);
  HBITMAP     bm    = CreateCompatibleBitmap(hdc, textSize.cx, textSize.cy);
  HGDIOBJ     oldBM = SelectObject(hdc, bm);
  CBrush      brush;
  brush.CreateSolidBrush(WHITE);
  CRect       rect(0,0,textSize.cx,textSize.cy);
  FillRect(hdc, &rect, brush);
  oldFont = SelectObject(hdc, *font);
  SetTextColor(hdc,0    );
  SetBkColor(  hdc,WHITE);
  int         y     = 0;
  const int   ch    = (int)(textSize.cy / textLines.size());
  for (size_t l = 0; l < textLines.size(); l++, y += ch) {
    TextOut(hdc, 0,y,textLines[l].cstr(), (int)textLines[l].length());
  }
  CPoint p;
  String bstr;
  SelectObject(hdc, oldFont);
  for(p.y = 0; p.y < textSize.cy; p.y++) {
    for(p.x = 0; p.x < textSize.cx; p.x++) {
      const COLORREF c = GetPixel(hdc,p.x,p.y);
      if(c == WHITE) {
        bstr += _T("....");
      } else {
        bstr += _T("##");
      }
    }
    bstr += '\n';
  }
  const String fileName = _T("c:\\temp\\fisk.txt");
  FILE *f = MKFOPEN(fileName,_T("w"));
  _ftprintf(f, _T("%s"), bstr.cstr());
  fclose(f);


  ExternProcess::run(false, _T("c:\\windows\\system32\\notepad.exe"), fileName.cstr(), NULL);
}

void CBigTextDlg::OnClose() {
  EndDialog(IDOK);
}

void CBigTextDlg::OnCancel() {
}

void CBigTextDlg::OnOK() {
}
