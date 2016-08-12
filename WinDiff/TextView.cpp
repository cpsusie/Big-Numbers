#include "stdafx.h"
#include "Mainfrm.h"

IMPLEMENT_DYNCREATE(TextView, CView)

BEGIN_MESSAGE_MAP(TextView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
// Standard printing commands
    ON_COMMAND(ID_FILE_PRINT        , CView::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_DIRECT , CView::OnFilePrint       )
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

#define SELECTFONT(dc) (dc).SelectObject(&m_font)

TextView::TextView() {
  init();
  m_winSize               = CSize(-1,-1);
  m_caretSize             = CSize(-1,-1);
  m_arrowSize             = CSize(-1,-1);
  m_arrowDC.CreateCompatibleDC(NULL);
  m_workDC.CreateCompatibleDC( NULL);
  m_maxOffset.set(0,0);
}

void TextView::init() {
  m_state.init();
  m_lastCaretPos = getCaretPosition();
  resetArrow();
  m_anchor.reset();
  m_lastAnchor.reset();
}

void TextView::Create(const CRect &r, CWnd *parent, Diff &diff) {
  m_diff = &diff;
  if(CView::Create(NULL, NULL, AFX_WS_DEFAULT_VIEW ,r, parent, 0) == -1) {
    throwException(_T("TextView::Create failed"));
  }
}

int TextView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(CView::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }
  setFlagValue(VIEW_ISCREATED, true);
  return 0;
}

void TextView::OnDestroy() {
  CView::OnDestroy();
  setFlagValue(VIEW_ISCREATED, false);
}

void TextView::OnDraw(CDC *pDC) {
  SELECTFONT(m_workDC);
  paint(&m_workDC);
  pDC->BitBlt(0,0,m_workSize.cx, m_workSize.cy, &m_workDC, 0,0,SRCCOPY);
}

void TextView::OnInitialUpdate() {
  CView::OnInitialUpdate();
  m_state.init(getDiffView().getId());

  bool idCollision = false;
  if(hasPartner() && getId() == getPartner()->getId()) {
    m_state.init(1 - getPartner()->getId());
    idCollision = true;
  }

  setFlagValue(VIEW_ISACTIVE, getId() == 0);

  if(idCollision) {
    repaintAll();
  } else {
    refreshDoc();
  }
  setWindowCursor(this,IDC_IBEAM);
}

void TextView::OnSize(UINT nType, int cx, int cy) {
  updateWindowSize(cx,cy);
}

#define ERASEONINVALIDATE false

void TextView::repaintAll() {
  Invalidate(ERASEONINVALIDATE);
}

void TextView::savePositionState() {
  if(m_diff->isEmpty()) {
    return;
  }
  const DiffLine &dl = m_diff->getDiffLine(getCurrentLine());
  int index0;
  if((index0 = dl.getText(0).getIndex()) >= 0) {
    m_savedPositionState.set(0, TextPosition(index0                  , getLineOffset()), getCaretPosition(), getPreferredColumn());
  } else {
    m_savedPositionState.set(1, TextPosition(dl.getText(1).getIndex(), getLineOffset()), getCaretPosition(), getPreferredColumn());
  }
}

void TextView::restorePositionState() {
  const DiffLineArray &lines = m_diff->getDiffLines();
  for(int i = 0; i < getLineCount(); i++) {
    const DiffLine &dl = lines[i];
    const int        l = dl.getText(m_savedPositionState.m_id).getIndex();
    if(l == m_savedPositionState.m_offset.m_line) {
      setTopLine(      i-m_savedPositionState.m_caret.y);
      setLineOffset(     m_savedPositionState.m_offset.m_column);
      setCaretY(         m_savedPositionState.m_caret.y);
      setPreferredColumn(m_savedPositionState.m_preferredColumn);
      break;
    }
  }
  m_savedPositionState.reset();
}

void TextView::refreshDoc() {
  m_diff      = &(getDocument()->m_diff);
  m_lineCount = m_diff->getLineCount();

  if(getShow1000Separator()) {
    m_margin = (int)format1000(getLineCount()).length() + 1;
  } else {
    m_margin = (int)format(_T("%d"), getLineCount()).length() + 1;
  }
  updateWindowSize();

  clearDC();
  getDiffView().setScrollRange(true);
  getDiffView().updateTitle();
}

void TextView::refreshBoth() {
  refreshDoc();
  if(hasPartner()) {
    getPartner()->refreshDoc();
  }
  if(!m_savedPositionState.isEmpty()) {
    restorePositionState();
  }
  redrawBoth();

  const String title = format(_T("Differences between %s and %s"),m_diff->getName(0).cstr(),m_diff->getName(1).cstr());
  theApp.GetMainWnd()->SetWindowText(title.cstr());
}

#define SAVESTATE    if(repaint) savePositionState()
#define REFRESHBOTH  if(repaint) refreshBoth()

// ---------------------------------------- Set Option functions ------------------------------------

bool TextView::setIgnoreCase(bool newValue, bool repaint) {
  SAVESTATE;
  const bool changed = getDocument()->setIgnoreCase(newValue, repaint);
  REFRESHBOTH;
  return changed;
}

bool TextView::setIgnoreWhiteSpace(bool newValue, bool repaint) {
  SAVESTATE;
  const bool changed = getDocument()->setIgnoreWhiteSpace(newValue, repaint);
  REFRESHBOTH;
  return changed;
}

bool TextView::setIgnoreComments(bool newValue, bool repaint) {
  SAVESTATE;
  const bool changed = getDocument()->setIgnoreComments(newValue, repaint);
  REFRESHBOTH;
  return changed;
}

bool TextView::setIgnoreColumns(bool newValue, bool repaint) {
  SAVESTATE;
  bool changed;
  if(newValue) {
    changed = getDocument()->setFileFormat(&getOptions().m_fileFormat, repaint);
  } else {
    changed = getDocument()->setFileFormat(NULL, repaint);
  }
  REFRESHBOTH;
  return changed;
}

bool TextView::setIgnoreRegex(bool newValue, bool repaint) {
  SAVESTATE;
  bool changed;
  if(newValue) {
    changed = getDocument()->setRegexFilter(&getOptions().m_regexFilter, repaint);
  } else {
    changed = getDocument()->setRegexFilter(NULL, repaint);
  }
  REFRESHBOTH;
  return changed;
}

bool TextView::setStripComments(bool newValue, bool repaint) {
  SAVESTATE;
  const bool changed = getDocument()->setStripComments(newValue, repaint);
  REFRESHBOTH;
  return changed;
}

bool TextView::setIgnoreStrings(bool newValue, bool repaint) {
  SAVESTATE;
  const bool changed = getDocument()->setIgnoreStrings(newValue, repaint);
  REFRESHBOTH;
  return changed;
}

