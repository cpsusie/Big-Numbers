// DiffEquationGraphDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ShowGraf.h"
#define APSTUDIO_INVOKED
#include "resource.h"
#include "DiffEquationGraphDlg.h"

IMPLEMENT_DYNAMIC(CDiffEquationGraphDlg, CDialog)

CDiffEquationGraphDlg::CDiffEquationGraphDlg(DiffEquationGraphParameters &param, CWnd* pParent /*=NULL*/)
	: m_param(param)
  , CDialog(IDD_DIFFEQUATIONGRAPH_DIALOG, pParent)
{
}

CDiffEquationGraphDlg::~CDiffEquationGraphDlg() {
}

void CDiffEquationGraphDlg::OnDestroy() {
  setEquationCount(0);
  CDialog::OnDestroy();
}

BOOL CDiffEquationGraphDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  LOGFONT lf;
  GetFont()->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_exprFont.CreateFontIndirect(&lf);

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_DIFFEQUATION));
  m_layoutManager.OnInitDialog(this);

  paramToWin(m_param);
  UpdateData(false);

//  m_layoutManager.scaleFont(1.5,false);

  gotoEditBox(this, IDC_EDITNAME);
  return FALSE;
}

void CDiffEquationGraphDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITNAME, m_name);
  DDX_Text(pDX, IDC_EDITXFROM, m_xFrom);
  DDX_Text(pDX, IDC_EDITXTO, m_xTo);
  DDX_Text(pDX, IDC_EDITMAXERROR, m_maxError);
  DDX_CBString(pDX, IDC_COMBOSTYLE, m_style);
  for (size_t i = 0; i < getEquationCount(); i++) {
    getEquationEdit(i)->DoDataExchange(pDX);
  }
}

#define FIRST_EQCONTROLID (_APS_NEXT_CONTROL_VALUE+1)
#define LAST_EQCONTROLID  (FIRST_EQCONTROLID      + 10*20)

BEGIN_MESSAGE_MAP(CDiffEquationGraphDlg, CDialog)
  ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_BN_CLICKED(IDOK                                              , OnBnClickedOk               )
  ON_BN_CLICKED(IDC_BUTTONADDEQ                                   , OnBnClickedButtonaddeq      )
  ON_CONTROL_RANGE(BN_CLICKED, FIRST_EQCONTROLID, LAST_EQCONTROLID, OnBnClickedEquation         )
  ON_COMMAND(   ID_FILE_OPEN                                      , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                      , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                   , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                    , OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_GOTO_NAME                                      , OnGotoName                  )
  ON_COMMAND(   ID_GOTO_STYLE                                     , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_MAXERROR                                  , OnGotoMaxError              )
  ON_COMMAND(   ID_GOTO_XINTERVAL                                 , OnGotoXInterval             )
END_MESSAGE_MAP()

bool CDiffEquationGraphDlg::validate() {
  if(m_name.GetLength() == 0) {
    OnGotoName();
    MessageBox(_T("Must specify name"));
    return false;
  }
  if(m_xFrom >= m_xTo) {
    OnGotoXInterval();
    MessageBox(_T("x-from must be less than x-to"));
    return false;
  }
  if(m_maxError <= 0) {
    OnGotoMaxError();
    MessageBox(_T("Maximal error must be > 0"));
    return false;
  }
  if (getEquationCount() == 0) {
    OnGotoName();
    MessageBox(_T("At least 1 equation is required"));
    return false;
  }
  DiffEquationGraphParameters param;
  winToParam(param);
  if (param.getVisibleEquationSet().size() == 0) {
    gotoEquation(0);
    MessageBox(_T("At least 1 function must be visible"));
    return false;
  }
  try {
    DiffEquationSystem::validate(param.m_equationsDescription);
  } catch (Exception e) {
    MessageBox(e.what());
    return false;
  }
  return true;
}

static const TCHAR *fileDialogExtensions = _T("Differential equation-files (*.deq)\0*.deq\0All files (*.*)\0*.*\0\0");

