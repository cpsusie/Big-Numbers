#pragma once

#include <Math/IntervalTransformation.h>

class CMainFrame;

class CHexViewView : public CView, SettingsAccessor {
private:
  unsigned __int64      m_docSize;           // Size of document in bytes.
  __int64               m_topLine;           // Index of the first visible line in the view (= [0..m__maxTopLine])
  int                   m_lineOffset;        // Horizontal scroll offset for each line, in characters (=[0..m_maxLineOffset])
  CPoint                m_caret;             // Current caret-position in window in units
                                             // exact caret position in pixels is calculated in showCaret
  char                  m_digitIndex;        // m_digitIndex is in [m_minDigitIndex..m_maxDigitIndex].
                                             // m_minDigitindex = leftmost digit, m_maxDigitIndex = rightMost digit.

  CRect                 m_contentRect;       // Part of the clientRect, where data (not addresses) are shown, in pixels
  CSize                 m_byteSize;          // Size of 1 byte in pixels (depends on settings.m_dataRadix (and m_hex3Pos)
  CSize                 m_charSize;          // Size of 1 character in pixels (always the same)
  CSize                 m_pageSize;          // cx = Number of visible bytes on one line, cy = Number of visible lines in window
  int                   m_lineSize;          // Total number of bytes on 1 line
  int                   m_lastLineSize;      // Can be shorter than m_lineSize.
  __int64               m_lineCount;         // Number of lines in doc (depends on docSize and m_lineSize)
  __int64               m_maxTopLine;        // Max value for m_topline,
  int                   m_maxLineOffset;     // Max value for m_lineoffset
  int                   m_maxLastLineOffset; // Max value for m_lineoffset for the last line
  CPoint                m_maxCaret;          // Max caret-position in window in units
  char                  m_minDigitIndex, m_maxDigitIndex; // 0 <= m_minDigitIndex <= m_maxDigitIndex <= 2
  bool                  m_useScrollBarHelper;
  LinearTransformation  m_scrollBarHelper;
  bool                  m_reverseVideo;
  bool                  m_asciiColor;
  bool                  m_caretVisible;
  bool                  m_shift;
  bool                  m_keepSelection;
  bool                  m_readOnly;
  __int64               m_anchor;
  __int64               m_lastCurrentAddr;   // updated when view is repainted
  AddrRange             m_lastSelection;
  AddressToString       m_addrFormat;

  const TCHAR          *m_radixFormat, *m_asciiFormat;
  CSize                 m_addrTextSize;      // Size of addresses in pixels

  const Settings  *m_settings;
  CFont            m_font;
  CDC              m_workDC;
  CBitmap          m_workBitmap;
  CRect            m_workRect;
#if defined(_DEBUG)
  String           m_debugString;
#endif
  void createWorkDC(const CSize &size);
  void destroyWorkDC();
  void draw(CDC *dc);
  void updateVerticalScrollBar();
  __int64 getTopLineFromTrackPosition();
  void setReverseVideo(CDC *dc, bool on);
  void setAddrColor(   CDC *dc, bool on);
  void setAsciiColor(  CDC *dc, bool on);
  CHexViewView &setTopLine(__int64 value, bool invalidate = true);
  CHexViewView &setLineOffset(int  value, bool invalidate = true);
  bool isValidCaretPos(const CPoint &cp) const;
  __int64 getFileAddrFromPoint(const CPoint &p) const; // return -1 if not valid
  bool isNewSelection(); // Called after (m_topLine,m_lineOffset,m_caret) has been set.
                         // Should return true if marked range has changed

  BYTE makeNewByte(BYTE oldByte, unsigned char ch) const;
  CHexViewView &setDigitIndex(char index);

  void debug(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void repaint();

  void scrollVert(int count) {
    setTopLine(m_topLine + count);
  }

  void scrollHorz(int count) {
    setLineOffset(m_lineOffset + count);
  }

  int getMaxCaretX() const;
  bool isOnLastLine() const;
  bool isCaretOnByte() const {
    return m_caret.x < getMaxCaretX();
  }

  CHexViewView &setCaretX(int x, bool invalidate = true) {
    return setCaret(x, m_caret.y, invalidate);
  }

  CHexViewView &setCaretY(int y, bool invalidate = true) {
    return setCaret(m_caret.x, y, invalidate);
  }

  CHexViewView &setCaret(int x, int y, bool invalidate = true);

  __int64 getBestTopLine(   __int64 addr) const;
  int     getBestLineOffset(__int64 addr) const;

  CMainFrame *getMainFrame() {
    return (CMainFrame*)GetParent();
  }

protected:
  CHexViewView();
  DECLARE_DYNCREATE(CHexViewView)

public:
  CHexViewDoc *GetDocument();

  void charLeft(     bool shift);
  void charRight(    bool shift);
  void ctrlCharLeft( bool shift);
  void ctrlCharRight(bool shift);
  void lineUp(       bool shift);
  void lineDown(     bool shift);
  void home(         bool shift); // start-of-line
  void end(          bool shift); // end-of-line
  void ctrlHome(     bool shift); // start-of-doc
  void ctrlEnd(      bool shift); // end-of-doc
  void pageLeft(     bool shift);
  void pageRight(    bool shift);
  void pageUp(       bool shift);
  void pageDown(     bool shift);
  CRect updateSettings(CDC *dc = nullptr);
  void initScrollBarHelper();
  void setCurrentAddr(unsigned __int64 addr, bool invalidate = false);

  unsigned __int64 getCurrentAddr() const;    // Address of current position in the file

  String getCurrentAddrAsString() const {
    return getAddrAsString(getCurrentAddr());
  }

  String getAddrAsString(__int64 addr) const;
  void showCaret();
  void hideCaret();

  bool dropAnchor(unsigned __int64 index);
  bool resetAnchor();

  bool hasAnchor() const {
    return m_anchor >= 0;
  }

  unsigned __int64 getAnchor() const;

  AddrRange getSelection() const; // Return (positive) interval between anchor and current address, or empty AddrRange if no anchor is dropped

  CHexViewView &keepSelection() {
    m_keepSelection = true;
    return *this;
  }
  CHexViewView &resetDigitIndex();

#if defined(_DEBUG)
  const String &getDebugString() const {
    return m_debugString;
  }
#endif

  public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual void OnInitialUpdate();

public:
  virtual ~CHexViewView();
#if defined(_DEBUG)
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    DECLARE_MESSAGE_MAP()
};

#if !defined(_DEBUG  )// debug version in hexviewView.cpp
inline CHexViewDoc *CHexViewView::GetDocument()
   { return (CHexViewDoc*)m_pDocument; }
#endif