bool TextView::setViewWhiteSpace(bool newValue, bool repaint) {
  SAVESTATE;
  const bool changed = getDocument()->setViewWhiteSpace(newValue, repaint);
  REFRESHBOTH;
  return changed;
}

bool TextView::setTabSize(int newValue, bool repaint) {
  SAVESTATE;
  const bool changed = getDocument()->setTabSize(newValue, repaint);
  REFRESHBOTH;
  return changed;
}

static bool operator==(const LOGFONT &lf1, const LOGFONT &lf2) {
  return (memcmp(&lf1, &lf2, offsetof(LOGFONT,lfFaceName)) == 0)
      && (_tcsicmp(lf1.lfFaceName,  lf2.lfFaceName) == 0);
}

LOGFONT TextView::getLogFont() {
  LOGFONT result;
  if(m_font.m_hObject != NULL) {
    m_font.GetLogFont(&result);
  } else {
    memset(&result,0,sizeof(LOGFONT));
  }
  return result;
}

bool TextView::setFont(const LOGFONT &newValue, bool repaint) {
  if(getLogFont() == newValue) return false;
  SAVESTATE;
  if(m_font.m_hObject) {
    m_font.DeleteObject();
  }
  if(!m_font.CreateFontIndirect(&newValue)) {
    throwLastErrorOnSysCallException(_T("CreateFontIndirect"));
  }
  getOptions().m_logFont = newValue;
  if(hasPartner()) { getPartner()->setFont(newValue, false); }
  REFRESHBOTH;
  return true;
}

bool TextView::setShow1000Separator(bool newValue, bool repaint) {
  if(getShow1000Separator() == newValue) {
    return false;
  }
  SAVESTATE;
  setFlagValue(SHOW_1000SEPARATOR, newValue);
  getOptions().m_show1000Separator = newValue;
  if(hasPartner()) { 
    getPartner()->setFlagValue(SHOW_1000SEPARATOR, newValue);
  }
  REFRESHBOTH;
  return true;
}

bool TextView::setNameFontSizePct(int newValue, bool repaint) {
  if(newValue == getDiffView().getNameFontSizePct()) {
    return false;
  }
  SAVESTATE;
  getDiffView().setNameFontSizePct(newValue);
  getOptions().m_nameFontSizePct = newValue;
  REFRESHBOTH;
  return true;
}

bool TextView::setHighLightCompareEqual(bool newValue, bool repaint) {
  if(getHighLightCompareEqual() == newValue) {
    return false;
  }
  setFlagValue(HIGHLIGhT_COMPAREEQUAL, newValue);
  if(hasPartner()) { 
    getPartner()->setFlagValue(HIGHLIGhT_COMPAREEQUAL, newValue);
  }

  REFRESHBOTH;
  return true;
}

void TextView::setFlagValue(TextViewFlags id, bool value) {
  if(value) {
    m_viewFlags.add(id);
  } else {
    m_viewFlags.remove(id);
  }
}

void TextView::setOptions(const Options &options) {
  savePositionState();
  bool recomp = false;

  recomp  |= setIgnoreCase(        options.m_ignoreCase       , false);
  recomp  |= setIgnoreWhiteSpace(  options.m_ignoreWhiteSpace , false);
  recomp  |= setIgnoreComments(    options.m_ignoreComments   , false);
  recomp  |= setStripComments(     options.m_stripComments    , false);
  recomp  |= setIgnoreStrings(     options.m_ignoreStrings    , false);
  recomp  |= setIgnoreColumns(     options.m_ignoreColumns    , false);
  recomp  |= setIgnoreRegex(       options.m_ignoreRegex      , false);

  bool refresh = recomp;
  
  refresh |= setViewWhiteSpace(    options.m_viewWhiteSpace   , false);
  refresh |= setTabSize(           options.m_tabSize          , false);
  refresh |= setShow1000Separator( options.m_show1000Separator, false);
  refresh |= setNameFontSizePct(   options.m_nameFontSizePct  , false);
  refresh |= setFont(              options.m_logFont          , false);

  if(recomp) {
    getDocument()->recompare();
  }
  if(refresh) {
    if(!recomp) {
      getDocument()->refresh();
    }
    refreshBoth();
  }
}

// ---------------------------------- paint functions ------------------------------------

void TextView::paint(CDC *pDC) {
  const int topLine = getTopLine();
  int y = 0;
  clearDC(pDC);

  for(int i = topLine; (i < getLineCount()) && (y <= m_winSize.cy); i++, y++) {
    paintLineNumber(pDC, y);
    paintTextLine(pDC, y);
  }
  if(y < m_winSize.cy) { // clear the rest
    const int textL = m_characterSize.cx * m_margin;
    const int textB = m_characterSize.cy * y;
    const int w     = m_workSize.cx - textL;
    const int h     = m_workSize.cy - textB;
    pDC->FillSolidRect(0    ,textB, textL, h, LINENUMBERBACKGROUND);
    pDC->FillSolidRect(textL,textB, w    , h, WHITE               );
/*
    paintLineNumber( pDC, y, -1);
    paintTextSegment(pDC, 0, y, m_winSize.cx+1, WHITE, WHITE, _T(""));
*/
  }
  if(isActive()) {
    displayCaret(pDC);
  }
  getDiffView().setScrollPos();
}

void TextView::clearDC(CDC *pDC) {
  if(pDC) {
    pDC->FillSolidRect(0,0,m_workSize.cx, m_workSize.cy,WHITE);
    resetArrow();
  } else {
    CClientDC dc(this);
    const CSize sz = getClientRect(this).Size();
    dc.FillSolidRect(0,0,sz.cx,sz.cy,WHITE);
  }
}

void TextView::repaintLines(CDC *pDC, int from, int to) {
  SELECTFONT(*pDC);
  for(int y = from; y <= to; y++) {
    paintTextLine(pDC, y);
  }
}

void TextView::unMarkSelectedText(CDC *pDC) {
  if(m_anchor != m_lastAnchor) {
    if(!m_lastAnchor.isEmpty()) {
      repaintLines(pDC, 0,m_winSize.cy);
    }
    m_lastAnchor = m_anchor;
  }
}

void TextView::markSelectedText(CDC *pDC) {
  unMarkSelectedText(pDC);
  if(!m_anchor.isEmpty()) {
    if(m_lastCaretPos.y >= 0) {
      const CPoint cp = getCaretPosition();
      repaintLines(pDC,min(cp.y,m_lastCaretPos.y),max(cp.y,m_lastCaretPos.y));
    }
  }
}

// ------------------------------------ Arrow functions ---------------------------------

