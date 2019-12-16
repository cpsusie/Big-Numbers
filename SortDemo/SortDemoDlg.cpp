#include "stdafx.h"
#include "ParameterDlg.h"
#include "SortDemoDlg.h"
#include "AboutBoxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define foreachPanel(p)      if(m_panels.size()) for(SortPanelWnd **_##p = &m_panels[0], *p = *_##p; _##p <= &m_panels.last(); p = *(++_##p))
#define foreachConstPanel(p) if(m_panels.size()) for(const SortPanelWnd * const *_##p = &m_panels[0], *p = *_##p; _##p <= &m_panels.last(); p = *(++_##p))

CSortDemoDlg::CSortDemoDlg(CWnd *pParent /*=NULL*/) : CDialog(CSortDemoDlg::IDD, pParent) {
  m_hIcon            = theApp.LoadIcon(IDR_MAINFRAME);
  m_ctrlId           = WM_USER;
  m_movingPanelIndex = -1;
  m_selectedPanel    = NULL;
}

void CSortDemoDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSortDemoDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_RBUTTONDOWN()
    ON_COMMAND(ID_FILE_INITIALIZE         , OnFileInitialize            )
    ON_COMMAND(ID_FILE_GO                 , OnFileGo                    )
    ON_COMMAND(ID_FILE_PAUSE              , OnFilePause                 )
    ON_COMMAND(ID_FILE_RESUME             , OnFileResume                )
    ON_COMMAND(ID_FILE_EXIT               , OnFileExit                  )
    ON_COMMAND(ID_SELECT_ALLMETHODS       , OnSelectAllMethods          )
    ON_COMMAND(ID_OPTIONS_PARAMETERS      , OnOptionsParameters         )
    ON_COMMAND(ID_OPTIONS_ANIMATE         , OnOptionsAnimate            )
    ON_COMMAND(ID_OPTIONS_SPEED_SLOW      , OnOptionsSpeedSlow          )
    ON_COMMAND(ID_OPTIONS_SPEED_FAST      , OnOptionsSpeedFast          )
    ON_COMMAND(ID_CONTEXTMENU_ANALYZE     , OnContextAnalyze            )
    ON_COMMAND(ID_CONTEXTMENU_REMOVE      , OnContextmenuRemove         )
    ON_MESSAGE(ID_MSG_RECEIVESTATESHIFT   , OnReceiveStateShift         )
    ON_MESSAGE(ID_MSG_ENABLEGOMENUITEMS   , OnEnableGoMenuItems         )
    ON_COMMAND_RANGE(ID_SORT_FIRSTMETHOD  , ID_SORT_LASTMETHOD , OnToggleSortMethod)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
  ON_COMMAND(IDOK, &CSortDemoDlg::OnIdok)
END_MESSAGE_MAP()

void CSortDemoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX)   {
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

HCURSOR CSortDemoDlg::OnQueryDragIcon() {
  return (HCURSOR) m_hIcon;
}

void CSortDemoDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  adjustLayout();
  Invalidate(false);
}

void CSortDemoDlg::OnClose() {
  OnFileExit();
}
void CSortDemoDlg::OnIdok() {
}
void CSortDemoDlg::OnCancel() {
}

BOOL CSortDemoDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

BOOL CSortDemoDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);       // Set big icon
  SetIcon(m_hIcon, FALSE);      // Set small icon

  m_accelTable      = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

  buildSortMethodMenu();

  m_parameters.load();

  m_origMenuTextGo    = getMenuItemText(this, ID_FILE_GO   );
  m_origMenuTextPause = getMenuItemText(this, ID_FILE_PAUSE);
  m_fast              = isMenuItemChecked(this,ID_OPTIONS_SPEED_FAST);

//  toggleSortMethod(ID_SORT_MULTITHREADQUICKSORT, false);
  toggleSortMethod(ID_SORT_QUICKSORTTEMPLATES , false);
/*
  toggleSortMethod(ID_SORT_QUICKSORT4         , false);
  toggleSortMethod(ID_SORT_QUICKSORTPIVOT5    , false);
  toggleSortMethod(ID_SORT_QUICKSORT5_OPTIMAL , false);
  toggleSortMethod(ID_SORT_QUICKSORT_EX       , false);
*/

//  makeMinSizeTimePlot();

  setThreadDescription("Main Windoww");
  return TRUE;
}

