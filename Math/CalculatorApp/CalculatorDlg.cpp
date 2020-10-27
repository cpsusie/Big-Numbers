#include "stdafx.h"
#include <ThreadPool.h>
#include <MFCUtil/Clipboard.h>

#include "CalculatorDlg.h"
#include "PrecisionDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

#define WHITE      RGB(255,255,255)
#define RED        RGB(255,0,0)
#define BLUE       RGB(0,0,255)
#define PURPLE     RGB(200,0,200)
#define GRAY       RGB(192,192,192)
#define DIMMEDGRAY RGB(128,128,128)

typedef struct {
  int      m_id;
  COLORREF m_color;
} ButtonAttribute ;

#define ATTR(id,color) id,color

static ButtonAttribute allButtons[] = {
  ATTR( IDC_BUTTON0        , BLUE  )
 ,ATTR( IDC_BUTTON1        , BLUE  )
 ,ATTR( IDC_BUTTON2        , BLUE  )
 ,ATTR( IDC_BUTTON3        , BLUE  )
 ,ATTR( IDC_BUTTON4        , BLUE  )
 ,ATTR( IDC_BUTTON5        , BLUE  )
 ,ATTR( IDC_BUTTON6        , BLUE  )
 ,ATTR( IDC_BUTTON7        , BLUE  )
 ,ATTR( IDC_BUTTON8        , BLUE  )
 ,ATTR( IDC_BUTTON9        , BLUE  )
 ,ATTR( IDC_BUTTONA        , BLUE  )
 ,ATTR( IDC_BUTTONB        , BLUE  )
 ,ATTR( IDC_BUTTONC        , BLUE  )
 ,ATTR( IDC_BUTTOND        , BLUE  )
 ,ATTR( IDC_BUTTONE        , BLUE  )
 ,ATTR( IDC_BUTTONF        , BLUE  )
 ,ATTR( IDC_BUTTONSIGN     , BLUE  )
 ,ATTR( IDC_BUTTONCOMMA    , BLUE  )
 ,ATTR( IDC_BUTTONPI       , BLUE  )
 ,ATTR( IDC_BUTTONDIV      , RED   )
 ,ATTR( IDC_BUTTONMULT     , RED   )
 ,ATTR( IDC_BUTTONSUB      , RED   )
 ,ATTR( IDC_BUTTONADD      , RED   )
 ,ATTR( IDC_BUTTONCLEAR    , RED   )
 ,ATTR( IDC_BUTTONCE       , RED   )
 ,ATTR( IDC_BUTTONBACKSPACE, RED   )
 ,ATTR( IDC_BUTTONMC       , RED   )
 ,ATTR( IDC_BUTTONMR       , RED   )
 ,ATTR( IDC_BUTTONMS       , RED   )
 ,ATTR( IDC_BUTTONMADD     , RED   )
 ,ATTR( IDC_BUTTONMOD      , RED   )
 ,ATTR( IDC_BUTTONOR       , RED   )
 ,ATTR( IDC_BUTTONLSH      , RED   )
 ,ATTR( IDC_BUTTONEQUAL    , RED   )
 ,ATTR( IDC_BUTTONAND      , RED   )
 ,ATTR( IDC_BUTTONXOR      , RED   )
 ,ATTR( IDC_BUTTONNOT      , RED   )
 ,ATTR( IDC_BUTTONINT      , RED   )
 ,ATTR( IDC_BUTTONRPAR     , PURPLE)
 ,ATTR( IDC_BUTTONLPAR     , PURPLE)
 ,ATTR( IDC_BUTTONEE       , PURPLE)
 ,ATTR( IDC_BUTTONPOW      , PURPLE)
 ,ATTR( IDC_BUTTONLN       , PURPLE)
 ,ATTR( IDC_BUTTONLOG      , PURPLE)
 ,ATTR( IDC_BUTTONFAC      , PURPLE)
 ,ATTR( IDC_BUTTONPOW3     , PURPLE)
 ,ATTR( IDC_BUTTONRECIPROC , PURPLE)
 ,ATTR( IDC_BUTTONSQUARE   , PURPLE)
 ,ATTR( IDC_BUTTONSIN      , PURPLE)
 ,ATTR( IDC_BUTTONCOS      , PURPLE)
 ,ATTR( IDC_BUTTONTAN      , PURPLE)
 ,ATTR( IDC_BUTTONDMS      , PURPLE)
};

static ButtonAttribute *getAttribute(int id) {
  for(int i = 0; i < ARRAYSIZE(allButtons); i++) {
    if(allButtons[i].m_id == id) {
      return allButtons+i;
    }
  }
  return nullptr;
}

CCalculatorDlg::CCalculatorDlg(CWnd *pParent /*=nullptr*/)	: CDialog(CCalculatorDlg::IDD, pParent) {
  m_display = EMPTYSTRING;
  m_calc    = new Calculator(); TRACE_NEW(m_calc);
  m_hIcon   = theApp.LoadIcon(IDR_MAINFRAME);
}

CCalculatorDlg::~CCalculatorDlg() {
  SAFEDELETE(m_calc);
}

void CCalculatorDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_DISPLAY, m_display);
}