void TextView::paintArrow(CDC *pDC) {
  if(m_state.m_caret.y == getArrowLine()) return;
  if(hasArrow()) {
    unpaintArrow(pDC);
  } else {
    TextView *partner = getPartner();
    if(partner && partner->hasArrow()) {
      CClientDC dc(partner);
      partner->unpaintArrow(&dc);
    }
  }
  
  m_arrowLine = m_state.m_caret.y;
  const CPoint pos = getArrowPosition(getArrowLine());
  saveBackground(pDC);
  pDC->BitBlt(pos.x,pos.y, m_arrowSize.cx,m_arrowSize.cy, &m_arrowDC,0,0,SRCCOPY);
}

void TextView::unpaintArrow(CDC *pDC) {
  if(!hasArrow()) return;
  
  const CPoint pos = getArrowPosition(getArrowLine());
  CBitmap *oldBitmap = m_arrowDC.SelectObject(&m_backgroundBitmap);
  if(pDC) {
    pDC->BitBlt(pos.x,pos.y, m_arrowSize.cx,m_arrowSize.cy, &m_arrowDC, 0, 0, SRCCOPY);
  } else {
    CClientDC dc(this);
    dc.BitBlt(pos.x,pos.y, m_arrowSize.cx,m_arrowSize.cy, &m_arrowDC, 0, 0, SRCCOPY);
  }
  m_arrowDC.SelectObject(oldBitmap);
  resetArrow();
}

void TextView::saveBackground(CDC *pDC) {
  const CPoint pos = getArrowPosition(getArrowLine());
  CBitmap *oldBitmap = m_arrowDC.SelectObject(&m_backgroundBitmap);
  m_arrowDC.BitBlt(0,0, m_arrowSize.cx,m_arrowSize.cy, pDC, pos.x,pos.y, SRCCOPY);
  m_arrowDC.SelectObject(oldBitmap);
}


CPoint TextView::getArrowPosition(int lineno) const {
  const int x = m_characterSize.cx * m_margin - m_arrowSize.cx - m_characterSize.cx * 2 / 3;
  const int y = m_characterSize.cy * lineno   + max(m_characterSize.cy - m_arrowSize.cy,0)/2;
  return CPoint(x,y);
}

void TextView::setArrowSize(const CSize &size) {
  if(size == m_arrowSize) {
    return;
  }
  resetArrow();
  if(m_arrowBitmap.m_hObject) {
    m_arrowDC.SelectObject((CBitmap*)NULL);
    m_arrowBitmap.DeleteObject();
    m_backgroundBitmap.DeleteObject();
  }

  CBitmap rawBM;
  rawBM.LoadBitmap(IDB_ARROW);
  const CSize rawSize = getBitmapSize(rawBM);

  m_arrowBitmap.CreateBitmap(size.cx, size.cy, 1, 32,NULL);
  m_arrowDC.SelectObject(&m_arrowBitmap);
  CDC rawDC;
  rawDC.CreateCompatibleDC(NULL);
  CBitmap *oldbm = rawDC.SelectObject(&rawBM);
  SetStretchBltMode(m_arrowDC, COLORONCOLOR /*HALFTONE*/);
  m_arrowDC.StretchBlt( 0,0,size.cx, size.cy, &rawDC, 0,0,rawSize.cx, rawSize.cy, SRCCOPY);
  m_arrowSize = size;
  m_backgroundBitmap.CreateBitmap(m_arrowSize.cx, m_arrowSize.cy, 1, 32,NULL);
}

// ------------------------------------ Caret functions ---------------------------------

void TextView::setCaretSize(const CSize &size) {
  m_caretSize = size;
}

void TextView::showCaret() {
  int id = getId();

  CPoint caret = getCaretPosition();
  if((caret.x >= 0) && !isCaretVisible()) {
    CPoint cp;
    cp.x = (caret.x + m_margin) * m_characterSize.cx;
    cp.y = caret.y * m_characterSize.cy;
    if(!caretExist()) createCaret();
    SetCaretPos(cp);
    ShowCaret();
    setFlagValue(CARET_VISIBLE, true);
  }
}

void TextView::hideCaret() {
  if(isCaretVisible()) {
    HideCaret();
    setFlagValue(CARET_VISIBLE, false);
  }
}

void TextView::createCaret() {
  if(!caretExist()) {
    CreateSolidCaret(m_caretSize.cx, m_caretSize.cy);
    setFlagValue(CARET_EXIST  , true );
    setFlagValue(CARET_VISIBLE, false);
  }
}

void TextView::destroyCaret() {
  if(caretExist()) {
    DestroyCaret();
    setFlagValue(CARET_EXIST  , false);
    setFlagValue(CARET_VISIBLE, false);
  }
}

void TextView::displayCaret(CDC *pDC) {
  m_state.m_caret.x = min(m_state.getPreferredCaretX(), getMaxCaretX());

  TextView *partner = getPartner();
  if(partner) {
    const CPoint caret           = getCaretPosition();
    int          newColumnOffset = partner->getLineOffset();
    int          xpos            = getLineOffset() + max(caret.x, 0);

    if(xpos < partner->getLineOffset()) {
      newColumnOffset = xpos;
    } else if(xpos >= partner->getRightOffset()) {
      newColumnOffset = xpos - partner->m_winSize.cx;
    }

    if(newColumnOffset < 0) {
      newColumnOffset = 0;
    }

    partner->setPreferredColumn(getPreferredColumn());
    partner->m_state.m_caret.x = caret.x;
    partner->setCaretY(caret.y);

    const TextPosition newTopLeft(getTopLine(), newColumnOffset); // for partner

    if(newTopLeft != partner->getTopLeft()) {
      partner->setTopLeft(newTopLeft);
      partner->repaintAll();
    }
  }

  CClientDC dc(this);
  if(pDC == NULL) {
    pDC = &dc;
  }

  markSelectedText(pDC);

  CWinDiffSplitterWnd *splitter = (CWinDiffSplitterWnd*)getDiffView().GetParent();
  splitter->setActivePanel(getId());

  paintArrow(pDC);
  m_lastCaretPos = getCaretPosition();

  CMainFrame *frame = (CMainFrame*)(splitter->GetParent());
  frame->ajourMenuItems();
}

// ----------------------------------- paint line function ---------------------------------------

static void setColor(CDC *pDC, const COLORREF &tc, const COLORREF &bc) {
  pDC->SetBkColor(bc);
  pDC->SetTextColor(tc);
}

void TextView::paintLineNumber(CDC *pDC, int y, int lineno) {
  String tmp;
  if(lineno < 0) {
    tmp = format(_T("%*.*s"), m_margin, m_margin, _T(""));
  } else if(getShow1000Separator()) {
    tmp = format(_T("%*s "), m_margin - 1, format1000(lineno+1).cstr());
  } else {
    tmp = format(_T("%*d "), m_margin - 1, lineno+1);
  }
  if(y == getArrowLine()) {
    unpaintArrow(pDC);
  }
  setColor(pDC,BLACK,LINENUMBERBACKGROUND);
  pDC->TextOut(0,y * m_characterSize.cy,tmp.cstr());
}

