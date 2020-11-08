#include "stdafx.h"
#include <FileNameSplitter.h>
#include "ShowGraf.h"
#include "DiffEquationGraphDlg.h"

IMPLEMENT_DYNAMIC(CDiffEquationGraphDlg, CDialog)

CDiffEquationGraphDlg::CDiffEquationGraphDlg(DiffEquationGraphParameters &param, CWnd *pParent)
  : m_param(param)
  , CDialog(IDD, pParent)
  , m_currentAdjustSet(32)
  , m_createListFile(FALSE)
{
}

CDiffEquationGraphDlg::~CDiffEquationGraphDlg() {
}

void CDiffEquationGraphDlg::OnDestroy() {
  setEquationCount(0);
  __super::OnDestroy();
}

BOOL CDiffEquationGraphDlg::OnInitDialog() {
  __super::OnInitDialog();
  LOGFONT lf;
  GetFont()->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("courier new"));
  BOOL ret = m_exprFont.CreateFontIndirect(&lf);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_DIFFEQUATION));
  m_layoutManager.OnInitDialog(this);

#if !defined(_DEBUG)
  CStatic *adjustSet = (CStatic*)GetDlgItem(IDC_STATICADJUSTSET);
  adjustSet->ShowWindow(SW_HIDE);
  CRect clRect = getClientRect(this);
  CRect erRect = getWindowRect(this, IDC_LISTERRORS);
  erRect.bottom = clRect.bottom;
  setWindowRect(this, IDC_LISTERRORS, erRect);
#endif
  resetLowerPanelHeight();
  m_layoutManager.addControl(IDC_EDITCOMMON    , PCT_RELATIVE_BOTTOM | RELATIVE_WIDTH );
  m_layoutManager.addControl(IDC_STATICINTERVAL, PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT);
  m_layoutManager.addControl(IDC_EDITXFROM     , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT);
  m_layoutManager.addControl(IDC_STATICDASH    , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT);
  m_layoutManager.addControl(IDC_EDITXTO       , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT);
  m_layoutManager.addControl(IDC_STATICEPS     , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT);
  m_layoutManager.addControl(IDC_EDITMAXERROR  , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT);
  m_layoutManager.addControl(IDC_BUTTONADDEQ   , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT | RELATIVE_X_POS);
  m_layoutManager.addControl(IDOK              , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT | RELATIVE_X_POS);
  m_layoutManager.addControl(IDCANCEL          , PCT_RELATIVE_BOTTOM | CONSTANT_HEIGHT | RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_LISTERRORS    , PCT_RELATIVE_TOP    | RELATIVE_BOTTOM | RELATIVE_WIDTH);
#if defined(_DEBUG)
  m_layoutManager.addControl(IDC_STATICADJUSTSET , RELATIVE_TOP | CONSTANT_HEIGHT | RELATIVE_WIDTH);
#endif

  paramToWin(m_param);
  clearErrorList();
  UpdateData(false);
  ajourCommonEnabled();

//  m_layoutManager.scaleFont(1.5,false);

  gotoEditBox(this, IDC_EDITNAME);
  return FALSE;
}

void CDiffEquationGraphDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(    pDX, IDC_EDITNAME           , m_name          );
  DDX_CBString(pDX, IDC_COMBOSTYLE         , m_style         );
  DDX_Check(   pDX, IDC_CHECKCREATELISTFILE, m_createListFile);
  DDX_Text(    pDX, IDC_EDITCOMMON         , m_commonText    );
  DDX_Text(    pDX, IDC_EDITXFROM          , m_xFrom         );
  DDX_Text(    pDX, IDC_EDITXTO            , m_xTo           );
  DDX_Text(    pDX, IDC_EDITMAXERROR       , m_maxError      );
  m_equationControlArray.DoDataExchange(pDX);
}

String CDiffEquationGraphDlg::getListFileName(const DiffEquationGraphParameters &param) const {
  if(!m_createListFile) return EMPTYSTRING;
  return FileNameSplitter(param.getName()).setExtension(_T("lst")).getFullPath();
}