BEGIN_MESSAGE_MAP(CCalculatorDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_ACTIVATE()
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_WM_CONTEXTMENU()
    ON_WM_CLOSE()
    ON_COMMAND(     ID_FILE_QUIT         , OnFileQuit          )
    ON_COMMAND(     ID_EDIT_COPY         , OnEditCopy          )
    ON_COMMAND(     ID_EDIT_PASTE        , OnEditPaste         )
    ON_COMMAND(     ID_VIEW_BIN          , OnViewBin           )
    ON_COMMAND(     ID_VIEW_OCT          , OnViewOct           )
    ON_COMMAND(     ID_VIEW_DEC          , OnViewDec           )
    ON_COMMAND(     ID_VIEW_HEX          , OnViewHex           )
    ON_COMMAND(     ID_VIEW_DEGREES      , OnF2                )
    ON_COMMAND(     ID_VIEW_RADIANS      , OnF3                )
    ON_COMMAND(     ID_VIEW_GRADS        , OnF4                )
    ON_COMMAND(     ID_VIEW_BYTE         , OnF2                )
    ON_COMMAND(     ID_VIEW_WORD         , OnF3                )
    ON_COMMAND(     ID_VIEW_DWORD        , OnF4                )
    ON_COMMAND(     ID_VIEW_QWORD        , OnF12               )
    ON_COMMAND(     ID_VIEW_OWORD        , OnShftF12           )
    ON_COMMAND(     ID_VIEW_DIGITGROUPING, OnViewDigitGrouping )
    ON_COMMAND(     ID_VIEW_PRECISION    , OnViewPrecision     )
    ON_COMMAND(     ID_HELP_ABOUT        , OnHelpAbout         )
    ON_COMMAND(     ID_CONTROL_WHATSTHIS , OnControlWhatsthis  )
    ON_EN_SETFOCUS( IDC_DISPLAY          , OnSetfocusDisplay   )
    ON_BN_CLICKED(  IDC_CHECKINV         , OnCheckInv          )
    ON_BN_CLICKED(  IDC_CHECKHYP         , OnCheckHyp          )
    ON_BN_CLICKED(  IDC_RADIOBIN         , OnRadioBin          )
    ON_BN_CLICKED(  IDC_RADIOOCT         , OnRadioOct          )
    ON_BN_CLICKED(  IDC_RADIODEC         , OnRadioDec          )
    ON_BN_CLICKED(  IDC_RADIOHEX         , OnRadioHex          )
    ON_BN_CLICKED(  IDC_RADIODEGREES     , OnF2                )
    ON_BN_CLICKED(  IDC_RADIORADIANS     , OnF3                )
    ON_BN_CLICKED(  IDC_RADIOGRADS       , OnF4                )
    ON_BN_CLICKED(  IDC_RADIOBYTE        , OnF2                )
    ON_BN_CLICKED(  IDC_RADIOWORD        , OnF3                )
    ON_BN_CLICKED(  IDC_RADIODWORD       , OnF4                )
    ON_BN_CLICKED(  IDC_RADIOQWORD       , OnF12               )
    ON_BN_CLICKED(  IDC_RADIOOWORD       , OnShftF12           )
    ON_COMMAND(     ID_CE                , OnCe                )
    ON_COMMAND(     ID_BACK              , OnBack              )
    ON_COMMAND(     ID_CLEAR             , OnClear             )
    ON_COMMAND(     ID_EQUAL             , OnEqual             )
    ON_COMMAND(     ID_NUMBER0           , OnNumber0           )
    ON_COMMAND(     ID_NUMBER1           , OnNumber1           )
    ON_COMMAND(     ID_NUMBER2           , OnNumber2           )
    ON_COMMAND(     ID_NUMBER3           , OnNumber3           )
    ON_COMMAND(     ID_NUMBER4           , OnNumber4           )
    ON_COMMAND(     ID_NUMBER5           , OnNumber5           )
    ON_COMMAND(     ID_NUMBER6           , OnNumber6           )
    ON_COMMAND(     ID_NUMBER7           , OnNumber7           )
    ON_COMMAND(     ID_NUMBER8           , OnNumber8           )
    ON_COMMAND(     ID_NUMBER9           , OnNumber9           )
    ON_COMMAND(     ID_NUMBERA           , OnNumberA           )
    ON_COMMAND(     ID_NUMBERB           , OnNumberB           )
    ON_COMMAND(     ID_NUMBERC           , OnNumberC           )
    ON_COMMAND(     ID_NUMBERD           , OnNumberD           )
    ON_COMMAND(     ID_NUMBERE           , OnNumberE           )
    ON_COMMAND(     ID_NUMBERF           , OnNumberF           )
    ON_COMMAND(     ID_COMMA             , OnComma             )
    ON_COMMAND(     ID_SIGN              , OnSign              )
    ON_COMMAND(     ID_EE                , OnEE                )
    ON_COMMAND(     ID_ADD               , OnAdd               )
    ON_COMMAND(     ID_SUB               , OnSub               )
    ON_COMMAND(     ID_MULT              , OnMult              )
    ON_COMMAND(     ID_DIV               , OnDiv               )
    ON_COMMAND(     ID_MOD               , OnMod               )
    ON_COMMAND(     ID_SQUARE            , OnSquare            )
    ON_COMMAND(     ID_POW3              , OnPow3              )
    ON_COMMAND(     ID_POW               , OnPow               )
    ON_COMMAND(     ID_LN                , OnLn                )
    ON_COMMAND(     ID_LOG               , OnLog               )
    ON_COMMAND(     ID_SIN               , OnSin               )
    ON_COMMAND(     ID_COS               , OnCos               )
    ON_COMMAND(     ID_TAN               , OnTan               )
    ON_COMMAND(     ID_PI                , OnPi                )
    ON_COMMAND(     ID_RECIPROC          , OnReciproc          )
    ON_COMMAND(     ID_FAC               , OnFac               )
    ON_COMMAND(     ID_DMS               , OnDMS               )
    ON_COMMAND(     ID_AND               , OnAnd               )
    ON_COMMAND(     ID_OR                , OnOr                )
    ON_COMMAND(     ID_XOR               , OnXor               )
    ON_COMMAND(     ID_LSH               , OnLsh               )
    ON_COMMAND(     ID_NOT               , OnNot               )
    ON_COMMAND(     ID_INT               , OnInt               )
    ON_COMMAND(     IDIGNORE             , OnIgnore            )
    ON_COMMAND(     ID_LPAR              , OnLpar              )
    ON_COMMAND(     ID_RPAR              , OnRpar              )
    ON_COMMAND(     ID_SHOWINFO          , OnShowinfo          )
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipNotify)
END_MESSAGE_MAP()