void CDiffEquationGraphDlg::OnFileOpen() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Open differential equation");
  if((dlg.DoModal() != IDOK) || (_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }

  try {
    DiffEquationGraphParameters param;
    const String fileName = dlg.m_ofn.lpstrFile;
    param.load(fileName);
    paramToWin(param);
    addToRecent(fileName);
    UpdateData(false);
  } catch(Exception e) {
    showException(e);
  }
}

void CDiffEquationGraphDlg::OnFileSave() {
  if(!UpdateData() || !validate()) return;
  
  DiffEquationGraphParameters param;
  winToParam(param);

  if(param.hasDefaultName()) {
    saveAs(param);
  } else {
    save(param.getFullName(), param);
  }
}

void CDiffEquationGraphDlg::OnFileSaveAs() {
  if(!UpdateData() || !validate()) return;
  
  DiffEquationGraphParameters param;
  winToParam(param);
  saveAs(param);
}

void CDiffEquationGraphDlg::addToRecent(const String &fileName) {
  AfxGetApp()->AddToRecentFileList(fileName.cstr());
}

void CDiffEquationGraphDlg::OnEditFindmatchingparentesis() {
  CEquationEdit *eq = getFocusEquation();
  if(eq == NULL) return;
  if(eq->getFocusField() == EQ_EXPR_EDIT) {
    gotoMatchingParanthes(this, eq->getExprId());
  }
}

void CDiffEquationGraphDlg::paramToWin(const DiffEquationGraphParameters &param) {
  assert(param.m_equationsDescription.size() == param.m_attrArray.size());
  m_fullName = param.getFullName();
  m_name     = param.getPartialName().cstr();
  m_style    = GraphParameters::graphStyleToString(param.m_style);
  m_xFrom    = param.m_interval.getMin();
  m_xTo      = param.m_interval.getMax();
  m_maxError = param.m_eps;
  const size_t eqCount = param.getEquationCount();
  setEquationCount(eqCount);
  for (size_t i = 0; i < eqCount; i++) {
    const DiffEquationDescription &desc = param.m_equationsDescription[i];
    const EquationAttributes      &attr = param.m_attrArray[i];
    equationToWin(i, desc, attr);
  }
}

void CDiffEquationGraphDlg::winToParam(DiffEquationGraphParameters &param) {
  FileNameSplitter info(m_fullName);
  if(info.getFileName() != m_name) {
    m_fullName = info.setFileName(m_name).getFullPath();
  }
  param.setName(m_fullName);
  param.m_style = (GraphStyle)getStyleCombo()->GetCurSel();
  param.m_interval.setFrom(m_xFrom);
  param.m_interval.setTo(m_xTo);
  param.m_eps   = m_maxError;
  param.m_equationsDescription.clear();
  param.m_attrArray.clear();
  const size_t eqCount = getEquationCount();
  for (size_t i = 0; i < eqCount; i++) {
    DiffEquationDescription desc;
    EquationAttributes      attr;
    winToEquation(i, desc, attr);
    param.m_equationsDescription.add(desc);
    param.m_attrArray.add(attr);
  }
}

void CDiffEquationGraphDlg::equationToWin(size_t index, const DiffEquationDescription &desc, const EquationAttributes &attr) {
  getEquationEdit(index)->paramToWin(desc, attr);
}

void CDiffEquationGraphDlg::winToEquation(size_t index, DiffEquationDescription &desc, EquationAttributes &attr) {
  getEquationEdit(index)->winToParam(desc, attr);
}

void CDiffEquationGraphDlg::saveAs(DiffEquationGraphParameters &param) {
  CString objname = param.getFullName().cstr();
  CFileDialog dlg(FALSE,_T("*.deq"), objname);
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  dlg.m_ofn.lpstrTitle  = _T("Save Differentialequation");
  if((dlg.DoModal() != IDOK) ||(_tcslen(dlg.m_ofn.lpstrFile) == 0)) {
    return;
  }
  save(dlg.m_ofn.lpstrFile, param);
}

void CDiffEquationGraphDlg::save(const String &fileName, DiffEquationGraphParameters &param) {
  try {
    param.save(fileName);
    paramToWin(param);
    addToRecent(fileName);
    UpdateData(FALSE);
  } catch(Exception e) {
    showException(e);
  }
}

