#include "stdafx.h"
#include "ParametricGraphDlg.h"

CParametricGraphDlg::CParametricGraphDlg(ParametricGraphParameters &param, CWnd *pParent)
: SaveLoadExprDialog<ParametricGraphParameters>(IDD, pParent, param, _T("parametric curve"), _T("par"))
{
}

CParametricGraphDlg::~CParametricGraphDlg() {
}

void CParametricGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITCOMMON, m_commonText);
  DDX_Text(pDX, IDC_EDITEXPRX, m_exprX);
  DDX_Text(pDX, IDC_EDITEXPRY, m_exprY);
  DDX_Text(pDX, IDC_EDITTFROM, m_tFrom);
  DDX_Text(pDX, IDC_EDITTTO  , m_tTo  );
  DDX_Text(pDX, IDC_EDITSTEPS, m_steps);
  DDV_MinMaxUInt(pDX, m_steps, 1, 10000);
  DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
}

BEGIN_MESSAGE_MAP(CParametricGraphDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_COMMON                                    , OnGotoCommon                )
  ON_COMMAND(   ID_GOTO_EXPRX                                     , OnGotoExprX                 )
  ON_COMMAND(   ID_GOTO_EXPRY                                     , OnGotoExprY                 )
  ON_COMMAND(   ID_GOTO_TINTERVAL                                 , OnGotoTInterval             )
  ON_COMMAND(   ID_GOTO_STEP                                      , OnGotoStep                  )
  ON_BN_CLICKED(IDC_BUTTON_HELPX                                  , OnButtonHelpX               )
  ON_BN_CLICKED(IDC_BUTTON_HELPY                                  , OnButtonHelpY               )
END_MESSAGE_MAP()

BOOL CParametricGraphDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELPX, IDC_EDITEXPRX);
  createExprHelpButton(IDC_BUTTON_HELPY, IDC_EDITEXPRY);
  setCommonExprFieldId(IDC_EDITCOMMON);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDOK                 , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL             , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDC_STATICCOMMONLABEL, PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITCOMMON       , RELATIVE_WIDTH | RESIZE_FONT | PCT_RELATIVE_BOTTOM);
  m_layoutManager.addControl(IDC_STATICEXPRXLABEL , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPX     , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITEXPRX        , RELATIVE_WIDTH | RESIZE_FONT | PCT_RELATIVE_TOP | PCT_RELATIVE_BOTTOM);
  m_layoutManager.addControl(IDC_STATICEXPRYLABEL , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPY     , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITEXPRY        , RELATIVE_WIDTH | RESIZE_FONT | PCT_RELATIVE_TOP | RELATIVE_BOTTOM);

  m_layoutManager.addControl(IDC_STATICTINTERVAL , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITTFROM       , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_STATICDASH      , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITTTO         , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITTFROM       , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_STATICSTEPS     , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_EDITSTEPS       , RELATIVE_Y_POS);

  gotoEditBox(this, IDC_EDITEXPRX);
  return FALSE;
}

bool CParametricGraphDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(!validateInterval(IDC_EDITTFROM, IDC_EDITTTO)) {
    return false;
  }
  return true;
}

void CParametricGraphDlg::OnGotoCommon() {
  gotoExpr(IDC_EDITCOMMON);
}
void CParametricGraphDlg::OnGotoExprX() {
  gotoExpr(IDC_EDITEXPRX);
}

void CParametricGraphDlg::OnGotoExprY() {
  gotoExpr(IDC_EDITEXPRY);
}

void CParametricGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CParametricGraphDlg::OnGotoTInterval() {
  gotoEditBox(this,IDC_EDITTFROM);
}

void CParametricGraphDlg::OnGotoStep() {
  gotoEditBox(this,IDC_EDITSTEPS);
}

void CParametricGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

void CParametricGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParentesis();
}

void CParametricGraphDlg::OnButtonHelpX() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPX);
}

void CParametricGraphDlg::OnButtonHelpY() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPY);
}

void CParametricGraphDlg::paramToWin(const ParametricGraphParameters &param) {
  m_style      = GraphParameters::graphStyleToString(param.m_style);
  getColorButton()->SetColor(param.m_color);
  m_commonText = param.m_commonText.cstr();
  m_exprX      = param.m_exprX.cstr();
  m_exprY      = param.m_exprY.cstr();
  m_tFrom      = param.m_interval.getMin();
  m_tTo        = param.m_interval.getMax();
  m_steps      = param.m_steps;
  __super::paramToWin(param);
}

void CParametricGraphDlg::winToParam(ParametricGraphParameters &param) const {
  param.m_commonText = m_commonText;
  param.m_exprX      = m_exprX;
  param.m_exprY      = m_exprY;
  param.m_style      = (GraphStyle)getStyleCombo()->GetCurSel();
  param.m_color      = getColorButton()->GetColor();
  param.m_interval.setFrom(m_tFrom);
  param.m_interval.setTo(m_tTo);
  param.m_steps      = m_steps;
  __super::winToParam(param);
}