void CCalculatorDlg::OnHelpAbout() {
  CAboutDlg().DoModal();
}

HCURSOR CCalculatorDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

MyButton::MyButton() {
  m_pressed = false;
  m_enabled = true;
}

void MyButton::press(BOOL pressed) {
  m_pressed = pressed ? true : false;
  Invalidate(false);
}

void MyButton::enable(bool enabled) {
  m_enabled = enabled;
  Invalidate(true);
}

bool MyButton::pointInWindow(const CPoint &p) {
  CRect r;
  GetWindowRect(&r);
  return r.PtInRect(p) ? true : false;
}

static CFont m_font;

void MyButton::initBitmaps(CButton *button) {
  CRect rect;
  TCHAR text[256];
  int id = button->GetDlgCtrlID();
  button->GetClientRect(rect);
  button->GetWindowText(text,sizeof(text));
  CClientDC screenDC(button);
  CDC dc;
  dc.CreateCompatibleDC(&screenDC);
  m_normalBitmap.CreateBitmap(  rect.Width(),rect.Height(),screenDC.GetDeviceCaps(PLANES),screenDC.GetDeviceCaps(BITSPIXEL),nullptr);
  m_disabledBitmap.CreateBitmap(rect.Width(),rect.Height(),screenDC.GetDeviceCaps(PLANES),screenDC.GetDeviceCaps(BITSPIXEL),nullptr);
  m_pressedBitmap.CreateBitmap( rect.Width(),rect.Height(),screenDC.GetDeviceCaps(PLANES),screenDC.GetDeviceCaps(BITSPIXEL),nullptr);

  CSize textSize = dc.GetTextExtent(text);
  CBrush grayBrush(GRAY);
  LOGBRUSH logBrush;
  logBrush.lbStyle = BS_SOLID;
  logBrush.lbColor = WHITE;
  CPen whitePen;
  whitePen.CreatePen(PS_SOLID, 1, &logBrush);

  dc.SelectObject(m_normalBitmap);
  dc.SelectObject(&whitePen);
  dc.MoveTo(rect.Width(),0);
  dc.LineTo(0,0);
  dc.LineTo(0,rect.Height());

  dc.SelectObject(m_disabledBitmap);
  dc.SelectObject(&whitePen);
  dc.MoveTo(rect.Width(),0);
  dc.LineTo(0,0);
  dc.LineTo(0,rect.Height());

  dc.SelectObject(m_pressedBitmap);
  dc.SelectObject(&whitePen);
  dc.MoveTo(rect.Width()-1,0);
  dc.LineTo(rect.Width()-1,rect.Height()-1);
  dc.LineTo(0,rect.Height()-1);

  rect.left+=1; rect.right-=1; rect.top+=1; rect.bottom-=1;

  CFont *oldFont = dc.SelectObject(&m_font);

  dc.SelectObject(m_normalBitmap);
  dc.FillRect(rect,&grayBrush);
  dc.SetBkColor(GRAY);
  dc.SetTextColor(getAttribute(id)->m_color);
  dc.TextOut((rect.Width()-textSize.cx)/2,(rect.Height()-textSize.cy)/2,text);

  dc.SelectObject(m_pressedBitmap);
  dc.FillRect(rect,&grayBrush);
  dc.SetBkColor(GRAY);
  dc.SetTextColor(getAttribute(id)->m_color);
  dc.TextOut((rect.Width()-textSize.cx)/2,(rect.Height()-textSize.cy)/2,text);

  dc.SelectObject(m_disabledBitmap);
  dc.FillRect(rect,&grayBrush);
  dc.SetBkColor(GRAY);
  dc.SetTextColor(DIMMEDGRAY);
  dc.TextOut((rect.Width()-textSize.cx)/2,(rect.Height()-textSize.cy)/2,text);
}

void MyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
  CDC *screenDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  CDC tmpDC;
  CRect rect;
  GetClientRect(&rect);
  tmpDC.CreateCompatibleDC(screenDC);
  CBitmap &bitmap = m_enabled ? m_pressed ? m_pressedBitmap : m_normalBitmap : m_disabledBitmap;
  tmpDC.SelectObject(bitmap);
  screenDC->BitBlt(0,0,rect.Width(),rect.Height(),&tmpDC,0,0,SRCCOPY);
}

BOOL CCalculatorDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != nullptr) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);		// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon
  setWindowCursor(GetDlgItem(IDC_DISPLAY    ),OCR_NORMAL);

  m_accelTabel     = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));
  m_timerInterval  = 0;
  m_waitCursorOn   = false;
  m_selectedButton = nullptr;

  m_font.CreateFont(16, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH | FF_MODERN,
                         _T("System")
                        );

  for(int i = 0; i < ARRAYSIZE(allButtons); i++) {
    MyButton *myButton = new MyButton(); TRACE_NEW(myButton);
    m_buttons.add(myButton);
    ButtonAttribute &attr = allButtons[i];
    CButton *buttonTemplate = (CButton*)GetDlgItem(attr.m_id);
    DWORD style = buttonTemplate->GetStyle() | BS_BITMAP | BS_OWNERDRAW;

    TCHAR text[256];
    CRect rect;
    buttonTemplate->GetWindowText(text,sizeof(text));
    buttonTemplate->GetWindowRect(&rect);
    ScreenToClient(rect);
    myButton->Create(text,style,rect,this,buttonTemplate->GetDlgCtrlID());
    myButton->initBitmaps(buttonTemplate);
    buttonTemplate->ShowWindow(SW_HIDE);
  }

  EnableToolTips();
  ThreadPool::executeNoWait(*m_calc);