void CDiffEquationGraphDlg::setEquationCount(size_t n) {
  if (getEquationCount() > n) {
    while(getEquationCount() > n) removeEquation(getEquationCount()-1);
  } else {
    while(getEquationCount() < n) addEquation();
  }
}

void CDiffEquationGraphDlg::addEquation() {
  int newTotalEqHeight = (getEquationCount()==0) ? 40 :  getTotalEquationRect().Height() + 30;
  CEquationEdit *eq = new CEquationEdit(m_exprFont);

  const CompactIntArray oldTabOrder = getTabOrder(this);
  eq->Create(this, getEquationCount());
  m_equationControlArray.add(eq);
  distributeEquationRectangles(newTotalEqHeight);
  adjustTabOrder(oldTabOrder);
}

void CDiffEquationGraphDlg::CDiffEquationGraphDlg::removeEquation(size_t index) {
  CEquationEdit *eq = getEquationEdit(index);
  m_equationControlArray.remove(index);
  delete eq;
  if(getEquationCount() == 0) return;
  distributeEquationRectangles(getTotalEquationRect().Height());
}

// assume equationcount >= 1
void CDiffEquationGraphDlg::adjustTabOrder(const CompactIntArray &oldTabOrder) {
  CompactIntArray       tabOrder       = oldTabOrder;
  const CEquationEdit  *eq             = getLastEquationEdit();
  const CompactIntArray eqTabOrder     = eq->getTabOrderArray();
  const intptr_t        xIntervalIndex = tabOrder.getFirstIndex(IDC_EDITXFROM);
  if(xIntervalIndex < 0) return; // actually an error
  tabOrder.add(xIntervalIndex, eqTabOrder.getBuffer(), eqTabOrder.size());
  setTabOrder(this, tabOrder);
}

void CDiffEquationGraphDlg::distributeEquationRectangles(int totalEquationsHeight) {
  const int eqCount = (int)getEquationCount();
  if(eqCount > 0) {
    const CRect  cr = getClientRect(this);
    CRect        totalRect;
    totalRect.left   = cr.left;
    totalRect.right  = cr.right;
    totalRect.top    = getWindowRect(this, IDC_EDITNAME).bottom + 5;
    totalRect.bottom = totalRect.top + totalEquationsHeight;

    for (size_t i = 0; i < eqCount; i++) removeEquationFromLOManager(i);

    int eqiTop = totalRect.top;
    for(size_t i = 0; i < eqCount; i++) {
      const int   eqh = totalRect.Height() / eqCount;
      const CRect eqr(totalRect.left, eqiTop, totalRect.right, eqiTop+eqh-3);
      getEquationEdit(i)->setWindowRect(eqr);
      eqiTop += eqh;
    }
  }
  adjustWindowSize();
  for (size_t i = 0; i < eqCount; i++) addEquationToLOManager(i);
}

void CDiffEquationGraphDlg::addEquationToLOManager(size_t index) {
  int flags = 0;
  if(index == 0                     ) flags |= TOP_EQUATION;
  if(index == getEquationCount() - 1) flags |= BOTTOM_EQUATION;
  getEquationEdit(index)->addToLayoutManager(m_layoutManager, flags);
}

void CDiffEquationGraphDlg::removeEquationFromLOManager(size_t index) {
  getEquationEdit(index)->removeFromLayoutManager(m_layoutManager);
}

void CDiffEquationGraphDlg::adjustWindowSize() {
  int topCtrlsBottom = getEquationCount()
               ? getLastEquationEdit()->getWindowRect().bottom
               : getWindowRect(this, IDC_EDITNAME).bottom;
  topCtrlsBottom += 10;

  CRect cr     = getClientRect(this);
  const CSize okSize = getWindowSize(this, IDOK);

  const int wantedCrHeight = topCtrlsBottom + okSize.cy + 20;
  cr.bottom = wantedCrHeight;
  setClientRectSize(this, cr.Size());
}