void CSortDemoDlg::buildSortMethodMenu() {
  HMENU methodMenu = findMenuContainingId(*GetMenu(), ID_SORT_FIRSTMETHOD);
  removeAllMenuItems(methodMenu); // remove dummy item
  const int n = SortMethodId::getMethodCount();
  int i;
  for(i = 0; i < n; i++) {
    const SortMethodId &m = SortMethodId::getMethodByIndex(i);
    insertMenuItem(methodMenu, i, m.getMenuText(),  m.getId());
  }
  insertMenuSeparator(methodMenu, i++);
  insertMenuItem(methodMenu, i++, _T("Select all\tCtrl+A"), ID_SELECT_ALLMETHODS);
}

void CSortDemoDlg::checkActiveMethodsItems() {
  for(int id = ID_SORT_FIRSTMETHOD; id <= ID_SORT_LASTMETHOD; id++) {
    checkMenuItem(this, id, false);
  }
  foreachConstPanel(p) {
    checkMenuItem(this, p->getMethodId().getId(), true);
  }
}

void CSortDemoDlg::OnFileInitialize() {
  m_parameters.nextRandomSeed();
  foreachPanel(p) p->initArray();
  Invalidate();
}

int CSortDemoDlg::getPanelCountByState(SortJobState state) const {
  int count = 0;
  foreachConstPanel(p) if(p->getJobState() == state) count++;
  return count;
}

bool CSortDemoDlg::allPanelsInState(SortJobState state) const {
  foreachConstPanel(p) if(p->getJobState() != state) return false;
  return true;
}

bool CSortDemoDlg::noPanelsInState(SortJobState state) const {
  foreachConstPanel(p) if(p->getJobState() == state) return false;
  return true;
}

void CSortDemoDlg::OnFileGo() {
  if(allPanelsInState(STATE_IDLE)) {
    OnFileInitialize();
    foreachPanel(p) p->doSort();
  } else {
    foreachPanel(p) {
      switch(p->getJobState()) {
      case STATE_PAUSED:
        p->stopSort(TERMINATE_SORT);
        p->resumeSort();
        break;
      case STATE_RUNNING:
        p->stopSort(TERMINATE_SORT);
        break;
      case STATE_IDLE:
        break;
      }
    }
  }
}

void CSortDemoDlg::OnFilePause() {
  foreachPanel(p) {
    switch(p->getJobState()) {
    case STATE_RUNNING:
      p->stopSort(PAUSE_SORT);
      break;
    default:
      break;
    }
  }
}

void CSortDemoDlg::OnFileResume() {
  foreachPanel(p) {
    switch(p->getJobState()) {
    case STATE_PAUSED:
      p->resumeSort();
      break;
    default:
      break;
    }
  }
}

void CSortDemoDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CSortDemoDlg::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  SortPanelWnd *panel = (SortPanelWnd*)source;
  const SortJobState oldState = *(SortJobState*)oldValue, newState = *(SortJobState*)newValue;
  switch(id) {
  case JOBSTATE:
    PostMessage(ID_MSG_RECEIVESTATESHIFT, (WPARAM)panel, STATESHIFT(oldState, newState));
    break;
  }
}

LRESULT CSortDemoDlg::OnReceiveStateShift(WPARAM wp, LPARAM lp) {
  SortPanelWnd         *panel    = (SortPanelWnd*)wp;
//  const SortJobState oldState = GETOLDSTATE(lp);
  const SortJobState newState = GETNEWSTATE(lp);
/*
  debugLog(_T("stateShift %-5s -> %-5s : method:%s\n")
          ,SortPanelWnd::getStateString(oldState)
          ,SortPanelWnd::getStateString(newState)
          ,panel->getMethodId().getName().cstr()
          );
*/
  switch(newState) {
  case STATE_RUNNING:
    setMenuItemText(this, ID_FILE_GO, _T("&Stop\tF5")           );
    if(menuItemExists(this, ID_FILE_RESUME)) {
      insertMenuItem(this, ID_FILE_RESUME, 0, m_origMenuTextPause, ID_FILE_PAUSE);
      removeMenuItem(this, ID_FILE_RESUME);
    }
    enableMenuItem( this, ID_FILE_PAUSE, true               );
    enableMenuItemsOnSortGo(false);
    break;

  case STATE_IDLE  :
  case STATE_KILLED:
    if(allPanelsInState(STATE_IDLE)) {
      setMenuItemText(this, ID_FILE_GO   , m_origMenuTextGo   );
      setMenuItemText(this, ID_FILE_PAUSE, m_origMenuTextPause);
      enableMenuItem( this, ID_FILE_PAUSE, false              );
      enableMenuItemsOnSortGo(true);
    }
    break;

  case STATE_PAUSED:
    if(noPanelsInState(STATE_RUNNING)) {
      insertMenuItem(this, ID_FILE_PAUSE, 0, _T("&Resume\tCtrl+R"),ID_FILE_RESUME);
      removeMenuItem(this, ID_FILE_PAUSE);
    }
    break;
  case STATE_ERROR :
    { OnFilePause();
      errorMessage(_T("Sort method %s has finished with an unsorted array"), panel->getMethodId().getName().cstr());
    }
    break;
  }
  return 0;
}

