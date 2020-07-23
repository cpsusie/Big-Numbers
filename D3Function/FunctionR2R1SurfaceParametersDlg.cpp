#include "stdafx.h"
#include "FunctionR2R1SurfaceParametersDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CExprFunctionR2R1SurfaceParametersDlg::CExprFunctionR2R1SurfaceParametersDlg(const ExprFunctionR2R1SurfaceParameters &param, AbstractTextureFactory &atf, CWnd *pParent)
: SaveLoadExprWithCommonParametersDlg(IDD, pParent, param, atf, _T("expression"), _T("exp"))
{
}

void CExprFunctionR2R1SurfaceParametersDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_EXPR  , m_expr      );
  DDX_Text(pDX, IDC_EDIT_XFROM , m_xfrom     );
  DDX_Text(pDX, IDC_EDIT_XTO   , m_xto       );
  DDX_Text(pDX, IDC_EDIT_YFROM , m_yfrom     );
  DDX_Text(pDX, IDC_EDIT_YTO   , m_yto       );
  DDX_Text(pDX, IDC_EDIT_POINTS, m_pointCount);
}

BEGIN_MESSAGE_MAP(CExprFunctionR2R1SurfaceParametersDlg, CDialog)
  ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
  ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
  ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
  ON_BN_CLICKED(IDC_BUTTON_HELP            , OnButtonHelp                     )
  ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnBnClickedCheckIncludeTime      )
  ON_BN_CLICKED(IDC_CHECK_MACHINECODE      , OnBnClickedCheckMachineCode      )
  ON_BN_CLICKED(IDC_CHECK_CALCULATETEXTURE , OnBnClickedCheckCalculateTexture )
  ON_BN_CLICKED(IDC_BUTTON_BROWSETEXTURE   , OnBnClickedButtonBrowseTexture   )
END_MESSAGE_MAP()

BOOL CExprFunctionR2R1SurfaceParametersDlg::OnInitDialog() {
  __super::OnInitDialog();
  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDIT_EXPR);
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

void CExprFunctionR2R1SurfaceParametersDlg::OnButtonHelp() {
  handleExprHelpButtonClick(IDC_BUTTON_HELP);
}

void CExprFunctionR2R1SurfaceParametersDlg::paramToWin(const ExprFunctionR2R1SurfaceParameters &param) {
  m_expr             = param.m_expr.cstr();
  m_xfrom            = param.getXInterval().getMin();
  m_xto              = param.getXInterval().getMax();
  m_yfrom            = param.getYInterval().getMin();
  m_yto              = param.getYInterval().getMax();
  m_pointCount       = param.m_pointCount;
  __super::paramToWin(param);
}

bool CExprFunctionR2R1SurfaceParametersDlg::winToParam(ExprFunctionR2R1SurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr       = m_expr;
  param.m_xInterval  = DoubleInterval(   m_xfrom, m_xto);
  param.m_yInterval  = DoubleInterval(   m_yfrom, m_yto);
  param.m_pointCount = m_pointCount;
  return true;
}
