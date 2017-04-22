#include "pch.h"
#include <Tokenizer.h>
#include <MFCUtil/ConfirmDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CConfirmDlg::CConfirmDlg(const TCHAR *message, const TCHAR *caption, UINT nType, CWnd* pParent /*=NULL*/)
: m_message(message)
, m_caption(caption)
, m_nType(nType)
, CDialog(CConfirmDlg::IDD, pParent)
{
  m_icon            = NULL;
  m_hasCancelButton = false;
  m_accelTable      = NULL;
  m_dontShowAgain = FALSE;
}


void CConfirmDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, _IDC_CHECKDONTSHOWAGAIN, m_dontShowAgain);
}


BEGIN_MESSAGE_MAP(CConfirmDlg, CDialog)
  ON_BN_CLICKED(_IDC_BUTTON0, OnButton0)
  ON_BN_CLICKED(_IDC_BUTTON1, OnButton1)
  ON_BN_CLICKED(_IDC_BUTTON2, OnButton2)
	ON_WM_PAINT()
END_MESSAGE_MAP()

#define LEFTMARG          25
#define RIGHTMARG         25
#define TOPMARG           25
#define BOTTOMMARG        10
#define GAPICONTEXT       10
#define GAPTEXTBUTTON     25
#define BUTTONAREAHEIGHT  50
#define GAPBUTTON         10

