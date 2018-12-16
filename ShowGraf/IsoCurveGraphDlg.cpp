#include "stdafx.h"
#include "IsoCurveGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIsoCurveGraphDlg::CIsoCurveGraphDlg(IsoCurveGraphParameters &param, CWnd *pParent)
: SaveLoadExprDialog<IsoCurveGraphParameters>(IDD, pParent, param, _T("iso curve"), _T("iso"))
{
}

void CIsoCurveGraphDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
    DDX_Text(pDX, IDC_EDITEXPR    , m_expr    );
    DDX_Text(pDX, IDC_EDITCELLSIZE, m_cellSize);
    DDX_Text(pDX, IDC_EDITXFROM   , m_xFrom   );
    DDX_Text(pDX, IDC_EDITXTO     , m_xTo     );
    DDX_Text(pDX, IDC_EDITYFROM   , m_yFrom   );
    DDX_Text(pDX, IDC_EDITYTO     , m_yTo     );
}

BEGIN_MESSAGE_MAP(CIsoCurveGraphDlg, CDialog)
  ON_WM_SIZE()
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_EXPR                                      , OnGotoExpr                  )
  ON_COMMAND(   ID_GOTO_XINTERVAL                                 , OnGotoXInterval             )
  ON_COMMAND(   ID_GOTO_YINTERVAL                                 , OnGotoYInterval             )
  ON_COMMAND(   ID_GOTO_CELLSIZE                                  , OnGotoCellSize              )
  ON_BN_CLICKED(IDC_BUTTON_HELP                                   , OnButtonHelp                )
END_MESSAGE_MAP()

BOOL CIsoCurveGraphDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDITEXPR);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICEXPRLABEL  , PCT_RELATIVE_Y_CENTER                 );
  m_layoutManager.addControl(IDC_EDITEXPR         , RELATIVE_SIZE         | RESIZE_FONT   );
  m_layoutManager.addControl(IDC_STATICEQUALZERO  , PCT_RELATIVE_Y_CENTER | RELATIVE_X_POS);

  gotoEditBox(this, IDC_EDITEXPR);
  return FALSE;
}

bool CIsoCurveGraphDlg::validate() {
  if(!validateExpr(IDC_EDITEXPR)) {
    return false;
  }
  if(!validateInterval(IDC_EDITXFROM, IDC_EDITXTO)) {
    return false;
  }
  if(!validateInterval(IDC_EDITYFROM, IDC_EDITYTO)) {
    return false;
  }
  if(m_cellSize <= 0) {
    OnGotoCellSize();
    showWarning(_T("Cellsize must be > 0"));
    return false;
  }
  return true;
}

void CIsoCurveGraphDlg::OnGotoExpr() {
  gotoExpr(IDC_EDITEXPR);
}

void CIsoCurveGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CIsoCurveGraphDlg::OnGotoXInterval() {
  gotoEditBox(this,IDC_EDITXFROM);
}

void CIsoCurveGraphDlg::OnGotoYInterval() {
  gotoEditBox(this,IDC_EDITYFROM);
}

void CIsoCurveGraphDlg::OnGotoCellSize() {
  gotoEditBox(this,IDC_EDITCELLSIZE);
}

void CIsoCurveGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

void CIsoCurveGraphDlg::OnEditFindmatchingparentesis() {
  gotoMatchingParentesis();
}

void CIsoCurveGraphDlg::OnButtonHelp() {
  handleExprHelpButtonClick(IDC_BUTTON_HELP);
}

void CIsoCurveGraphDlg::paramToWin(const IsoCurveGraphParameters &param) {
  m_style    = param.getGraphStyleStr();
  getColorButton()->SetColor(param.getColor());
  m_expr     = param.m_expr.cstr();
  m_xFrom    = param.m_boundingBox.getMinX();
  m_xTo      = param.m_boundingBox.getMaxX();
  m_yFrom    = param.m_boundingBox.getMinY();
  m_yTo      = param.m_boundingBox.getMaxY();
  m_cellSize = param.m_cellSize;
  __super::paramToWin(param);
}

bool CIsoCurveGraphDlg::winToParam(IsoCurveGraphParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.m_expr        = m_expr;
  param.setGraphStyle((GraphStyle)getStyleCombo()->GetCurSel());
  param.setColor(getColorButton()->GetColor());
  param.m_boundingBox = Rectangle2D(m_xFrom, m_yFrom, m_xTo-m_xFrom, m_yTo-m_yFrom);
  param.m_cellSize    = m_cellSize;
  return true;
}