//  MultiplierThreadPool::startLogging();

  showStatus();

  return FALSE;  // return TRUE  unless you set the focus to a control
}


void CCalculatorDlg::OnDestroy() {
  __super::OnDestroy();

  for(int i = 0; i < ARRAYSIZE(allButtons); i++) {
    MyButton *button = m_buttons[i];
    SAFEDELETE(button);
  }
}

void CCalculatorDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CCalculatorDlg::OnPaint() {
  if (IsIconic())	{
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
    __super::OnPaint();
  }
}

void CCalculatorDlg::OnFileQuit() {
  SAFEDELETE(m_calc);
  EndDialog(IDOK);
}

void CCalculatorDlg::OnClose() {
  OnFileQuit();
}

void CCalculatorDlg::enableButton(int id, bool enabled) {
  MyButton *but = findButton(id);
  if(but != nullptr) {
    but->enable(enabled);
  }
}

void CCalculatorDlg::enableNumButtons(int radix) { // enable button "0","1","2",...radix
  int buttonid[] = {
    IDC_BUTTON0, IDC_BUTTON1, IDC_BUTTON2, IDC_BUTTON3,
    IDC_BUTTON4, IDC_BUTTON5, IDC_BUTTON6, IDC_BUTTON7,
    IDC_BUTTON8, IDC_BUTTON9, IDC_BUTTONA, IDC_BUTTONB,
    IDC_BUTTONC, IDC_BUTTOND, IDC_BUTTONE, IDC_BUTTONF
  };
  int i;
  for(i = 0; i < radix; i++) {
    enableButton(buttonid[i],true);
  }
  for(;i < 16; i++) {
    enableButton(buttonid[i],false);
  }
}

void CCalculatorDlg::setDecButtons() {
  enableButton(IDC_BUTTONSIN   ,true);
  enableButton(IDC_BUTTONCOS   ,true);
  enableButton(IDC_BUTTONTAN   ,true);
  enableButton(IDC_BUTTONDMS   ,true);
  enableButton(IDC_BUTTONEE    ,true);
  enableButton(IDC_BUTTONPI    ,true);
  enableNumButtons(10);

  GetDlgItem(IDC_RADIORADIANS   )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIODEGREES   )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIOGRADS     )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIORADIANS   )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_RADIODEGREES   )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_RADIOGRADS     )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_STATICTRIGOMODE)->ShowWindow(  SW_SHOW);

  GetDlgItem(IDC_RADIOBYTE      )->EnableWindow(FALSE  );
  GetDlgItem(IDC_RADIOWORD      )->EnableWindow(FALSE  );
  GetDlgItem(IDC_RADIODWORD     )->EnableWindow(FALSE  );
  GetDlgItem(IDC_RADIOQWORD     )->EnableWindow(FALSE  );
  GetDlgItem(IDC_RADIOOWORD     )->EnableWindow(FALSE  );
  GetDlgItem(IDC_RADIOBYTE      )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_RADIOWORD      )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_RADIODWORD     )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_RADIOQWORD     )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_RADIOOWORD     )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_STATICOPSIZE   )->ShowWindow(  SW_HIDE);

  if(menuItemExists(this,ID_VIEW_QWORD)) {
    insertMenuItem(this,ID_VIEW_QWORD,0,_T("D&egrees\tF2" ),ID_VIEW_DEGREES);
    insertMenuItem(this,ID_VIEW_QWORD,0,_T("&Radians\tF3" ),ID_VIEW_RADIANS);
    insertMenuItem(this,ID_VIEW_QWORD,0,_T("&Grads\tF4"   ),ID_VIEW_GRADS  );
    removeMenuItem(this,ID_VIEW_BYTE );
    removeMenuItem(this,ID_VIEW_WORD );
    removeMenuItem(this,ID_VIEW_DWORD);
    removeMenuItem(this,ID_VIEW_QWORD);
    removeMenuItem(this,ID_VIEW_OWORD);
  }
  checkTrigonometricBase(m_calc->getTrigonometricBase());
}

void CCalculatorDlg::checkTrigonometricBase(Trigonometric tm) {
  CheckDlgButton(       IDC_RADIORADIANS, BST_UNCHECKED );
  CheckDlgButton(       IDC_RADIODEGREES, BST_UNCHECKED );
  CheckDlgButton(       IDC_RADIOGRADS  , BST_UNCHECKED );
  checkMenuItem(  this, ID_VIEW_RADIANS , false         );
  checkMenuItem(  this, ID_VIEW_DEGREES , false         );
  checkMenuItem(  this, ID_VIEW_GRADS   , false         );

  switch(tm) {
  case TRIGO_RADIANS:
    CheckDlgButton(     IDC_RADIORADIANS, BST_CHECKED   );
    checkMenuItem(this, ID_VIEW_RADIANS , true          );
    break;
  case TRIGO_DEGREES:
    CheckDlgButton(     IDC_RADIODEGREES, BST_CHECKED   );
    checkMenuItem(this, ID_VIEW_DEGREES , true          );
    break;
  case TRIGO_GRADS  :
    CheckDlgButton(     IDC_RADIOGRADS  , BST_CHECKED   );
    checkMenuItem(this, ID_VIEW_GRADS   , true          );
    break;
  }
}

