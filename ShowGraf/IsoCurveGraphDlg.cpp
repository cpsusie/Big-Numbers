#include "stdafx.h"
#include "showgraf.h"
#include "IsoCurveGraphDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIsoCurveGraphDlg::CIsoCurveGraphDlg(IsoCurveGraphParameters &param, CWnd *pParent) 
: m_param(param)
, CDialog(CIsoCurveGraphDlg::IDD, pParent)
{
  if(!m_param.hasName()) {
    m_param.setDefaultName();
  }
}

void CIsoCurveGraphDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
    DDX_Text(pDX, IDC_EDITEXPR    , m_expr    );
    DDX_Text(pDX, IDC_EDITNAME    , m_name    );
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
  ON_COMMAND(   ID_GOTO_NAME                                      , OnGotoName                  )
	ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_EXPR                                      , OnGotoExpr                  )
	ON_COMMAND(   ID_GOTO_XINTERVAL                                 , OnGotoXInterval             )
	ON_COMMAND(   ID_GOTO_YINTERVAL                                 , OnGotoYInterval             )
  ON_COMMAND(   ID_GOTO_CELLSIZE                                  , OnGotoCellSize              )
END_MESSAGE_MAP()


BOOL CIsoCurveGraphDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

BOOL CIsoCurveGraphDlg::OnInitDialog() {
  CDialog::OnInitDialog();
	
  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_ISOCURVE));
  LOGFONT lf;
  GetFont()->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_exprFont.CreateFontIndirect(&lf);
  GetDlgItem(IDC_EDITEXPR)->SetFont(&m_exprFont, FALSE);
	
  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICEXPRLABEL  , PCT_RELATIVE_Y_CENTER                 );
  m_layoutManager.addControl(IDC_EDITEXPR         , RELATIVE_SIZE         | RESIZE_FONT   );
  m_layoutManager.addControl(IDC_STATICEQUALZERO  , PCT_RELATIVE_Y_CENTER | RELATIVE_X_POS);

  m_layoutManager.scaleFont(1.5,false);

  paramToWin(m_param);
  UpdateData(FALSE);
  gotoEditBox(this, IDC_EDITEXPR);

  return FALSE;
}

void CIsoCurveGraphDlg::OnOK() {
  if(!UpdateData() || !validate()) return;
  winToParam(m_param);
  CDialog::OnOK();
}

bool CIsoCurveGraphDlg::validate() {
  if(m_name.GetLength() == 0) {
    OnGotoName();
    MessageBox(_T("Must specify name"));
    return false;
  }

  Expression expr;
  expr.compile((LPCTSTR)m_expr, true);
  if(!expr.isOk()) {
    OnGotoExpr();
    if(expr.getErrors().size() == 0) {
      MessageBox(_T("Unknown error!!!"));
    } else {
      MessageBox(expr.getErrors()[0].cstr());
    }
    return false;
  }
  if(m_xFrom >= m_xTo) {
    OnGotoXInterval();
    MessageBox(_T("x-from must be less than x-to"));
    return false;
  }
  if(m_yFrom >= m_yTo) {
    OnGotoYInterval();
    MessageBox(_T("y-from must be less than y-to"));
    return false;
  }
  if(m_cellSize <= 0) {
    OnGotoCellSize();
    MessageBox(_T("Cellsize must be > 0"));
    return false;
  }
  return true;
}

void CIsoCurveGraphDlg::OnGotoExpr() {
  GetDlgItem(IDC_EDITEXPR)->SetFocus(); 
}

void CIsoCurveGraphDlg::OnGotoName() {
  gotoEditBox(this,IDC_EDITNAME);
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

void CIsoCurveGraphDlg::OnSize(UINT nType, int cx, int cy) {
  m_layoutManager.OnSize(nType, cx, cy);
  CDialog::OnSize(nType, cx, cy);
}

void CIsoCurveGraphDlg::OnFileNew() {
  IsoCurveGraphParameters param;
  paramToWin(param);
  UpdateData(false);
}

static const TCHAR *fileDialogExtensions = _T("Iso curve-files (*.iso)\0*.iso\0All files (*.*)\0*.*\0\0");

void CIsoCurveGraphDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open expression");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    const String fileName = dlg.m_ofn.lpstrFile;

    IsoCurveGraphParameters param;
    param.load(fileName);
    paramToWin(param);
    addToRecent(dlg.m_ofn.lpstrFile);
    UpdateData(false);
  } catch(Exception e) {
    showException(e);
  }
}

void CIsoCurveGraphDlg::OnFileSave() {
  if(!UpdateData() || !validate()) return;
  
  IsoCurveGraphParameters param;
  winToParam(param);

  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getFullName(), param);
  }
}

void CIsoCurveGraphDlg::OnFileSaveAs() {
  if(!UpdateData() || !validate()) return;
  
  IsoCurveGraphParameters param;
  winToParam(param);
  saveAs(param);
}

void CIsoCurveGraphDlg::saveAs(IsoCurveGraphParameters &param) {
  CString objname = param.getFullName().cstr();
  CFileDialog dlg(FALSE,_T("*.iso"), objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save expression");
  if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CIsoCurveGraphDlg::save(const String &fileName, IsoCurveGraphParameters &param) {
  try {
    param.save(fileName);
    paramToWin(param);
    addToRecent(fileName);
    UpdateData(FALSE);
  } catch(Exception e) {
    showException(e);
  }
}

void CIsoCurveGraphDlg::addToRecent(const String &fileName) {
  AfxGetApp()->AddToRecentFileList(fileName.cstr());
}

void CIsoCurveGraphDlg::OnEditFindmatchingparentesis() {
  CEdit *e = (CEdit*)GetDlgItem(IDC_EDITEXPR);
  UpdateData();
  String expr = m_expr;
  int cursorPos,endChar;
  e->GetSel(cursorPos, endChar);
  int m = findMatchingpParanthes(expr.cstr(), cursorPos);
  if(m >= 0) {
    e->SetSel(m, m);
  }
}

void CIsoCurveGraphDlg::paramToWin(const IsoCurveGraphParameters &param) {
  m_fullName = param.getFullName();
  m_name     = param.getPartialName().cstr();
  m_style    = GraphParameters::graphStyleToString(param.m_style);
  getColorButton()->SetColor(param.m_color);
  m_expr     = param.m_expr.cstr();
  m_xFrom    = param.m_boundingBox.getMinX();
  m_xTo      = param.m_boundingBox.getMaxX();
  m_yFrom    = param.m_boundingBox.getMinY();
  m_yTo      = param.m_boundingBox.getMaxY();
  m_cellSize = param.m_cellSize;
}

void CIsoCurveGraphDlg::winToParam(IsoCurveGraphParameters &param) {
  FileNameSplitter info(m_fullName);
  if(info.getFileName() != m_name) {
    m_fullName = info.setFileName(m_name).getFullPath();
  }
  param.setName(m_fullName);
  param.m_expr        = m_expr;
  param.m_style       = (GraphStyle)getStyleCombo()->GetCurSel();
  param.m_color       = getColorButton()->GetColor();
  param.m_boundingBox = Rectangle2D(m_xFrom, m_yFrom, m_xTo-m_xFrom, m_yTo-m_yFrom);
  param.m_cellSize    = m_cellSize;
}