BEGIN_MESSAGE_MAP(CDiffEquationGraphDlg, CDialog)
  ON_WM_SIZE()
  ON_WM_DESTROY()
  ON_BN_CLICKED(IDC_BUTTONADDEQ                                                       , OnBnClickedButtonaddeq      )
  ON_CONTROL_RANGE(BN_CLICKED  , FIRST_DIFFEUQUATIONFIELDID, LAST_DIFFEUQUATIONFIELDID, OnBnClickedEquation         )
  ON_CONTROL_RANGE(EN_CHANGE   , FIRST_DIFFEUQUATIONFIELDID, LAST_DIFFEUQUATIONFIELDID, OnEditChangeEquation        )
  ON_CONTROL_RANGE(EN_SETFOCUS , FIRST_DIFFEUQUATIONFIELDID, LAST_DIFFEUQUATIONFIELDID, OnEditSetFocusEquation      )
  ON_CONTROL_RANGE(EN_KILLFOCUS, FIRST_DIFFEUQUATIONFIELDID, LAST_DIFFEUQUATIONFIELDID, OnEditKillFocusEquation     )
  ON_EN_CHANGE(   IDC_EDITCOMMON                                    , OnEditChangeCommon          )
  ON_EN_SETFOCUS( IDC_EDITCOMMON                                    , OnEditSetFocusCommon        )
  ON_EN_KILLFOCUS(IDC_EDITCOMMON                                    , OnEditKillFocusCommon       )
  ON_COMMAND(   ID_FILE_NEW                                         , OnFileNew                   )
  ON_COMMAND(   ID_FILE_OPEN                                        , OnFileOpen                  )
  ON_COMMAND(   ID_FILE_SAVE                                        , OnFileSave                  )
  ON_COMMAND(   ID_FILE_SAVE_AS                                     , OnFileSaveAs                )
  ON_COMMAND(   ID_EDIT_FINDMATCHINGPARENTESIS                      , OnEditFindmatchingparentesis)
  ON_COMMAND(   ID_EDIT_NEXTERROR                                   , OnEditNexterror             )
  ON_COMMAND(   ID_EDIT_PREVERROR                                   , OnEditPreverror             )
  ON_COMMAND(   ID_GOTO_NAME                                        , OnGotoName                  )
  ON_COMMAND(   ID_GOTO_STYLE                                       , OnGotoStyle                 )
  ON_COMMAND(   ID_GOTO_COMMON                                      , OnGotoCommon                )
  ON_COMMAND(   ID_GOTO_MAXERROR                                    , OnGotoMaxError              )
  ON_COMMAND(   ID_GOTO_XINTERVAL                                   , OnGotoXInterval             )
  ON_LBN_SELCHANGE(IDC_LISTERRORS                                   , OnLbnSelchangeListerrors    )
END_MESSAGE_MAP()

bool CDiffEquationGraphDlg::validate() {
  if(m_name.GetLength() == 0) {
    OnGotoName();
    showWarning(_T("Must specify name"));
    return false;
  }
  if(m_xFrom >= m_xTo) {
    OnGotoXInterval();
    showWarning(_T("x-from must be less than x-to"));
    return false;
  }
  if(m_maxError <= 0) {
    OnGotoMaxError();
    showWarning(_T("Maximal error must be > 0"));
    return false;
  }
  if(getEquationCount() == 0) {
    OnGotoName();
    showWarning(_T("At least 1 equation is required"));
    return false;
  }
  DiffEquationGraphParameters param;
  winToParam(param);
  if(param.getVisibleEquationSet().size() == 0) {
    gotoEquation(0);
    showWarning(_T("At least 1 function must be visible"));
    return false;
  }
  bool  ok       = true;
  FILE *listFile = nullptr;
  try {
    const String listFileName = getListFileName(param);
    if(listFileName.length() > 0) {
      listFile = MKFOPEN(listFileName, _T("w"));
    }
    CompilerErrorList errorList;
    if(!DiffEquationSystem::validate(param.getEquationDescriptionArray(), errorList, listFile)) {
      showErrors(errorList);
      ok = false;
    } else {
      clearErrorList();
    }
  } catch(Exception e) {
    showException(e);
    ok = false;
  }
  if(listFile != nullptr) {
    fclose(listFile);
  }
  return ok;
}

void CDiffEquationGraphDlg::showErrors(const CompilerErrorList &errorList) {
  CListBox *listBox = getErrorList();
  listBox->ResetContent();
  m_errorPosArray.clear();
  for (size_t i = 0; i < errorList.size(); i++) {
    listBox->AddString(errorList[i].cstr());
    m_errorPosArray.add(ErrorPosition(errorList[i]));
  }
  setSelectedError(0);
  enableMenuItem(this, ID_EDIT_NEXTERROR, true);
  enableMenuItem(this, ID_EDIT_PREVERROR, true);
}

