#include "stdafx.h"
#include "EnterAddressDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHexViewView, CView)

BEGIN_MESSAGE_MAP(CHexViewView, CView)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
    ON_WM_DROPFILES()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CHexViewView::CHexViewView() : m_scrollBarHelper(DoubleInterval(0,1), DoubleInterval(0,1)) {
  m_docSize              = 0;
  m_topLine              = 0;
  m_lineOffset           = 0;
  m_caret                = m_maxCaret = CPoint(0,0);
  m_minDigitIndex        = m_maxDigitIndex = 0;
  m_pageSize             = CSize(0,0);
  m_lineSize             = 0;
  m_lineCount            = 0;
  m_maxTopLine           = 0;
  m_maxLineOffset        = 0;
  m_useScrollBarHelper   = false;
  m_reverseVideo         = false;
  m_asciiColor           = false;
  m_caretVisible         = 0;
  m_shift                = false;
  m_keepSelection        = false;
  m_readOnly             = true;
  m_settings             = &getSettings();
  resetAnchor();
  resetDigitIndex();
  m_font.CreateFont(10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                   ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                   ,DEFAULT_PITCH | FF_MODERN
                   ,_T("Courier") );
}

CHexViewView::~CHexViewView() {
  destroyWorkDC();
}

void CHexViewView::OnInitialUpdate() {
  DragAcceptFiles();
  CHexViewDoc *doc       = GetDocument();
  CMainFrame  *mainFrame = getMainFrame();
  mainFrame->ajourMenuItems();
  CView::OnInitialUpdate();
}

void CHexViewView::OnDraw(CDC* pDC) {
  CFont           *oldFont  = m_workDC.SelectObject(&m_font);

  m_workDC.FillSolidRect(0, 0, m_workRect.Width(), m_workRect.Height(), m_settings->getDataBackColor());
  draw(&m_workDC);
  pDC->BitBlt(0, 0, m_workRect.Width(), m_workRect.Height(), &m_workDC, 0, 0, SRCCOPY);
  m_workDC.SelectObject(oldFont);
  CMainFrame *mainFrame = getMainFrame();
  mainFrame->updateTitle();
  mainFrame->ajourMenuItems();
}

void CHexViewView::draw(CDC *dc) {
  CHexViewDoc *doc = GetDocument();
  ASSERT_VALID(doc);

  const CRect cr = updateSettings(dc);

  if(m_docSize > 0) {

    const bool    showAddr          = m_settings->getShowAddr();
    const bool    showAscii         = m_settings->getShowAscii();
    const __int64 indexByte0TopLine = m_topLine * m_lineSize;

    ByteArray content;
    doc->getBytes(indexByte0TopLine, m_lineSize * m_pageSize.cy, content);

    const BYTE      *buffer         = content.getData();
    const AddrRange  selection    = getSelection();

    if(showAddr) { // paint headline with offset from leftmost byte address
      dc->FillSolidRect(0, 0, cr.Width(), m_addrTextSize.cy, m_settings->getAddrBackColor());
      setAddrColor(dc, true);
      for(int col = 0, xPos = m_contentRect.left; col < m_lineSize; col++, xPos += m_byteSize.cx) {
        if(xPos > m_contentRect.right) {
          break;
        }
        dc->TextOut(xPos, 0, m_addrFormat.offsetToString(col).cstr());
      }
      setAddrColor(dc, false);
    }

#define OFFSET_LEFTMOSTBYTE(row) ((row)*m_lineSize + m_lineOffset)

    const BYTE *lastByte = &content[content.size()-1];
    setReverseVideo(dc, false);
    for(int row = 0, yPos = m_contentRect.top; yPos <= m_contentRect.bottom; row++, yPos += m_byteSize.cy) {
      const BYTE *bp = buffer + OFFSET_LEFTMOSTBYTE(row);
      if(bp > lastByte) {
        break;
      }
      if(showAddr) {
        setAddrColor(dc, true);
        dc->TextOut(0,yPos, getAddrAsString(indexByte0TopLine + OFFSET_LEFTMOSTBYTE(row)).cstr());
        setAddrColor(dc, false);
      }
      __int64 addr = indexByte0TopLine + OFFSET_LEFTMOSTBYTE(row);
      for(int col = 0, xPos = m_contentRect.left; (col < m_lineSize) && (xPos <= m_contentRect.right) && (bp <= lastByte); col++, xPos += m_byteSize.cx, bp++, addr++) {
        if(!selection.isEmpty()) {
          if(selection.contains(addr)) {
            if(!m_reverseVideo) {
              setReverseVideo(dc, true);
            }
          } else if(m_reverseVideo) {
            setReverseVideo(dc, false);
          }
        }

        TCHAR tmp[40];
        if(showAscii && isprint(*bp)) {
          if(!m_asciiColor) {
            setAsciiColor(dc, true);
          }
          _stprintf(tmp, m_asciiFormat, *bp);
        } else {
          if(m_asciiColor) {
            setAsciiColor(dc, false);
          }
          _stprintf(tmp, m_radixFormat, *bp);
        }
        dc->TextOut(xPos, yPos, tmp);
      }
    }
  }

  showCaret();

  if(m_maxTopLine == 0) { // all bytes shown. hide scrollbar
    ShowScrollBar(SB_VERT, FALSE);
  } else {
    // dont use SetScrollPos as it is only 16-bit int
    ShowScrollBar(SB_VERT, TRUE );

    updateVerticalScrollBar();
  }

  if(m_docSize == 0 || m_maxLineOffset == 0) {
    ShowScrollBar(SB_HORZ, FALSE);
  } else {
    ShowScrollBar(SB_HORZ, TRUE );
    SCROLLINFO scrollInfo;
    GetScrollInfo(SB_HORZ, &scrollInfo);
    scrollInfo.nMin  = 0;
    scrollInfo.nMax  = m_lineSize - 1;
    scrollInfo.nPos  = m_lineOffset;
    scrollInfo.nPage = m_pageSize.cx;
    SetScrollInfo(SB_HORZ, &scrollInfo);
  }
}

