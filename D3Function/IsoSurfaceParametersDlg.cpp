#include "stdafx.h"
#include "IsoSurfaceParametersDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

/*
static char *torusexpr  = "sqr(sqr(x)+sqr(y)+sqr(z)+0.25-0.01) - 4*0.25*(sqr(y)+sqr(z))";
static char *jackexpr   = "(1/(sqr(x)/9+4*sqr(y)+4*sqr(z))**4 +"
                          " 1/(sqr(y)/9+4*sqr(x)+4*sqr(z))**4 +"
                          " 1/(sqr(z)/9+4*sqr(y)+4*sqr(x))**4 +"
                          " 1/((1.3333333*x-4)**2+1.77777*sqr(y)+1.77777*sqr(z))**4 +"
                          " 1/((1.3333333*x+4)**2+1.77777*sqr(y)+1.77777*sqr(z))**4 +"
                          " 1/((1.3333333*y-4)**2+1.77777*sqr(x)+1.77777*sqr(z))**4 +"
                          " 1/((1.3333333*y+4)**2+1.77777*sqr(x)+1.77777*sqr(z))**4)**-0.25-1";
static char *wifflecube = "1-(sqr(0.4348*x)+sqr(0.4348*y)+sqr(0.4348*z))**-6 -"
                          "((0.5*x)**8+(0.5*y)**8+(0.5*z)**8)**6";

static char *blob3      = "s1=1/max(sqr(x+1)+sqr(y  )+sqr(z  ),0.00001);"
                          "s2=1/max(sqr(x  )+sqr(y+1)+sqr(z  ),0.00001);"
                          "s3=1/max(sqr(x  )+sqr(y  )+sqr(z+1),0.00001);"
                          "4 - s1 - s2 - s3";

*/

CIsoSurfaceParametersDlg::CIsoSurfaceParametersDlg(const ExprIsoSurfaceParameters &param, AbstractTextureFactory &atf, CWnd *pParent /*=NULL*/)
: SaveLoadExprWithCommonParametersDialog(IDD, pParent, param, atf, _T("implicit surface"), _T("imp"))
{
  m_debugPolygonizer = false;
}

void CIsoSurfaceParametersDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text( pDX, IDC_EDIT_EXPR             , m_expr            );
  DDX_Text( pDX, IDC_EDIT_CELLSIZE         , m_cellSize        );
  DDX_Text( pDX, IDC_EDIT_LAMBDA           , m_lambda          );
  DDX_Check(pDX, IDC_CHECK_TETRAHEDRAL     , m_tetrahedral     );
  DDX_Check(pDX, IDC_CHECK_TETRAOPTIMIZE4  , m_tetraOptimize4  );
  DDX_Check(pDX, IDC_CHECK_ADAPTIVECELLSIZE, m_adaptiveCellSize);
  DDX_Check(pDX, IDC_CHECK_ORIGINOUTSIDE   , m_originOutside   );
  DDX_Check(pDX, IDC_CHECK_DEBUGPOLYGONIZER, m_debugPolygonizer);
  DDX_Text( pDX, IDC_EDIT_XFROM            , m_xfrom           );
  DDX_Text( pDX, IDC_EDIT_XTO              , m_xto             );
  DDX_Text( pDX, IDC_EDIT_YFROM            , m_yfrom           );
  DDX_Text( pDX, IDC_EDIT_YTO              , m_yto             );
  DDX_Text( pDX, IDC_EDIT_ZFROM            , m_zfrom           );
  DDX_Text( pDX, IDC_EDIT_ZTO              , m_zto             );
}

BEGIN_MESSAGE_MAP(CIsoSurfaceParametersDlg, CDialog)
  ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                       )
  ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                       )
  ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                     )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis     )
  ON_BN_CLICKED(IDC_BUTTON_HELP            , OnButtonHelp                     )
  ON_BN_CLICKED(IDC_CHECK_MACHINECODE      , OnBnClickedCheckMachineCode      )
  ON_BN_CLICKED(IDC_CHECK_DOUBLESIDED      , OnBnClickedCheckDoubleSided      )
  ON_BN_CLICKED(IDC_CHECK_TETRAHEDRAL      , OnBnClickedCheckTetrahedral      )
  ON_BN_CLICKED(IDC_CHECK_CALCULATETEXTURE , OnBnClickedCheckCalculateTexture )
  ON_BN_CLICKED(IDC_BUTTON_BROWSETEXTURE   , OnBnClickedButtonBrowseTexture   )
  ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnBnClickedCheckIncludeTime      )
END_MESSAGE_MAP()

BOOL CIsoSurfaceParametersDlg::OnInitDialog() {
  __super::OnInitDialog();
  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDIT_EXPR);
  gotoEditBox(this, IDC_EDIT_EXPR);
  return FALSE;
}

#define MAXFRAMECOUNT 300

bool CIsoSurfaceParametersDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(m_cellSize <= 0) {
    gotoEditBox(this, IDC_EDIT_CELLSIZE);
    showWarning(_T("Size must be > 0"));
    return false;
  }
  if((m_lambda < 0) || (m_lambda > 0.45)) {
    gotoEditBox(this, IDC_EDIT_LAMBDA);
    showWarning(_T("Lambda must be >= 0, <= 0.45"));
    return false;
  }
  if(!validateInterval(IDC_EDIT_XFROM, IDC_EDIT_XTO)) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_YFROM, IDC_EDIT_YTO)) {
    return false;
  }
  if(!validateInterval(IDC_EDIT_ZFROM, IDC_EDIT_ZTO)) {
    return false;
  }
  return __super::validate();
}

void CIsoSurfaceParametersDlg::enableTimeFields() {
  __super::enableTimeFields();
  setWindowText(this, IDC_STATIC_FUNCTION, m_includeTime ? _T("&S(t,x,y,z) =") : _T("&S(x,y,z) ="));
#if !defined(ISODEBUGGER)
  const bool enableDebugPolygonizer = false;
#else
  const bool enableDebugPolygonizer = !m_includeTime;
#endif // ISODEBUGGER

  GetDlgItem(IDC_CHECK_DEBUGPOLYGONIZER)->EnableWindow(enableDebugPolygonizer);
  if(!enableDebugPolygonizer) {
    m_debugPolygonizer = false;
    CheckDlgButton(IDC_CHECK_DEBUGPOLYGONIZER, BST_UNCHECKED);
  }
}

void CIsoSurfaceParametersDlg::enableCheckBoxOrigin() {
  GetDlgItem(IDC_CHECK_ORIGINOUTSIDE)->EnableWindow(!IsDlgButtonChecked(IDC_CHECK_DOUBLESIDED));
}
void CIsoSurfaceParametersDlg::enableCheckBoxTetraOptimize4() {
  GetDlgItem(IDC_CHECK_TETRAOPTIMIZE4)->EnableWindow(IsDlgButtonChecked(IDC_CHECK_TETRAHEDRAL));
}

void CIsoSurfaceParametersDlg::OnBnClickedCheckDoubleSided()  { enableCheckBoxOrigin();                      }
void CIsoSurfaceParametersDlg::OnBnClickedCheckTetrahedral()  { enableCheckBoxTetraOptimize4();              }
void CIsoSurfaceParametersDlg::OnEditFindMatchingParentesis() { gotoMatchingParentesis();                    }
void CIsoSurfaceParametersDlg::OnButtonHelp()                 { handleExprHelpButtonClick(IDC_BUTTON_HELP);  }

void CIsoSurfaceParametersDlg::paramToWin(const ExprIsoSurfaceParameters &param) {
  m_expr             = param.m_expr.cstr();
  m_cellSize         = param.m_cellSize;
  m_lambda           = param.m_lambda;
  setXInterval(param.m_boundingBox.getXInterval());
  setYInterval(param.m_boundingBox.getYInterval());
  setZInterval(param.m_boundingBox.getZInterval());
  m_tetrahedral      = param.m_tetrahedral      ? TRUE : FALSE;
  m_tetraOptimize4   = param.m_tetraOptimize4   ? TRUE : FALSE;
  m_adaptiveCellSize = param.m_adaptiveCellSize ? TRUE : FALSE;
  m_originOutside    = param.m_originOutside    ? TRUE : FALSE;
  __super::paramToWin(param);
  enableCheckBoxOrigin();
  enableCheckBoxTetraOptimize4();
}

bool CIsoSurfaceParametersDlg::winToParam(ExprIsoSurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr             = m_expr;
  param.m_cellSize         = m_cellSize;
  param.m_lambda           = m_lambda;
  param.m_boundingBox      = Cube3D(getXInterval(),getYInterval(),getZInterval());
  param.m_tetrahedral      = m_tetrahedral      ? true : false;
  param.m_tetraOptimize4   = m_tetraOptimize4   ? true : false;
  param.m_adaptiveCellSize = m_adaptiveCellSize ? true : false;
  param.m_originOutside    = m_originOutside    ? true : false;
  return true;
}