void CCalculatorDlg::setByteButtons() {
  enableButton(IDC_BUTTONSIN   ,false);
  enableButton(IDC_BUTTONCOS   ,false);
  enableButton(IDC_BUTTONTAN   ,false);
  enableButton(IDC_BUTTONDMS   ,false);
  enableButton(IDC_BUTTONEE    ,false);
  enableButton(IDC_BUTTONPI    ,false);
  enableNumButtons(m_calc->getRadix());

  GetDlgItem(IDC_RADIORADIANS   )->EnableWindow(FALSE  );
  GetDlgItem(IDC_RADIODEGREES   )->EnableWindow(FALSE  );
  GetDlgItem(IDC_RADIOGRADS     )->EnableWindow(FALSE  );

  GetDlgItem(IDC_RADIORADIANS   )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_RADIODEGREES   )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_RADIOGRADS     )->ShowWindow(  SW_HIDE);
  GetDlgItem(IDC_STATICTRIGOMODE)->ShowWindow(  SW_HIDE);

  GetDlgItem(IDC_RADIOBYTE      )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIOWORD      )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIODWORD     )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIOQWORD     )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIOOWORD     )->EnableWindow(TRUE   );
  GetDlgItem(IDC_RADIOBYTE      )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_RADIOWORD      )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_RADIODWORD     )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_RADIOQWORD     )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_RADIOOWORD     )->ShowWindow(  SW_SHOW);
  GetDlgItem(IDC_STATICOPSIZE   )->ShowWindow(  SW_SHOW);

  if(menuItemExists(this,ID_VIEW_DEGREES)) {
    insertMenuItem(this,ID_VIEW_DEGREES, 0, _T("B&yte\tF2"       ), ID_VIEW_BYTE );
    insertMenuItem(this,ID_VIEW_DEGREES, 0, _T("&Word\tF3"       ), ID_VIEW_WORD );
    insertMenuItem(this,ID_VIEW_DEGREES, 0, _T("Dwo&rd\tF4"      ), ID_VIEW_DWORD);
    insertMenuItem(this,ID_VIEW_DEGREES, 0, _T("&Qword\tF12"     ), ID_VIEW_QWORD);
    insertMenuItem(this,ID_VIEW_DEGREES, 0, _T("&Oword\tShft F12"), ID_VIEW_OWORD);

    removeMenuItem(this,ID_VIEW_DEGREES);
    removeMenuItem(this,ID_VIEW_RADIANS);
    removeMenuItem(this,ID_VIEW_GRADS  );
  }
  checkOpeandSize(m_calc->getOperandSize());
}

void CCalculatorDlg::checkOpeandSize(OperandSize size) {
  CheckDlgButton(       IDC_RADIOBYTE , BST_UNCHECKED );
  CheckDlgButton(       IDC_RADIOWORD , BST_UNCHECKED );
  CheckDlgButton(       IDC_RADIODWORD, BST_UNCHECKED );
  CheckDlgButton(       IDC_RADIOQWORD, BST_UNCHECKED );
  CheckDlgButton(       IDC_RADIOOWORD, BST_UNCHECKED );
  checkMenuItem(  this, ID_VIEW_BYTE  , false         );
  checkMenuItem(  this, ID_VIEW_WORD  , false         );
  checkMenuItem(  this, ID_VIEW_DWORD , false         );
  checkMenuItem(  this, ID_VIEW_QWORD , false         );
  checkMenuItem(  this, ID_VIEW_OWORD , false         );
  switch(size) {
  case OPSIZE_BYTE :
    CheckDlgButton(     IDC_RADIOBYTE , BST_CHECKED );
    checkMenuItem(this, ID_VIEW_BYTE  , true        );
    break;
  case OPSIZE_WORD :
    CheckDlgButton(     IDC_RADIOWORD , BST_CHECKED );
    checkMenuItem(this, ID_VIEW_WORD  , true        );
    break;
  case OPSIZE_DWORD:
    CheckDlgButton(     IDC_RADIODWORD, BST_CHECKED );
    checkMenuItem(this, ID_VIEW_DWORD , true        );
    break;
  case OPSIZE_QWORD:
    CheckDlgButton(     IDC_RADIOQWORD, BST_CHECKED );
    checkMenuItem(this, ID_VIEW_QWORD , true        );
    break;
  case OPSIZE_OWORD:
    CheckDlgButton(     IDC_RADIOOWORD, BST_CHECKED );
    checkMenuItem(this, ID_VIEW_OWORD , true        );
    break;
  }
}

void CCalculatorDlg::showStatus() {
  m_display = m_calc->getDisplayText().cstr();
  UpdateData(false);

  if(m_calc->getMemoryIndicator()) {
    SetDlgItemText(IDC_MEMORYINDICATOR, _T("M"));
  } else {
    SetDlgItemText(IDC_MEMORYINDICATOR, EMPTYSTRING );
  }
  int plevel = m_calc->getParanthesLevel();

  if(plevel == 0) {
    SetDlgItemText(IDC_PARANTHESLEVEL, EMPTYSTRING  );
  } else {
    TCHAR tmp[20];
    _stprintf(tmp,_T("(=%d"), plevel);
    SetDlgItemText(IDC_PARANTHESLEVEL, tmp );
  }

  if(m_calc->getInverse()) {
    CheckDlgButton(IDC_CHECKINV, BST_CHECKED  );
  } else {
    CheckDlgButton(IDC_CHECKINV, BST_UNCHECKED);
  }

  if(m_calc->getHyperbolic()) {
    CheckDlgButton(IDC_CHECKHYP, BST_CHECKED  );
  } else {
    CheckDlgButton(IDC_CHECKHYP, BST_UNCHECKED);
  }

  checkMenuItem(this, ID_VIEW_DIGITGROUPING, m_calc->getDigitGrouping());

  checkRadix(m_calc->getRadix());
  switch(m_calc->getRadix()) {
  case 2:
  case 8:
  case 16:
    setByteButtons();
    break;
  case 10:
    setDecButtons();
    break;
  }

  setFocus();
}

