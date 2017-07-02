#include "stdafx.h"
#include "TestTreesEqualDlg.h"
#include "ExpressionTreeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestTreesEqualDlg::CTestTreesEqualDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestTreesEqualDlg::IDD, pParent) {
	m_expr1 = EMPTYSTRING;
	m_expr2 = EMPTYSTRING;

  m_focusCtrlId = -1;
}

void CTestTreesEqualDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_EDITEXPR1, m_expr1);
	DDX_CBString(pDX, IDC_EDITEXPR2, m_expr2);
}

BEGIN_MESSAGE_MAP(CTestTreesEqualDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
  ON_BN_CLICKED(    IDC_TESTTREESEQUAL            , OnTestTreesEqual            )
	ON_BN_CLICKED(    IDC_TESTTREESEQUALMINUS       , OnTestTreesEqualMinus       )
	ON_BN_CLICKED(    IDC_BUTTONCOMPILE             , OnButtonCompile             )
	ON_BN_CLICKED(    IDC_BUTTONCONVERT             , OnButtonConvert             )
	ON_COMMAND(       ID_CONTEXTMENU_SHOWNODETREE   , OnContextMenuShowTree       )
	ON_COMMAND(       ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis)
  ON_COMMAND(       ID_GOTOEXPR1                  , OnGotoExpr1                 )
  ON_COMMAND(       ID_GOTOEXPR2                  , OnGotoExpr2                 )
	ON_COMMAND(       ID_COMPILE                    , OnButtonCompile             )
	ON_CBN_SETFOCUS(  IDC_EDITEXPR1                 , OnSetFocusEditExpr1         )
	ON_CBN_KILLFOCUS( IDC_EDITEXPR1                 , OnKillFocusEditExpr1        )
	ON_CBN_SETFOCUS(  IDC_EDITEXPR2                 , OnSetFocusEditExpr2         )
	ON_CBN_KILLFOCUS( IDC_EDITEXPR2                 , OnKillFocusEditExpr2        )
END_MESSAGE_MAP()

BOOL CTestTreesEqualDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTabel = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDD_TREESEQUAL_ACCELERATOR));
  
  m_cb[0].substituteControl(this, IDC_EDITEXPR1, _T("EqualExpr1"));
  m_cb[1].substituteControl(this, IDC_EDITEXPR2, _T("EqualExpr2"));

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_BUTTONCOMPILE        , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_BUTTONCONVERT        , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_TESTTREESEQUAL       , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_TESTTREESEQUALMINUS  , RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_EDITEXPR1            , RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATICIMAGE1         , RELATIVE_WIDTH | PCT_RELATIVE_BOTTOM);
  m_layoutManager.addControl(IDC_STATICLABEL2         ,                  PCT_RELATIVE_Y_POS );
  m_layoutManager.addControl(IDC_EDITEXPR2            , RELATIVE_WIDTH | PCT_RELATIVE_Y_POS );
  m_layoutManager.addControl(IDC_STATICIMAGE2         , RELATIVE_WIDTH | PCT_RELATIVE_TOP   | RELATIVE_BOTTOM);

  m_device.attach(*this);

  return false;
}

void CTestTreesEqualDlg::OnPaint() {
  CPaintDC dc(this);
  paintImage(0);
  paintImage(1);
  ajourButtons();
}

void CTestTreesEqualDlg::ajourButtons() {
  const bool expOk           = m_e[0].isOk() && m_e[1].isOk();
  const bool isCanonicalForm = expOk && (m_e[0].getTreeForm() == TREEFORM_CANONICAL) && (m_e[1].getTreeForm() == TREEFORM_CANONICAL);

  GetDlgItem(IDC_TESTTREESEQUAL     )->EnableWindow(expOk && isCanonicalForm);
  GetDlgItem(IDC_TESTTREESEQUALMINUS)->EnableWindow(expOk && isCanonicalForm);
  GetDlgItem(IDC_BUTTONCONVERT      )->EnableWindow(expOk);
  if(expOk) {
    setWindowText(this, IDC_BUTTONCONVERT, isCanonicalForm ? _T("&To standard form") : _T("&To canonical form"));
  }
}

void CTestTreesEqualDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);	
}

BOOL CTestTreesEqualDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTabel, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CTestTreesEqualDlg::OnTestTreesEqual() {
  try {
    const ExpressionNode *root1 = m_e[0].getRoot();
    const ExpressionNode *root2 = m_e[1].getRoot();
    const bool            eq    = m_e[0].treesEqual(root1, root2);
    MessageBox(format(_T("Exspressions are %sequal"), eq?EMPTYSTRING:_T("not ")).cstr(), _T("Test"), MB_ICONINFORMATION);
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
  }
}

void CTestTreesEqualDlg::OnTestTreesEqualMinus() {
  try {
    const bool eq = m_e[0].equalMinus(m_e[1]);
    MessageBox(format(_T("Exspressions are %sNegative-Equal"), eq?EMPTYSTRING:_T("not ")).cstr(), _T("Test"), MB_ICONINFORMATION);
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
  }
}

