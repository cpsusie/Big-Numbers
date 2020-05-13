#include "stdafx.h"
#include "FunctionR2R1SurfaceParametersDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CExprFunctionR2R1SurfaceParametersDlg::CExprFunctionR2R1SurfaceParametersDlg(const ExprFunctionR2R1SurfaceParameters &param, CWnd *pParent)
: SaveLoadExprWithAnimationDialog(IDD, pParent, param, _T("expression"), _T("exp"))
{
}

void CExprFunctionR2R1SurfaceParametersDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text( pDX, IDC_EDIT_EXPR             , m_expr                      );
  DDX_Text( pDX, IDC_EDIT_XFROM            , m_xfrom                     );
  DDX_Text( pDX, IDC_EDIT_XTO              , m_xto                       );
  DDX_Text( pDX, IDC_EDIT_YFROM            , m_yfrom                     );
  DDX_Text( pDX, IDC_EDIT_YTO              , m_yto                       );
  DDX_Text( pDX, IDC_EDIT_POINTS           , m_pointCount                );
  DDX_Check(pDX, IDC_CHECK_DOUBLESIDED     , m_doubleSided               );
}

BEGIN_MESSAGE_MAP(CExprFunctionR2R1SurfaceParametersDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
  ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
  ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
  ON_COMMAND(ID_GOTO_EXPR                  , OnGotoExpr                       )
  ON_COMMAND(ID_GOTO_XINTERVAL             , OnGotoXInterval                  )
  ON_COMMAND(ID_GOTO_YINTERVAL             , OnGotoYInterval                  )
  ON_COMMAND(ID_GOTO_TIMEINTERVAL          , OnGotoTimeInterval               )
  ON_COMMAND(ID_GOTO_POINTCOUNT            , OnGotoPointCount                 )
  ON_COMMAND(ID_GOTO_FRAMECOUNT            , OnGotoFrameCount                 )
  ON_BN_CLICKED(IDC_BUTTON_HELP            , OnButtonHelp                     )
  ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime               )
  ON_BN_CLICKED(IDC_CHECK_MACHINECODE      , OnCheckMachineCode               )
END_MESSAGE_MAP()

BOOL CExprFunctionR2R1SurfaceParametersDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDIT_EXPR);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATIC_FUNCTION     , PCT_RELATIVE_Y_CENTER);
  m_layoutManager.addControl(IDC_EDIT_EXPR           , RELATIVE_SIZE        );
  m_layoutManager.addControl(IDC_STATIC_XINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_XFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH1        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_XTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_YINTERVAL    , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_YFROM          , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_DASH2        , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_YTO            , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_STATIC_POINTCOUNT   , RELATIVE_Y_POS       );
  m_layoutManager.addControl(IDC_EDIT_POINTS         , RELATIVE_Y_POS       );
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

  gotoEditBox(this, IDC_EDIT_EXPR);
  return FALSE;  // return TRUE  unless you set the focus to a control
}

#define MAXPOINTCOUNT 200

bool CExprFunctionR2R1SurfaceParametersDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(!validateMinMax(IDC_EDIT_POINTS, 1, MAXPOINTCOUNT)) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_XFROM, IDC_EDIT_XTO)) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_YFROM,IDC_EDIT_YTO)) {
    return false;
  }
  return __super::validate();
}


void CExprFunctionR2R1SurfaceParametersDlg::enableTimeFields() {
  __super::enableTimeFields();
  setWindowText(this, IDC_STATIC_FUNCTION, m_includeTime ? _T("&z = F(t,x,y) =") : _T("&z = F(x,y) ="));
}

void CExprFunctionR2R1SurfaceParametersDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CExprFunctionR2R1SurfaceParametersDlg::OnGotoExpr() {
  gotoExpr(IDC_EDIT_EXPR);
}

void CExprFunctionR2R1SurfaceParametersDlg::OnGotoXInterval() {
  gotoEditBox(this, IDC_EDIT_XFROM);
}

void CExprFunctionR2R1SurfaceParametersDlg::OnGotoYInterval() {
  gotoEditBox(this, IDC_EDIT_YFROM);
}

void CExprFunctionR2R1SurfaceParametersDlg::OnGotoPointCount() {
  gotoEditBox(this, IDC_EDIT_POINTS);
}

void CExprFunctionR2R1SurfaceParametersDlg::OnButtonHelp() {
  handleExprHelpButtonClick(IDC_BUTTON_HELP);
}

void CExprFunctionR2R1SurfaceParametersDlg::paramToWin(const ExprFunctionR2R1SurfaceParameters &param) {
  m_expr          = param.m_expr.cstr();
  m_xfrom         = param.getXInterval().getMin();
  m_xto           = param.getXInterval().getMax();
  m_yfrom         = param.getYInterval().getMin();
  m_yto           = param.getYInterval().getMax();
  m_pointCount    = param.m_pointCount;
  m_doubleSided   = param.m_doubleSided ? TRUE : FALSE;
  __super::paramToWin(param);
}

bool CExprFunctionR2R1SurfaceParametersDlg::winToParam(ExprFunctionR2R1SurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr        = m_expr;
  param.m_xInterval.setFrom(   m_xfrom   );
  param.m_xInterval.setTo(     m_xto     );
  param.m_yInterval.setFrom(   m_yfrom   );
  param.m_yInterval.setTo(     m_yto     );
  param.m_pointCount  = m_pointCount;
  param.m_doubleSided = m_doubleSided ? true : false;
  return true;
}