CRect CDiffEquationGraphDlg::getTotalEquationRect() const {
  if (getEquationCount() == 0) {
    return CRect(0,0,0,0);
  } else {
    CRect rfirst      = getEquationEdit(0)->getWindowRect();
    const CRect rlast = getLastEquationEdit()->getWindowRect();
    rfirst.UnionRect(rfirst, rlast);
    return  rfirst;
  }
}

CComboBox *CDiffEquationGraphDlg::getStyleCombo() {
  return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
}

int CDiffEquationGraphDlg::findEquationIndexByCtrlId(UINT id) {
  const size_t eqCount = getEquationCount();
  for (size_t i = 0; i < eqCount; i++) {
    if (getEquationEdit(i)->containsCtrlId(id)) {
      return (int)i;
    }
  }
  return -1;
}

CEquationEdit *CDiffEquationGraphDlg::findEquationByCtrlId(UINT id) {
  const int index = findEquationIndexByCtrlId(id);
  return (index < 0) ? NULL : getEquationEdit(index);
}

void CDiffEquationGraphDlg::gotoEquation(size_t index) {
  if(index >= getEquationCount()) return;
  gotoEditBox(this, getEquationEdit(index)->getNameId());
}

BOOL CDiffEquationGraphDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return CDialog::PreTranslateMessage(pMsg);
}

// CDiffEquationGraphDlg message handlers

void CDiffEquationGraphDlg::OnBnClickedOk() {
  if(!UpdateData() || !validate()) return;
  winToParam(m_param);
  CDialog::OnOK();
}

void CDiffEquationGraphDlg::OnBnClickedButtonaddeq() {
  addEquation();
}

void CDiffEquationGraphDlg::OnBnClickedEquation(UINT id) {
  const int eqIndex = findEquationIndexByCtrlId(id);
  if(eqIndex < 0) return;
  CEquationEdit *eq = getEquationEdit(eqIndex);

  switch(eq->findFieldByCtrlId(id)) {
  case EQ_DELETE_BUTTON:
    removeEquation(eqIndex);
    break;
  case EQ_VISIBLE_BUTTON:
    break;
  }
}

void CDiffEquationGraphDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CDiffEquationGraphDlg::OnGotoName() {
  gotoEditBox(this, IDC_EDITNAME);
}

void CDiffEquationGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CDiffEquationGraphDlg::OnGotoXInterval() {
  gotoEditBox(this, IDC_EDITXFROM);
}

void CDiffEquationGraphDlg::OnGotoMaxError() {
  gotoEditBox(this, IDC_EDITMAXERROR);
}

// ---------------------------------- CEquationEdit --------------------------------------------

#define EQEXPRFIELDID(i) (((i)*(int)m_subWndArray.size()) + _APS_NEXT_CONTROL_VALUE + 1)

CBitmap CEquationEdit:: s_deleteBitmap;

CEquationEdit::CEquationEdit(CFont &font) : m_font(font) {
  m_subWndArray.add(&m_editName    );
  m_subWndArray.add(&m_label       );
  m_subWndArray.add(&m_editStartV  );
  m_subWndArray.add(&m_checkVisible);
  m_subWndArray.add(&m_colorButton );
  m_subWndArray.add(&m_buttonDelete);
  m_subWndArray.add(this           );
}

