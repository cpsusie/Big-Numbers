#include "stdafx.h"
#include "ParametricR2R3SurfaceParametersDlg.h"

CParametricR2R3SurfaceParametersDlg::CParametricR2R3SurfaceParametersDlg(const ExprParametricR2R3SurfaceParameters &param, CWnd *pParent /*=NULL*/)
: SaveLoadExprWithAnimationDialog(IDD, pParent, param,_T("Parametric Surface"), _T("par"))
, m_sStepCount(0)
, m_tStepCount(0)
{
}

CParametricR2R3SurfaceParametersDlg::~CParametricR2R3SurfaceParametersDlg() {
}

void CParametricR2R3SurfaceParametersDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text( pDX, IDC_EDITCOMMON         , m_commonText    );
  DDX_Text( pDX, IDC_EDIT_EXPRX         , m_exprX         );
  DDX_Text( pDX, IDC_EDIT_EXPRY         , m_exprY         );
  DDX_Text( pDX, IDC_EDIT_EXPRZ         , m_exprZ         );
  DDX_Text( pDX, IDC_EDIT_TFROM         , m_tfrom         );
  DDX_Text( pDX, IDC_EDIT_TTO           , m_tto           );
  DDX_Text( pDX, IDC_EDIT_SFROM         , m_sfrom         );
  DDX_Text( pDX, IDC_EDIT_STO           , m_sto           );
  DDX_Check(pDX, IDC_CHECK_DOUBLESIDED  , m_doubleSided   );
  DDX_Text( pDX, IDC_EDIT_TSTEPCOUNT    , m_tStepCount    );
  DDV_MinMaxUInt(pDX, m_tStepCount      , 1, 200          );
  DDX_Text( pDX, IDC_EDIT_SSTEPCOUNT    , m_sStepCount    );
  DDV_MinMaxUInt(pDX, m_sStepCount      , 1, 200          );
}

BEGIN_MESSAGE_MAP(CParametricR2R3SurfaceParametersDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
  ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
  ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
  ON_COMMAND(ID_GOTO_COMMON                , OnGotoCommon                     )
  ON_COMMAND(ID_GOTO_EXPRX                 , OnGotoExprX                      )
  ON_COMMAND(ID_GOTO_EXPRY                 , OnGotoExprY                      )
  ON_COMMAND(ID_GOTO_EXPRZ                 , OnGotoExprZ                      )
  ON_COMMAND(ID_GOTO_TINTERVAL             , OnGotoTInterval                  )
  ON_COMMAND(ID_GOTO_SINTERVAL             , OnGotoSInterval                  )
  ON_COMMAND(ID_GOTO_TSTEPCOUNT            , OnGotoTStepCount                 )
  ON_COMMAND(ID_GOTO_SSTEPCOUNT            , OnGotoSStepCount                 )
  ON_COMMAND(ID_GOTO_TIMEINTERVAL          , OnGotoTimeInterval               )
  ON_COMMAND(ID_GOTO_FRAMECOUNT            , OnGotoFrameCount                 )
  ON_BN_CLICKED(IDC_BUTTON_HELPX           , OnButtonHelpX                    )
  ON_BN_CLICKED(IDC_BUTTON_HELPY           , OnButtonHelpY                    )
  ON_BN_CLICKED(IDC_BUTTON_HELPZ           , OnButtonHelpZ                    )
  ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime               )
  ON_BN_CLICKED(IDC_CHECK_MACHINECODE      , OnCheckMachineCode               )
END_MESSAGE_MAP()

BOOL CParametricR2R3SurfaceParametersDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELPX, IDC_EDIT_EXPRX);
  createExprHelpButton(IDC_BUTTON_HELPY, IDC_EDIT_EXPRY);
  createExprHelpButton(IDC_BUTTON_HELPZ, IDC_EDIT_EXPRZ);
  setCommonExprFieldId(IDC_EDITCOMMON);
  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICCOMMON        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDITCOMMON          , PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONX    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPX        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRX          , PCT_RELATIVE_TOP    | PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONY    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPY        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRY          , PCT_RELATIVE_TOP    | PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_FUNCTIONZ    , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_BUTTON_HELPZ        , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPRZ          , PCT_RELATIVE_TOP    | RELATIVE_BOTTOM     | RELATIVE_WIDTH);
  m_layoutManager.addControl(IDC_STATIC_TINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH1        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_SINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_SFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH2        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_STO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TSTEPCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TSTEPCOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_SSTEPCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_SSTEPCOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_MACHINECODE   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECKCREATELISTFILE , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_INCLUDETIME   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_TIMEINTERVAL , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMEFROM       , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH3        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_TIMETO         , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_FRAMECOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_FRAMECOUNT     , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDOK                    , RELATIVE_POSITION    );
  m_layoutManager.addControl(IDCANCEL                , RELATIVE_POSITION    );

  gotoEditBox(this, IDC_EDIT_EXPRX);
  return FALSE;  // return TRUE  unless you set the focus to a control
}