CRect CHexViewView::updateSettings(CDC *dc) {
  CRect cr;
  GetClientRect(&cr);
  CFont *oldFont = NULL;

  if(dc == NULL) {
    dc = &m_workDC;
    oldFont = dc->SelectObject(&m_font);
  }

  m_readOnly    = GetDocument()->isReadOnly();
  m_radixFormat = m_settings->getRadixFormat();
  m_asciiFormat = m_settings->getAsciiFormat();
  m_byteSize    = dc->GetTextExtent(m_settings->getDataSampleText());
  m_charSize    = dc->GetTextExtent(_T("0"));
  m_docSize     = GetDocument()->getSize();

  m_addrFormat.update(m_docSize, *m_settings);

  const bool showAddr           = m_settings->getShowAddr();
  const bool showAscii          = m_settings->getShowAscii();

  if(showAddr) {
    const String sampleText     = m_addrFormat.getSampleText();

    m_addrTextSize              = getTextExtent(*dc,sampleText);
  } else {
    m_addrTextSize = CSize(0,0);
  }
  m_contentRect.left            = m_addrTextSize.cx;
  m_contentRect.right           = cr.right  - m_byteSize.cx;
  m_contentRect.top             = m_addrTextSize.cy;
  m_contentRect.bottom          = cr.bottom - m_byteSize.cy;

  m_pageSize          = CSize((m_contentRect.Width()-4) / m_byteSize.cx + 1, m_contentRect.Height()/m_byteSize.cy);

  if(m_settings->getFitLinesToWindow()) {
    m_lineSize        = m_contentRect.Width()/m_byteSize.cx + 1; // Use windowsize to determine the number of bytes on each line
  } else {
    m_lineSize        = m_settings->getLineSize();
  }

  m_lineSize          = max(m_lineSize, 1);   // just to prevent division by zero. see below
  m_maxLineOffset     = max(m_lineSize - m_pageSize.cx, 0);
  m_lineOffset        = min(m_lineOffset, m_maxLineOffset);
  m_lineCount         = (m_docSize - 1)/m_lineSize + 1;
  m_maxTopLine        = max(m_lineCount - m_pageSize.cy, 0);
  m_topLine           = min(m_topLine, m_maxTopLine);
  m_maxCaret          = CPoint(m_pageSize.cx, m_pageSize.cy - 1);
  m_maxCaret.x        = minMax(m_maxCaret.x, (LONG)0, (LONG)min(m_docSize, m_lineSize));
  if(m_maxCaret.y >= m_lineCount) {
    m_maxCaret.y = (int)m_lineCount - 1;
  }
  if (m_maxCaret.y < 0) {
    m_maxCaret.y = 0;
  }
  m_lastLineSize      = m_docSize % m_lineSize;

  if(m_lastLineSize == 0) {
    m_lastLineSize = m_lineSize;
  }
  m_maxLastLineOffset = min(m_lastLineSize - 1, m_maxLineOffset);

  if(m_readOnly || m_settings->getShowAscii()) {  // readonly or ascii => only 0 is valid
    m_minDigitIndex = m_maxDigitIndex = 0;
  } else if((m_settings->getDataRadix() != 16)) { // dec or oct => [0..2]
    m_minDigitIndex = 0;
    m_maxDigitIndex = 2;
  } else  if(m_settings->getHex3Pos()) {          // hex 3-pos  => [1..2]
    m_minDigitIndex = 1;
    m_maxDigitIndex = 2;
  } else {                                        // hex 2-pos  => [0..1]
    m_minDigitIndex = 0;
    m_maxDigitIndex = 1;
  }

  m_digitIndex = minMax(m_digitIndex, m_minDigitIndex, m_maxDigitIndex);

  if(dc == &m_workDC) {
    dc->SelectObject(oldFont);
  }
  return cr;
}

