#include "stdafx.h"
#include <io.h>
#include <errno.h>
#include <MFCUtil/Clipboard.h>
#include <Tokenizer.h>
#include "MainFrm.h"
#include "FindDlg.h"
#include "TabSizeDlg.h"
#include "DefineFileFormatDlg.h"
#include "ZoomDlg.h"
#include "RegexDlg.h"
#include "GotoDlg.h"
#include "EnterOptionsNameDlg.h"
#include "OptionsOrganizerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
  ON_WM_CREATE()
  ON_WM_SIZE()
  ON_WM_SHOWWINDOW()
  ON_WM_ACTIVATE()
  ON_COMMAND(ID_ACTIVATEPARTNER           , OnActivatePartner          )
  ON_COMMAND(ID_FILE_OPEN_PANEL0          , OnFileOpenPanel0           )
  ON_COMMAND(ID_FILE_OPEN_PANEL1          , OnFileOpenPanel1           )
  ON_COMMAND(ID_FILE_PRINT_PREVIEW        , OnFilePrintPreview         )
  ON_COMMAND(ID_FILE_MRU_FILE1            , OnFileMruFile1             )
  ON_COMMAND(ID_FILE_MRU_FILE2            , OnFileMruFile2             )
  ON_COMMAND(ID_FILE_MRU_FILE3            , OnFileMruFile3             )
  ON_COMMAND(ID_FILE_MRU_FILE4            , OnFileMruFile4             )
  ON_COMMAND(ID_FILE_MRU_FILE5            , OnFileMruFile5             )
  ON_COMMAND(ID_FILE_MRU_FILE6            , OnFileMruFile6             )
  ON_COMMAND(ID_FILE_MRU_FILE7            , OnFileMruFile7             )
  ON_COMMAND(ID_FILE_MRU_FILE8            , OnFileMruFile8             )
  ON_COMMAND(ID_FILE_MRU_FILE9            , OnFileMruFile9             )
  ON_COMMAND(ID_FILE_MRU_FILE10           , OnFileMruFile10            )
  ON_COMMAND(ID_FILE_MRU_FILE11           , OnFileMruFile11            )
  ON_COMMAND(ID_FILE_MRU_FILE12           , OnFileMruFile12            )
  ON_COMMAND(ID_FILE_MRU_FILE13           , OnFileMruFile13            )
  ON_COMMAND(ID_FILE_MRU_FILE14           , OnFileMruFile14            )
  ON_COMMAND(ID_FILE_MRU_FILE15           , OnFileMruFile15            )
  ON_COMMAND(ID_FILE_MRU_FILE16           , OnFileMruFile16            )
  ON_COMMAND(ID_EDIT_COPY                 , OnEditCopy                 )
  ON_COMMAND(ID_EDIT_PASTE                , OnEditPaste                )
  ON_COMMAND(ID_EDIT_SELECTALL            , OnEditSelectAll            )
  ON_COMMAND(ID_EDIT_FIND                 , OnEditFind                 )
  ON_COMMAND(ID_EDIT_FIND_NEXT            , OnEditFindNext             )
  ON_COMMAND(ID_EDIT_FIND_PREV            , OnEditFindPrev             )
  ON_COMMAND(ID_EDIT_PREV_DIFF            , OnEditPrevDiff             )
  ON_COMMAND(ID_EDIT_NEXT_DIFF            , OnEditNextDiff             )
  ON_COMMAND(ID_EDIT_SHOWDETAILS          , OnEditShowDetails          )
  ON_COMMAND(ID_EDIT_GOTO                 , OnEditGoto                 )
  ON_COMMAND(ID_EDIT_REFRESHFILES         , OnEditRefreshFiles         )
  ON_COMMAND(ID_EDIT_SWAPPANELS           , OnEditSwapPanels           )
  ON_COMMAND(ID_VIEW_SHOWWHITESPACE       , OnViewShowWhiteSpace       )
  ON_COMMAND(ID_VIEW_TABSIZE              , OnViewTabSize              )
  ON_COMMAND(ID_VIEW_FONT                 , OnViewFont                 )
  ON_COMMAND(ID_VIEW_NAMEFONTSIZE_100     , OnViewNameFontSize100      )
  ON_COMMAND(ID_VIEW_NAMEFONTSIZE_125     , OnViewNameFontSize125      )
  ON_COMMAND(ID_VIEW_NAMEFONTSIZE_150     , OnViewNameFontSize150      )
  ON_COMMAND(ID_VIEW_NAMEFONTSIZE_175     , OnViewNameFontSize175      )
  ON_COMMAND(ID_VIEW_NAMEFONTSIZE_200     , OnViewNameFontSize200      )
  ON_COMMAND(ID_VIEW_SHOW1000SEPARATOR    , OnViewShow1000separator    )
  ON_COMMAND(ID_VIEW_HIGHLIGHTCOMPAREEQUAL, OnViewHighlightCompareEqual)
  ON_COMMAND(ID_OPTIONS_IGNOREBLANKS      , OnOptionsIgnoreWhiteSpace  )
  ON_COMMAND(ID_OPTIONS_IGNORECASE        , OnOptionsIgnoreCase        )
  ON_COMMAND(ID_OPTIONS_IGNORECOMMENTS    , OnOptionsIgnoreComments    )
  ON_COMMAND(ID_OPTIONS_IGNORESTRINGS     , OnOptionsIgnoreStrings     )
  ON_COMMAND(ID_OPTIONS_STRIPCOMMENTS     , OnOptionsStripComments     )
  ON_COMMAND(ID_OPTIONS_IGNORECOLUMNS     , OnOptionsIgnoreColumns     )
  ON_COMMAND(ID_OPTIONS_DEFINECOLUMNS     , OnOptionsDefineColumns     )
  ON_COMMAND(ID_OPTIONS_IGNOREREGEX       , OnOptionsIgnoreRegex       )
  ON_COMMAND(ID_OPTIONS_DEFINEREGEX       , OnOptionsDefineRegex       )
  ON_COMMAND(ID_OPTIONS_SAVEOPTIONS       , OnOptionsSaveOptions       )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS1      , OnOptionsLoadOptions1      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS2      , OnOptionsLoadOptions2      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS3      , OnOptionsLoadOptions3      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS4      , OnOptionsLoadOptions4      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS5      , OnOptionsLoadOptions5      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS6      , OnOptionsLoadOptions6      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS7      , OnOptionsLoadOptions7      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS8      , OnOptionsLoadOptions8      )
  ON_COMMAND(ID_OPTIONS_LOADOPTIONS9      , OnOptionsLoadOptions9      )
  ON_COMMAND(ID_OPTIONS_ORGANIZEOPTIONS   , OnOptionsOrganizeOptions   )
  ON_COMMAND(ID_CHARLEFT                  , OnCharLeft                 )
  ON_COMMAND(ID_CHARRIGHT                 , OnCharRight                )
  ON_COMMAND(ID_CTRLCHARLEFT              , OnCtrlCharLeft             )
  ON_COMMAND(ID_CTRLCHARRIGHT             , OnCtrlCharRight            )
  ON_COMMAND(ID_LINEDOWN                  , OnLineDown                 )
  ON_COMMAND(ID_LINEUP                    , OnLineUp                   )
  ON_COMMAND(ID_PAGEDOWN                  , OnPageDown                 )
  ON_COMMAND(ID_PAGEUP                    , OnPageUp                   )
  ON_COMMAND(ID_HOME                      , OnHome                     )
  ON_COMMAND(ID_END                       , OnEnd                      )
  ON_COMMAND(ID_CTRLHOME                  , OnCtrlHome                 )
  ON_COMMAND(ID_CTRLEND                   , OnCtrlEnd                  )
  ON_COMMAND(ID_DUMPFLAGS                 , OnDumpFlags                )
  ON_WM_PAINT()