void TextView::paintLineNumber(CDC *pDC, int y) {
  const int topLine = getTopLine();
  if(topLine + y >= getLineCount()) {
    return;
  }
  const DiffLine &df = m_diff->getDiffLine(topLine + y);
  const int       id = getId();

  switch(df.getAttr()) {
  case EQUALLINES   :
  case CHANGEDLINES :
    paintLineNumber(pDC, y, df.getText(id).getIndex());
    break;
  case DELETEDLINES :
    if(id == 1) {
      paintLineNumber(pDC, y, -1);
    } else {
      paintLineNumber(pDC, y, df.getText(0).getIndex());
    }
    break;

  case INSERTEDLINES:
    if(id == 1) {
      paintLineNumber(pDC, y, df.getText(1).getIndex());
    } else {
      paintLineNumber(pDC, y, -1);
    }
    break;
  }
}

LineSegments TextView::getLineSegments(int l) const {
  LineSegments result;
  if(m_anchor.isEmpty()) {
    result.m_segments = 0;
    return result;
  }

  const TextPosition &topLeft     = getTopLeft();
  const int           currentLine = getCurrentLine();
  const int           currentCol  = getCurrentColumn();

#define ISBETWEEN(x,a,b) (((a)<=(x)) && ((x)<=(b)))

  if(currentLine == m_anchor.m_line) {
    if((l == currentLine) && (currentCol != m_anchor.m_column)) {
      if(currentCol > m_anchor.m_column) {
        result.m_c1 = m_anchor.m_column - topLeft.m_column;
        result.m_c2 = currentCol        - topLeft.m_column;
      } else { // currentCol < m_anchor.m_column
        result.m_c1 = currentCol        - topLeft.m_column;
        result.m_c2 = m_anchor.m_column - topLeft.m_column;
      }
      result.m_segments   = ((result.m_c1>0              ) ? HAS_LEFTSEGMENT   : 0)
                          |                                  HAS_MIDDLESEGMENT
                          | ((result.m_c2<m_winSize.cx+1 ) ? HAS_RIGHTSEGMENT  : 0);
    } else {
      result.m_segments = 0;
    }
  } else if(currentLine > m_anchor.m_line) {
    if(ISBETWEEN(l, m_anchor.m_line, currentLine)) {
      if(l == m_anchor.m_line) {
        result.m_c1 = m_anchor.m_column - topLeft.m_column;
        result.m_c2 = m_winSize.cx + 1;

        result.m_segments = ((result.m_c1>0              ) ? HAS_LEFTSEGMENT   : 0)
                          | ((result.m_c2>result.m_c1    ) ? HAS_MIDDLESEGMENT : 0);
        // no RIGHTSEGMENT
      } else if(l == currentLine) {
        result.m_c1 = 0;
        result.m_c2 = currentCol - topLeft.m_column;

        result.m_segments = ((result.m_c2>0              ) ? HAS_MIDDLESEGMENT : 0)
                          | ((result.m_c2<m_winSize.cx+1 ) ? HAS_RIGHTSEGMENT  : 0);

        // no LEFTSEGMENT
      } else { // mark whole line
        result.m_c1 = 0;
        result.m_c2 = m_winSize.cx + 1;
        result.m_segments =                                  HAS_MIDDLESEGMENT;
      }
    } else {
      result.m_segments   = 0;
    }
  } else { // currentLine < m_anchor.m_line
    if(ISBETWEEN(l, currentLine, m_anchor.m_line)) {
      if(l == currentLine) {
        result.m_c1 = currentCol - topLeft.m_column;
        result.m_c2 = m_winSize.cx + 1;

        result.m_segments = ((result.m_c1>0              ) ? HAS_LEFTSEGMENT   : 0)
                          | ((result.m_c2>result.m_c1    ) ? HAS_MIDDLESEGMENT : 0);
        // no RIGHTSEGMENT
      } else if(l == m_anchor.m_line) {
        result.m_c1 = 0;
        result.m_c2 = m_anchor.m_column - topLeft.m_column;

        result.m_segments = ((result.m_c2>0              ) ? HAS_MIDDLESEGMENT : 0)
                          | ((result.m_c2<m_winSize.cx+1 ) ? HAS_RIGHTSEGMENT  : 0);
        // no LEFTSEGMENT
      } else { // mark whole line
        result.m_c1 = 0;
        result.m_c2 = m_winSize.cx + 1;
        result.m_segments =                                  HAS_MIDDLESEGMENT;
      }
    } else {
      result.m_segments   = 0;
    }
  }
  return result;
}

void TextView::paintTextSegment(CDC *pDC, int x, int y, int length, COLORREF tc, COLORREF bc, const TCHAR *s, int *textEndX) {
  const TextPosition &topLeft = getTopLeft();
  const int tx = m_characterSize.cx*(m_margin+x);

  if(x + topLeft.m_column < (int)_tcsclen(s)) {
    TCHAR tmp[4096];
    _stprintf(tmp, _T("%-*.*s"), length, length, s + topLeft.m_column + x);
    setColor(pDC, tc, bc);
    pDC->TextOut(tx, y*m_characterSize.cy, tmp, length);
    if(textEndX) *textEndX = tx + getTextExtent(*pDC, tmp).cx;
  } else {
    if(textEndX) *textEndX = tx;
  }
}

void TextView::paintTextLine(CDC *pDC, int y, COLORREF tc, COLORREF bc, const TCHAR *s, const LineSegments &lineSeg) {
  int length;

  int      textEnd;
  COLORREF backgroundColorAfterText;

  if(lineSeg.m_segments == 0) {
    backgroundColorAfterText = bc;
    length = m_winSize.cx + 1;
    paintTextSegment(pDC, 0, y, length, tc, bc, s, &textEnd);
  } else {
//    pDC->MoveTo(m_characterSize.cx*m_margin, y*m_characterSize.cy);
//    pDC->SetTextAlign(TA_LEFT | TA_UPDATECP);
    backgroundColorAfterText = BLUE;
    if(lineSeg.m_segments & HAS_LEFTSEGMENT) {
      length = lineSeg.m_c1;
      paintTextSegment(pDC, 0, y, length, tc, bc, s, (lineSeg.m_segments == HAS_LEFTSEGMENT) ? &textEnd : NULL);
    }
    if(lineSeg.m_segments & HAS_MIDDLESEGMENT) {
      const int left = max(0, lineSeg.m_c1);
      length         = max(0, lineSeg.m_c2 - left);
      paintTextSegment(pDC, left, y, length, WHITE, BLUE, s, (lineSeg.m_segments & HAS_RIGHTSEGMENT) ? NULL : &textEnd);
    }
    if(lineSeg.m_segments & HAS_RIGHTSEGMENT) {
      const int left = max(0, lineSeg.m_c2);
      length         = max(0, m_winSize.cx + 1 - left);
      paintTextSegment(pDC, left, y, length, tc, bc, s, &textEnd);
      backgroundColorAfterText = bc;
    }
//    pDC->SetTextAlign(TA_LEFT | TA_NOUPDATECP);
  }

  const int width = m_workSize.cx - textEnd;
  if(width > 0) {
    pDC->FillSolidRect(textEnd, y*m_characterSize.cy, width, m_characterSize.cy, backgroundColorAfterText);
  }
}