void CCalculatorDlg::checkRadix(int radix) {
  CheckDlgButton(          IDC_RADIOBIN , BST_UNCHECKED );
  CheckDlgButton(          IDC_RADIOOCT , BST_UNCHECKED );
  CheckDlgButton(          IDC_RADIODEC , BST_UNCHECKED );
  CheckDlgButton(          IDC_RADIOHEX , BST_UNCHECKED );
  checkMenuItem( this,     ID_VIEW_BIN  , false         );
  checkMenuItem( this,     ID_VIEW_OCT  , false         );
  checkMenuItem( this,     ID_VIEW_DEC  , false         );
  checkMenuItem( this,     ID_VIEW_HEX  , false         );
  switch(radix) {
  case 2:
    CheckDlgButton(        IDC_RADIOBIN , BST_CHECKED   );
    checkMenuItem(this,    ID_VIEW_BIN  , true          );
    break;
  case 8:
    CheckDlgButton(        IDC_RADIOOCT , BST_CHECKED   );
    checkMenuItem(this,    ID_VIEW_OCT  , true          );
    break;
  case 10:
    CheckDlgButton(        IDC_RADIODEC , BST_CHECKED   );
    checkMenuItem(this,    ID_VIEW_DEC  , true          );
    break;
  case 16:
    CheckDlgButton(        IDC_RADIOHEX , BST_CHECKED   );
    checkMenuItem(this,    ID_VIEW_HEX  , true          );
    break;
  }
}

void CCalculatorDlg::setFocus() {
  GetDlgItem(IDC_DISPLAY)->SetFocus();
}

void CCalculatorDlg::OnSetfocusDisplay() {
  CEdit *e = (CEdit*)GetDlgItem(IDC_DISPLAY);
  e->HideCaret();
}

void CCalculatorDlg::startTimer(int msec) {
  if(!m_timerInterval && SetTimer(1, msec, nullptr)) {
    m_timerInterval = msec;
  }
}

void CCalculatorDlg::stopTimer() {
  if(m_timerInterval) {
    KillTimer(1);
    m_timerInterval = 0;
  }
}

void CCalculatorDlg::waitCursor(bool on) {
  if(on != m_waitCursorOn) {
    if(on) {
      setWindowCursor(this, IDC_WAIT);
      setWindowCursor(GetDlgItem(IDC_DISPLAY    ), IDC_WAIT);
      setWindowCursor(GetDlgItem(IDC_BUTTONCLEAR), IDC_WAIT);
      theApp.BeginWaitCursor();
    } else {
      setWindowCursor(this,OCR_NORMAL);
      setWindowCursor(GetDlgItem(IDC_DISPLAY    ), OCR_NORMAL);
      setWindowCursor(GetDlgItem(IDC_BUTTONCLEAR), OCR_NORMAL);
      theApp.EndWaitCursor();
    }
    m_waitCursorOn = on;
  }
}

void CCalculatorDlg::OnTimer(UINT_PTR nIDEvent) {
  if(!m_calc->isBusy()) {
    waitCursor(false);
    showStatus();
    stopTimer();
  } else {
    time_t now;
    time(&now);
    if(now - m_enterTime >= 10) {
      stopTimer();
      waitCursor(false);
      int iresult = MessageBox(_T("The requested operation may take a very long time to complete.\n\rDo you want to continue?"),EMPTYSTRING
                              ,MB_YESNO + MB_ICONQUESTION);
      time(&m_enterTime);
      if(iresult == IDNO) {
        m_calc->terminateCalculation();
        waitCursor(false);
        startTimer(10);
      } else {
        waitCursor(true);
        startTimer(500);
      }
    } else if(now - m_enterTime >= 1 && m_timerInterval != 500) {
      waitCursor(true);
      stopTimer();
      startTimer(500);
    }
  }
  __super::OnTimer(nIDEvent);
}

void CCalculatorDlg::activateButton(int id) {
  MyButton *but = findButton(id);
  if(but != nullptr && !but->isEnabled()) {
    return;
  }
  m_calc->enter(id);
  time(&m_enterTime);
  startTimer(50);
}

MyButton *CCalculatorDlg::findButton(int id) {
  for(size_t i = 0; i < m_buttons.size(); i++) {
    if(m_buttons[i]->GetDlgCtrlID() == id) {
      return m_buttons[i];
    }
  }
  return nullptr;
}

MyButton *CCalculatorDlg::findButton(const CPoint &p) {
  for(size_t i = 0; i < m_buttons.size(); i++) {
    if(m_buttons[i]->pointInWindow(p)) {
      return m_buttons[i];
    }
  }
  return nullptr;
}

void CCalculatorDlg::scrollLine(int count) {
  CEdit *d = (CEdit*)GetDlgItem(IDC_DISPLAY);
  int i = d->GetScrollPos(SB_VERT);
  d->SetScrollPos(SB_VERT, i+count);
}

void CCalculatorDlg::info(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  const String str = vformat(format, argptr);
  va_end(argptr);
  setWindowText(this, IDC_INFO, str);
}

