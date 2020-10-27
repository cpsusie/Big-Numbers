#include "stdafx.h"
#include "ParametricR1R3SurfaceParametersDlg.h"
#include <D3DGraphics/Profile2D.h>

CParametricR1R3SurfaceParametersDlg::CParametricR1R3SurfaceParametersDlg(const ExprParametricR1R3SurfaceParameters &param, AbstractTextureFactory &atf, CWnd *pParent /*=nullptr*/)
: SaveLoadExprWithCommonParametersDlg(IDD, pParent, param, atf, _T("Parametric Surface"), _T("parR1R3"))
, m_tStepCount(0)
, m_profileFileName(_T("")) {
}

CParametricR1R3SurfaceParametersDlg::~CParametricR1R3SurfaceParametersDlg() {
}

void CParametricR1R3SurfaceParametersDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_COMMON, m_commonText);
  DDX_Text(pDX, IDC_EDIT_EXPRX, m_exprX);
  DDX_Text(pDX, IDC_EDIT_EXPRY, m_exprY);
  DDX_Text(pDX, IDC_EDIT_EXPRZ, m_exprZ);
  DDX_Text(pDX, IDC_EDIT_TFROM, m_tfrom);
  DDX_Text(pDX, IDC_EDIT_TTO, m_tto);
  DDX_Text(pDX, IDC_EDIT_TSTEPCOUNT, m_tStepCount);
  DDV_MinMaxUInt(pDX, m_tStepCount, 1, 200);
  DDX_Text(pDX, IDC_EDIT_PROFILEFILENAME, m_profileFileName);
}

BEGIN_MESSAGE_MAP(CParametricR1R3SurfaceParametersDlg, CDialog)
  ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
  ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
  ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
  ON_BN_CLICKED(IDC_BUTTON_HELPX           , OnButtonHelpX                    )
  ON_BN_CLICKED(IDC_BUTTON_HELPY           , OnButtonHelpY                    )
  ON_BN_CLICKED(IDC_BUTTON_HELPZ           , OnButtonHelpZ                    )
  ON_BN_CLICKED(IDC_CHECK_MACHINECODE      , OnBnClickedCheckMachineCode      )
  ON_BN_CLICKED(IDC_BUTTON_BROWSEPROFILE   , OnBnClickedButtonBrowseProfile   )
  ON_BN_CLICKED(IDC_CHECK_CALCULATETEXTURE , OnBnClickedCheckCalculateTexture )
  ON_BN_CLICKED(IDC_BUTTON_BROWSETEXTURE   , OnBnClickedButtonBrowseTexture   )
  ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnBnClickedCheckIncludeTime      )
END_MESSAGE_MAP()

BOOL CParametricR1R3SurfaceParametersDlg::OnInitDialog() {
  __super::OnInitDialog();
  createExprHelpButton(IDC_BUTTON_HELPX, IDC_EDIT_EXPRX);
  createExprHelpButton(IDC_BUTTON_HELPY, IDC_EDIT_EXPRY);
  createExprHelpButton(IDC_BUTTON_HELPZ, IDC_EDIT_EXPRZ);
  setCommonExprFieldId(IDC_EDIT_COMMON);
  gotoEditBox(this, IDC_EDIT_EXPRX);
  return FALSE;  // return TRUE  unless you set the focus to a control
}

#define MAXPOINTCOUNT 200

bool CParametricR1R3SurfaceParametersDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_TFROM, IDC_EDIT_TTO)) {
    return false;
  }
  if(!validateMinMax(IDC_EDIT_TSTEPCOUNT, 1, MAXPOINTCOUNT)) {
    return false;
  }
  if(m_profileFileName.GetLength() == 0) {
    gotoEditBox(this, IDC_EDIT_PROFILEFILENAME);
    showWarning(_T("Profile filename must be filled"));
    return false;
  }

  try {
    Profile2D profile;
    profile.load((LPCTSTR)m_profileFileName);
  } catch (Exception e) {
    gotoEditBox(this, IDC_EDIT_PROFILEFILENAME);
    showWarning(_T("File %s is not a cvalid profile"));
    return false;
  }
  return __super::validate();
}

void CParametricR1R3SurfaceParametersDlg::enableTimeFields() {
  __super::enableTimeFields();
  setWindowText(this, IDC_STATIC_FUNCTIONX, m_includeTime ? _T("&X(time,t) =") : _T("&X(t) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONY, m_includeTime ? _T("&Y(time,t) =") : _T("&Y(t) ="));
  setWindowText(this, IDC_STATIC_FUNCTIONZ, m_includeTime ? _T("&Z(time,t) =") : _T("&Z(t) ="));
}

void CParametricR1R3SurfaceParametersDlg::OnEditFindMatchingParentesis() {
  gotoMatchingParentesis();
}

void CParametricR1R3SurfaceParametersDlg::OnButtonHelpX() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPX);
}

void CParametricR1R3SurfaceParametersDlg::OnButtonHelpY() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPY);
}

void CParametricR1R3SurfaceParametersDlg::OnButtonHelpZ() {
  handleExprHelpButtonClick(IDC_BUTTON_HELPZ);
}

void CParametricR1R3SurfaceParametersDlg::OnBnClickedButtonBrowseProfile() {
  const String fileName = selectAndValidateProfileFile();
  if(fileName.length() > 0) {
    setWindowText(this, IDC_EDIT_PROFILEFILENAME, fileName);
  }
}


void CParametricR1R3SurfaceParametersDlg::paramToWin(const ExprParametricR1R3SurfaceParameters &param) {
  const Expr3 &e = param.m_expr;
  m_commonText      = e.getCommonText().cstr();
  m_exprX           = e.getRawText(0).cstr();
  m_exprY           = e.getRawText(1).cstr();
  m_exprZ           = e.getRawText(2).cstr();
  m_tfrom           = param.getTInterval().getMin();
  m_tto             = param.getTInterval().getMax();
  m_tStepCount      = param.m_tStepCount;
  m_profileFileName = param.m_profileFileName.cstr();
  __super::paramToWin(param);
}

bool CParametricR1R3SurfaceParametersDlg::winToParam(ExprParametricR1R3SurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr            = Expr3((LPCTSTR)m_exprX,(LPCTSTR)m_exprY,(LPCTSTR)m_exprZ, (LPCTSTR)m_commonText);
  param.m_tInterval       = DoubleInterval(m_tfrom, m_tto);
  param.m_tStepCount      = m_tStepCount;
  param.m_profileFileName = m_profileFileName;
  return true;
}