void CHexViewView::initScrollBarHelper() {
  if(m_lineCount < INT_MAX) {
    m_useScrollBarHelper = false;
  } else { // use LinearTransformation to make lineRange fit into [0..INT_MAX]
    m_useScrollBarHelper = true;
    m_scrollBarHelper.setFromInterval(DoubleInterval(0,(double)m_lineCount-1));
    m_scrollBarHelper.setToInterval(DoubleInterval(0, INT_MAX));
  }
}

void CHexViewView::updateVerticalScrollBar() {
  SCROLLINFO scrollInfo;

  GetScrollInfo(SB_VERT, &scrollInfo);
  if(!m_useScrollBarHelper) {
    scrollInfo.nMin  = 0;
    scrollInfo.nMax  = (int)m_lineCount-1;
    scrollInfo.nPos  = (int)m_topLine;
    scrollInfo.nPage = m_pageSize.cy;
  }
  else {
    const DoubleInterval &range = m_scrollBarHelper.getToInterval();
    scrollInfo.nMin  = (int)range.getFrom();
    scrollInfo.nMax  = (int)range.getTo();
    scrollInfo.nPos  = (int)m_scrollBarHelper.forwardTransform((double)m_topLine);
    scrollInfo.nPage = m_pageSize.cy;
  }
  SetScrollInfo(SB_VERT, &scrollInfo);
}

__int64 CHexViewView::getTopLineFromTrackPosition() {
  SCROLLINFO scrollInfo;
  GetScrollInfo(SB_VERT,&scrollInfo);

  if(!m_useScrollBarHelper) {
    return minMax(scrollInfo.nTrackPos, 0, (int)m_maxTopLine);
  } else {
    const DoubleInterval &range = m_scrollBarHelper.getToInterval();
    const double trackPos = scrollInfo.nTrackPos;
    const double line = m_scrollBarHelper.backwardTransform(minMax(trackPos, range.getFrom(), range.getTo()));
    return (__int64)line;
  }
}

#if defined(_DEBUG)
void CHexViewView::AssertValid() const {
  CView::AssertValid();
}

void CHexViewView::Dump(CDumpContext& dc) const {
  CView::Dump(dc);
}

CHexViewDoc *CHexViewView::GetDocument() { // non-debug version is inline
  ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CHexViewDoc)));
  return (CHexViewDoc*)m_pDocument;
}
#endif //_DEBUG


void CHexViewView::repaint() {
static const bool erase = false;
  Invalidate(erase);
}

void CHexViewView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  m_shift = shiftKeyPressed();
  switch(nSBCode) {
  case SB_TOP           :
    setTopLine(0, false).setCaretY(0);
    break;
  case SB_BOTTOM        :
    setTopLine(m_maxTopLine, false).setCaretY(m_maxCaret.y);
    break;
  case SB_ENDSCROLL     :                    break;
  case SB_LINEUP        : scrollVert(-1);    break;
  case SB_LINEDOWN      : scrollVert( 1);    break;
  case SB_PAGEUP        : pageUp(m_shift);   break;
  case SB_PAGEDOWN      : pageDown(m_shift); break;
  case SB_THUMBPOSITION :                    break;
  case SB_THUMBTRACK    :
    { // Dont use parameter nPos as it is only 16-bits int
      SCROLLINFO scrollInfo;
      GetScrollInfo(SB_VERT,&scrollInfo);
      if(scrollInfo.nTrackPos >= 0 && scrollInfo.nTrackPos <= m_maxTopLine) {
        setTopLine(scrollInfo.nTrackPos);
      }
    }
    break;
  }
}