CompactArray<CRect> CEquationEdit::calculateSubWinRect(const CRect &r) const {
  const int nameLeft     = r.left;
  const int nameWidth    = 40;
  const int nameRight    = nameLeft + nameWidth;
  const int nameHeight   = 22;
  const int nameTop      = r.CenterPoint().y - nameHeight/2;

  const int lblLeft      = nameRight + 2;
  const int lblWidth     = 30;
  const int lblRight     = lblLeft  + lblWidth;
  const int lblTop       = nameTop;
  const int lblHeight    = nameHeight;

  const int delWidth     = 28;
  const int delHeight    = 23;
  const int delRight     = r.right - 3;
  const int delLeft      = delRight - delWidth;
  const int delTop       = r.CenterPoint().y - delHeight/2;

  const int colorWidth   = 40;
  const int colorRight   = delLeft  - 10;
  const int colorLeft    = colorRight - colorWidth;
  const int colorHeight  = nameHeight;
  const int colorTop     = r.CenterPoint().y - colorHeight/2;

  const int visWidth     = 90;
  const int visRight     = colorLeft  - 10;
  const int visLeft      = visRight - visWidth;
  const int visHeight    = nameHeight;
  const int visTop       = r.CenterPoint().y - visHeight/2;

  const int startVRight  = visLeft - 10;
  const int startVWidth  = 90;
  const int startVLeft   = startVRight - startVWidth;
  const int startVHeight = nameHeight;
  const int startVTop    = r.CenterPoint().y - startVHeight/2;

  const int exprLeft     = lblLeft  + lblWidth + 10;
  const int exprRight    = max(exprLeft+10, startVLeft-10);
  const int exprTop      = r.top;
  const int exprHeight   = r.Height();

  const CRect nameRect(  nameLeft  , nameTop  , nameRight    , nameTop   + nameHeight  );
  const CRect lblRect(   lblLeft   , lblTop   , lblRight     , lblTop    + lblHeight   );
  const CRect startVRect(startVLeft, startVTop, startVRight  , startVTop + startVHeight);
  const CRect visRect (  visLeft   , visTop   , visRight     , visTop    + visHeight   );
  const CRect colorRect (colorLeft , colorTop , colorRight   , colorTop  + visHeight   );
  const CRect delBtnRect(delLeft   , delTop   , delRight     , delTop    + delHeight   );
  const CRect exprRect(  exprLeft  , exprTop  , exprRight    , exprTop   + exprHeight  );

  CompactArray<CRect> result(m_subWndArray.size());
  result.add(nameRect  );
  result.add(lblRect   );
  result.add(startVRect);
  result.add(visRect   );
  result.add(colorRect );
  result.add(delBtnRect);
  result.add(exprRect  );
  return result;
}

void CEquationEdit::Create(CWnd *parent, int eqIndex) {
  const CRect dummyRect(10,10,30,30);

#define STD_STYLES WS_VISIBLE | WS_TABSTOP | WS_CHILD

  m_exprId = EQEXPRFIELDID(eqIndex);
  m_editName.Create(               STD_STYLES | ES_RIGHT    | WS_BORDER      , dummyRect, parent, getNameId()   );
  m_editName.SetFont(&m_font, FALSE);

  m_label.Create(  _T("' = ")    , WS_VISIBLE | SS_CENTERIMAGE               , dummyRect, parent, getLabelId()  );
  m_label.SetFont(&m_font, FALSE);

#define EXPR_STYLE STD_STYLES | WS_VSCROLL | WS_BORDER | ES_AUTOVSCROLL | ES_MULTILINE | ES_WANTRETURN

  CEdit::Create(                        EXPR_STYLE                                , dummyRect, parent, getExprId()   );
  ModifyStyleEx(0, WS_EX_CLIENTEDGE);
  SetFont(&m_font, FALSE);

  m_editStartV.Create(                  STD_STYLES | ES_RIGHT        | WS_BORDER  , dummyRect, parent, getStartVId() );
  m_checkVisible.Create(_T("Visible:"), STD_STYLES | BS_AUTOCHECKBOX | BS_LEFTTEXT, dummyRect, parent, getVisibleId());
  m_colorButton.Create(_T("Color")    , STD_STYLES                                , dummyRect, parent, getColorId()  );
  m_buttonDelete.Create(_T("Del")     , STD_STYLES                                , dummyRect, parent, getDeleteId() );
//  m_deleteButton.SetBitmap(getDeleteBitmap());
}

#define FOREACHSUBWIN(i) for(Iterator<CWnd*> i = m_subWndArray.getIterator(); i.hasNext();) 

void CEquationEdit::DoDataExchange(CDataExchange *pDX) {
  DDX_Text(    pDX, getNameId()   , m_name      );
  DDX_Text(    pDX, getExprId()   , m_expr      );
  DDX_Text(    pDX, getStartVId() , m_startValue);
  DDX_Check(   pDX, getVisibleId(), m_visible   );
}

void CEquationEdit::paramToWin(const DiffEquationDescription &desc, const EquationAttributes &attr) {
  m_name         = desc.m_name.cstr();
  m_expr         = desc.m_expr.cstr();
  m_startValue   = attr.m_startValue;
  m_visible      = attr.m_visible;
  m_colorButton.SetColor(attr.m_color);
}

