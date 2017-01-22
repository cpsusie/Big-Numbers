#include "stdafx.h"
#include "TestMouseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestMouseDlg::CTestMouseDlg(CWnd* pParent) : CDialog(CTestMouseDlg::IDD, pParent) {
  m_systemCursor  = NULL;
  m_createdCursor = NULL;
}

CTestMouseDlg::~CTestMouseDlg() {
  if(m_systemCursor) {
    DestroyCursor(m_systemCursor);
  }
  releaseCreatedCursor();
}

void CTestMouseDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestMouseDlg, CDialog)
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_BN_CLICKED(   IDC_NORMALSYSTEMCURSOR  , OnNormalSystemCursor       )
    ON_BN_CLICKED(   IDC_SETSYSTEMCURSOR1    , OnSetSystemCursor1         )
    ON_BN_CLICKED(   IDC_SYSTEMCURSOR2       , OnSetSystemCursor2         )
    ON_BN_CLICKED(   IDC_NORMALWINDOWCURSOR  , OnNormalWindowCursor       )
    ON_BN_CLICKED(   IDC_WINDOWCURSOR1       , OnSetWindowCursor1         )
    ON_BN_CLICKED(   IDC_WINDOWCURSOR2       , OnSetWindowCursor2         )
    ON_BN_CLICKED(   IDC_NORMALCONTROLCURSOR , OnNormalControlCursor      )
    ON_BN_CLICKED(   IDC_CONTROLCURSOR1      , OnSetControlCursor1        )
    ON_BN_CLICKED(   IDC_CONTROLCURSOR2      , OnSetControlCursor2        )
    ON_BN_CLICKED(   IDC_SETCURSORPOS        , OnSetCursorPos             )
    ON_BN_CLICKED(   IDC_HIDECURSOR          , OnHideCursor               )
    ON_BN_CLICKED(   IDC_WAITCURSOR          , OnWaitCursor               )
    ON_BN_CLICKED(   IDC_CLIPCURSOR          , OnClipCursor               )
    ON_BN_CLICKED(   IDC_UNCLIPCURSOR        , OnUnclipCursor             )
    ON_BN_CLICKED(   IDC_CREATECURSOR        , OnCreateCursor             )
    ON_CBN_SELCHANGE(IDC_COMBO_MOUSECURSOR   , OnSelchangeComboMouseCursor)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_RBUTTONDOWN()
    ON_BN_CLICKED(IDC_LOADANICURSOR, OnLoadAnimatedCursor)
END_MESSAGE_MAP()

BOOL CTestMouseDlg::OnInitDialog() {
  CDialog::OnInitDialog();

  // ------------- NB! NB! NB!   husk #define OEMRESOURCE i stdafx.h ----------------------

  initComboMouseCursor();
  m_systemCursor = CopyCursor(AfxGetApp()->LoadOEMCursor(OCR_NORMAL));
  //    m_systemCursor = CopyCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

  /*
  WNDCLASS wc;
  char classname[100];

  if(GetClassName(m_hWnd,classname,sizeof(classname)) == 0)
    ::MessageBox("Error GetClassName:%s",getLastErrorText().cstr());
  else
    if(::GetClassInfo(AfxGetApp()->m_hInstance,classname,&wc) == 0)
      ::MessageBox("Error GetClassInfo:%s",getLastErrorText().cstr());
  if(::UnregisterClass(classname,AfxGetApp()->m_hInstance) == 0)
      ::MessageBox("Error UnregisterClass(%s):%s",classname,getLastErrorText().cstr());
  wc.hCursor = NULL;
  if(::RegisterClass(&wc) == 0)
    ::MessageBox("Error RegisterClass:%s",getLastErrorText().cstr());
  */

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestMouseDlg::showFlags(const TCHAR *function, UINT flags) {
  const String s = format(_T("%-13s:flags:%3d (%02x) [%s]"), function, flags, flags, sprintbin((unsigned char)flags).cstr()).cstr();
  CClientDC(this).TextOut(10,10,s.cstr(), (int)s.length());
}

#define SHOWFLAGS() showFlags(__TFUNCTION__, nFlags)

void CTestMouseDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  SHOWFLAGS();
  CDialog::OnLButtonDown(nFlags, point);
//  setWindowCursor(this,AfxGetApp()->LoadCursor(IDC_CURSOR2));
}

void CTestMouseDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  SHOWFLAGS();
  CDialog::OnRButtonDown(nFlags, point);
}