void CHexViewView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  m_shift = shiftKeyPressed();
  switch(nSBCode) {
  case SB_TOP           :
    setLineOffset(0, false).setCaretX(0);
    break;
  case SB_BOTTOM        :
    setLineOffset(m_maxLineOffset, false).setCaretX(m_maxCaret.x);
    break;
  case SB_ENDSCROLL     :                     break;
  case SB_LINELEFT      : scrollHorz(-1);     break;
  case SB_LINERIGHT     : scrollHorz( 1);     break;
  case SB_PAGELEFT      : pageLeft(m_shift);  break;
  case SB_PAGERIGHT     : pageRight(m_shift); break;
  case SB_THUMBPOSITION :                     break;
  case SB_THUMBTRACK    :
    { // Dont use parameter nPos as it is only 16-bits int
      SCROLLINFO scrollInfo;
      GetScrollInfo(SB_HORZ,&scrollInfo);
      if(scrollInfo.nTrackPos >= 0 && scrollInfo.nTrackPos <= m_maxLineOffset) {
        setLineOffset(scrollInfo.nTrackPos);
      }
    }
    break;
  }
}

void CHexViewView::charLeft(bool shift) {
  m_shift = shift;
  if(m_caret.x > 0) {
    setCaretX(m_caret.x-1, false);
  } else if(m_lineOffset > 0) {
    scrollHorz(-1);
  } else if(getSettings().getWrapEndOfLine() && getCurrentAddr() > 0) {
    if(m_caret.y > 0) {
      if(m_lineOffset < m_maxLineOffset) {
        setLineOffset(m_maxLineOffset, false).setCaret(m_maxCaret.x, m_caret.y-1);
      } else {
        setCaret(m_maxCaret.x, m_caret.y-1, false);
      }
    } else {
      setTopLine(m_topLine-1, false).setLineOffset(m_maxLineOffset, false).setCaretX(m_maxCaret.x);
    }
  }
}

void CHexViewView::charRight(bool shift) {
  m_shift = shift;
  if(m_caret.x < m_maxCaret.x) {
    setCaretX(m_caret.x+1, false);
  } else if(m_lineOffset < m_maxLineOffset) {
    scrollHorz(1);
  } else if(getSettings().getWrapEndOfLine() && getCurrentAddr() < m_docSize - 1) {
    if(m_caret.y < m_maxCaret.y) {
      if(m_lineOffset > 0) {
        setLineOffset(0, false).setCaret(0, m_caret.y+1);
      } else {
        setCaret(0, m_caret.y+1, false);
      }
    } else {
      setTopLine(m_topLine+1, false).setLineOffset(0, false).setCaretX(0);
    }
  }
}

void CHexViewView::ctrlCharLeft(bool shift) {
  m_shift = shift;
  if(m_digitIndex > m_minDigitIndex) {
    setDigitIndex(m_digitIndex-1).showCaret();
  } else {
    const __int64 addr = getCurrentAddr();
    charLeft(shift);
    if(getCurrentAddr() != addr) {
      setDigitIndex(m_maxDigitIndex).showCaret();
    }
  }
}

void CHexViewView::ctrlCharRight(bool shift) {
  m_shift = shift;
  if(m_digitIndex < m_maxDigitIndex && m_caret.x < getMaxCaretX()) {
    setDigitIndex(m_digitIndex+1).showCaret();
  } else {
    charRight(shift);
  }
}

void CHexViewView::lineUp(bool shift) {
  m_shift = shift;
  if(m_caret.y > 0) {
    setCaretY(m_caret.y-1, false);
  } else if(m_topLine > 0) {
    scrollVert(-1);
  }
}

void CHexViewView::lineDown(bool shift) {
  m_shift = shift;
  if(m_caret.y < m_maxCaret.y) {
    setCaretY(m_caret.y+1, false);
  } else if(m_topLine < m_maxTopLine) {
    scrollVert(1);
  }
}