#define MAXPOINTCOUNT 200

bool CParametricR2R3SurfaceParametersDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_TFROM, IDC_EDIT_TTO)) {
    return false;
  }
  if(!validateMinMax(IDC_EDIT_TSTEPCOUNT, 1, MAXPOINTCOUNT)) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_SFROM, IDC_EDIT_STO)) {
    return false;
  }
  if(!validateMinMax(IDC_EDIT_SSTEPCOUNT, 1, MAXPOINTCOUNT)) {
    return false;
  }
  return __super::validate();
}

void CParametricR2R3SurfaceParametersDlg::enableTimeFields() {
  __super::enableTimeFields();
  setWindowText(this, IDC_STATIC_FUNCTIONX, m_includeTime ? _T("&X(time,t,s) =") : _T("&X(t,s) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONY, m_includeTime ? _T("&Y(time,t,s) =") : _T("&Y(t,s) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONZ, m_includeTime ? _T("&Z(time,t,s) =") : _T("&Z(t,s) ="));
}

void CParametricR2R3SurfaceParametersDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CParametricR2R3SurfaceParametersDlg::OnGotoCommon() {
  gotoExpr(IDC_EDITCOMMON);
}
void CParametricR2R3SurfaceParametersDlg::OnGotoExprX() {
  gotoExprX();
}
void CParametricR2R3SurfaceParametersDlg::OnGotoExprY() {
  gotoExprY();
}
void CParametricR2R3SurfaceParametersDlg::OnGotoExprZ() {
  gotoExprZ();
}
void CParametricR2R3SurfaceParametersDlg::OnGotoTInterval() {
  gotoEditBox(this, IDC_EDIT_TFROM);
}
void CParametricR2R3SurfaceParametersDlg::OnGotoSInterval() {
  gotoEditBox(this, IDC_EDIT_SFROM);
}
void CParametricR2R3SurfaceParametersDlg::OnGotoTStepCount() {
  gotoEditBox(this, IDC_EDIT_TSTEPCOUNT);
}
void CParametricR2R3SurfaceParametersDlg::OnGotoSStepCount() {
  gotoEditBox(this, IDC_EDIT_SSTEPCOUNT);
}

void CParametricR2R3SurfaceParametersDlg::OnButtonHelpX() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPX);
}

void CParametricR2R3SurfaceParametersDlg::OnButtonHelpY() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPY);
}

void CParametricR2R3SurfaceParametersDlg::OnButtonHelpZ() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPZ);
}

void CParametricR2R3SurfaceParametersDlg::paramToWin(const ExprParametricR2R3SurfaceParameters &param) {
  const Expr3 &e = param.m_expr;
  m_commonText    = e.getCommonText().cstr();
  m_exprX         = e.getRawText(0).cstr();
  m_exprY         = e.getRawText(1).cstr();
  m_exprZ         = e.getRawText(2).cstr();
  m_tfrom         = param.getTInterval().getMin();
  m_tto           = param.getTInterval().getMax();
  m_sfrom         = param.getSInterval().getMin();
  m_sto           = param.getSInterval().getMax();
  m_tStepCount    = param.m_tStepCount;
  m_sStepCount    = param.m_sStepCount;
  m_doubleSided   = param.m_doubleSided ? TRUE : FALSE;
  __super::paramToWin(param);
}

bool CParametricR2R3SurfaceParametersDlg::winToParam(ExprParametricR2R3SurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr = Expr3((LPCTSTR)m_exprX,(LPCTSTR)m_exprY,(LPCTSTR)m_exprZ, (LPCTSTR)m_commonText);
  param.m_tInterval.setFrom(   m_tfrom);
  param.m_tInterval.setTo(     m_tto);
  param.m_sInterval.setFrom(   m_sfrom);
  param.m_sInterval.setTo(     m_sto);
  param.m_tStepCount  = m_tStepCount;
  param.m_sStepCount  = m_sStepCount;
  param.m_doubleSided = m_doubleSided ? true : false;
  return true;
}
