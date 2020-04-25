#include "stdafx.h"
#include <FileNameSplitter.h>
#include "IsoSurfaceDlg.h"

#ifdef _DEBUG
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

CIsoSurfaceDlg::CIsoSurfaceDlg(const IsoSurfaceParameters &param, CWnd *pParent /*=NULL*/)
: SaveLoadExprDialog<IsoSurfaceParameters>(IDD, pParent, param, _T("implicit surface"), _T("imp"))
, m_createListFile(FALSE)
{
  m_debugPolygonizer = false;
}

void CIsoSurfaceDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_EXPR, m_expr);
  DDX_Text(pDX, IDC_EDIT_CELLSIZE, m_cellSize);
  DDX_Text(pDX, IDC_EDIT_LAMBDA, m_lambda);
  DDX_Check(pDX, IDC_CHECK_TETRAHEDRAL, m_tetrahedral);
  DDX_Check(pDX, IDC_CHECK_TETRAOPTIMIZE4, m_tetraOptimize4);
  DDX_Check(pDX, IDC_CHECK_ADAPTIVECELLSIZE, m_adaptiveCellSize);
  DDX_Check(pDX, IDC_CHECK_ORIGINOUTSIDE, m_originOutside);
  DDX_Check(pDX, IDC_CHECK_MACHINECODE, m_machineCode);
  DDX_Check(pDX, IDC_CHECKCREATELISTFILE, m_createListFile);
  DDX_Check(pDX, IDC_CHECK_DEBUGPOLYGONIZER, m_debugPolygonizer);
  DDX_Check(pDX, IDC_CHECK_DOUBLESIDED, m_doubleSided);
  DDX_Check(pDX, IDC_CHECK_INCLUDETIME, m_includeTime);
  DDX_Text(pDX, IDC_EDIT_FRAMECOUNT, m_frameCount);
  DDV_MinMaxUInt(pDX, m_frameCount, 1, 300);
  DDX_Text(pDX, IDC_EDIT_TIMEFROM, m_timefrom);
  DDX_Text(pDX, IDC_EDIT_TIMETO, m_timeto);
  DDX_Text(pDX, IDC_EDIT_XFROM, m_xfrom);
  DDX_Text(pDX, IDC_EDIT_XTO, m_xto);
  DDX_Text(pDX, IDC_EDIT_YFROM, m_yfrom);
  DDX_Text(pDX, IDC_EDIT_YTO, m_yto);
  DDX_Text(pDX, IDC_EDIT_ZFROM, m_zfrom);
  DDX_Text(pDX, IDC_EDIT_ZTO, m_zto);
}