END_MESSAGE_MAP()


static UINT indicators[] = {
  ID_SEPARATOR
 ,ID_INDICATOR_DELETEDLINES
 ,ID_INDICATOR_CHANGEDLINES
 ,ID_INDICATOR_INSERTEDLINES
 ,ID_INDICATOR_POSITION
#ifdef _DEBUG
 ,ID_INDICATOR_DEBUG
#endif
};

#ifdef _DEBUG
void CMainFrame::showDebugMsg(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr,format);
  m_wndStatusBar.SetPaneText(5,vformat(format,argptr).cstr());
  va_end(argptr);
}
#endif

#define INDICATORCOUNT ARRAYSIZE(indicators)

CMainFrame::CMainFrame() {
  m_statusPanesVisible = true;
  m_hasFocus           = false;
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  if(!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
      | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
    TRACE0("Failed to create toolbar\n");
    return -1;      // fail to create
  }

  if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, INDICATORCOUNT)) {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }
 
  m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);

  m_wndStatusBar.Invalidate();
  m_bAutoMenuEnable = false;

  EnableDocking(CBRS_ALIGN_ANY);
  DockControlBar(&m_wndToolBar);
  return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext) {
  m_relativeWidth = 0.5;

  BOOL ret = m_wndSplitter.Create(
    this,
    1, 2,
    CSize(10, 10),
    pContext,
    WS_CHILD | WS_VISIBLE | SPLS_DYNAMIC_SPLIT | WS_VSCROLL |WS_HSCROLL
    // create WITH scrollbars. We make them invisible in function Onsize (calling SetScrollStyle(0) !!)
  );
  return ret;
}

void CWinDiffSplitterWnd::RecalcLayout() {
  CSplitterWnd::RecalcLayout();
  if(m_splitPointMoved) {
    ((CMainFrame*)GetParent())->saveRelativeWidth();
    m_splitPointMoved = false;
  }
}

void CWinDiffSplitterWnd::OnInvertTracker(const CRect& rect) {
  CSplitterWnd::OnInvertTracker(rect);
  m_splitPointMoved = true;
}

CWinDiffView *CWinDiffSplitterWnd::getDiffView(int index) {
  if((index >= 0) && (index < getPanelCount())) {
    return (CWinDiffView*)GetPane(0,index);
  } else {
    return NULL;
  }
}

void CWinDiffSplitterWnd::setActivePanel(int index) {
  if(index >= 0 && index < getPanelCount()) {
    SetActivePane(0,index);
  }
}

int CMainFrame::getPanelCount() {
  return m_wndSplitter.getPanelCount();
}

int CMainFrame::getActivePanelIndex() {
  CWinDiffView *view = getActiveDiffView();
  return view ? view->getId() : -1;
}

void CMainFrame::setActivePanel(int index) {
  if(index >= 0 && index < getPanelCount()) {
    getDiffView(index)->setActive(true);
  }
}

CWinDiffView *CMainFrame::getActiveDiffView() {
  return (CWinDiffView *)m_wndSplitter.GetActivePane();
}