void CHexViewView::ctrlHome(bool shift) {
  m_shift = shift;
  if(m_topLine || m_lineOffset) {
    setTopLine(0, false).setLineOffset(0, false).setCaret(0,0);
  } else {
    setCaret(0,0, false);
  }
}

void CHexViewView::ctrlEnd(bool shift) {
  m_shift = shift;
  if((m_topLine < m_maxTopLine) || (m_lineOffset < m_maxLineOffset)) {
    setTopLine(m_maxTopLine, false).setLineOffset(m_maxLineOffset, false).setCaret(m_maxCaret.x, m_maxCaret.y);
  } else {
    setCaret(m_maxCaret.x, m_maxCaret.y, false);
  }
}

void CHexViewView::home(bool shift) {
  m_shift = shift;
  if(m_lineOffset > 0 || m_caret.x > 0) {
    setLineOffset(0, false).setCaretX(0);
  } else {
    setCaretX(0, false);
  }
}

void CHexViewView::end(bool shift) {
  m_shift = shift;
  if(m_lineOffset < m_maxLineOffset) {
    setLineOffset(m_maxLineOffset, false).setCaretX(m_maxCaret.x);
  } else {
    setCaretX(m_maxCaret.x, false);
  }
}

void CHexViewView::pageLeft(bool shift) {
  m_shift = shift;
  if(m_lineOffset > 0) {
    scrollVert(-m_pageSize.cx);
  } else if(m_caret.x > 0) {
    setCaretX(0, false);
  }
}

void CHexViewView::pageRight(bool shift) {
  m_shift = shift;
  if(m_lineOffset < m_maxLineOffset) {
    scrollHorz(m_pageSize.cx);
  } else if(m_caret.x < m_maxCaret.x) {
    setCaretX(m_maxCaret.x, false);
  }
}

void CHexViewView::pageUp(bool shift) {
  m_shift = shift;
  if(m_topLine > 0) {
    scrollVert(-m_pageSize.cy);
  } else if(m_caret.y > 0) {
    setCaretY(0, false);
  }
}

void CHexViewView::pageDown(bool shift) {
  m_shift = shift;
  if(m_topLine < m_maxTopLine) {
    scrollVert(m_pageSize.cy);
  } else if(m_caret.y < m_maxCaret.y) {
    setCaretY(m_maxCaret.y, false);
  }
}

#define MOUSEPOS2CARETPOS(p) CPoint((p.x + m_byteSize.cx/2 - m_addrTextSize.cx) / m_byteSize.cx, (p.y - m_addrTextSize.cy) / m_byteSize.cy)
#define CARETPOS2ADDR(cp)    ((m_topLine + cp.y) * m_lineSize + m_lineOffset + cp.x)

bool CHexViewView::isValidCaretPos(const CPoint &cp) const {
  return cp.x >= 0 && cp.x <= m_maxCaret.x && cp.y >= 0 && cp.y <= m_maxCaret.y;
}

__int64 CHexViewView::getFileAddrFromPoint(const CPoint &p) const {
  const CPoint cp = MOUSEPOS2CARETPOS(p);
  if(!isValidCaretPos(cp)) {
    return -1;
  }
  const __int64 addr = CARETPOS2ADDR(cp);
  if(addr < 0) {
    return -1;
  } else if(addr > (__int64)m_docSize) {
    return m_docSize;
  }
  return addr;
}

void CHexViewView::OnLButtonDown(UINT nFlags, CPoint point) {
  const __int64 addr = getFileAddrFromPoint(point);
  if(addr >= 0) {
    dropAnchor(addr);
    setCurrentAddr(addr);
  }
  CView::OnLButtonDown(nFlags, point);
}

void CHexViewView::OnMouseMove(UINT nFlags, CPoint point) {
  if((nFlags & MK_LBUTTON) && hasAnchor()) {
    const __int64 addr = getFileAddrFromPoint(point);
    if(addr >= 0) {
      setCurrentAddr(addr);
    }
  }
  CView::OnMouseMove(nFlags, point);
}