void CDiffEquationGraphDlg::clearErrorList() {
  CListBox *listBox = getErrorList();
  listBox->ResetContent();
  m_errorPosArray.clear();
  clearCurrentAdjustSet();
  enableMenuItem(this, ID_EDIT_NEXTERROR, false);
  enableMenuItem(this, ID_EDIT_PREVERROR, false);
}

void CDiffEquationGraphDlg::setSelectedError(int index) {
  if (index >= 0 && index < getErrorCount()) {
    getErrorList()->SetCurSel(index);
    gotoErrorPosition(index);
  }
}

void CDiffEquationGraphDlg::gotoTextPosition(int ctrlId, const SourcePosition &pos) {
  gotoEditBox(this, ctrlId);
  const int selstart = pos.findCharIndex(getWindowText(this, ctrlId));
  ((CEdit*)GetDlgItem(ctrlId))->SetSel(selstart, selstart+1);
}

void CDiffEquationGraphDlg::gotoErrorPosition(int index) {
  const ErrorPosition &pos = m_errorPosArray[index];
  CDiffEquationEdit   *eq  = nullptr;
  switch(pos.getLocation()) {
  case ERROR_INNAME   :
    if(eq = getEquationEdit(pos.getEquationIndex())) {
      gotoEditBox(this, eq->getNameId());
    }
    break;
  case ERROR_INEXPR   :
    if(eq = getEquationEdit(pos.getEquationIndex())) {
      gotoTextPosition(eq->getExprId(), pos.getSourcePosition());
    }
    break;
  case ERROR_INCOMMON :
    gotoTextPosition(IDC_EDITCOMMON, pos.getSourcePosition());
    break;
  }
}

void CDiffEquationGraphDlg::OnFileNew() {
  DiffEquationGraphParameters param;
  paramToWin(param);
  clearErrorList();
  UpdateData(false);
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
    clearErrorList();
    addToRecent(fileName);
    UpdateData(false);
//    Invalidate();
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
    save(param.getName(), param);
  }
}

void CDiffEquationGraphDlg::OnFileSaveAs() {
  if(!UpdateData() || !validate()) return;

  DiffEquationGraphParameters param;
  winToParam(param);
  saveAs(param);
}

void CDiffEquationGraphDlg::addToRecent(const String &fileName) {
  theApp.AddToRecentFileList(fileName.cstr());
}

void CDiffEquationGraphDlg::OnEditFindmatchingparentesis() {
  CDiffEquationEdit *eq = getFocusEquation();
  if(eq == nullptr) return;
  if(eq->getFocusField() == EQ_EXPR_EDIT) {
    gotoMatchingParanthes(this, eq->getExprId());
  }
}

void CDiffEquationGraphDlg::OnEditNexterror() {
  setSelectedError(getSelectedError()+1);
}

void CDiffEquationGraphDlg::OnEditPreverror() {
  setSelectedError(getSelectedError()-1);
}

void CDiffEquationGraphDlg::paramToWin(const DiffEquationGraphParameters &param) {
  assert(param.getEquationDescriptionArray().size() == param.getAttributeArray().size());
  m_fullName   = param.getName();
  m_name       = param.getDisplayName().cstr();
  m_style      = param.getGraphStyleStr();
  m_commonText = param.getEquationDescriptionArray().getCommonText().cstr();
  const DoubleInterval &interval = param.getInterval();
  m_xFrom      = interval.getMin();
  m_xTo        = interval.getMax();
  m_maxError   = param.getMaxError();
  const size_t eqCount = param.getEquationCount();
  setEquationCount(eqCount);
  m_equationControlArray.paramToWin(param);
}

void CDiffEquationGraphDlg::winToParam(DiffEquationGraphParameters &param) {
  FileNameSplitter info(m_fullName);
  if(info.getFileName() != m_name) {
    m_fullName = info.setFileName(m_name).getFullPath();
  }
  param.setName(m_fullName);
  param.setGraphStyle((GraphStyle)getStyleCombo()->GetCurSel());
  param.getEquationDescriptionArray().setCommonText((LPCTSTR)m_commonText);
  param.setInteval(m_xFrom, m_xTo);
  param.setMaxError(m_maxError);
  m_equationControlArray.winToParam(param);
}