TextView *CMainFrame::getActiveTextView() {
  CWinDiffView *view = getActiveDiffView();
  if(view == NULL) {
    return NULL;
  }
  return &view->m_textView;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
  if(!CFrameWnd::PreCreateWindow(cs)) {
    return FALSE;
  }
  return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
  CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
  CFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::updateLoadOptionsMenu() {
  const StringArray optionNames = Options::getExistingNames();
  const String saveOptionsMenuPath = _T("3/11");
  int index;
  HMENU optionsMenu = findMenuByString(m_hMenuDefault, saveOptionsMenuPath, index);
  HMENU loadMenu    = (optionsMenu && (index >= 0)) ? getSubMenu(optionsMenu, _T("load options")) : NULL;

  if(loadMenu == NULL) {
    if(optionNames.size() > 0) {
      loadMenu = insertSubMenu(optionsMenu, 12, _T("&Load options"));
    } else {
      return;
    }
  } else {
    removeAllMenuItems(loadMenu);
  }
  const int n = min((int)optionNames.size(), 9);
  for(int i = 0; i < n; i++) {
    insertMenuItem(loadMenu,i, format(_T("%s\tCtrl+%d"), optionNames[i].cstr(), i+1), ID_OPTIONS_LOADOPTIONS+i+1);
  }
}

void CMainFrame::saveRelativeWidth() {
  CRect r;
  GetClientRect(&r);

  if(getPanelCount() == 2) {
    int w1,cxMin;
    m_wndSplitter.GetColumnInfo(0,w1,cxMin);
    m_relativeWidth = (double)w1 / (r.right - r.left);
  }
}

void CMainFrame::setRelativeWidth(int cx) {
  if(getPanelCount() == 2) {
    m_wndSplitter.SetColumnInfo(0,(int)(m_relativeWidth * cx),10);
  }
}

// ---------------------------------------------- non menu commands ----------------------------------------

#define SC_WIDTH 18
void CMainFrame::OnSize(UINT nType, int cx, int cy) {
  CFrameWnd::OnSize(nType, cx, cy);
  if(IsIconic())  {
    return;
  }

  TextView *view1 = getActiveTextView();
  TextView *view2 = view1 ? view1->getPartner() : NULL;
  if(view1) view1->savePositionState();
  if(view2) view2->savePositionState();

  if(m_wndSplitter.IsWindowVisible()) {
    m_wndSplitter.SetScrollStyle(0);

    setRelativeWidth(cx);
    m_wndSplitter.SetScrollStyle(0);
    m_wndSplitter.RecalcLayout();
//    m_wndSplitter.GetPane(0,0)->ShowScrollBar(SB_BOTH,FALSE);
//    m_wndSplitter.GetPane(0,0)->ShowScrollBar(SB_BOTH,FALSE);
    getOptions().m_windowSize = getWindowSize(this);
  }
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) {
  CFrameWnd::OnShowWindow(bShow, nStatus);
  updateLoadOptionsMenu();
  RECT r;
  GetClientRect(&r);
  int cx = r.right;
  if(getPanelCount() == 2) {
    m_wndSplitter.SetColumnInfo(0,(int)(m_relativeWidth * cx) - SC_WIDTH,10);
  } else {
    m_wndSplitter.SplitColumn((int)(m_relativeWidth * cx));
  }
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
  CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
  CWinDiffView *view = getActiveDiffView();
  if(view) {
    switch(nState) {
    case WA_INACTIVE    : 
      m_savedActivePanelIndex = getActivePanelIndex();
      setFocus(false);
      break;
    case WA_ACTIVE      :
    case WA_CLICKACTIVE :
      view->setActive(true);
      setFocus(true);
      break;
    }
  } else {
    if(nState == WA_CLICKACTIVE) {
      view = getDiffView(m_savedActivePanelIndex);
      if(view) {
        view->setActive(true);
      }
      setFocus(true);
    }
  }
}

void CMainFrame::OnPaint() {
  CPaintDC dc(this); // device context for painting
  TextView   *view = getActiveTextView();
  if(view) {
    view->refreshBoth();
  }
}

void CMainFrame::setFocus(bool value) {
  if(value != m_hasFocus) {
    m_hasFocus = value;
  }
}


void CMainFrame::OnActivatePartner() {
  TextView *view = getActiveTextView();
  if(view) { 
    view->activatePartner();
  }
}

#define APPLY_TO_ACTIVE_TEXTVIEW(f)  { TextView *view = getActiveTextView(); if(view) { view->f; } }

void CMainFrame::OnCharLeft()      { APPLY_TO_ACTIVE_TEXTVIEW(charLeft()     ) }
void CMainFrame::OnCharRight()     { APPLY_TO_ACTIVE_TEXTVIEW(charRight()    ) }
void CMainFrame::OnCtrlCharLeft()  { APPLY_TO_ACTIVE_TEXTVIEW(ctrlCharLeft() ) }
void CMainFrame::OnCtrlCharRight() { APPLY_TO_ACTIVE_TEXTVIEW(ctrlCharRight()) }
void CMainFrame::OnLineDown()      { APPLY_TO_ACTIVE_TEXTVIEW(lineDown()     ) }
void CMainFrame::OnLineUp()        { APPLY_TO_ACTIVE_TEXTVIEW(lineUp()       ) }
void CMainFrame::OnPageDown()      { APPLY_TO_ACTIVE_TEXTVIEW(pageDown()     ) }
void CMainFrame::OnPageUp()        { APPLY_TO_ACTIVE_TEXTVIEW(pageUp()       ) }
void CMainFrame::OnHome()          { APPLY_TO_ACTIVE_TEXTVIEW(home()         ) }
void CMainFrame::OnEnd()           { APPLY_TO_ACTIVE_TEXTVIEW(end()          ) }
void CMainFrame::OnCtrlHome()      { APPLY_TO_ACTIVE_TEXTVIEW(ctrlHome()     ) }
void CMainFrame::OnCtrlEnd()       { APPLY_TO_ACTIVE_TEXTVIEW(ctrlEnd()      ) }

void CMainFrame::refreshDoc(bool recomp) {
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }

  CWinDiffDoc *doc = view->getDocument();
  if(recomp) {
    doc->recompare();
  }
  view->savePositionState();
  view->refreshBoth();
}

#ifdef _DEBUG

String CMainFrame::flagsToString() {
  CWinDiffView *v0 = getDiffView(0);
  CWinDiffView *v1 = getDiffView(1);

  String str;
  str += m_hasFocus ? _T("GOTFOCUS. ") : _T("NO FOCUS. ");
  if(v0) {
    str += format(_T("v[0]:%-55s "), v0->m_textView.flagValuesToString().cstr());
  }

  if(v1) {
    str += format(_T("v[1]:%-55s "), v1->m_textView.flagValuesToString().cstr());
  }
  return str;
}