void CTestMouseDlg::OnMouseMove(UINT nFlags, CPoint point) {
  SHOWFLAGS();
  CDialog::OnMouseMove(nFlags, point);
}

void CTestMouseDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
  MessageBox(_T("Hello"));
  CDialog::OnChar(nChar, nRepCnt, nFlags);
}

void CTestMouseDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  MessageBox(_T("KeyDown"));    
  CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ------------- NB! NB! NB!   husk #define OEMRESOURCE i stdafx.h ----------------------

void CTestMouseDlg::OnNormalSystemCursor() {
  setSystemCursor(OCR_NORMAL, m_systemCursor);
}

void CTestMouseDlg::OnSetSystemCursor1() {
  setSystemCursor(OCR_NORMAL, IDC_CURSOR1   ); 
}

void CTestMouseDlg::OnSetSystemCursor2() {
  setSystemCursor(OCR_NORMAL, IDC_CURSOR2   );
}

void CTestMouseDlg::OnNormalWindowCursor() {
  setWindowCursor(this, m_systemCursor);
}

void CTestMouseDlg::OnSetWindowCursor1() {
  setWindowCursor(this, IDC_CURSOR1   );
}

void CTestMouseDlg::OnSetWindowCursor2() {
  setWindowCursor(this, IDC_CURSOR2   );
}

void CTestMouseDlg::OnWaitCursor() {
  AfxGetApp()->BeginWaitCursor();
  Sleep(1000);
//  AfxGetApp()->EndWaitCursor();
}

BOOL CTestMouseDlg::DestroyWindow() {
  // husk #define OEMRESOURCE i stdafx.h
  ::SetSystemCursor( CopyCursor(m_systemCursor), OCR_NORMAL);
  return CDialog::DestroyWindow();
}

void CTestMouseDlg::OnNormalControlCursor() {
  setWindowCursor(GetDlgItem(IDC_STATIC_FRAME       ), m_systemCursor);
  setWindowCursor(GetDlgItem(IDC_NORMALCONTROLCURSOR), m_systemCursor);
}

void CTestMouseDlg::OnSetControlCursor1() {
  setWindowCursor(GetDlgItem(IDC_STATIC_FRAME       ), IDC_CURSOR1   );
  setWindowCursor(GetDlgItem(IDC_NORMALCONTROLCURSOR), IDC_CURSOR1   );
}

void CTestMouseDlg::OnSetControlCursor2() {
  setWindowCursor(GetDlgItem(IDC_STATIC_FRAME       ), IDC_CURSOR2   );
  setWindowCursor(GetDlgItem(IDC_NORMALCONTROLCURSOR), IDC_CURSOR2   );
}

void CTestMouseDlg::OnSetCursorPos() {
  ::SetCursorPos(10,10);
}

void CTestMouseDlg::OnHideCursor() {
//  getimage();
  ShowCursor(false);
  Sleep(3000);
  ShowCursor(true);
}

void CTestMouseDlg::OnClipCursor() {
  CRect r;
  GetWindowRect(&r);
  ClipCursor(&r);   
}

void CTestMouseDlg::OnUnclipCursor() {
  ClipCursor(NULL);
}

void CTestMouseDlg::OnCreateCursor() {
  try {
    releaseCreatedCursor();
    m_createdCursor = createCursor(AfxGetApp()->m_hInstance, IDB_BITMAPCOLOR, IDB_BITMAPMASK, 24,24);
    setWindowCursor(this, m_createdCursor);
  } catch(Exception e) {
    MessageBox(e.what(), _T("CreateCursor failed"), MB_ICONEXCLAMATION);
  }
}

void CTestMouseDlg::releaseCreatedCursor() {
  if(m_createdCursor) {
    DestroyCursor(m_createdCursor);
    m_createdCursor = NULL;
  }
}

typedef struct {
public:
  int id;
  const TCHAR *str;
} MouseCursorIndex;

#define MOUSECURSOR(id) id, _T(#id)

#define OCR_COMPACTDISC1  OCR_COMPACTDISC+1
#define OCR_COMPACTDISC2  OCR_COMPACTDISC+2
#define OCR_COMPACTDISC3  OCR_COMPACTDISC+3
#define OCR_COMPACTDISC4  OCR_COMPACTDISC+4
#define OCR_COMPACTDISC5  OCR_COMPACTDISC+5
#define OCR_COMPACTDISC6  OCR_COMPACTDISC+6
#define OCR_COMPACTDISC7  OCR_COMPACTDISC+7
#define OCR_COMPACTDISC8  OCR_COMPACTDISC+8
#define OCR_COMPACTDISC9  OCR_COMPACTDISC+9
#define OCR_COMPACTDISC10 OCR_COMPACTDISC+10