BOOL CCalculatorDlg::PreTranslateMessage(MSG *pMsg) {
/*
  CStatic *stat = (CStatic*)GetDlgItem(IDC_CHARPRESSED);
  char tmp[100];
  sprintf(tmp,"%d. %x %x",pMsg->message, pMsg->lParam,pMsg->wParam);
*/
  if(TranslateAccelerator(m_hWnd, m_accelTabel, pMsg)) {
    return true;
  }

  switch(pMsg->message) {
  case WM_LBUTTONDOWN:
  case WM_LBUTTONDBLCLK:
    m_selectedButton = findButton(pMsg->pt);
    if(m_selectedButton != nullptr && m_selectedButton->isEnabled()) {
      info(_T("%s"), m_selectedButton->isEnabled() ? _T("enabled") : _T("disabled"));
      m_selectedButton->press(true);
      return true;
    }
    break;
  case WM_LBUTTONUP:
    if(m_selectedButton != nullptr && m_selectedButton->isPressed() && m_selectedButton->pointInWindow(pMsg->pt)) {
      m_selectedButton->press(false);
      activateButton(m_selectedButton->GetDlgCtrlID());
      m_selectedButton = nullptr;
      return true;
    }
    break;
  case WM_MOUSEMOVE:
    if((pMsg->lParam & MK_LBUTTON) && m_selectedButton != nullptr) {
      m_selectedButton->press(m_selectedButton->pointInWindow(pMsg->pt));
    }
    break;
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_UP      : scrollLine(-1);  return true;
    case VK_DOWN    : scrollLine(1);   return true;
    case VK_PRIOR   : scrollLine(-4);  return true;
    case VK_NEXT    : scrollLine(4);   return true;
    case VK_HOME    :
    case VK_END     :                  break;
/*
    case VK_SHIFT   :
    case VK_CONTROL :
    case VK_MENU    : break;
    default         : return true;
*/
    }
  }
  return __super::PreTranslateMessage(pMsg);
}

void CCalculatorDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  DWORD helpid = pWnd->GetWindowContextHelpId();
  if(helpid != 0) {
    m_helpid = helpid;
    CMenu menu;
    if(!menu.LoadMenu(IDR_MENUCONTEXTHELP)) {
      showWarning(_T("Loadmenu failed"));
      return;
    }
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
  }
}

void CCalculatorDlg::OnControlWhatsthis() {
  showInformation(_T("helpid:%lx"),m_helpid);
}

void CCalculatorDlg::OnEditCopy() {
  String tmp = (LPCTSTR)m_display;
  tmp.replace(',', '.');
  putClipboard(m_hWnd, tmp);
}

void CCalculatorDlg::OnEditPaste() {
  const String tmp = getClipboardText();
  m_calc->pasteText(tmp);
  showStatus();
}

void CCalculatorDlg::OnViewPrecision() {
  PrecisionDlg dlg(m_calc->getPrecision());
  if(dlg.DoModal() == IDOK) {
    m_calc->setPrecision(dlg.m_precision);
    showStatus();
  }
}

void CCalculatorDlg::OnF2() {
  if(m_calc->getRadix() == 10) {
    activateButton(IDC_RADIODEGREES);
  } else {
    activateButton(IDC_RADIOBYTE);
  }
}

void CCalculatorDlg::OnF3() {
  if(m_calc->getRadix() == 10) {
    activateButton(IDC_RADIORADIANS);
  } else {
    activateButton(IDC_RADIOWORD);
  }
}

void CCalculatorDlg::OnF4() {
  if(m_calc->getRadix() == 10) {
    activateButton(IDC_RADIOGRADS);
  } else {
    activateButton(IDC_RADIODWORD);
  }
}