void TextView::paintTextLine(CDC *pDC, int y) {
  const int line = getTopLine() + y;

  if(line >= getLineCount()) {
    return;
  }

  const DiffLine    &df      = m_diff->getDiffLine(line);
  const int          id      = getId();
  const LineSegments lineSeg = getLineSegments(line);

  switch(df.getAttr()) {
  case EQUALLINES   :
    if(getHighLightCompareEqual() && !df.linesAreEqual()) {
      paintTextLine(pDC, y, BLACK, LIGHTGREY, df.getText(id).getString(), lineSeg);
    } else {
      paintTextLine(pDC, y, BLACK, WHITE    , df.getText(id).getString(), lineSeg);
    }
    break;

  case CHANGEDLINES :
    paintTextLine(  pDC, y, RED  , GREY     , df.getText(id).getString(), lineSeg);
    break;

  case DELETEDLINES :
    if(id == 1) {
      paintTextLine(pDC, y, GREY, GREY      , _T("")                    , lineSeg);
    } else {
      paintTextLine(pDC, y, BLUE, GREY      , df.getText(id).getString(), lineSeg);
    }
    break;

  case INSERTEDLINES:
    if(id == 1) {
      paintTextLine(pDC, y, GREEN,GREY      , df.getText(id).getString(), lineSeg);
    } else {
      paintTextLine(pDC, y, GREY, GREY      , _T("")                    , lineSeg);
    }
    break;
  }
}

// ----------------------------- String functions ------------------------------------

const TCHAR *TextView::getString(int index) const {
  if(index < 0 || index >= m_diff->getLineCount()) {
    return _T("");
  }
  return m_diff->getDiffLine(index).getText(getId()).getString();
}

const TCHAR *TextView::getCurrentString(int offset) const {
  return getString(getCurrentLine() + offset);
}

int TextView::getFirstNonSpacePosition() const {
  const TCHAR *str = getCurrentString();
  for(const TCHAR *cp = str; *cp; cp++) {
    if(!_istspace(*cp)) {
      return (int)(cp - str);
    }
  }
  return 0;
}

class OrigDiffFilter : public DiffFilter { // original doc-filtering but no lineFilter
private:
  DiffFilter &m_f;
public:
  OrigDiffFilter(DiffFilter &f) : m_f(f) {};
  String lineFilter(const TCHAR *s) const { return s;                } // no filter
  String docFilter( const TCHAR *s, CompareJob *job) const {
    return m_f.docFilter(s, job);
  } // inherit original document-filtering
  bool   hasLineFilter() const {
    return false;
  }
  bool   hasDocFilter()  const {
    return m_f.hasDocFilter();
  }
};

String TextView::getOrigString(int index) {
  LineArray s;
  OrigDiffFilter filter(getDocument()->m_filter);
  m_diff->getDoc(getId()).getLines(filter,s, NULL);
  return s[index];
}

String TextView::getCurrentOrigString(int offset) {
  int index = getCurrentLine() + offset;
  if(index >= m_diff->getLineCount() || index < 0) {
    return _T("");
  }
  const DiffLine &dl = m_diff->getDiffLine(index);
  const int l = dl.getText(getId()).getIndex();
  return (l < 0) ? _T("") : getOrigString(l);
}

static bool inline isWordLetter(_TUCHAR ch) {
  return _istalpha(ch) || (ch == _T('_'));
}

String getWord(const String &str, UINT pos) {
  if(pos >= str.length()) {
    return _T("");
  }
  int start = pos, end = pos;

  const _TUCHAR *s = (_TUCHAR*)str.cstr();
  if(isWordLetter(s[pos])) {
    while(start > 0 && isWordLetter(s[start-1])) {
      start--;
    }
    while(isWordLetter(s[end+1])) {
      end++;
    }
  } else if(_istdigit(s[pos])) {
    while(start > 0 && _istdigit(s[start-1])) {
      start--;
    }
    while(_istdigit(s[end+1])) {
      end++;
    }
  } else if(s[pos] == _T('#')) {
    while(start > 0 && s[start-1] == _T('#')) {
      start--;
    }
    while(s[end+1] == _T('#')) {
      end++;
    }
  } else if(_istspace(s[pos])) {
    return _T("");
  }
  return substr(str, start, end-start+1);
}

String TextView::getCurrentWord() {
  String line = getCurrentString();
  return getWord(line, getCurrentColumn());
}

int TextView::getCurrentLineLength(int offset) {
  return (int)_tcsclen(getCurrentString(offset));;
}

TCHAR TextView::getCurrentChar() {
  const String &s = getCurrentString();
  size_t x = getCurrentColumn();
  if(x < s.length()) {
    return s[x];
  } else {
    return 0;
  }
}

int TextView::searchNextWordPos() {
  _TUCHAR ch = getCurrentChar();
  const String &s = getCurrentString();
  size_t i = getCurrentColumn();
  if(i >= s.length()) {
    return (int)s.length();
  }
  if(isSpace(ch)) {
    while(i < s.length() && isSpace(s[i])) {
      i++;
    }
  } else if(isAlnum(ch)) {
    while(i < s.length() && isAlnum((_TUCHAR)s[i])) {
      i++;
    }
  } else if(_istcntrl(ch)) {
    while(i < s.length() && _istcntrl((_TUCHAR)s[i])) {
      i++;
    }
  } else if(_istpunct(ch)) {
    while(i < s.length() && _istpunct((_TUCHAR)s[i])) {
      i++;
    }
  } else if(i < s.length()) {
    i++;
  }
  while(i < s.length() && isSpace(s[i])) {
    i++;
  }
  return (int)i;
}

int TextView::searchPrevWordPos() {
  const String &s = getCurrentString();
  int i = getCurrentColumn();
  if(i >= (int)s.length()) {
    i = (int)s.length();
  }
  while(i > 0 && isSpace(s[i-1])) {
    i--;
  }
  if(i == 0) {
    return i;
  }
  const _TUCHAR ch = s[i-1];

  if(isAlnum(ch)) {
    while(i > 0 && isAlnum((_TUCHAR)s[i-1])) {
      i--;
    }
  } else if(_istcntrl(ch)) {
    while(i > 0 && _istcntrl((_TUCHAR)s[i-1])) {
      i--;
    }
  } else if(_istpunct(ch)) {
    while(i > 0 && _istpunct((_TUCHAR)s[i-1])) {
      i--;
    }
  } else if(i > 0) {
    i--;
  }
  return i;
}