void CMainFrame::dumpFlags() {
  debugLog(_T("%s\n"), flagsToString().cstr());
}

#define DUMPFLAGS() dumpFlags()
#else
#define DUMPFLAGS()
#endif

void CMainFrame::OnDumpFlags() {
  DUMPFLAGS();
}

void CMainFrame::enableToolbarButtonAndMenuItem(int id, bool enable) {
  CToolBarCtrl &ctrlBar = m_wndToolBar.GetToolBarCtrl();
  if(ctrlBar.m_hWnd) {
    if(enable != isToolbarButtonEnabled(id)) {
      if(enable) {
        ctrlBar.SetState(id, TBSTATE_ENABLED);
        ctrlBar.EnableButton(id, TRUE);
      } else {
        ctrlBar.SetState(id, TBSTATE_INDETERMINATE);
        ctrlBar.EnableButton(id, FALSE);
      }
    }
  }
  enableMenuItem(this, id, enable);
}

bool CMainFrame::isToolbarButtonEnabled(int id) {
  return !m_wndToolBar.GetToolBarCtrl().IsButtonIndeterminate(id);
}

void CMainFrame::showStatusBarPanes(bool show) {
  if(show != m_statusPanesVisible) {
    if(show) {
      for(size_t i = 0; i < m_paneInfo.size(); i++) {
        const StatusBarPaneInfo &info = m_paneInfo[i];
        m_wndStatusBar.SetPaneInfo(info.m_index, info.m_id, info.m_style, info.m_width);
      }
    } else {
      if(m_paneInfo.isEmpty()) {
        for(int i = 1; i < INDICATORCOUNT; i++) {
          StatusBarPaneInfo info;
          m_wndStatusBar.GetPaneInfo(i, info.m_id, info.m_style, info.m_width);
          info.m_index = i;
          m_paneInfo.add(info);
        }
      }
      for(int i = 1; i < INDICATORCOUNT; i++) {
        m_wndStatusBar.SetPaneInfo(i, indicators[i], SBPS_NOBORDERS | SBPS_DISABLED ,0);
      }
    }
    m_statusPanesVisible = show;
  }
}

void CMainFrame::ajourMenuItems() {
  TextView    *view    = getActiveTextView();
  bool         hasView = view != NULL;
  CWinDiffDoc *doc     = getDoc();
  const Diff &diff     = doc->m_diff;

  if(!hasView) {
    enableMenuItem(this, ID_EDIT_COPY                     , false);
    enableMenuItem(this, ID_EDIT_SELECTALL                , false);

    enableToolbarButtonAndMenuItem(ID_EDIT_FIND           , false);
    enableToolbarButtonAndMenuItem(ID_EDIT_FIND_NEXT      , false);
    enableToolbarButtonAndMenuItem(ID_EDIT_FIND_PREV      , false);
    enableToolbarButtonAndMenuItem(ID_EDIT_PREV_DIFF      , false);

    enableMenuItem(this, ID_EDIT_GOTO                     , false);
    enableMenuItem(this, ID_EDIT_REFRESHFILES             , false);
    enableMenuItem(this, ID_VIEW_HIGHLIGHTCOMPAREEQUAL    , false);

    enableToolbarButtonAndMenuItem(ID_EDIT_NEXT_DIFF      , false);
    enableToolbarButtonAndMenuItem(ID_EDIT_SHOWDETAILS    , false);
    showStatusBarPanes(false);

  } else {
    const bool hasText = !diff.getDoc(view->getId()).isEmpty();

    enableMenuItem(this, ID_EDIT_COPY                     , !view->getSelectedRange().isEmpty());
    enableMenuItem(this, ID_EDIT_SELECTALL                , hasText);

    enableToolbarButtonAndMenuItem(ID_EDIT_FIND           , hasText);
    enableToolbarButtonAndMenuItem(ID_EDIT_FIND_NEXT      , hasText);
    enableToolbarButtonAndMenuItem(ID_EDIT_FIND_NEXT      , hasText);
    enableToolbarButtonAndMenuItem(ID_EDIT_FIND_PREV      , hasText);
    enableToolbarButtonAndMenuItem(ID_EDIT_FIND_PREV      , hasText);

    enableMenuItem(this, ID_EDIT_GOTO                     , hasText);
    enableMenuItem(this, ID_EDIT_REFRESHFILES             , diff.hasFileDoc());
    enableMenuItem(this, ID_VIEW_HIGHLIGHTCOMPAREEQUAL    , doc->m_filter.hasLineFilter());
    if(!doc->m_filter.hasLineFilter()) {
      checkMenuItem(this, ID_VIEW_HIGHLIGHTCOMPAREEQUAL, false);
    }
    enableToolbarButtonAndMenuItem( ID_EDIT_SHOWDETAILS   , !diff.isEmpty() && !diff.getDiffLines()[view->getCurrentLine()].linesAreEqual());
    enableToolbarButtonAndMenuItem( ID_EDIT_PREV_DIFF     , !diff.isEmpty() && (view->getCurrentLine() > diff.getFirstDiffLine()));
    enableToolbarButtonAndMenuItem( ID_EDIT_NEXT_DIFF     , !diff.isEmpty() && (view->getCurrentLine() < diff.getLastDiffLine()));

    showStatusBarPanes(!diff.isEmpty());
  }
  updateNameFontSizeMenuItems(getOptions().m_nameFontSizePct);
}  