void CTestTreesEqualDlg::OnGotoExpr1() {
  m_cb[0].SetFocus();
}

void CTestTreesEqualDlg::OnGotoExpr2() {
  m_cb[1].SetFocus();
}

void CTestTreesEqualDlg::OnButtonCompile() {
  compile(0);
  compile(1);
  Invalidate();
}

void CTestTreesEqualDlg::OnEditFindMatchingParentesis() {
  const int ctrlId = m_focusCtrlId;

  if((ctrlId != IDC_EDITEXPR1) && (ctrlId != IDC_EDITEXPR2)) return;

  CComboBox *cb = (CComboBox*)GetDlgItem(ctrlId);
  const String text = getWindowText(this, ctrlId);
  int cursorPos = cb->GetEditSel() & 0xffff;
  const int m = findMatchingpParanthes(text.cstr(), cursorPos);
  if(m >= 0) {
    cb->SetEditSel(m, m);
  }
}

void CTestTreesEqualDlg::OnButtonConvert() {
  const bool expOk = m_e[0].isOk() && m_e[1].isOk();
  if(expOk) {
    const bool isCanonicalForm = (m_e[0].getTreeForm() == TREEFORM_CANONICAL) && (m_e[1].getTreeForm() == TREEFORM_CANONICAL);
    if(isCanonicalForm) {
      m_e[0].toStandardForm();
      m_e[1].toStandardForm();
    } else {
      m_e[0].toCanonicalForm();
      m_e[1].toCanonicalForm();
    }
    makeImage(0);
    makeImage(1);
    Invalidate();
  }
}

bool CTestTreesEqualDlg::compile(int index) {
  Expression &e = m_e[index];

  const String str = getWindowText(&m_cb[index]);
  e.compile(str, false);
  if(e.isOk()) {
    makeImage(index);
    m_cb[index].updateList();
    return true;
  } else {
    destroyImage(index);
  }

  const StringArray &errors = e.getErrors();
  String error = errors[0];
  int pos = e.decodeErrorString(str, error);
  MessageBox(error.cstr(), _T("Error"), MB_ICONWARNING);
  return false;
}

void CTestTreesEqualDlg::makeImage(int index) {
  destroyImage(index);
  m_image[index] = makeImage(m_e[index]);
}

ExpressionImage CTestTreesEqualDlg::makeImage(const Expression &e) {
  try {
    return expressionToImage(m_device, e, 24);
  } catch(Exception e) {
    MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    return ExpressionImage();
  }
}

void CTestTreesEqualDlg::paintImage(int index) {
  int imageWinId = (index == 0) ? IDC_STATICIMAGE1 : IDC_STATICIMAGE2;
  paintImage(imageWinId, m_image[index]);
}

void CTestTreesEqualDlg::paintImage(int winId, const ExpressionImage &image) {
  CWnd        *window = GetDlgItem(winId);
  const CRect cr      = getClientRect(window);
  CPaintDC    dc(window);
  dc.FillSolidRect(0,0,cr.Width(), cr.Height(), GetSysColor(COLOR_BTNFACE));

  if(!image.isEmpty()) {
    const PixRect *pr = image.getImage();
    const CSize    sz = pr->getSize();
    PixRect::bitBlt(dc,0,0,sz.cx,sz.cy,SRCCOPY,pr,0,0);
  }
}

void CTestTreesEqualDlg::destroyImage(int index) {
  m_image[index].clear();
}

void CTestTreesEqualDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  CPoint mouseDown = point;
  ScreenToClient(&mouseDown);
  const CRect image1Rect  = getWindowRect(this, IDC_STATICIMAGE1);
  const CRect image2Rect  = getWindowRect(this, IDC_STATICIMAGE2);

  if(image1Rect.PtInRect(mouseDown)) {
    m_contextWinIndex = 0;
  } else if(image2Rect.PtInRect(mouseDown)) {
    m_contextWinIndex = 1;
  } else {
    return;
  }

  if(!m_e[m_contextWinIndex].isOk()) return;

  CMenu menu;
  int ret = menu.LoadMenu(IDR_CONTEXTMENUSTACK);
  if(!ret) {
    MessageBox(_T("Loadmenu failed"), _T("Error"), MB_ICONWARNING);
    return;
  }
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
}

void CTestTreesEqualDlg::OnContextMenuShowTree() {
  CExpressionTreeDlg dlg(m_e[m_contextWinIndex]);
  dlg.DoModal();
}

void CTestTreesEqualDlg::OnSetFocusEditExpr1() {
  m_focusCtrlId = IDC_EDITEXPR1;
}

void CTestTreesEqualDlg::OnKillFocusEditExpr1() {
  m_focusCtrlId = -1;
}

void CTestTreesEqualDlg::OnSetFocusEditExpr2() {
  m_focusCtrlId = IDC_EDITEXPR2;
}

void CTestTreesEqualDlg::OnKillFocusEditExpr2() {
  m_focusCtrlId = -1;
}