BOOL CHexViewView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  m_shift = (nFlags & MK_SHIFT) != 0;
  if(nFlags & MK_CONTROL) {
    if(zDelta > 0) {
      scrollHorz(-4);
    } else if(zDelta < 0) {
      scrollHorz(4);
    }
  } else {
    if(zDelta > 0) {
      scrollVert(-4);
    } else if(zDelta < 0) {
      scrollVert(4);
    }
  }
  return CView::OnMouseWheel(nFlags, zDelta, pt);
}

#define GETLINENO(      addr)    ((addr) / m_lineSize)
#define GETLINEOFFSET(  addr)    ((addr) % m_lineSize)
#define ISVISIBLELINE(  line)   (((line)   >= m_topLine)    && ((line)   < m_topLine    + m_pageSize.cy))
#define ISVISIBLEOFFSET(offset) (((offset) >= m_lineOffset) && ((offset) < m_lineOffset + m_pageSize.cx))

void CHexViewView::setCurrentAddr(unsigned __int64 addr, bool invalidate) {
  if(addr > m_docSize) {
    return;
  }

  keepSelection();
  if(addr != getCurrentAddr() && hasAnchor()) {
    invalidate = true;
  }
  const __int64 newTopLine = getBestTopLine(   addr);
  const int     newOffset  = getBestLineOffset(addr);
  if((newTopLine != m_topLine) || (newOffset != m_lineOffset)) {
    setTopLine(newTopLine, false).setLineOffset(newOffset, false);
    invalidate = true;
  }
  setCaret(GETLINEOFFSET(addr) - m_lineOffset, (int)(GETLINENO(addr) - m_topLine), invalidate);
}

unsigned __int64 CHexViewView::getCurrentAddr() const {    // Address of current position in the file
  return (m_topLine + m_caret.y) * m_lineSize + m_lineOffset + m_caret.x;
}

String CHexViewView::getAddrAsString(__int64 addr) const {
  return m_addrFormat.addrToString(addr);
}

__int64 CHexViewView::getBestTopLine(__int64 addr) const {
  const __int64 line1  = GETLINENO(addr);
  const __int64 addr2  = hasAnchor() ? getAnchor() : addr;
  const __int64 line2  = GETLINENO(addr2);

  if(ISVISIBLELINE(line1) && ISVISIBLELINE(line2)) {
    return m_topLine; // no need to do vertical scroll
  }
  const __int64 selectedLineCount = (addr == addr2) ? 0 : (abs(line1 - line2) + 1);
  if(selectedLineCount <= m_pageSize.cy) {
    return min(line1, line2) - (m_pageSize.cy - selectedLineCount)/2;
  } else {
    return min(line1, line2);
  }
}

int CHexViewView::getBestLineOffset(__int64 addr) const {
  const __int64 offset1 = GETLINEOFFSET(addr);
  const __int64 addr2   = hasAnchor() ? getAnchor() : addr;
  const __int64 offset2 = GETLINEOFFSET(addr2);

  if(ISVISIBLEOFFSET(offset1) && ISVISIBLEOFFSET(offset2)) {
    return m_lineOffset; // no need to do horizontal scroll
  }
  const __int64 line1 = GETLINENO(addr );
  const __int64 line2 = GETLINENO(addr2);
  if(line1 == line2) {
    const int length = (int)abs(addr - addr2);
    if(length <= m_pageSize.cx) {
      return (int)(min(offset1, offset2) - (m_pageSize.cx - length)/2);
    } else {
      return (int)min(offset1, offset2);
    }
  } else {
    return (int)min(offset1, offset2);
  }
}

CHexViewView &CHexViewView::setTopLine(__int64 value, bool invalidate) {
  m_topLine = minMax(value, 0i64, m_maxTopLine);
  resetDigitIndex();
  if(invalidate) {
    repaint();
  }
  return *this;
}

CHexViewView &CHexViewView::setLineOffset(int value, bool invalidate) {
  m_lineOffset = minMax(value, 0, m_maxLineOffset);
  resetDigitIndex();
  if(invalidate) {
    repaint();
  }
  return *this;
}

CHexViewView &CHexViewView::setCaret(int x, int y, bool invalidate) {
  m_caret.x = x;
  m_caret.y = y;
  resetDigitIndex();
  if(invalidate) {
    repaint();
  } else {
    showCaret();
  }
  return *this;
}

int CHexViewView::getMaxCaretX() const {
  return isOnLastLine() ? max(0, m_lastLineSize - m_lineOffset) : max(0, m_lineSize - m_lineOffset);
}