void CSortDemoDlg::enableMenuItemsOnSortGo(bool enabled) {
  PostMessage(ID_MSG_ENABLEGOMENUITEMS, enabled?1:0,0);
}

LRESULT CSortDemoDlg::OnEnableGoMenuItems(WPARAM wp, LPARAM lp) {
  bool enabled = wp ? true : false;
  enableMenuItem(this,ID_FILE_INITIALIZE   , enabled);
//  enableMenuItem(this,ID_FILE_ANALYZE      , enabled);
  enableMenuItem(this,_T("1")                  , enabled); // Sorting algorithm
  enableMenuItem(this,ID_OPTIONS_PARAMETERS, enabled);
  enableMenuItem(this,ID_OPTIONS_ANIMATE   , enabled);
  return 0;
}

void CSortDemoDlg::OnToggleSortMethod(UINT id) {
  toggleSortMethod(id, true);
}

void CSortDemoDlg::toggleSortMethod(int methodId, bool redraw) {
  bool on = toggleMenuItem(this, methodId);
  if(on) {
    if(addSortPanel(methodId)) {
      if(redraw) {
        Invalidate();
      }
    }
  } else {
    if(deleteSortPanel(methodId)) {
      if(redraw) {
        Invalidate();
      }
    }
  }
}

void CSortDemoDlg::OnSelectAllMethods() {
  const int n = SortMethodId::getMethodCount();
  bool changed = false;
  for(int i = 0; i < n; i++) {
    const SortMethodId &m = SortMethodId::getMethodByIndex(i);
    if(!isMenuItemChecked(this, m.getId())) {
      checkMenuItem(this, m.getId(), true);
      addSortPanel(m.getId());
      changed = true;
    }
  }
  if(changed) {
    Invalidate();
  }
}

bool CSortDemoDlg::addSortPanel(int methodId) {
  const int index = findPanelIndexByMethod(methodId);
  if(index >= 0) {
    return false;
  }

  try {
    SortPanelWnd *panel = new SortPanelWnd(this, methodId); TRACE_NEW(panel);
    m_panels.add(panel);
    adjustLayout();
    return true;
  } catch(Exception e) {
    showException(e);
    return false;
  }
}

bool CSortDemoDlg::deleteSortPanel(int methodId) {
  const int index = findPanelIndexByMethod(methodId);
  if(index < 0) {
    return false;
  }
  SortPanelWnd *panel = m_panels[index];
  panel->DestroyWindow();
  SAFEDELETE(panel);
  m_panels.remove(index);
  adjustLayout();
  return true;
}

int CSortDemoDlg::findPanelIndexByMethod(int methodId) const {
  for(size_t i = 0; i < m_panels.size(); i++) {
    if(m_panels[i]->getMethodId().getId() == methodId) {
      return (int)i;
    }
  }
  return -1;
}

void CSortDemoDlg::adjustLayout() {
  const int n = (int)m_panels.size();
  if(n == 0) {
    return;
  }
  const CRect cr = getClientRect(this);
  const int totalWidth  = cr.Width();
  const int totalHeight = cr.Height();
  const int colCount    = (n-1)/5 + 1;

  CompactIntArray panelsInCol(colCount);
  for(int c = 0; c < colCount; c++) {
    panelsInCol.add(n / colCount);
  }
  for(int rest = (int)(n - (panelsInCol.size()*panelsInCol[0])); rest > 0; rest--) {
    panelsInCol[rest]++;
  }
  int panelCountCheck = 0;
  for(size_t c = 0; c < panelsInCol.size(); c++) {
    panelCountCheck += panelsInCol[c];
  }
  if(panelCountCheck != n) {
    errorMessage(_T("panelCountcheck = %d != n (=%d)"), panelCountCheck, n);
  }
  if(panelsInCol.size() != colCount) {
    errorMessage(_T("panelsInCol.size=%d != colcount (=%d)"), panelsInCol.size(), colCount);
  }

  const int panelWidth  = totalWidth / colCount;
  int left = 0;
  int panelIndex = 0;
  for(size_t c = 0; c < panelsInCol.size(); c++) {
    const int cn = panelsInCol[c];
    const int panelHeight = totalHeight / cn;

    int top  = 0;
    for(int p = 0; p < cn; p++) {
      m_panels[panelIndex++]->setRect(CRect(left, top, left + panelWidth, top+panelHeight));
      top += panelHeight;
    }
    left += panelWidth;
  }
}