void CDiffEquationGraphDlg::saveAs(DiffEquationGraphParameters &param) {
  CString objname = param.getName().cstr();
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
  if(getEquationCount() > n) {
    while(getEquationCount() > n) {
      removeEquation(getEquationCount()-1);
    }
  } else {
    while(getEquationCount() < n) {
      addEquation();
    }
  }
}

#define MINEQHEIGHT     30
#define MAXEQHEIGHT     100
#define UPPERLOWERSPACE 10

void CDiffEquationGraphDlg::addEquation() {
  const int eqCount = getEquationCount();
  int       newTotalEqHeight;

  if(eqCount == 0) {
    newTotalEqHeight = MINEQHEIGHT;
  } else {
    newTotalEqHeight = getSpaceBetweenTopAndBottomPanel(eqCount+1) - UPPERLOWERSPACE;
    if(newTotalEqHeight / (eqCount + 1) < MINEQHEIGHT) {
      newTotalEqHeight = MINEQHEIGHT * (eqCount + 1);
    }
  }
  TabOrder tabs(this);
  m_equationControlArray.addEquation(this, m_exprFont);
  adjustPanels(newTotalEqHeight);
  adjustTabOrder(tabs);
  ajourCommonEnabled();
}

void CDiffEquationGraphDlg::CDiffEquationGraphDlg::removeEquation(size_t index) {
  const int eqCount = getEquationCount();
  int       newTotalEqHeight;
  if(eqCount <= 1) {
    newTotalEqHeight = 0;
  } else {
    newTotalEqHeight = getSpaceBetweenTopAndBottomPanel(eqCount-1) - UPPERLOWERSPACE;
    if(newTotalEqHeight / (eqCount - 1) > MAXEQHEIGHT) {
      newTotalEqHeight = MAXEQHEIGHT * (eqCount - 1);
    }
  }

  m_equationControlArray.remove(index);
  adjustPanels(newTotalEqHeight);
  ajourCommonEnabled();
}

void CDiffEquationGraphDlg::adjustPanels(int totalEquationHeight) {
  adjustTopPanel();

  const UINT eqCount  = getEquationCount();
  if(eqCount == 0) {
    m_equationHeight   = 0;
    m_upperPanelBottom = getTopPanelBottom();
  } else {
    m_equationHeight = totalEquationHeight / eqCount;
    const CRect  cr  = getClientRect(this);
    CRect        totalRect;
    totalRect.left     = cr.left;
    totalRect.right    = cr.right;
    totalRect.top      = getTopPanelBottom() + 5;
    totalRect.bottom   = totalRect.top + getSumEquationHeight();

    removeEquationsFromLOManager();

    int eqTop = totalRect.top;
    for(size_t i = 0; i < eqCount; i++) {
      const CRect eqr(totalRect.left, eqTop+2, totalRect.right, eqTop+m_equationHeight);
      getEquationEdit(i)->setWindowRect(eqr);
      eqTop += m_equationHeight;
    }
    m_upperPanelBottom = eqTop;
  }
  adjustWindowSize();
  addEquationsToLOManager();
}

void CDiffEquationGraphDlg::adjustWindowSize() {
  CRect cr = getClientRect(this);
  cr.bottom = m_upperPanelBottom + UPPERLOWERSPACE + getLowerPanelHeight();
  setClientRectSize(this, cr.Size());
  adjustLowerPanel();
}

void CDiffEquationGraphDlg::adjustTopPanel() {
  CRect      r = getWindowRect(this, IDC_EDITCOMMON);
  r.bottom = findTopPanelBottom(getEquationCount());
  setWindowRect(this, IDC_EDITCOMMON, r);
}

const int CDiffEquationGraphDlg::s_bottomIdArray[] = {
  IDC_STATICINTERVAL
 ,IDC_EDITXFROM
 ,IDC_STATICDASH
 ,IDC_EDITXTO
 ,IDC_STATICEPS
 ,IDC_EDITMAXERROR
 ,IDC_BUTTONADDEQ
 ,IDOK
 ,IDCANCEL
};

int CDiffEquationGraphDlg::getLowerPanelTop() {
  return getUnionRect(s_bottomIdArray, ARRAYSIZE(s_bottomIdArray)).top;
}