static MouseCursorIndex mouseCursors[] = {
  MOUSECURSOR(OCR_NORMAL       )
 ,MOUSECURSOR(OCR_IBEAM        )
 ,MOUSECURSOR(OCR_WAIT         )
 ,MOUSECURSOR(OCR_CROSS        )
 ,MOUSECURSOR(OCR_UP           )
 ,MOUSECURSOR(OCR_SIZEALL      )
 ,MOUSECURSOR(OCR_SIZENWSE     )
 ,MOUSECURSOR(OCR_SIZENESW     )
 ,MOUSECURSOR(OCR_SIZEWE       )
 ,MOUSECURSOR(OCR_SIZENS       )
 ,MOUSECURSOR(OCR_ICOCUR       )
 ,MOUSECURSOR(OIC_WINLOGO      )
 ,MOUSECURSOR(OCR_NO           )
 ,MOUSECURSOR(OCR_APPSTARTING  )
 ,MOUSECURSOR(OCR_HAND         )
 ,MOUSECURSOR(OCR_QUESTION     )
 ,MOUSECURSOR(OCR_SCROLLNS     )
 ,MOUSECURSOR(OCR_SCROLLWE     )
 ,MOUSECURSOR(OCR_SCROLLALL    )
 ,MOUSECURSOR(OCR_SCROLLDOWN   )
 ,MOUSECURSOR(OCR_SCROLLUP     )
 ,MOUSECURSOR(OCR_SCROLLLEFT   )
 ,MOUSECURSOR(OCR_SCROLLRIGHT  )
 ,MOUSECURSOR(OCR_SCROLLNW     )
 ,MOUSECURSOR(OCR_SCROLLNE     )
 ,MOUSECURSOR(OCR_SCROLLSW     )
 ,MOUSECURSOR(OCR_SCROLLSE     )
 ,MOUSECURSOR(OCR_COMPACTDISC  )
 ,MOUSECURSOR(OCR_COMPACTDISC1 )
 ,MOUSECURSOR(OCR_COMPACTDISC2 )
 ,MOUSECURSOR(OCR_COMPACTDISC3 )
 ,MOUSECURSOR(OCR_COMPACTDISC4 )
 ,MOUSECURSOR(OCR_COMPACTDISC5 )
 ,MOUSECURSOR(OCR_COMPACTDISC6 )
 ,MOUSECURSOR(OCR_COMPACTDISC7 )
 ,MOUSECURSOR(OCR_COMPACTDISC8 )
 ,MOUSECURSOR(OCR_COMPACTDISC9 )
 ,MOUSECURSOR(OCR_COMPACTDISC10)
};

void CTestMouseDlg::initComboMouseCursor() {
  CComboBox *cb = (CComboBox*)GetDlgItem(IDC_COMBO_MOUSECURSOR);
  for(int i = 0; i < ARRAYSIZE(mouseCursors); i++) {
    cb->AddString(mouseCursors[i].str);
  }
  cb->SetWindowContextHelpId(12);
}

void CTestMouseDlg::OnSelchangeComboMouseCursor() {
  CComboBox *cb = (CComboBox*)GetDlgItem(IDC_COMBO_MOUSECURSOR);
  int index = cb->GetCurSel();
  setWindowCursor(this, MAKEINTRESOURCE(mouseCursors[index].id));
  setWindowCursor(GetDlgItem(IDC_STATIC_FRAME), MAKEINTRESOURCE(mouseCursors[index].id));
}

void CTestMouseDlg::OnLoadAnimatedCursor() {
  try {
    ByteArray cursorData;
    cursorData.loadFromResource(IDR_ANICURSOR1, _T("ANICURSORS"));
    releaseCreatedCursor();
    m_createdCursor = (HCURSOR)CreateIconFromResourceEx((BYTE*)cursorData.getData(), (DWORD)cursorData.size(), FALSE, 0x00030000, 0, 0, LR_DEFAULTSIZE);
//    m_createdCursor = (HCURSOR)CreateIconFromResource((BYTE*)cursorData.getData(), cursorData.size(), FALSE, 0x00030000);
    if (m_createdCursor == NULL) {
      throwLastErrorOnSysCallException(__TFUNCTION__);
    }
    setWindowCursor(this, m_createdCursor);
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
  }
}