void CSortDemoDlg::OnOptionsParameters() {
  CParameterDlg dlg(m_parameters);
  if(dlg.DoModal() == IDOK) {
    m_parameters = dlg.getParameters();
    OnFileInitialize();
  }
}

void CSortDemoDlg::OnOptionsAnimate() {
  toggleMenuItem(this, ID_OPTIONS_ANIMATE);
}

void CSortDemoDlg::OnOptionsSpeedSlow() {
  checkMenuItem(this, ID_OPTIONS_SPEED_SLOW, true );
  checkMenuItem(this, ID_OPTIONS_SPEED_FAST, false);
  m_fast = false;
}

void CSortDemoDlg::OnOptionsSpeedFast() {
  checkMenuItem(this, ID_OPTIONS_SPEED_SLOW, false);
  checkMenuItem(this, ID_OPTIONS_SPEED_FAST, true );
  m_fast = true;
}

bool CSortDemoDlg::isAnimatedSort() {
  return isMenuItemChecked(this, ID_OPTIONS_ANIMATE);
}

void CSortDemoDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  m_selectedPanel = findPanelFromPoint(point);
  if(m_selectedPanel == NULL) {
    return;
  }
  CMenu menu;
  menu.LoadMenu(MAKEINTRESOURCE(IDR_CONTEXTMENU));
  CPoint scrPoint = point;
  ClientToScreen(&scrPoint);
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, scrPoint.x,scrPoint.y, this);
  __super::OnRButtonDown(nFlags, point);
}

SortPanelWnd *CSortDemoDlg::findPanelFromPoint(const CPoint &p) {
  const int index = findPanelIndexFromPoint(p);
  return (index < 0) ? NULL  : m_panels[index];
}

int CSortDemoDlg::findPanelIndexFromPoint(const CPoint &p) {
  for(size_t i = 0; i < m_panels.size(); i++) {
    SortPanelWnd *panel = m_panels[i];
    const CRect rect = getWindowRect(panel);
    if(rect.PtInRect(p)) {
      return (int)i;
    }
  }
  return -1;
}

void CSortDemoDlg::OnContextAnalyze() {
  analyzeSortMethod(&m_selectedPanel->getMethodId(), m_parameters);
}

void CSortDemoDlg::OnContextmenuRemove() {
  const int id = m_selectedPanel->getMethodId().getId();
  if(deleteSortPanel(id)) {
    checkActiveMethodsItems();
    Invalidate();
  }
}

void CSortDemoDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  m_movingPanelIndex = findPanelIndexFromPoint(point);
  if(isMoveingPanel()) {
    startDragRect(point);
  }
  __super::OnLButtonDown(nFlags, point);
}

void CSortDemoDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  if(isMoveingPanel()) {
    const int panel1Index = m_movingPanelIndex;
    endDragRect();
    const int panel2Index = findPanelIndexFromPoint(point);
    if((panel2Index >= 0) && (panel2Index != panel1Index)) {
      SortPanelWnd *panel1 = m_panels[panel1Index];
      SortPanelWnd *panel2 = m_panels[panel2Index];
      const CRect   r1     = getWindowRect(panel1);
      const CRect   r2     = getWindowRect(panel2);
      setWindowRect(panel1, r2);
      setWindowRect(panel2, r1);
      m_panels.swap(panel1Index, panel2Index);
    }
  }
  __super::OnLButtonUp(nFlags, point);
}

void CSortDemoDlg::OnMouseMove(UINT nFlags, CPoint point) {
  if(isMoveingPanel()) {
    if(nFlags && MK_LBUTTON) {
      dragRect(point);
    } else {
      endDragRect();
    }
  }
  __super::OnMouseMove(nFlags, point);
}

void CSortDemoDlg::startDragRect(CPoint &p) {
  m_mouseDownPoint = p;
  m_dragRect = getWindowRect(m_panels[m_movingPanelIndex]);
  CClientDC dc(this);
  const CSize sz(2,2);
  dc.DrawDragRect(&m_dragRect, sz, NULL,sz);
}

void CSortDemoDlg::dragRect(CPoint &p) {
  CSize dp = p - m_mouseDownPoint;
  const CRect newDragRect = getWindowRect(m_panels[m_movingPanelIndex]) + dp;
  CClientDC dc(this);
  const CSize sz(2,2);
  dc.DrawDragRect(newDragRect, sz, &m_dragRect, sz);
  m_dragRect = newDragRect;
}

void CSortDemoDlg::endDragRect() {
  CClientDC dc(this);
  const CSize sz(2,2);
  dc.DrawDragRect(m_dragRect, sz, NULL, sz);
  m_movingPanelIndex = -1;
}