void CDiffEquationGraphDlg::adjustLowerPanel() {
  const int h        = getLowerPanelHeight();
  CRect     cr       = getClientRect(this);
  int       th       = getMaxHeight(s_bottomIdArray, ARRAYSIZE(s_bottomIdArray));
  int       y        = cr.bottom - h;
  for (int i = 0; i < ARRAYSIZE(s_bottomIdArray); i++) {
    int ctrlId = s_bottomIdArray[i];
    CPoint p = getWindowPosition(this, ctrlId);
    p.y = y;
    setWindowPosition(this, ctrlId, p);
  }

  CRect errRect  = getWindowRect(this, IDC_LISTERRORS);
  errRect.top    = y + th + 5;
  errRect.bottom = cr.bottom;
  CStatic *asWnd = (CStatic*)GetDlgItem(IDC_STATICADJUSTSET);
  if(asWnd->IsWindowVisible()) {
    CRect asRect = getWindowRect(asWnd);
    const int ash = asRect.Height();
    asRect.bottom = cr.bottom;
    asRect.top    = asRect.bottom - ash;
    setWindowRect(asWnd, asRect);
    errRect.bottom = asRect.top + 1;
  }
  setWindowRect(this, IDC_LISTERRORS, errRect);
}

void CDiffEquationGraphDlg::resetLowerPanelHeight() {
  m_lowerPanelHeight = getWindowRect(this, IDC_LISTERRORS).bottom - getWindowRect(this, IDOK).top;
  CWnd *asWnd = GetDlgItem(IDC_STATICADJUSTSET);
  if(asWnd->IsWindowVisible()) {
    m_lowerPanelHeight += getWindowRect(asWnd).Height();
  }
}

// assume equationcount >= 1
void CDiffEquationGraphDlg::adjustTabOrder(const TabOrder &tabs) {
  TabOrder tabOrder = tabs;
  if(tabOrder.getFirstIndex(IDC_EDITCOMMON) < 0) {
    tabOrder.insert(tabOrder.getFirstIndex(IDC_COMBOSTYLE)+1, IDC_EDITCOMMON);
  }

  const CDiffEquationEdit *eq             = getLastEquationEdit();
  const CompactUIntArray    eqTabOrder     = eq->getTabOrderArray();
  const intptr_t           xIntervalIndex = tabOrder.getFirstIndex(IDC_EDITXFROM);
  if(xIntervalIndex < 0) return; // actually an error
  tabOrder.insert(xIntervalIndex, eqTabOrder.getBuffer(), eqTabOrder.size());
  tabOrder.restoreTabOrder();
}

void CDiffEquationGraphDlg::ajourCommonEnabled() {
  GetDlgItem(IDC_EDITCOMMON)->EnableWindow(getEquationCount() > 0);
}

CRect CDiffEquationGraphDlg::getUnionRect(const int *ids, int n) {
  CRect result = getWindowRect(this, *(ids++)); n--;
  while(n--) {
    CRect r = getWindowRect(this, *(ids++));
    result.UnionRect(result, r);
  }
  return result;
}

int CDiffEquationGraphDlg::getMaxHeight(const int *ids, int n) {
  return getUnionRect(ids,n).Height();
}

CComboBox *CDiffEquationGraphDlg::getStyleCombo() {
  return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
}

void CDiffEquationGraphDlg::gotoEquation(size_t index) {
  if(index >= getEquationCount()) return;
  gotoEditBox(this, getEquationEdit(index)->getNameId());
}

// CDiffEquationGraphDlg message handlers

BOOL CDiffEquationGraphDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
/*
  switch (pMsg->message) {
  case WM_TIMER:
  case WM_MOUSEMOVE:
  case 0x118:
    break;
  default:
    debugLog(_T("%s\n"), getMessageName(pMsg->message).cstr());
    break;
  }
*/
  return __super::PreTranslateMessage(pMsg);
}

void CDiffEquationGraphDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  if (m_layoutManager.isInitialized()) {
    Invalidate(FALSE);
  }
}

void CDiffEquationGraphDlg::OnOK() {
  if(!UpdateData() || !validate()) return;
  winToParam(m_param);
  __super::OnOK();
}

void CDiffEquationGraphDlg::OnBnClickedButtonaddeq() {
  addEquation();
  Invalidate();
}

void CDiffEquationGraphDlg::OnBnClickedEquation(UINT id) {
  const int eqIndex = findEquationIndexByCtrlId(id);
  if(eqIndex < 0) return;
  CDiffEquationEdit *eq = getEquationEdit(eqIndex);

  switch(eq->findFieldByCtrlId(id)) {
  case EQ_DELETE_BUTTON:
    removeEquation(eqIndex);
    Invalidate();
    break;
  case EQ_VISIBLE_BUTTON:
    eq->setVisibleChecked(eq->getVisibleChecked());
    break;
  }
}