// --------------------------------- Navigation function ------------------------------

void TextView::setTopLeft(const TextPosition &pos) {
  setTopLine(   pos.m_line);
  setLineOffset(pos.m_column);
}

void TextView::handleAnchor() {
  getDiffView().handleAnchor();
}

bool TextView::adjustLineOffset() {
  const int cl = min(getPreferredColumn(), getCurrentLineLength());
  if(cl < getLineOffset()) {
    setLineOffset(cl-1);
    return true;
  } else if(cl > getRightOffset()) {
    setLineOffset(cl - m_winSize.cx);
    return true;
  }
  return false;
}

void TextView::home() {
  handleAnchor();
  const int firstNonSpace = getFirstNonSpacePosition();
  if(getCurrentColumn() == firstNonSpace) {
    if(firstNonSpace == 0) {
      return;
    }
    setPreferredColumn(0);
  } else {
    setPreferredColumn(firstNonSpace);
  }
  if(adjustLineOffset()) {
    repaintAll();
  } else {
    displayCaret();
  }
}

void TextView::ctrlHome() {
  handleAnchor();
  if(getTopLeft() != TextPosition(0,0)) {
    m_state.init();
    repaintAll();
  } else {
    setPreferredColumn(0);
    setCaretY(0);
    displayCaret();
  }
}

void TextView::end() {
  handleAnchor();
  setPreferredColumn(getCurrentLineLength());
  if(getPreferredColumn() - m_winSize.cx > getLineOffset()) {
    setLineOffset(getPreferredColumn() - m_winSize.cx);
    repaintAll();
  } else {
    displayCaret();
  }
}

void TextView::ctrlEnd() {
  handleAnchor();
  if(getTopLine() != getMaxTopLine()) {
    setTopLeft(TextPosition(getMaxTopLine(), 0));
    setPreferredColumn(0);
    setCaretY(getMaxCaretY());
    repaintAll();
  } else {
    setCaretY(getMaxCaretY());
    displayCaret();
  }
}

void TextView::lineUp() {
  handleAnchor();
  if(getCaretPosition().y > 0) {
    setCaretY(getCaretPosition().y-1);
    if(adjustLineOffset()) {
      repaintAll();
    } else {
      displayCaret();
    }
  } else if(getTopLine() > 0) {
    setTopLine(getTopLine()-1);
    adjustLineOffset();
    repaintAll();
  }
}

void TextView::lineDown() {
  handleAnchor();
  if(getCaretPosition().y < getMaxCaretY()) {
    setCaretY(getCaretPosition().y+1);
    if(adjustLineOffset()) {
      repaintAll();
    } else {
      displayCaret();
    }
  } else if(getTopLine() < getMaxTopLine()) {
    setTopLine(getTopLine()+1);
    adjustLineOffset();
    repaintAll();
  }
}

void TextView::pageUp() {
  handleAnchor();
  if(getTopLine() > 0) {
    setTopLine(getTopLine() - m_winSize.cy);
    adjustLineOffset();
    repaintAll();
  } else {
    if(getCaretPosition().y > 0) {
      setCaretY(0);
      if(adjustLineOffset()) {
        repaintAll();
      } else {
        displayCaret();
      }
    }
  }
}

void TextView::pageDown() {
  handleAnchor();
  if(getTopLine() < getMaxTopLine()) {
    setTopLine(getTopLine() + m_winSize.cy);
    adjustLineOffset();
    repaintAll();
  } else {
    if(getCaretPosition().y < getMaxCaretY()) {
      setCaretY(getMaxCaretY());
      if(adjustLineOffset()) {
        repaintAll();
      } else {
        displayCaret();
      }
    }
  }
}

void TextView::charLeft() {
  handleAnchor();
  const CPoint &caret = getCaretPosition();
  if(caret.x > 0) {
    setPreferredColumn(getCurrentColumn() - 1);
    displayCaret();
  } else if(getLineOffset() > 0) {
    setLineOffset(getLineOffset()-1);
    decrPreferredColumn();
    repaintAll();
  } else if(caret.y > 0 || getTopLine() > 0) {
    setPreferredColumn(getCurrentLineLength(-1));
    lineUp();
  }
}

void TextView::charRight() {
  handleAnchor();
  const CPoint &caret = getCaretPosition();
  if(getCurrentColumn() < getCurrentLineLength()) {
    if(caret.x < m_winSize.cx) {
      setPreferredColumn(getCurrentColumn() + 1);
      displayCaret();
    } else if(getLineOffset() < getMaxLineOffset()) {
      setLineOffset(getLineOffset()+1);
      incrPreferredColumn();
      repaintAll();
    }
  } else if(getCurrentLine() < getLineCount()) {
    setPreferredColumn(0);
    lineDown();
  }
}

void TextView::ctrlCharLeft() {
  handleAnchor();
  if(getCurrentColumn() == 0) {
    if(getCurrentLine() == 0) {
      return;
    }
    charLeft();
    displayCaret();
  }
  setPreferredColumn(searchPrevWordPos());
  if(getPreferredColumn() >= getLineOffset()) {
    displayCaret();
  } else {
    setLineOffset(getPreferredColumn());
    repaintAll();
  }
}

void TextView::ctrlCharRight() {
  handleAnchor();
  if(getCurrentColumn() >= getCurrentLineLength()) {
    if(getCurrentLine() == getLineCount()) {
      return;
    }
    charRight();
    displayCaret();
  }

  setPreferredColumn(searchNextWordPos());
  if(getPreferredColumn() <= getRightOffset()) {
    displayCaret();
  } else if(getPreferredColumn() != getRightOffset()) {
    setLineOffset(getPreferredColumn() - m_winSize.cx);
    repaintAll();
  }
}

void TextView::redrawBoth() {
  TextView *partner = getPartner();
  CDC *dc  = GetDC();
  CDC *pdc = partner ? partner->GetDC() : NULL;

  try { 
    if(!partner) {
      OnDraw(dc);
    } else if(isActive()) {
      OnDraw(dc);
      partner->OnDraw(pdc);
    } else {
      partner->OnDraw(pdc);
      OnDraw(dc);
    }
    ReleaseDC(dc);
    if(pdc) ReleaseDC(pdc);
  } catch(...) {
    ReleaseDC(dc);
    if(pdc) ReleaseDC(pdc);
    throw;
  }

}

void TextView::setCaret(const MousePosition &p) {
  setCaretY(p.y);
  setPreferredColumn(minMax(p.x + getLineOffset(), 0, getCurrentLineLength()));
}

void TextView::scrollDown(int count) {
  setTopLine(getTopLine() + count);

  TextView *partner = getPartner();
  if(partner) {
    partner->setTopLine(getTopLine());
  }
  redrawBoth();
}

void TextView::scrollUp(int count) {
  setTopLine(getTopLine() - count);

  TextView *partner = getPartner();
  if(partner) {
    partner->setTopLine(getTopLine());
  }
  redrawBoth();
}