BOOL CConfirmDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  setControlText(IDD, this);
  CWnd *messageArea = GetDlgItem(_IDC_STATICMESSAGEAREA);
  m_msgCtrl.Create(m_message, 0, CRect(58,47,99,62), messageArea, _IDC_STATICTEXT);
  m_msgCtrl.SetFont(GetFont(), FALSE);

  if(m_caption) {
    SetWindowText(m_caption);
  }
  CClientDC dc(&m_msgCtrl);
  String tmp = m_message;
  StringArray lineArray(Tokenizer(tmp, _T("\n")));

  CSize textSize(0,0);
  for(size_t i = 0; i < lineArray.size(); i++) {
    const String &line = lineArray[i];
    const CSize lineSize = dc.GetTextExtent(line.cstr(), (int)line.length());
    if(lineSize.cx > textSize.cx) {
      textSize.cx = lineSize.cx;
    }
    textSize.cy += lineSize.cy;
  }

  m_msgCtrl.SetWindowText(tmp.cstr());
  setClientRectSize(&m_msgCtrl, textSize);

  String butStr[3];

  switch(m_nType & MB_TYPEMASK) {
  case MB_OK                       :
    butStr[0] = loadString(_IDS_OK      );  m_buttonResult[0] = IDOK;
    m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(_IDR_CONFIRM_ACCELERATOR));
    break;
  case MB_OKCANCEL                 :
    butStr[0] = loadString(_IDS_OK      );  m_buttonResult[0] = IDOK;
    butStr[1] = loadString(_IDS_CANCEL  );  m_buttonResult[1] = IDCANCEL;
    m_hasCancelButton = true;
    break;
  case MB_ABORTRETRYIGNORE         :
    butStr[0] = loadString(_IDS_ABORT   );  m_buttonResult[0] = IDABORT;
    butStr[1] = loadString(_IDS_RETRY   );  m_buttonResult[1] = IDRETRY;
    butStr[2] = loadString(_IDS_IGNORE  );  m_buttonResult[2] = IDIGNORE;
    disableCloseButton();
    break;
  case MB_YESNOCANCEL              :
    butStr[0] = loadString(_IDS_YES     );  m_buttonResult[0] = IDYES;
    butStr[1] = loadString(_IDS_NO      );  m_buttonResult[1] = IDNO;
    butStr[2] = loadString(_IDS_CANCEL  );  m_buttonResult[2] = IDCANCEL;
    m_hasCancelButton = true;
    break;
  case MB_YESNO                    :
    butStr[0] = loadString(_IDS_YES     );  m_buttonResult[0] = IDYES;
    butStr[1] = loadString(_IDS_NO      );  m_buttonResult[1] = IDNO;
    disableCloseButton();
    break;
  case MB_RETRYCANCEL              :
    butStr[0] = loadString(_IDS_RETRY   );  m_buttonResult[0] = IDRETRY;
    butStr[1] = loadString(_IDS_CANCEL  );  m_buttonResult[1] = IDCANCEL;
    m_hasCancelButton = true;
    break;
  case MB_CANCELTRYCONTINUE:
    butStr[0] = loadString(_IDS_CANCEL  );  m_buttonResult[0] = IDCANCEL;
    butStr[1] = loadString(_IDS_RETRY   );  m_buttonResult[1] = IDTRYAGAIN;
    butStr[2] = loadString(_IDS_CONTINUE);  m_buttonResult[2] = IDCONTINUE;
    m_hasCancelButton = true;
    break;
  }

  switch(m_nType & MB_ICONMASK) { 
  case MB_ICONINFORMATION  : m_icon = LoadIcon(NULL, IDI_INFORMATION); break;
  case MB_ICONQUESTION     : m_icon = LoadIcon(NULL, IDI_QUESTION   ); break;
  case MB_ICONWARNING      : m_icon = LoadIcon(NULL, IDI_WARNING    ); break;
  case MB_ICONERROR        : m_icon = LoadIcon(NULL, IDI_ERROR      ); break; 
  }

  CSize iconSize(0, 0);
  int msgBottom;
  if(m_icon) {
    m_iconCtrl.Create( EMPTYSTRING, SS_WHITEFRAME|WS_CHILD|WS_VISIBLE, CRect(58,47,99,62), messageArea, _IDC_STATICICON);
    iconSize = getIconSize(m_icon);
    setWindowSize(&m_iconCtrl, iconSize);
    if(iconSize.cy > textSize.cy) {
      setWindowPosition(&m_iconCtrl, CPoint(LEFTMARG, TOPMARG));
      setWindowPosition(&m_msgCtrl , CPoint(LEFTMARG + iconSize.cx + GAPICONTEXT, TOPMARG + (iconSize.cy - textSize.cy)/2));
      msgBottom = TOPMARG + iconSize.cy + GAPTEXTBUTTON;
    } else {
      setWindowPosition(&m_iconCtrl, CPoint(LEFTMARG, TOPMARG + (textSize.cy - iconSize.cy)/2));
      setWindowPosition(&m_msgCtrl , CPoint(LEFTMARG + iconSize.cx + GAPICONTEXT, TOPMARG));
      msgBottom = TOPMARG + textSize.cy + GAPTEXTBUTTON;
    }
  } else {
    setWindowPosition(&m_msgCtrl, CPoint(LEFTMARG, TOPMARG));
    msgBottom = TOPMARG + textSize.cy + GAPTEXTBUTTON;
  }

  const int buttonId[] = { _IDC_BUTTON0, _IDC_BUTTON1, _IDC_BUTTON2 };
  int visibleButtonCount = 0;
  CSize buttonSize(0,0);
  for(int i = 0; i < 3; i++) {
    const String &s = butStr[i];
    if(s.length()) {
      visibleButtonCount++;
      CSize textSize = dc.GetTextExtent(s.cstr(), (int)s.length());
      if(textSize.cx > buttonSize.cx) {
        buttonSize.cx = textSize.cx;
      }
      if(textSize.cy > buttonSize.cy) {
        buttonSize.cy = textSize.cy;
      }
    }
  }
  buttonSize.cx += 50; buttonSize.cy += 8;
  int totalButtonWidth = (buttonSize.cx + GAPBUTTON ) * visibleButtonCount - GAPBUTTON;

  CWnd *chkBox = GetDlgItem(_IDC_CHECKDONTSHOWAGAIN);
  String chkBoxText = getWindowText(chkBox);
  const CSize chkBoxTextSize = dc.GetTextExtent(chkBoxText.cstr(), (int)chkBoxText.length());
  const CSize chkBoxSize(chkBoxTextSize.cx + 20, chkBoxTextSize.cy + 15);
  setWindowPosition(chkBox, CPoint(LEFTMARG, msgBottom + GAPBUTTON));
  setWindowSize(chkBox, chkBoxSize);

  const int upperWidth = LEFTMARG + iconSize.cx + textSize.cx + RIGHTMARG;
  const int lowerWidth = LEFTMARG + chkBoxSize.cx + GAPBUTTON + totalButtonWidth + RIGHTMARG;

  const int clWidth = max(upperWidth, lowerWidth);
  setWindowPosition(this, _IDC_STATICMESSAGEAREA, CPoint(0,0));
  setWindowSize(    this, _IDC_STATICMESSAGEAREA, CSize(clWidth, msgBottom));

  setClientRectSize(this, CSize(clWidth, msgBottom + BUTTONAREAHEIGHT));

  int buttonRight = clWidth - RIGHTMARG;

  for(int i = 2; i >= 0; i--) {
    const String &str = butStr[i];
    CButton *but = (CButton*)GetDlgItem(buttonId[i]);
    if(str.length()) {
      but->SetWindowText(butStr[i].cstr());
      setWindowSize(but, buttonSize);
      setWindowPosition(but, CPoint(buttonRight - buttonSize.cx, msgBottom + BOTTOMMARG));
      buttonRight -= buttonSize.cx + GAPBUTTON;
    } else {
      but->ShowWindow(SW_HIDE);
    }
  }