void CMainFrame::updateCheckedMenuItems() {
  const Options &options = getOptions();
  checkMenuItem(this, ID_OPTIONS_IGNOREBLANKS  , options.m_ignoreWhiteSpace );
  checkMenuItem(this, ID_OPTIONS_IGNORECASE    , options.m_ignoreCase       );
  checkMenuItem(this, ID_OPTIONS_IGNORESTRINGS , options.m_ignoreStrings    );
  checkMenuItem(this, ID_OPTIONS_IGNORECOMMENTS, options.m_ignoreComments   );
  checkMenuItem(this, ID_OPTIONS_IGNORECOLUMNS , options.m_ignoreColumns    );
  checkMenuItem(this, ID_OPTIONS_IGNOREREGEX   , options.m_ignoreRegex      );
  checkMenuItem(this, ID_OPTIONS_STRIPCOMMENTS , options.m_stripComments    );
  checkMenuItem(this, ID_VIEW_SHOWWHITESPACE   , options.m_viewWhiteSpace   );
  checkMenuItem(this, ID_VIEW_SHOW1000SEPARATOR, options.m_show1000Separator);
  updateNameFontSizeMenuItems(options.m_nameFontSizePct);
}

typedef struct {
  int m_id, m_pct;
} FontSizePctIdTable;

void CMainFrame::updateNameFontSizeMenuItems(int pct) {
  const FontSizePctIdTable menuIdTable[] = {
    ID_VIEW_NAMEFONTSIZE_100 , 100
   ,ID_VIEW_NAMEFONTSIZE_125 , 125
   ,ID_VIEW_NAMEFONTSIZE_150 , 150
   ,ID_VIEW_NAMEFONTSIZE_175 , 175
   ,ID_VIEW_NAMEFONTSIZE_200 , 200
  };
  for(int i = 0; i < ARRAYSIZE(menuIdTable); i++) {
    const FontSizePctIdTable &fs = menuIdTable[i];
    checkMenuItem(this, fs.m_id, pct == fs.m_pct);
  }
}

BYTE CMainFrame::getFocusFlags() {
  BYTE result;
  if(m_hasFocus) result |= MAINHASFOCUS;
  TextView *view = getActiveTextView();
  if(view) {
    result |= (view->getId() == 0) ? PANEL0ACTIVE : PANEL1ACTIVE;
  }
  return result;
}
  
#define MSG_CARETBLINK 0x118

#define IS_CARETBLINKMESSAGE(msg) (msg->message == MSG_CARETBLINK)

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
//  const BYTE focus0 = getFocusFlags();

  if(!IS_CARETBLINKMESSAGE(pMsg)) {
//    debugLog(_T("msg:%-18s %s\n"), getMessageName(pMsg->message).cstr()+3, flagsToString().cstr());
    CWinDiffView *view = getActiveDiffView();
    if(view) view->hideCaret();
  }

  const BOOL ret = doPreTranslateMessage(pMsg);

//  const BYTE focus1 = getFocusFlags();

  if(m_hasFocus && !IS_CARETBLINKMESSAGE(pMsg)) {
    CWinDiffView *view = getActiveDiffView();
    if(view) {
      view->showCaret();
    }
  }

  return ret;
}

BOOL CMainFrame::doPreTranslateMessage(MSG* pMsg) {
  const BOOL ret  = CFrameWnd::PreTranslateMessage(pMsg);
  TextView  *view = getActiveTextView();
  if(view == NULL) return ret;

  const TextPosition pos = view->getCurrentPos();

  m_wndStatusBar.SetPaneText(4, format(_T("Ln %d, Col %d"), pos.m_line+1, pos.m_column+1).cstr());

  const TCHAR *msgStr = NULL;
  switch(pMsg->message) {
  case WM_PAINT      : msgStr = _T("WM_PAINT")      ; break;
  case WM_NCMOUSEMOVE: msgStr = _T("WM_NCMOUSEMOVE"); break;
  case WM_SIZING     : msgStr = _T("WM_SIZING")     ; break;
  case WM_SIZE       : msgStr = _T("WM_SIZE")       ; break;
  case 118           : msgStr = _T("msg(118)")      ; break;
  }
  if(m_statusPanesVisible && msgStr) {
    CRect rect;
    m_wndStatusBar.GetItemRect(1,&rect);
    CStatusBarCtrl &statusBar =  m_wndStatusBar.GetStatusBarCtrl();
    CClientDC dc(&statusBar);

    m_wndStatusBar.SetPaneText(1,_T(""));
    textOutTransparentBackground(dc, rect.TopLeft()+CSize(2,1), _T("Deleted lines"), *statusBar.GetFont(), BLUE);

    m_wndStatusBar.GetItemRect(2,&rect);
    m_wndStatusBar.SetPaneText(2,_T(""));
    textOutTransparentBackground(dc, rect.TopLeft()+CSize(2,1), _T("Changed lines"), *statusBar.GetFont(), RED);

    m_wndStatusBar.GetItemRect(3,&rect);
    m_wndStatusBar.SetPaneText(3,_T(""));
    textOutTransparentBackground(dc, rect.TopLeft()+CSize(2,1), _T("Inserted lines"), *statusBar.GetFont(), GREEN);
  }
  return ret;
}

// -------------------------------------------- File menu ----------------------------------------