void TextView::scrollLeft(int count) {
  setLineOffset(getLineOffset() - count);
  TextView *partner = getPartner();
  if(partner) {
    partner->setLineOffset(getLineOffset());
  }
  redrawBoth();
}

void TextView::scrollRight(int count) {
  setLineOffset(getLineOffset() + count);
  TextView *partner = getPartner();
  if(partner) {
    partner->setLineOffset(getLineOffset());
  }
  redrawBoth();
}

int TextView::searchNextDiff() {
  size_t pos = getCurrentLine();
  const DiffLineArray &lines = m_diff->getDiffLines();
  if(pos < lines.size()) {
    if(lines[pos].getAttr() != EQUALLINES) {
      for(pos++; pos < lines.size(); pos++) {
        if(lines[pos].getAttr() == EQUALLINES) {
          break;
        }
      }
    }
  }
  size_t i;
  for(i = pos; i < lines.size(); i++) {
    if(lines[i].getAttr() != EQUALLINES) {
      break;
    }
  }
  if(i < lines.size()) {
    return (int)i;
  }
  return -1;
}

int TextView::searchPrevDiff() {
  int pos = getCurrentLine();
  const DiffLineArray &lines = m_diff->getDiffLines();
  if(pos > 0) {
    if(lines[pos].getAttr() != EQUALLINES) {
      for(pos--;pos >= 0; pos--) {
        if(lines[pos].getAttr() == EQUALLINES) {
          break;
        }
      }
    }
  }
  int i;
  for(i = pos; i >= 0; i--) {
    if(lines[i].getAttr() != EQUALLINES) {
      break;
    }
  }
  if(i == 0) {
    return i;
  }
  for(;i > 0; i--) {
    if(lines[i].getAttr() == EQUALLINES) {
      i++;
      break;
    }
  }
  return i;
}

void TextView::nextDiff() {
  resetAnchor();
  if(hasPartner()) {
    getPartner()->resetAnchor();
  }
  const int line = searchNextDiff();
  if(line < 0) {
    return;
  }
  if(line < getBottomLine()) {
    setCaretY(line - getTopLine());
    setLineOffset(0);
    setPreferredColumn(0);
    repaintAll();
  } else {
    setTopLine(line - m_winSize.cy/2);
    setCaretY(line - getTopLine());
    setLineOffset(0);
    setPreferredColumn(0);
    repaintAll();
  }
}

void TextView::prevDiff() {
  resetAnchor();
  if(hasPartner()) {
    getPartner()->resetAnchor();
  }
  const int line = searchPrevDiff();
  if(line < 0) {
    return;
  }
  if(line >= getTopLine()) {
    setCaretY(line - getTopLine());
    setLineOffset(0);
    setPreferredColumn(0);
    repaintAll();
  } else {
    setTopLine(line - m_winSize.cy/2);
    setCaretY(line - getTopLine());
    setLineOffset(0);
    setPreferredColumn(0);
    repaintAll();
  }
}

void TextView::gotoFoundPosition(const TextPositionPair &tp) {
  m_anchor = tp.m_pos2;
  m_lastAnchor.reset();
  gotoPos(tp.m_pos1);
}

void TextView::gotoLine(UINT line) {
  gotoPos(TextPosition(line, 0));
}

void TextView::gotoPos(const TextPosition &pos) {
  bool repaint = false;
  if((pos.m_line < getTopLine()) || (pos.m_line >= getBottomLine())) {
    setTopLine(pos.m_line - m_winSize.cy/2);
    repaint = true;
  }

  setCaretY(pos.m_line - getTopLine());
  setPreferredColumn(pos.m_column);
  const int textr = max(m_anchor.m_column, getPreferredColumn());
  if(getPreferredColumn() < getLineOffset() || textr >= getRightOffset()) {
    if(textr - getPreferredColumn() > m_winSize.cx) {
      setLineOffset(getPreferredColumn());
    } else {
      setLineOffset(min(getPreferredColumn() - m_winSize.cx/2,textr - m_winSize.cx));
    }
    repaint = true;
  }
  if(repaint) {
    TextView *partner = getPartner();
    if(partner) {
      partner->setLineOffset(getLineOffset());
    }
    redrawBoth();
  } else {
    displayCaret();
  }
}

// -------------------------------------- misc ------------------------------------

CWinDiffView &TextView::getDiffView() {
  return *((CWinDiffView*)GetParent());
}

TextView *TextView::getPartner() {
  CWinDiffView *view = getDiffView().getPartner();
  return (view == NULL) ? NULL : &view->m_textView;
}

CWinDiffDoc *TextView::getDocument() {
  return getDiffView().GetDocument();
}

bool TextView::hasPartner() {
  return getPartner() != NULL;
}

int TextView::getMaxCaretX() {
  const int cl = getCurrentLineLength();
  if(cl > getRightOffset()) {
    return m_winSize.cx;
  } else {
    return cl - getLineOffset();
  }
}

int TextView::getMaxCaretY() {
  if(getLineCount() > getBottomLine()) {
    return m_winSize.cy - 1;
  } else {
    return getLineCount() - getTopLine() - 1;
  }
}

void TextView::activatePartner() {
  TextView *partner = getPartner();
  if(partner == NULL) {
    return;
  }
  setActive(false);
  partner->setActive(true);
}

void TextView::setActive(bool active) {
  if(!isCreated()) return;
  setFlagValue(VIEW_ISACTIVE, active);
  if(active) {
    displayCaret();
  } else {
    destroyCaret();
  }
}

String TextView::getTextLines(const TextPositionPair &tp) {
  int id = getId();
  String res;
  if(tp.m_pos1.m_line == tp.m_pos2.m_line) {
    res = substr(getString(tp.m_pos1.m_line),tp.m_pos1.m_column,tp.m_pos2.m_column-tp.m_pos1.m_column);
  } else {
    res = getString(tp.m_pos1.m_line);
    res = substr(res,tp.m_pos1.m_column,res.length());
    int y;
    for(y = tp.m_pos1.m_line + 1; y < tp.m_pos2.m_line; y++) {
      res += String(_T("\r\n")) + getString(y);
    }
    if(y == tp.m_pos2.m_line) {
      res += String(_T("\r\n")) + left(getString(y),tp.m_pos2.m_column);
    }
  }
  return res;
}

String TextView::getSelectedText() {
  if(m_anchor.isEmpty()) {
    return _T("");
  }
  return getTextLines(getSelectedRange());
}

TextPositionPair TextView::getSelectedRange() const {
  if(m_anchor.isEmpty()) {
    return TextPositionPair();
  } else {
    const TextPosition cp = getCurrentPos();
    return (cp <= m_anchor) ? TextPositionPair(cp, m_anchor) : TextPositionPair(m_anchor, cp);
  }
}