BEGIN_MESSAGE_MAP(CIsoSurfaceDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(ID_FILE_OPEN                  , OnFileOpen                  )
  ON_COMMAND(ID_FILE_SAVE                  , OnFileSave                  )
  ON_COMMAND(ID_FILE_SAVE_AS               , OnFileSaveAs                )
  ON_COMMAND(ID_EDIT_FINDMATCHINGPARENTESIS, OnEditFindMatchingParentesis)
  ON_COMMAND(ID_GOTO_EXPR                  , OnGotoExpr                  )
  ON_COMMAND(ID_GOTO_CELLSIZE              , OnGotoCellSize              )
  ON_COMMAND(ID_GOTO_XINTERVAL             , OnGotoXInterval             )
  ON_COMMAND(ID_GOTO_YINTERVAL             , OnGotoYInterval             )
  ON_COMMAND(ID_GOTO_ZINTERVAL             , OnGotoZInterval             )
  ON_COMMAND(ID_GOTO_TIMEINTERVAL          , OnGotoTimeInterval          )
  ON_COMMAND(ID_GOTO_FRAMECOUNT            , OnGotoFrameCount            )
  ON_BN_CLICKED(IDC_BUTTON_HELP            , OnButtonHelp                )
  ON_BN_CLICKED(IDC_CHECK_DOUBLESIDED      , OnCheckDoubleSided          )
  ON_BN_CLICKED(IDC_CHECK_INCLUDETIME      , OnCheckIncludeTime          )
  ON_BN_CLICKED(IDC_CHECK_TETRAHEDRAL      , OnCheckTetrahedral          )
  ON_BN_CLICKED(IDC_CHECK_MACHINECODE      , OnCheckMachineCode          )
END_MESSAGE_MAP()

BOOL CIsoSurfaceDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDIT_EXPR);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDIT_EXPR             , RELATIVE_SIZE          );
  m_layoutManager.addControl(IDC_STATIC_FUNCTION       , PCT_RELATIVE_Y_CENTER  );
  m_layoutManager.addControl(IDC_STATIC_EQUAL_ZERO     , PCT_RELATIVE_Y_CENTER | RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_STATIC_CELLSIZE       , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_CELLSIZE         , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_LAMBDA         , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_LAMBDA           , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_TETRAHEDRAL     , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_TETRAOPTIMIZE4  , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_ADAPTIVECELLSIZE, RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED     , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_ORIGINOUTSIDE   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_MACHINECODE     , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECKCREATELISTFILE   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_DEBUGPOLYGONIZER, RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_INCLUDETIME     , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_CHECK_DOUBLESIDED     , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_BOUNDINGBOX    , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_XINTERVAL      , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_XFROM            , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH1          , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_XTO              , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_YINTERVAL      , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_YFROM            , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH2          , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_YTO              , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_ZINTERVAL      , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_ZFROM            , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH3          , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_ZTO              , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_TIMEINTERVAL   , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_TIMEFROM         , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_DASH4          , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_TIMETO           , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_STATIC_FRAMECOUNT     , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDC_EDIT_FRAMECOUNT       , RELATIVE_Y_POS         );
  m_layoutManager.addControl(IDOK                      , RELATIVE_POSITION      );
  m_layoutManager.addControl(IDCANCEL                  , RELATIVE_POSITION      );

  enableTimeFields();

  gotoEditBox(this, IDC_EDIT_EXPR);
  return FALSE;
}

#define MAXFRAMECOUNT 300

String CIsoSurfaceDlg::getListFileName() const {
  if(!m_createListFile) return __super::getListFileName();
  return FileNameSplitter(getData().getName()).setExtension(_T("lst")).getFullPath();
}

bool CIsoSurfaceDlg::validate() {
  if(!validateAllExpr()) {
    return false;
  }
  if(m_cellSize <= 0) {
    OnGotoCellSize();
    showWarning(_T("Size must be > 0"));
    return false;
  }
  if((m_lambda < 0) || (m_lambda > 0.45)) {
    OnGotoLambda();
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
  if(m_includeTime) {
    if(!validateMinMax(IDC_EDIT_FRAMECOUNT, 1, MAXFRAMECOUNT)) {
      return false;
    }
    if(!validateInterval(IDC_EDIT_TIMEFROM, IDC_EDIT_TIMETO)) {
      return false;
    }
  }
  return true;
}

void CIsoSurfaceDlg::OnCheckMachineCode() {
  GetDlgItem(IDC_CHECKCREATELISTFILE)->EnableWindow(IsDlgButtonChecked(IDC_CHECK_MACHINECODE));
}

void CIsoSurfaceDlg::OnCheckIncludeTime() {
  UpdateData(TRUE);
  enableTimeFields();
}

void CIsoSurfaceDlg::enableTimeFields() {
  const BOOL enable = IsDlgButtonChecked(IDC_CHECK_INCLUDETIME);
  GetDlgItem(IDC_STATIC_TIMEINTERVAL)->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMEFROM      )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_TIMETO        )->EnableWindow(enable);
  GetDlgItem(IDC_EDIT_FRAMECOUNT    )->EnableWindow(enable);
  setWindowText(this, IDC_STATIC_FUNCTION, enable ? _T("&S(t,x,y,z) =") : _T("&S(x,y,z) ="));
#ifndef ISODEBUGGER
  const bool enableDebugPolygonizer = false;
#else
  const bool enableDebugPolygonizer = !enable;
#endif // ISODEBUGGER

  GetDlgItem(IDC_CHECK_DEBUGPOLYGONIZER)->EnableWindow(enableDebugPolygonizer);
  if(!enableDebugPolygonizer) {
    m_debugPolygonizer = false;
    CheckDlgButton(IDC_CHECK_DEBUGPOLYGONIZER, BST_UNCHECKED);
  }
}