void CDiffEquationGraphDlg::adjustErrorPositions(const String &s, int sel, int delta) {
  const SourcePosition sp(m_currentText, sel-delta);
  for(Iterator<size_t> it = m_currentAdjustSet.getIterator(); it.hasNext();) {
    ErrorPosition        &ep  = m_errorPosArray[it.next()];
    const SourcePosition &esp = ep.getSourcePosition();
    if(esp >= sp) {
      ep.setSourcePosition(SourcePosition(s, esp.findCharIndex(m_currentText)+delta));
    }
  }
}

void CDiffEquationGraphDlg::traceCurrentAdjustSet() {
#if defined(_DEBUG)
  String str;
  for(Iterator<size_t> it = m_currentAdjustSet.getIterator(); it.hasNext();) {
    const ErrorPosition &ep = m_errorPosArray[it.next()];
    str += ep.getSourcePosition().toString();
  }
  setWindowText(this, IDC_STATICADJUSTSET, str);
#endif
}

void CDiffEquationGraphDlg::setCurrentAdjustSet(UINT id) {
  clearCurrentAdjustSet();
  const int eqIndex = findEquationIndexByCtrlId(id);
  if(eqIndex < 0) {
    if(id != IDC_EDITCOMMON) return;
    for(UINT i = 0; i < m_errorPosArray.size(); i++) {
      const ErrorPosition &ep = m_errorPosArray[i];
      if(ep.getLocation() == ERROR_INCOMMON) {
        m_currentAdjustSet.add(i);
      }
    }
  } else {
    CDiffEquationEdit *eq       = getEquationEdit(eqIndex);
    ErrorLocation      location = ERROR_INNAME;
    switch(eq->findFieldByCtrlId(id)) {
    case EQ_EXPR_EDIT:
      location = ERROR_INEXPR;
      // NB continue case
    case EQ_NAME_EDIT:
      for(UINT i = 0; i < m_errorPosArray.size(); i++) {
        const ErrorPosition &ep = m_errorPosArray[i];
        if((ep.getEquationIndex() == eqIndex) && (ep.getLocation() == location)) {
          m_currentAdjustSet.add(i);
        }
      }
    }
  }
  traceCurrentAdjustSet();
}

void CDiffEquationGraphDlg::OnEditSetFocusEquation(UINT id) {
  setCurrentAdjustSet(id);
  if(isCurrentAdjustSetEmpty()) return; // no need to update the positions
  m_currentText = getWindowText(this, id);
}

void CDiffEquationGraphDlg::OnEditKillFocusEquation(UINT id) {
  clearCurrentAdjustSet();
}

void CDiffEquationGraphDlg::OnEditChangeEquation(UINT id) {
  if(isCurrentAdjustSetEmpty()) return; // no need to update the positions
  const String text = getWindowText(this, id);
  const int delta = (int)text.length() - (int)m_currentText.length();
  if(delta != 0) {
    int selStart, selEnd;
    ((CEdit*)GetDlgItem(id))->GetSel(selStart, selEnd);
    adjustErrorPositions(text, selStart, delta);
    traceCurrentAdjustSet();
  }
  m_currentText = text;
}

void CDiffEquationGraphDlg::OnEditSetFocusCommon() {
  OnEditSetFocusEquation(IDC_EDITCOMMON);
}

void CDiffEquationGraphDlg::OnEditKillFocusCommon() {
  OnEditKillFocusEquation(IDC_EDITCOMMON);
}

void CDiffEquationGraphDlg::OnEditChangeCommon() {
  OnEditChangeEquation(IDC_EDITCOMMON);
}

void CDiffEquationGraphDlg::OnLbnSelchangeListerrors() {
  setSelectedError(getSelectedError());
}

void CDiffEquationGraphDlg::OnGotoName() {
  gotoEditBox(this, IDC_EDITNAME);
}

void CDiffEquationGraphDlg::OnGotoStyle() {
  getStyleCombo()->SetFocus();
}

void CDiffEquationGraphDlg::OnGotoCommon() {
  GetDlgItem(IDC_EDITCOMMON)->SetFocus();
}

void CDiffEquationGraphDlg::OnGotoXInterval() {
  gotoEditBox(this, IDC_EDITXFROM);
}

void CDiffEquationGraphDlg::OnGotoMaxError() {
  gotoEditBox(this, IDC_EDITMAXERROR);
}