static const String &getFileDialogExtension() {
  static String result;

//#define extElement(s1, s2) String(_T(s1)) + spaceString(1,0) + String(_T(s2))

#define extElement(s1, s2) format(_T("%s%c%s"), _T(s1), 0, _T(s2))

  static const String fileDialogExtensions[] = {
    extElement("C++ files (.cpp;.c;.cxx;.h;.hpp;.inl;.rc;.dsp;.dsw;.dsm;.tli;.tlh)", "*.cpp; *.c; *.h; *.hpp; *.inl; *.rc; *.dsp; *.dsw; *.dsm; *.tli; *.tlh")
   ,extElement("C# source files (.cs)"                                             , "*.cs"                                     )
   ,extElement("Java source files (.java)"                                         , "*.java"                                   )
   ,extElement("JavaScript files (.js)"                                            , "*.js"                                     )
   ,extElement("JavaServer Pages script file (.jsp)"                               , "*.jsp"                                    )
   ,extElement("Pascal source files (.pas;.inc)"                                   , "*.pas; *.inc"                             )
   ,extElement("Perl source files (.pl;.pm;.plx)"                                  , "*.pl; *.pm; *.plx"                        )
   ,extElement("PHP (.php;.php3;.phtml)"                                           , "*.php; *.php3; *.phtml"                   )
   ,extElement("Ruby files (.rb;.rbw)"                                             , "*.rb; *.rbw"                              )
   ,extElement("Visual Basic files (.vb;.vbx)"                                     , "*.vb; *.vbx"                              )
   ,extElement("Assembly language source files (.asm)"                             , "*.asm"                                    )
   ,extElement("Batch files (.bat;.cmd;.nt)"                                       , "*.bat; *.cmd; *.nt"                       )
   ,extElement("Makefile (.mak)"                                                   , "*.mak"                                    )
   ,extElement("SQL Scripts (.sql;.tsq;.ssq)"                                      , "*.sql; *.tsq; *.ssq"                      )
   ,extElement("Text files (.txt;.ini;.log)"                                       , "*.txt; *.ini; *.log"                      )
   ,extElement("Web files (.html;.htm;.htw;.htx;.xml;.xsd)"                        , "*.html; *.htm; *.htw; *.htx; *.xml; *.xsd")
   ,extElement("All files (*.*)"                                                   , "*.*"                                      )
  };

  if(result.length() == 0)  {
    StringArray tmp;
    for(int i = 0; i < ARRAYSIZE(fileDialogExtensions); i++) {
      tmp.add(fileDialogExtensions[i]);
    }
    result = tmp.getAsDoubleNullTerminatedString();
  }
  return result;
}

static void getFileNames(TCHAR *dst[2], TCHAR *filenames) {
  dst[0] = filenames;
  dst[1] = filenames + _tcsclen(filenames) + 1;
  dst[2] = dst[1] + _tcsclen(dst[1]) + 1;
}

void CMainFrame::OnFileOpenPanel0() {
  OnFileOpen(0);
}

void CMainFrame::OnFileOpenPanel1() {
  OnFileOpen(1);
}

void CMainFrame::OnFileOpen(int id) {
  TextView   *view = getActiveTextView();
  if(view == NULL) {
    MessageBox(_T("No active view"), NULL,MB_ICONERROR);
    return;
  }
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrFilter  = getFileDialogExtension().cstr();
  dlg.m_ofn.lpstrTitle   = _T("Open files");
  dlg.m_ofn.nFilterIndex = getOptions().m_defaultExtensionIndex;
  dlg.m_ofn.Flags |= OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST;
  TCHAR fileNames[1024];
  fileNames[0]           = 0;
  dlg.m_ofn.lpstrFile    = fileNames;
  dlg.m_ofn.nMaxFile     = ARRAYSIZE(fileNames);

  if((dlg.DoModal() != IDOK) || (_tcsclen(fileNames) == 0)) {
    return;
  }
  getOptions().m_defaultExtensionIndex = dlg.m_ofn.nFilterIndex;

  TCHAR *files[3];
  getFileNames(files,fileNames);

  CWinDiffDoc *doc = view->getDocument();

  if(_tcsclen(files[1]) == 0) { // only one selected
    doc->setDoc(id, DIFFDOC_FILE, files[0]);
  } else {
    const String f1 = FileNameSplitter::getChildName(files[0],files[1]);
    const String f2 = FileNameSplitter::getChildName(files[0],files[2]);
    doc->setDocs(f1, f2);
  }
  Invalidate(FALSE);
}

void CMainFrame::OnFileMruFile1()  { onFileMruFile( 0);}
void CMainFrame::OnFileMruFile2()  { onFileMruFile( 1);}
void CMainFrame::OnFileMruFile3()  { onFileMruFile( 2);}
void CMainFrame::OnFileMruFile4()  { onFileMruFile( 3);}
void CMainFrame::OnFileMruFile5()  { onFileMruFile( 4);}
void CMainFrame::OnFileMruFile6()  { onFileMruFile( 5);}
void CMainFrame::OnFileMruFile7()  { onFileMruFile( 6);}
void CMainFrame::OnFileMruFile8()  { onFileMruFile( 7);}
void CMainFrame::OnFileMruFile9()  { onFileMruFile( 8);}
void CMainFrame::OnFileMruFile10() { onFileMruFile( 9);}
void CMainFrame::OnFileMruFile11() { onFileMruFile(10);}
void CMainFrame::OnFileMruFile12() { onFileMruFile(11);}
void CMainFrame::OnFileMruFile13() { onFileMruFile(12);}
void CMainFrame::OnFileMruFile14() { onFileMruFile(13);}
void CMainFrame::OnFileMruFile15() { onFileMruFile(14);}
void CMainFrame::OnFileMruFile16() { onFileMruFile(15);}

void CMainFrame::onFileMruFile(int index) {
  TextView *view = getActiveTextView();
  if(view == NULL) return;
  try {
    const String fname = theApp.getRecentFile(index);

    if(ACCESS(fname, 4) < 0) {
      const int errorCode = errno;
      MessageBox(getErrnoText().cstr(), _T("Error"), MB_ICONWARNING);
      if(errorCode == ENOENT) {
        theApp.removeFromRecentFiles(index);
      }
      return;
    }
    CWinDiffDoc *doc = view->getDocument();
    doc->setDoc(view->getId(),DIFFDOC_FILE, fname);
    Invalidate(FALSE);
    //view->refreshBoth();
  } catch(Exception e) {
    showException(e);
  }
}

