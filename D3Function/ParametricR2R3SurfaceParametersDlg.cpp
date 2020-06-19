#include "stdafx.h"
#include "ParametricR2R3SurfaceParametersDlg.h"

CParametricR2R3SurfaceParametersDlg::CParametricR2R3SurfaceParametersDlg(const ExprParametricR2R3SurfaceParameters &param, AbstractTextureFactory &atf, CWnd *pParent /*=NULL*/)
: SaveLoadExprWithCommonParametersDialog(IDD, pParent, param, atf, _T("Parametric Surface"), _T("par"))
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
  DDX_Text( pDX, IDC_EDIT_TSTEPCOUNT    , m_tStepCount    );
  DDV_MinMaxUInt(pDX, m_tStepCount      , 1, 200          );
  DDX_Text( pDX, IDC_EDIT_SSTEPCOUNT    , m_sStepCount    );
  DDV_MinMaxUInt(pDX, m_sStepCount      , 1, 200          );
}

BEGIN_MESSAGE_MAP(CParametricR2R3SurfaceParametersDlg, CDialog)
  ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
  ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
  ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
  ON_BN_CLICKED(IDC_BUTTON_HELPX           , OnButtonHelpX                    )
  ON_BN_CLICKED(IDC_BUTTON_HELPY           , OnButtonHelpY                    )
  ON_BN_CLICKED(IDC_BUTTON_HELPZ           , OnButtonHelpZ                    )
  ON_BN_CLICKED(IDC_CHECK_MACHINECODE      , OnBnClickedCheckMachineCode      )
  ON_BN_CLICKED(IDC_CHECK_CALCULATETEXTURE , OnBnClickedCheckCalculateTexture )
  ON_BN_CLICKED(IDC_BUTTON_BROWSETEXTURE   , OnBnClickedButtonBrowseTexture   )
  ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnBnClickedCheckIncludeTime      )
END_MESSAGE_MAP()

BOOL CParametricR2R3SurfaceParametersDlg::OnInitDialog() {
  __super::OnInitDialog();
  createExprHelpButton(IDC_BUTTON_HELPX, IDC_EDIT_EXPRX);
  createExprHelpButton(IDC_BUTTON_HELPY, IDC_EDIT_EXPRY);
  createExprHelpButton(IDC_BUTTON_HELPZ, IDC_EDIT_EXPRZ);
  setCommonExprFieldId(IDC_EDITCOMMON);
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
  __super::paramToWin(param);
}

bool CParametricR2R3SurfaceParametersDlg::winToParam(ExprParametricR2R3SurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr       = Expr3((LPCTSTR)m_exprX,(LPCTSTR)m_exprY,(LPCTSTR)m_exprZ, (LPCTSTR)m_commonText);
  param.m_tInterval  = DoubleInterval(m_tfrom, m_tto);
  param.m_sInterval  = DoubleInterval(m_sfrom, m_sto);
  param.m_tStepCount = m_tStepCount;
  param.m_sStepCount = m_sStepCount;
  return true;
}