void CIsoSurfaceDlg::enableCheckBoxOrigin() {
  GetDlgItem(IDC_CHECK_ORIGINOUTSIDE)->EnableWindow(!IsDlgButtonChecked(IDC_CHECK_DOUBLESIDED));
}
void CIsoSurfaceDlg::enableCheckBoxTetraOptimize4() {
  GetDlgItem(IDC_CHECK_TETRAOPTIMIZE4)->EnableWindow(IsDlgButtonChecked(IDC_CHECK_TETRAHEDRAL));
}

void CIsoSurfaceDlg::OnCheckDoubleSided()           { enableCheckBoxOrigin();                      }
void CIsoSurfaceDlg::OnCheckTetrahedral()           { enableCheckBoxTetraOptimize4();              }
void CIsoSurfaceDlg::OnEditFindMatchingParentesis() { gotoMatchingParentesis();                    }
void CIsoSurfaceDlg::OnGotoExpr()                   { gotoExpr(IDC_EDIT_EXPR);                     }
void CIsoSurfaceDlg::OnGotoCellSize()               { gotoEditBox(this, IDC_EDIT_CELLSIZE);        }
void CIsoSurfaceDlg::OnGotoLambda()                 { gotoEditBox(this, IDC_EDIT_LAMBDA  );        }
void CIsoSurfaceDlg::OnGotoXInterval()              { gotoEditBox(this, IDC_EDIT_XFROM   );           }
void CIsoSurfaceDlg::OnGotoYInterval()              { gotoEditBox(this, IDC_EDIT_YFROM   );           }
void CIsoSurfaceDlg::OnGotoZInterval()              { gotoEditBox(this, IDC_EDIT_ZFROM   );           }
void CIsoSurfaceDlg::OnGotoTimeInterval()           { gotoEditBox(this, IDC_EDIT_TIMEFROM);        }
void CIsoSurfaceDlg::OnGotoFrameCount()             { gotoEditBox(this, IDC_EDIT_FRAMECOUNT);      }
void CIsoSurfaceDlg::OnButtonHelp()                 { handleExprHelpButtonClick(IDC_BUTTON_HELP);  }

void CIsoSurfaceDlg::paramToWin(const IsoSurfaceParameters &param) {
  m_expr             = param.m_expr.cstr();
  m_cellSize         = param.m_cellSize;
  m_lambda           = param.m_lambda;
  setXInterval(param.m_boundingBox.getXInterval());
  setYInterval(param.m_boundingBox.getYInterval());
  setZInterval(param.m_boundingBox.getZInterval());
  m_tetrahedral      = param.m_tetrahedral      ? TRUE : FALSE;
  m_tetraOptimize4   = param.m_tetraOptimize4   ? TRUE : FALSE;
  m_adaptiveCellSize = param.m_adaptiveCellSize ? TRUE : FALSE;
  m_doubleSided      = param.m_doubleSided      ? TRUE : FALSE;
  m_originOutside    = param.m_originOutside    ? TRUE : FALSE;
  m_machineCode      = param.m_machineCode      ? TRUE : FALSE;
  m_includeTime      = param.m_includeTime      ? TRUE : FALSE;
  m_frameCount       = param.m_frameCount;
  setTimeInterval(param.getTimeInterval());
  __super::paramToWin(param);
  enableCheckBoxOrigin();
  enableTimeFields();
  enableCheckBoxTetraOptimize4();
}

bool CIsoSurfaceDlg::winToParam(IsoSurfaceParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr             = m_expr;
  param.m_cellSize         = m_cellSize;
  param.m_lambda           = m_lambda;
  param.m_boundingBox      = Cube3D(getXInterval(),getYInterval(),getZInterval());
  param.m_tetrahedral      = m_tetrahedral      ? true : false;
  param.m_tetraOptimize4   = m_tetraOptimize4   ? true : false;
  param.m_adaptiveCellSize = m_adaptiveCellSize ? true : false;
  param.m_doubleSided      = m_doubleSided      ? true : false;
  param.m_originOutside    = m_originOutside    ? true : false;
  param.m_machineCode      = m_machineCode      ? true : false;
  param.m_includeTime      = m_includeTime      ? true : false;
  param.m_frameCount       = m_frameCount;
  param.m_timeInterval     = getTimeInterval();
  return true;
}