bool TextView::hasSelection() const {
  const TextPositionPair tp = getSelectedRange();
  return !tp.isEmpty() && tp.m_pos1 != tp.m_pos2;
}

TextPosition TextView::getTextPosFromScreenPos(const CPoint &p) {
  CRect rect;
  GetWindowRect(&rect);
  if(!rect.PtInRect(p)) {
    return TextPosition();
  }
  MousePosition       m       = getCaretPosFromScreenPos(p);
  const TextPosition &topLeft = getTopLeft();
  return TextPosition(m.y + topLeft.m_line, m.x + topLeft.m_column);
}

MousePosition TextView::getCaretPosFromScreenPos(const CPoint &p) {
  CRect rect;
  GetWindowRect(&rect);
  int cx = (p.x - rect.left) / m_characterSize.cx - m_margin;
  int cy = (p.y - rect.top ) / m_characterSize.cy;
  return MousePosition(CPoint(cx,cy));
}

int TextView::getProgressiveDistance(int distance) {
  int sign = 1;
  if(distance < 0) {
    sign = -1;
    distance = -distance;
  }
  if(distance < 20) {
    distance = 1;
  } else if(distance < 40) {
    distance = 5;
  } else if(distance < 60) {
    distance = 10;
  } else {
    distance = 50;
  }
  return sign * distance;
}

MousePosition TextView::getNearestCaretPos(const CPoint &p) {
  CRect rect;
  GetWindowRect(&rect);
  if(rect.PtInRect(p)) {
    return getCaretPosFromScreenPos(p);
  }
  CPoint newP;
  CPoint distance;
  if(p.x < rect.left) {
    distance.x = getProgressiveDistance(p.x - rect.left);
    newP.x = rect.left;
  } else if(p.x > rect.right-1) {
    distance.x = getProgressiveDistance(p.x - rect.right);
    newP.x = rect.right-1;
  } else {
    distance.x = 0;
    newP.x = p.x;
  }
  if(p.y < rect.top) {
    distance.y = getProgressiveDistance(p.y - rect.top);
    newP.y = rect.top;
  } else if(p.y > rect.bottom-1) {
    distance.y = getProgressiveDistance(p.y - rect.bottom);
    newP.y = rect.bottom-1;
  } else {
    distance.y = 0;
    newP.y = p.y;
  }
  return MousePosition(getCaretPosFromScreenPos(newP),distance);
}

void TextView::dropAnchor() {
  if(m_anchor.isEmpty()) {
    setAnchor();
  }
}

void TextView::resetAnchor() {
  if(!m_anchor.isEmpty()) {
    m_anchor.reset();
    CClientDC dc(this);
    unMarkSelectedText(&dc);
  }
}

void TextView::selectAll() {
  if(m_diff->isEmpty()) {
    return;
  }
  ctrlEnd();
  const DiffLine &df = m_diff->getDiffLines().last();
  const int len = (int)_tcsclen(df.getText(getId()).getString());
  m_anchor.set(getLineCount()-1, len);
  gotoPos(TextPosition(0,0));
}

void TextView::createWorkBitmap(int w, int h) {
  if(m_workBitmap.m_hObject != NULL) {
    m_workDC.SelectObject((CBitmap*)NULL);
    m_workBitmap.DeleteObject();
  }

  CClientDC screen(this);
  m_workSize.cx = w; m_workSize.cy = h;
  const int depth = screen.GetDeviceCaps(BITSPIXEL);
  m_workBitmap.CreateBitmap(w,h,screen.GetDeviceCaps(PLANES),depth,NULL);
  m_workDC.SelectObject(&m_workBitmap);
  clearDC(&m_workDC);
}

void TextView::updateWindowSize(int cx, int cy) {
  CDC *pDC = GetDC();
  SELECTFONT(*pDC);
//  pDC->SetMapMode(MM_TEXT);

  TEXTMETRIC tm;
  pDC->GetTextMetrics(&tm);

  ReleaseDC(pDC);

  m_characterSize.cy = tm.tmHeight;
  m_characterSize.cx = tm.tmAveCharWidth; // tmMaxCharWidth;

  setCaretSize(CSize(max(2, m_characterSize.cy / 8), m_characterSize.cy));

  CSize aSize;
  aSize.cy = m_characterSize.cy * 4 / 5;
  aSize.cx = aSize.cy * 80 / 96; // size of raw Arrowbitmap = (80,96)
  setArrowSize(aSize);

  m_winSize.cx = (cx-m_caretSize.cx) / m_characterSize.cx - m_margin; // -4 to make caret always visible

  if(m_winSize.cx < 0) {
    m_winSize.cx = 0;
  }

  m_winSize.cy = cy / m_characterSize.cy;

  m_maxOffset.m_column = max(0, m_diff->getMaxLineLength() - m_winSize.cx);
  m_maxOffset.m_line   = max(0, getLineCount() - m_winSize.cy);
  
  setTopLine(getTopLine());
  setCaretY(getCaretPosition().y);

  createWorkBitmap(cx, cy);
}

void TextView::updateWindowSize() {
  const CSize sz = getClientRect(this).Size();
  updateWindowSize(sz.cx, sz.cy);
}

bool TextView::attributeMatch(const FindParameters &param, int index) const {
  if(param.m_diffOnly) {
    return getLineAttribute(index) != EQUALLINES;
  } else if(param.m_nonDiffOnly) {
    return getLineAttribute(index) == EQUALLINES;
  } else {
    return true;
  }
}

// --------------------------------- debug functions -----------------------------------

#ifdef _DEBUG

String LineSegments ::toString() const {
  if(m_segments) {
    return format(_T("c1:%3d, c2:%3d, %s"), m_c1, m_c2, flagsToString(m_segments).cstr());
  } else {
    return _T("empty");
  }
}

String LineSegments::flagsToString(unsigned char flags) { // static
  String result;
  const TCHAR *delim = NULL;

#define ADDIFSET(f) { if(flags&HAS_##f) { if(delim) result += delim; else delim = _T(" | "); result += #f; } }

  ADDIFSET( LEFTSEGMENT   );
  ADDIFSET( MIDDLESEGMENT );
  ADDIFSET( RIGHTSEGMENT  );
  return result;
}
#undef ADDIFSET

String ViewFlags::toString() const {
  String result;
  const TCHAR *delim = NULL;

#define ADDIFSET(f) { if(contains(f)) { if(delim) result += delim; else delim = _T(" | "); result += #f; } }

  ADDIFSET( VIEW_ISCREATED         );
  ADDIFSET( VIEW_ISACTIVE          );
  ADDIFSET( CARET_EXIST            );
  ADDIFSET( CARET_VISIBLE          );
  ADDIFSET( SHOW_1000SEPARATOR     );
  ADDIFSET( HIGHLIGhT_COMPAREEQUAL );
  return result;
}

#endif