void CCalculatorDlg::OnCheckInv()          { activateButton(IDC_CHECKINV);          }
void CCalculatorDlg::OnCheckHyp()          { activateButton(IDC_CHECKHYP);          }
void CCalculatorDlg::OnRadioBin()          { activateButton(IDC_RADIOBIN);          }
void CCalculatorDlg::OnRadioOct()          { activateButton(IDC_RADIOOCT);          }
void CCalculatorDlg::OnRadioDec()          { activateButton(IDC_RADIODEC);          }
void CCalculatorDlg::OnRadioHex()          { activateButton(IDC_RADIOHEX);          }
void CCalculatorDlg::OnViewBin()           { activateButton(IDC_RADIOBIN);          }
void CCalculatorDlg::OnViewOct()           { activateButton(IDC_RADIOOCT);          }
void CCalculatorDlg::OnViewDec()           { activateButton(IDC_RADIODEC);          }
void CCalculatorDlg::OnViewHex()           { activateButton(IDC_RADIOHEX);          }
void CCalculatorDlg::OnViewDigitGrouping() { activateButton(ID_VIEW_DIGITGROUPING); }
void CCalculatorDlg::OnF12()               { activateButton(IDC_RADIOQWORD);        }
void CCalculatorDlg::OnShftF12()           { activateButton(IDC_RADIOOWORD);        }
void CCalculatorDlg::OnCe()                { activateButton(IDC_BUTTONCE);          }
void CCalculatorDlg::OnBack()              { activateButton(IDC_BUTTONBACKSPACE);   }
void CCalculatorDlg::OnClear()             { activateButton(IDC_BUTTONCLEAR);       }
void CCalculatorDlg::OnEqual()             { activateButton(IDC_BUTTONEQUAL);       }
void CCalculatorDlg::OnNumber0()           { activateButton(IDC_BUTTON0);           }
void CCalculatorDlg::OnNumber1()           { activateButton(IDC_BUTTON1);           }
void CCalculatorDlg::OnNumber2()           { activateButton(IDC_BUTTON2);           }
void CCalculatorDlg::OnNumber3()           { activateButton(IDC_BUTTON3);           }
void CCalculatorDlg::OnNumber4()           { activateButton(IDC_BUTTON4);           }
void CCalculatorDlg::OnNumber5()           { activateButton(IDC_BUTTON5);           }
void CCalculatorDlg::OnNumber6()           { activateButton(IDC_BUTTON6);           }
void CCalculatorDlg::OnNumber7()           { activateButton(IDC_BUTTON7);           }
void CCalculatorDlg::OnNumber8()           { activateButton(IDC_BUTTON8);           }
void CCalculatorDlg::OnNumber9()           { activateButton(IDC_BUTTON9);           }
void CCalculatorDlg::OnNumberA()           { activateButton(IDC_BUTTONA);           }
void CCalculatorDlg::OnNumberB()           { activateButton(IDC_BUTTONB);           }
void CCalculatorDlg::OnNumberC()           { activateButton(IDC_BUTTONC);           }
void CCalculatorDlg::OnNumberD()           { activateButton(IDC_BUTTOND);           }
void CCalculatorDlg::OnNumberE()           { activateButton(IDC_BUTTONE);           }
void CCalculatorDlg::OnNumberF()           { activateButton(IDC_BUTTONF);           }
void CCalculatorDlg::OnComma()             { activateButton(IDC_BUTTONCOMMA);       }
void CCalculatorDlg::OnSign()              { activateButton(IDC_BUTTONSIGN);        }
void CCalculatorDlg::OnEE()                { activateButton(IDC_BUTTONEE);          }
void CCalculatorDlg::OnAdd()               { activateButton(IDC_BUTTONADD);         }
void CCalculatorDlg::OnSub()               { activateButton(IDC_BUTTONSUB);         }
void CCalculatorDlg::OnMult()              { activateButton(IDC_BUTTONMULT);        }
void CCalculatorDlg::OnDiv()               { activateButton(IDC_BUTTONDIV);         }
void CCalculatorDlg::OnMod()               { activateButton(IDC_BUTTONMOD);         }
void CCalculatorDlg::OnSquare()            { activateButton(IDC_BUTTONSQUARE);      }
void CCalculatorDlg::OnPow3()              { activateButton(IDC_BUTTONPOW3);        }
void CCalculatorDlg::OnPow()               { activateButton(IDC_BUTTONPOW);         }
void CCalculatorDlg::OnLn()                { activateButton(IDC_BUTTONLN);          }
void CCalculatorDlg::OnLog()               { activateButton(IDC_BUTTONLOG);         }
void CCalculatorDlg::OnSin()               { activateButton(IDC_BUTTONSIN);         }
void CCalculatorDlg::OnCos()               { activateButton(IDC_BUTTONCOS);         }
void CCalculatorDlg::OnTan()               { activateButton(IDC_BUTTONTAN);         }
void CCalculatorDlg::OnPi()                { activateButton(IDC_BUTTONPI);          }
void CCalculatorDlg::OnReciproc()          { activateButton(IDC_BUTTONRECIPROC);    }
void CCalculatorDlg::OnFac()               { activateButton(IDC_BUTTONFAC);         }
void CCalculatorDlg::OnDMS()               { activateButton(IDC_BUTTONDMS);         }
void CCalculatorDlg::OnAnd()               { activateButton(IDC_BUTTONAND);         }
void CCalculatorDlg::OnOr()                { activateButton(IDC_BUTTONOR);          }
void CCalculatorDlg::OnXor()               { activateButton(IDC_BUTTONXOR);         }
void CCalculatorDlg::OnLsh()               { activateButton(IDC_BUTTONLSH);         }
void CCalculatorDlg::OnNot()               { activateButton(IDC_BUTTONNOT);         }
void CCalculatorDlg::OnInt()               { activateButton(IDC_BUTTONINT);         }
void CCalculatorDlg::OnLpar()              { activateButton(IDC_BUTTONLPAR);        }
void CCalculatorDlg::OnRpar()              { activateButton(IDC_BUTTONRPAR);        }

void CCalculatorDlg::OnIgnore() {
  // do nothing
}

void CCalculatorDlg::OnShowinfo() {
  CWnd *w = GetDlgItem(IDC_INFO);
  w->ShowWindow(w->IsWindowVisible() ? SW_HIDE : SW_SHOW);
}

static String loadString(int id) {
  TCHAR buffer[4096];
  LoadString(GetModuleHandle(nullptr),id,buffer,sizeof(buffer));
  buffer[ARRAYSIZE(buffer)-1] = '\0';
  return buffer;
};

BOOL CCalculatorDlg::OnToolTipNotify(UINT id, NMHDR *pNMHDR, LRESULT *pResult) {
  TOOLTIPTEXT *pTTT = (TOOLTIPTEXT*)pNMHDR; // Get the tooltip structure.
  UINT_PTR CtrlHandle = pNMHDR->idFrom; // Actually the idFrom holds Control's handle.

  // Check once again that the idFrom holds handle itself.
  if(pTTT->uFlags & TTF_IDISHWND) {
    UINT nID = ::GetDlgCtrlID(HWND(CtrlHandle));

#if (_MFC_VER < 0x0700)
    _AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
#else
     AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
#endif
    CToolTipCtrl* pToolTip = pThreadState->m_pToolTip;
    if(pToolTip) {
      pToolTip->SetMaxTipWidth(SHRT_MAX); // Do this to make \r\n work!
      CRect mr(15,15,15,15);
      pToolTip->SetMargin(&mr);
      pToolTip->SetDelayTime(TTDT_AUTOPOP,10000);
      pToolTip->SetTipBkColor(RGB(255,255,190));
    }

    m_toolTipText = loadString(nID);
    if(m_toolTipText.length()) {
      pTTT->lpszText = m_toolTipText.cstr();
      return TRUE;
    }
  }
  return FALSE;
}
