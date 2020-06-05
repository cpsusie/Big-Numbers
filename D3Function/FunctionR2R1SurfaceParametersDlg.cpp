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
  DDX_Text(pDX, IDC_EDIT_EXPR, m_expr);
  DDX_Text(pDX, IDC_EDIT_XFROM, m_xfrom);
  DDX_Text(pDX, IDC_EDIT_XTO, m_xto);
  DDX_Text(pDX, IDC_EDIT_YFROM, m_yfrom);
  DDX_Text(pDX, IDC_EDIT_YTO, m_yto);
  DDX_Text(pDX, IDC_EDIT_POINTS, m_pointCount);
  DDX_Check(pDX, IDC_CHECK_DOUBLESIDED, m_doubleSided);
  DDX_Check(pDX, IDC_CHECK_HASTEXTURE, m_hasTexture);
  DDX_Check(pDX, IDC_CHECK_CALCULATENORMALS, m_calculateNormals);
  DDX_Text(pDX, IDC_EDIT_TEXTUREFILENAME, m_textureFileName);
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
  ON_BN_CLICKED(IDC_CHECK_HASTEXTURE       , OnBnClickedCheckHasTexture       )
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
  if(m_hasTexture) {
    if(m_textureFileName.GetLength() == 0) {
      gotoEditBox(this, IDC_EDIT_TEXTUREFILENAME);
      showWarning(_T("Must specify texture image file"));
      return false;
    }
  }
  return __super::validate();
}

void CExprFunctionR2R1SurfaceParametersDlg::OnBnClickedCheckHasTexture() {
  UpdateData();
  enableTextureFields(m_hasTexture);
}

void CExprFunctionR2R1SurfaceParametersDlg::enableTextureFields(bool enable) {
  enableWindowList(*this, enable,IDC_EDIT_TEXTUREFILENAME, IDC_BUTTON_BROWSETEXTURE, 0);
}

const TCHAR *s_loadFileDialogExtensions = _T("Picture files\0*.bmp;*.jpg;*.png;*.tiff;*.gif;*.ico;*.cur;*.dib;\0"
                                             "BMP-Files (*.bmp)\0*.bmp;\0"
                                             "JPG-files (*.jpg)\0*.jpg;\0"
                                             "PNG-files (*.png)\0*.png;\0"
                                             "TIFF-files (*.tiff)\0*.tiff;\0"
                                             "GIF-files (*.gif)\0*.gif;\0"
                                             "ICO-files (*.ico)\0*.ico;\0"
                                             "CUR-files (*.cur)\0*.cur;\0"
                                             "DIB-files (*.dib)\0*.dib;\0"
                                             "All files (*.*)\0*.*\0\0");


void CExprFunctionR2R1SurfaceParametersDlg::OnBnClickedButtonBrowseTexture() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = s_loadFileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open file");
  dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_ENABLESIZING;

  if(dlg.DoModal() == IDOK) {
    setWindowText(this, IDC_EDIT_TEXTUREFILENAME, dlg.m_ofn.lpstrFile);
  }
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
  m_expr             = param.m_expr.cstr();
  m_xfrom            = param.getXInterval().getMin();
  m_xto              = param.getXInterval().getMax();
  m_yfrom            = param.getYInterval().getMin();
  m_yto              = param.getYInterval().getMax();
  m_pointCount       = param.m_pointCount;
  m_doubleSided      = param.m_doubleSided ? TRUE : FALSE;
  m_calculateNormals = param.m_vertexParameters.m_hasNormals;
  m_hasTexture       = param.m_vertexParameters.m_hasTexture;
  m_textureFileName  = param.m_vertexParameters.m_textureFileName.cstr();
  enableTextureFields(m_hasTexture);
  __super::paramToWin(param);
}

bool CExprFunctionR2R1SurfaceParametersDlg::winToParam(ExprFunctionR2R1SurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr        = m_expr;
  param.m_xInterval.setFrom(   m_xfrom   );
  param.m_xInterval.setTo(     m_xto     );
  param.m_yInterval.setFrom(   m_yfrom   );
  param.m_yInterval.setTo(     m_yto     );
  param.m_pointCount                         = m_pointCount;
  param.m_doubleSided                        = m_doubleSided      ? true : false;
  param.m_vertexParameters.m_hasNormals      = m_calculateNormals ? true : false;
  param.m_vertexParameters.m_hasTexture      = m_hasTexture       ? true : false;
  param.m_vertexParameters.m_textureFileName = (LPCTSTR)m_textureFileName;
  return true;
}