void CMainFrame::OnFilePrintPreview() {
  CWinDiffView *view = getActiveDiffView();
  if(view) {
    CDC dc;
    theApp.CreatePrinterDC(dc);
    FindParameters param;
    CFindDlg dlg(param,view);
    dlg.DoModal();
//CPrinter 
//OnPrint
  // TODO: Add your command handler code here
  } 
}

// -------------------------------------------- Edit menu ----------------------------------------

void CMainFrame::OnEditCopy() {
  CWinDiffView *view = getActiveDiffView();
  if(view) {
    view->copyToClipboard();
  }
}

void CMainFrame::OnEditPaste() {
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }
  const String t = getClipboardText();
  CWinDiffDoc *doc = view->getDocument();

  doc->setDoc(view->getId(),DIFFDOC_BUF,t);
  view->refreshBoth();
}

void CMainFrame::OnEditSelectAll() {
  APPLY_TO_ACTIVE_TEXTVIEW(selectAll())
}

void CMainFrame::OnEditFind() {
  if(!isToolbarButtonEnabled(ID_EDIT_FIND)) {
    return;
  }
  CWinDiffView *view = getActiveDiffView();
  if(view) {
    TextView &tv = view->m_textView;
    if(tv.hasSelection() && (tv.getSelectedRange().getLineCount() == 1)) {
      m_findParameters.m_findWhat = tv.getSelectedText();
    } else {
      m_findParameters.m_findWhat = tv.getCurrentWord();
    }
    CFindDlg dlg(m_findParameters, view);
    if(dlg.DoModal() == IDOK) {
      view->find(m_findParameters);
    }
  }
}

void CMainFrame::OnEditFindNext() {
  if(!m_findParameters.isEmpty()) {
    CWinDiffView *view = getActiveDiffView();
    if(view) {
      m_findParameters.m_dirUp       = false;
      m_findParameters.m_skipCurrent = true;
      view->find(m_findParameters);
    }
  }
}

void CMainFrame::OnEditFindPrev() {
  if(!m_findParameters.isEmpty()) {
    CWinDiffView *view = getActiveDiffView();
    if(view) {
      m_findParameters.m_dirUp       = true;
      m_findParameters.m_skipCurrent = true;
      view->find(m_findParameters);
    }
  }
}

void CMainFrame::OnEditNextDiff() {
  if(!isToolbarButtonEnabled(ID_EDIT_NEXT_DIFF)) {
    return;
  }

  CWinDiffView *view = getActiveDiffView();
  if(view) {
    view->m_textView.nextDiff();
  }
}

void CMainFrame::OnEditPrevDiff() {
  if(!isToolbarButtonEnabled(ID_EDIT_PREV_DIFF)) {
    return;
  }

  CWinDiffView *view = getActiveDiffView();
  if(view) {
    view->m_textView.prevDiff();
  }
}

void CMainFrame::OnEditShowDetails() {
  if(!isToolbarButtonEnabled(ID_EDIT_SHOWDETAILS)) {
    return;
  }
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }
  if(!view->hasPartner()) {
    return;
  }
  String s1, s2;
  if(view->getId() == 0) {
    s1 = view->getCurrentString();
    s2 = view->getPartner()->getCurrentString();
  } else {
    s2 = view->getCurrentString();
    s1 = view->getPartner()->getCurrentString();
  }
  
  CZoomDlg dlg(s1,s2, view);
  dlg.DoModal();
}

void CMainFrame::OnEditGoto() {
  CGotoDlg dlg;
  if(dlg.DoModal() == IDOK) {
    CWinDiffView *view = getActiveDiffView();
    if(view) {
      view->gotoLine(dlg.m_line);
    }
  }
}

void CMainFrame::OnEditRefreshFiles() {
  refreshDoc(true);
}

void CMainFrame::OnEditSwapPanels() {
  TextView *view1 = getActiveTextView();
  if(view1 == NULL) {
    return;
  }
  TextView *view2 = view1->getPartner();
  if(view2 == NULL) {
    return;
  }
  view1->savePositionState();
  view2->savePositionState();
  PositionState tmpstate1 = view1->getSavedPositionState();
  PositionState tmpstate2 = view2->getSavedPositionState();
  tmpstate1.m_id = 1 - tmpstate1.m_id;
  tmpstate2.m_id = 1 - tmpstate2.m_id;
  view1->setSavedPositionState(tmpstate2);
  view2->setSavedPositionState(tmpstate1);
  view1->getDocument()->m_diff.swapDocs();
  view1->activatePartner();
  view2->refreshBoth();
}

// ---------------------------------------- View menu ------------------------------------------

void CMainFrame::OnViewShowWhiteSpace() {
  APPLY_TO_ACTIVE_TEXTVIEW(setViewWhiteSpace(toggleMenuItem(this, ID_VIEW_SHOWWHITESPACE)))
}

void CMainFrame::OnViewTabSize() { 
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }
  CWinDiffDoc *doc = view->getDocument();

  const int tabSize = doc->m_diff.getTabSize();
  CTabSizeDlg dlg(tabSize);
  if(dlg.DoModal() == IDOK) {
    if(dlg.m_tabSize != doc->m_diff.getTabSize()) {
      view->setTabSize(dlg.m_tabSize, true);
    }
  }
}

void CMainFrame::OnViewNameFontSize100() { setNameFontSize(100); }
void CMainFrame::OnViewNameFontSize125() { setNameFontSize(125); }
void CMainFrame::OnViewNameFontSize150() { setNameFontSize(150); }
void CMainFrame::OnViewNameFontSize175() { setNameFontSize(175); }
void CMainFrame::OnViewNameFontSize200() { setNameFontSize(200); }

void CMainFrame::OnViewFont() {
  LOGFONT curlf = getOptions().m_logFont;
  CFontDialog  dlg(&curlf);
  dlg.m_cf.Flags |= CF_FIXEDPITCHONLY | CF_NOSTYLESEL;
  if(dlg.DoModal() == IDOK) {
    LOGFONT lf;
    dlg.GetCurrentFont(&lf);
    APPLY_TO_ACTIVE_TEXTVIEW(setFont(lf));
  }
}