bool CHexViewView::isOnLastLine() const {
  return m_topLine + m_caret.y >= m_lineCount-1;
}

CHexViewView &CHexViewView::resetDigitIndex() {
  m_digitIndex = m_minDigitIndex;
  return *this;
}

CHexViewView &CHexViewView::setDigitIndex(char index) {
  m_digitIndex = minMax(index, m_minDigitIndex, m_maxDigitIndex);
  return *this;
}

#define CARETHEIGHT 13

void CHexViewView::showCaret() {
  if(!m_caretVisible) {
    CreateSolidCaret(2, CARETHEIGHT);
    ShowCaret();
    m_caretVisible = true;
  }
  m_caret.y = minMax(m_caret.y, 0l, m_maxCaret.y);
  m_caret.x = minMax(m_caret.x, 0l, m_maxCaret.x);

  bool redraw = false;
  if(m_lineCount && (m_lastLineSize < m_lineSize) && (m_topLine + m_caret.y == m_lineCount-1)) {
    if(m_lineOffset > m_maxLastLineOffset) {
      m_lineOffset = m_maxLastLineOffset;
      isNewSelection();
      redraw = true;
    } else {
      m_caret.x = minMax((int)m_caret.x, 0, m_lastLineSize - m_lineOffset);
      redraw = isNewSelection();
    }
  } else {
    redraw = isNewSelection();
  }
  m_lastCurrentAddr = getCurrentAddr();
  m_lastSelection   = getSelection();

  if(redraw) {
    repaint();
    return;
  }

  CPoint p(m_caret.x * m_byteSize.cx + m_contentRect.left + m_digitIndex*m_charSize.cx, m_caret.y * m_byteSize.cy + m_contentRect.top);
  SetCaretPos(p);
  m_keepSelection = false;
/*
  const String tmp = format(_T("%s bytes. Addr:%s. Lsize:%d[%d]. Lcount:%I64u, Psize(%d,%d), Scroll(%d,%I64u),max(%d,%I64u)[%d], Caret(%d,%d),max:(%d,%d), Anchor:%I64d, Mark:%s, %s")
                          ,format1000(m_docSize       ).cstr()
                          ,format1000(getCurrentAddr()).cstr()
                          ,m_lineSize, m_lastLineSize
                          ,m_lineCount
                          ,m_pageSize.cx, m_pageSize.cy
                          ,m_lineOffset , m_topLine      , m_maxLineOffset, m_maxTopLine, m_maxLastLineOffset
                          ,m_caret.x    , m_caret.y      , m_maxCaret.x   , m_maxCaret.y
                          ,m_anchor
                          ,getSelection().toString().cstr()
                          ,m_shift ? _T("shift") : EMPTYSTRING);

  theApp.GetMainWnd()->SetWindowText(tmp.cstr());
*/
}

void CHexViewView::hideCaret() {
  if(m_caretVisible) {
    DestroyCaret();
    m_caretVisible = false;
  }
}

bool CHexViewView::isNewSelection() {
  if(m_keepSelection) {
    return false;
  }
  if(m_shift) {
    if(!hasAnchor()) {
      dropAnchor(m_lastCurrentAddr);
      return true;
    }
  } else { // !m_shift
    if(hasAnchor()) {
      resetAnchor();
      return true;
    }
  }
  return getSelection() != m_lastSelection;
}

bool CHexViewView::resetAnchor() {
  const bool ret = hasAnchor();
  m_anchor = -1;
  return ret;
}

bool CHexViewView::dropAnchor(unsigned __int64 index) {
  if(index > m_docSize) {
    showWarning(_T("dropAnchor:index=%I64u, docSize=%I64d"), index, m_docSize);
    return false;
  }
  const bool ret = index != m_anchor;
  m_anchor = index;
  return ret;
}

unsigned __int64 CHexViewView::getAnchor() const {
  return hasAnchor() ? m_anchor : getCurrentAddr();
}

AddrRange CHexViewView::getSelection() const {
  return hasAnchor() ? AddrRange(m_anchor, getCurrentAddr()) : AddrRange();
}

void CHexViewView::setAddrColor(CDC *dc, bool on) {
  if(on) {
    dc->SetTextColor(m_settings->getAddrTextColor());
    dc->SetBkColor(  m_settings->getAddrBackColor());
  } else {
    setReverseVideo(dc, m_reverseVideo);
  }
}