void CEquationEdit::winToParam(DiffEquationDescription &desc, EquationAttributes &attr) {
  desc.m_name       = (LPCTSTR)m_name;
  desc.m_expr       = (LPCTSTR)m_expr;
  attr.m_startValue = m_startValue;
  attr.m_visible    = m_visible ? true : false;
  attr.m_color      = m_colorButton.GetColor();
}

CRect CEquationEdit::getWindowRect() {
  CRect r = ::getWindowRect(this);
  FOREACHSUBWIN(it) r.UnionRect(r, ::getWindowRect(it.next()));
  return r;
}

void CEquationEdit::setWindowRect(const CRect &r) {
  const CompactArray<CRect> ra = calculateSubWinRect(r);
  for(size_t i = 0; i < m_subWndArray.size(); i++) {
    ::setWindowRect(m_subWndArray[i], ra[i]);
  }
}

DiffEquationField CEquationEdit::findFieldByCtrlId(UINT id) const {
  if(!containsCtrlId(id)) return (DiffEquationField)-1;
  switch (id - getExprId()) {
  case 0 : return EQ_EXPR_EDIT;
  case 1 : return EQ_NAME_EDIT;
  case 2 : return EQ_LABEL_STATIC;
  case 3 : return EQ_STARTV_EDIT;
  case 4 : return EQ_VISIBLE_BUTTON;
  case 5 : return EQ_COLOR_BUTTON;
  case 6 : return EQ_DELETE_BUTTON;
  default: return (DiffEquationField)-1;
  }
}

CompactIntArray CEquationEdit::getTabOrderArray() const {
  CompactIntArray a;
  a.add(getNameId());
  a.add(getExprId());
  a.add(getStartVId());
  a.add(getVisibleId());
  a.add(getColorId());
  a.add(getDeleteId());
  return a;
}

int CEquationEdit::getExprId() const {
  return m_exprId;
}

int CEquationEdit::getNameId() const {
  return m_exprId + 1;
}

int CEquationEdit::getLabelId() const {
  return m_exprId + 2;
}

int CEquationEdit::getStartVId() const {
  return m_exprId + 3;
}

int CEquationEdit::getVisibleId() const {
  return m_exprId + 4;
}

int CEquationEdit::getColorId() const {
  return m_exprId + 5;
}

int CEquationEdit::getDeleteId() const {
  return m_exprId + 6;
}

HBITMAP CEquationEdit::getDeleteBitmap() {
  if (!s_deleteBitmap.m_hObject == 0) {
    s_deleteBitmap.LoadBitmap(IDB_BITMAPDELETE);
  }
  return s_deleteBitmap;
}

CEquationEdit::~CEquationEdit() {
  FOREACHSUBWIN(it) it.next()->DestroyWindow();
}

void CEquationEdit::addToLayoutManager(SimpleLayoutManager &layoutManager, int flags) {
  layoutManager.addControl(getNameId()   , PCT_RELATIVE_RIGHT | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getLabelId()  , PCT_RELATIVE_X_POS | PCT_RELATIVE_Y_POS);
  int exprAttr = PCT_RELATIVE_LEFT | RELATIVE_RIGHT;
  if(!(flags & TOP_EQUATION)) exprAttr |= PCT_RELATIVE_TOP;
  if(flags & BOTTOM_EQUATION) {
    exprAttr |= RELATIVE_BOTTOM;
  } else {
    exprAttr |= PCT_RELATIVE_BOTTOM;
  }
  layoutManager.addControl(getExprId()   , exprAttr);
  layoutManager.addControl(getStartVId() , RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getVisibleId(), RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getColorId()  , RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
  layoutManager.addControl(getDeleteId() , RELATIVE_X_POS     | PCT_RELATIVE_Y_POS);
}

void CEquationEdit::removeFromLayoutManager(SimpleLayoutManager &layoutManager) {
  for(int i = 0; i < m_subWndArray.size(); i++) {
    layoutManager.removeControl(m_exprId + i);
  }
}