void CMainFrame::setNameFontSize(int pct) {
  APPLY_TO_ACTIVE_TEXTVIEW(setNameFontSizePct(pct));
  updateNameFontSizeMenuItems(pct);  
}

void CMainFrame::OnViewShow1000separator() {
  APPLY_TO_ACTIVE_TEXTVIEW(setShow1000Separator(toggleMenuItem(this, ID_VIEW_SHOW1000SEPARATOR)))
}

void CMainFrame::OnViewHighlightCompareEqual() {
  APPLY_TO_ACTIVE_TEXTVIEW(setHighLightCompareEqual(toggleMenuItem(this, ID_VIEW_HIGHLIGHTCOMPAREEQUAL)));
}

// ------------------------------------- Options menu ---------------------------------

void CMainFrame::OnOptionsIgnoreWhiteSpace() {
  APPLY_TO_ACTIVE_TEXTVIEW(setIgnoreWhiteSpace(toggleMenuItem(this, ID_OPTIONS_IGNOREBLANKS)))
}

void CMainFrame::OnOptionsIgnoreCase() {
  APPLY_TO_ACTIVE_TEXTVIEW(setIgnoreCase(toggleMenuItem(this, ID_OPTIONS_IGNORECASE)))
}

void CMainFrame::OnOptionsIgnoreStrings() {
  APPLY_TO_ACTIVE_TEXTVIEW(setIgnoreStrings(toggleMenuItem(this, ID_OPTIONS_IGNORESTRINGS)))
}

void CMainFrame::OnOptionsIgnoreComments() {
  APPLY_TO_ACTIVE_TEXTVIEW(setIgnoreComments(toggleMenuItem(this, ID_OPTIONS_IGNORECOMMENTS)))
}

void CMainFrame::OnOptionsStripComments() {
  APPLY_TO_ACTIVE_TEXTVIEW(setStripComments(toggleMenuItem(this, ID_OPTIONS_STRIPCOMMENTS)))
}

void CMainFrame::OnOptionsIgnoreColumns() {
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }

  if(!isMenuItemChecked(this, ID_OPTIONS_IGNORECOLUMNS)) {
    const Options &options = getOptions();
    if(options.m_fileFormat.isEmpty()) {
      OnOptionsDefineColumns();
    }
    if(options.m_fileFormat.isEmpty()) {
      return;
    }
  }
  view->setIgnoreColumns(toggleMenuItem(this,ID_OPTIONS_IGNORECOLUMNS));
}

void CMainFrame::OnOptionsDefineColumns() {
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }

  FileFormat param = getOptions().m_fileFormat;

  CDefineFileFormatDlg dlg(param,view->getCurrentOrigString());
  if(dlg.DoModal() == IDOK) {
    getOptions().m_fileFormat = param;
    if(isMenuItemChecked(this, ID_OPTIONS_IGNORECOLUMNS)) {
      view->setIgnoreColumns(true);
    }
  }
}

void CMainFrame::OnOptionsIgnoreRegex() {
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }
  if(!isMenuItemChecked(this, ID_OPTIONS_IGNOREREGEX)) {
    const Options &options = getOptions();
    if(options.m_regexFilter.isEmpty()) {
      OnOptionsDefineRegex();
    }
    if(options.m_regexFilter.isEmpty()) {
      return;
    }
  }
  view->setIgnoreRegex(toggleMenuItem(this, ID_OPTIONS_IGNOREREGEX));
}

void CMainFrame::OnOptionsDefineRegex() {
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }
  RegexFilter param = getOptions().m_regexFilter;
  CRegexDlg dlg(param);
  if(dlg.DoModal() == IDOK) {
    getOptions().m_regexFilter = param;
    if(isMenuItemChecked(this, ID_OPTIONS_IGNOREREGEX)) {
      view->setIgnoreRegex(true);
    }
  }
}

void CMainFrame::OnOptionsSaveOptions() {
  Options &options = getOptions();
  CEnterOptionsNameDlg dlg(options.m_name);
  if(dlg.DoModal() == IDOK) {
    options.m_name = dlg.m_name;
    if(options.save()) {
      updateLoadOptionsMenu();
    }
  }
}

void CMainFrame::OnOptionsLoadOptions1() { loadOptions(1); }
void CMainFrame::OnOptionsLoadOptions2() { loadOptions(2); }
void CMainFrame::OnOptionsLoadOptions3() { loadOptions(3); }
void CMainFrame::OnOptionsLoadOptions4() { loadOptions(4); }
void CMainFrame::OnOptionsLoadOptions5() { loadOptions(5); }
void CMainFrame::OnOptionsLoadOptions6() { loadOptions(6); }
void CMainFrame::OnOptionsLoadOptions7() { loadOptions(7); }
void CMainFrame::OnOptionsLoadOptions8() { loadOptions(8); }
void CMainFrame::OnOptionsLoadOptions9() { loadOptions(9); }

void CMainFrame::loadOptions(int id) {
  const int index = id - 1;
  Array<Options> optionArray = Options::getExisting();
  if(index >= (int)optionArray.size()) {
    return;
  }
  getOptions() = optionArray[index];
  activateOptions();
}

void CMainFrame::activateOptions() {
  TextView *view = getActiveTextView();
  if(view == NULL) {
    return;
  }
  view->setOptions(getOptions());
  setWindowSize(this, getOptions().m_windowSize);
  updateCheckedMenuItems();
}

void CMainFrame::OnOptionsOrganizeOptions() {
  COptionsOrganizerDlg dlg;
  if(dlg.DoModal() == IDOK) {
    updateLoadOptionsMenu();
  }
}
