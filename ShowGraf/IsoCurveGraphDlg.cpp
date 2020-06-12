#include "stdafx.h"
#include <FileNameSplitter.h>
#include "IsoCurveGraphDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CIsoCurveGraphDlg::CIsoCurveGraphDlg(IsoCurveGraphParameters &param, CWnd *pParent)
: SaveLoadExprDialog<IsoCurveGraphParameters>(IDD, pParent, param, _T("iso curve"), _T("iso"))
, m_createListFile(FALSE)
{
}

void CIsoCurveGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_CBString(  pDX, IDC_COMBOSTYLE         , m_style         );
  DDX_Check(     pDX, IDC_CHECKCREATELISTFILE, m_createListFile);
  DDX_Text(      pDX, IDC_EDITEXPR           , m_expr          );
  DDX_Text(      pDX, IDC_EDITCELLSIZE       , m_cellSize      );
  DDX_Text(      pDX, IDC_EDITXFROM          , m_xFrom         );
  DDX_Text(      pDX, IDC_EDITXTO            , m_xTo           );
  DDX_Text(      pDX, IDC_EDITYFROM          , m_yFrom         );
  DDX_Text(      pDX, IDC_EDITYTO            , m_yTo           );
}

BEGIN_MESSAGE_MAP(CIsoCurveGraphDlg, CDialog)
  ON_COMMAND(   ID_FILE_NEW                                       , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_BN_CLICKED(IDC_BUTTON_HELP                                   , OnButtonHelp                )
END_MESSAGE_MAP()

BOOL CIsoCurveGraphDlg::OnInitDialog() {
  __super::OnInitDialog();

  createExprHelpButton(IDC_BUTTON_HELP, IDC_EDITEXPR);
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
    gotoEditBox(this, IDC_EDITCELLSIZE);
    showWarning(_T("Cellsize must be > 0"));
    return false;
  }
  return true;
}

void CIsoCurveGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

String CIsoCurveGraphDlg::getListFileName() const {
  if(!m_createListFile) return __super::getListFileName();
  return FileNameSplitter(getData().getName()).setExtension(_T("lst")).getFullPath();
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
  m_expr     = param.getExprText().cstr();
  const Rectangle2D &bb = param.getBoundingBox();
  m_xFrom    = bb.getMinX();
  m_xTo      = bb.getMaxX();
  m_yFrom    = bb.getMinY();
  m_yTo      = bb.getMaxY();
  m_cellSize = param.getCellSize();
  __super::paramToWin(param);
}

bool CIsoCurveGraphDlg::winToParam(IsoCurveGraphParameters &param) {
  if(!__super::winToParam(param)) return false;
  param.setExprText((LPCTSTR)m_expr);
  param.setGraphStyle((GraphStyle)getStyleCombo()->GetCurSel());
  param.setColor(getColorButton()->GetColor());
  param.setBoundingBox(Rectangle2D(m_xFrom, m_yFrom, m_xTo-m_xFrom, m_yTo-m_yFrom));
  param.setCellSize(m_cellSize);
  return true;
}