void CHexViewView::setAsciiColor(CDC *dc, bool on) {
  m_asciiColor = on;
  setReverseVideo(dc, m_reverseVideo);
}

void CHexViewView::setReverseVideo(CDC *dc, bool on) {
  const COLORREF textColor = m_asciiColor ? m_settings->getAsciiTextColor() : m_settings->getDataTextColor();
  const COLORREF backColor = m_asciiColor ? m_settings->getAsciiBackColor() : m_settings->getDataBackColor();
  if(on) {
    dc->SetTextColor(backColor);
    dc->SetBkColor(  textColor);
  } else {
    dc->SetTextColor(textColor);
    dc->SetBkColor(  backColor);
  }
  m_reverseVideo = on;
}

void CHexViewView::OnSize(UINT nType, int cx, int cy) {
  CView::OnSize(nType, cx, cy);
  createWorkDC(getClientRect(this).Size());
  keepSelection().repaint();
}

void CHexViewView::OnDropFiles(HDROP hDropInfo) {
  if(!getMainFrame()->checkSave()) {
    return;
  }
  TCHAR fname[256];
  DragQueryFile(hDropInfo, 0, fname, ARRAYSIZE(fname));
  getMainFrame()->newFile(fname, true);
  theApp.addToRecentFileList(fname);
  CView::OnDropFiles(hDropInfo);
}

void CHexViewView::createWorkDC(const CSize &size) {
  destroyWorkDC();

  CClientDC screen(this);
  m_workRect.left  = 0; m_workRect.top    = 0;
  m_workRect.right = size.cx; m_workRect.bottom = size.cy;
  m_workDC.CreateCompatibleDC(&screen);
  m_workBitmap.CreateBitmap(size.cx, size.cy, screen.GetDeviceCaps(PLANES), screen.GetDeviceCaps(BITSPIXEL), NULL);
  m_workBitmap.SetBitmapDimension(size.cx, size.cy);
  m_workDC.SelectObject(m_workBitmap);
}

void CHexViewView::destroyWorkDC() {
  if(m_workDC.m_hDC != NULL) {
    m_workDC.DeleteDC();
  }
  if(m_workBitmap.m_hObject != NULL) {
    m_workBitmap.DeleteObject();
  }
}

static int bytePow10(int n) {
  switch(n) {
  case 0 : return 1;
  case 1 : return 10;
  case 2 : return 100;
  default: throwException(_T("bytePow10:n=%d"), n);
  }
  return 0;
}

BYTE CHexViewView::makeNewByte(BYTE oldByte, unsigned char ch) const {
  const Settings &settings = getSettings();
  const int      len       = settings.getByteLength();
  const int      bytePart  = len - 1 - m_digitIndex;
  UINT result;
  switch(settings.getDataRadix()) {
  case 8 :
    { const int shift = bytePart*3;
      result = (oldByte & ~(7   << shift))              | (settings.charToByte(ch) << shift);
    }
    break;
  case 10:
    { const int e10 = bytePow10(bytePart);
      result = (oldByte - ((oldByte / e10) % 10) * e10) + (settings.charToByte(ch) * e10);
    }
    break;
  case 16:
    { const int shift = bytePart * 4;
      result = (oldByte & ~(0xf << shift))              | (settings.charToByte(ch) << shift);
    }
    break;
  }
  if(result > 255) {
    throwException(_T("Value (decimal %d) becomes to big to fit in a byte"), result);
  }
  return result;
}

void CHexViewView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  if(!m_readOnly && isCaretOnByte()) {
    CHexViewDoc         *doc      = GetDocument();
    const Settings      &settings = getSettings();
    const unsigned char asciiChar = toAscii(nChar);
    if(settings.getShowAscii()) {
      if(isprint(asciiChar)) {
        if(doc->setByte(getCurrentAddr(), asciiChar)) {
          charRight(false);
          repaint();
        } else {
          charRight(false);
        }
      }
    } else if(Settings::isValidRadixChar(asciiChar, settings.getDataRadix())) {
      try {
        const unsigned __int64 addr = getCurrentAddr();
        doc->setByte(addr, makeNewByte(doc->getByte(addr), asciiChar));
        repaint();
        ctrlCharRight(false);
      } catch(Exception e) {
        // ignore
      }
    }
  }
  CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