//  GetDlgItem(_IDC_STATICMESSAGEAREA)-> BringWindowToTop();
//  GetDlgItem(_IDC_STATICMESSAGEAREA)->ShowWindow(SW_HIDE); 

  centerWindow(this);
  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
}

int confirmDialogBox(const String &message, const String &caption, bool &showAgain, UINT nType) {
  CConfirmDlg dlg(message.cstr(), caption.cstr(), nType);
  int result = (int)dlg.DoModal();
  showAgain = dlg.getShowAgain();
  return result;
}

void CConfirmDlg::OnPaint() {
  CDialog::OnPaint();
  CWnd       *whiteWindow = GetDlgItem(_IDC_STATICMESSAGEAREA);
  const CSize whiteSize   = getClientRect(whiteWindow).Size();

#define bgColor RGB(255,255,255)

  CPaintDC(whiteWindow).FillSolidRect(0,0, whiteSize.cx, whiteSize.cy, bgColor);

  if(!m_icon) return;

  DrawIconEx(CPaintDC(&m_iconCtrl), 0, 0, m_icon, 0, 0, 0, NULL, DI_NORMAL);

//  paintIcon(m_icon, CPaintDC(&m_iconCtrl), CPoint(0,0), bgColor);
}

void CConfirmDlg::disableCloseButton() {
  HMENU hSysMenu = ::GetSystemMenu(m_hWnd, FALSE);
  if(hSysMenu) {
    ::EnableMenuItem(hSysMenu, SC_CLOSE, (MF_DISABLED | MF_GRAYED | MF_BYCOMMAND));
  }
}

void CConfirmDlg::OnButton0() {
  UpdateData();
  EndDialog(m_buttonResult[0]);
}

void CConfirmDlg::OnButton1() {
  UpdateData();
  EndDialog(m_buttonResult[1]);
}

void CConfirmDlg::OnButton2() {
  UpdateData();
  EndDialog(m_buttonResult[2]);
}

void CConfirmDlg::OnCancel() {
  if(m_hasCancelButton) {
    UpdateData();
    EndDialog(IDCANCEL);
  }
}

BOOL CConfirmDlg::PreTranslateMessage(MSG* pMsg) {
  if(m_accelTable && TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}
