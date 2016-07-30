#pragma once

#include <TinyBitSet.h>
#include "PositionState.h"
#include "FindDlg.h"

#define HAS_LEFTSEGMENT   0x01 /* set if 0    < m_c1       */
#define HAS_MIDDLESEGMENT 0x02 /* set if m_c1 < m_c2       */
#define HAS_RIGHTSEGMENT  0x04 /* set if m_c2 < winWidth+1 */

class LineSegments {
  static String flagsToString(unsigned char flags);
public:
  BYTE   m_segments; // 0 if no marking, else any combination of flags above
  int    m_c1;       // mark line from c1 to c2.
  int    m_c2;       // 0 <= c1 <= c2 <= TextView.m_winwidth + 1
  inline int getSegmentCount() const {
    return BitSet::setBitsCount[m_segments];
  }

#ifdef _DEBUG
  String toString() const;
#endif
};

typedef enum {
  VIEW_ISCREATED
 ,VIEW_ISACTIVE              // Is this view active
 ,CARET_EXIST                // Is the caret created
 ,CARET_VISIBLE              // Is caret visible
 ,SHOW_1000SEPARATOR         // Format linenumbers as 1.000.000 or 1000000
 ,HIGHLIGhT_COMPAREEQUAL
} TextViewFlags;

class ViewFlags : public TinyBitSet<unsigned char> {
public:
#ifdef _DEBUG
  String toString() const;
#endif
};

class CWinDiffView;

class TextView : public CView, public OptionsUpdater {
private:
  PositionState m_state;
  TextPosition  m_maxOffset;         // Max. value for offset in units
  CPoint        m_lastCaretPos;      // Last caret-position in units
  CSize         m_winSize;           // Size of view in units
  CSize         m_characterSize;     // Size of character in pixels
  CSize         m_caretSize;         // Size of caret
  int           m_lineCount;         // Number of lines in document(s)
  int           m_margin;            // Left margin of text in units
  TextPosition  m_anchor;            // Beginning (or end) of selected text
  TextPosition  m_lastAnchor;
  PositionState m_savedPositionState;
  ViewFlags     m_viewFlags;
  Diff         *m_diff;
  CDC           m_workDC;
  CBitmap       m_workBitmap;
  CSize         m_workSize;
  CFont         m_font;
  CBitmap       m_arrowBitmap,m_backgroundBitmap;
  CDC           m_arrowDC;
  int           m_arrowLine;            // in window units
  CSize         m_arrowSize;
  void init();
  void refreshDoc();
  void setTopLeft(const TextPosition &pos);
  void setFlagValue(TextViewFlags id, bool value);
  inline bool getFlagValue(TextViewFlags id) const {
    return m_viewFlags.contains(id);
  }
  inline const TextPosition &getTopLeft() const {
    return m_state.m_offset;
  }
  inline void setTopLine(int n) {
    m_state.m_offset.m_line = minMax(n, 0,getMaxTopLine());
  }
  inline int getTopLine() const {
    return getTopLeft().m_line;
  }
  inline int getBottomLine() const {
    return getTopLine() + m_winSize.cy;
  }
  inline int getMaxTopLine() const {
    return m_maxOffset.m_line;
  }
  inline void setLineOffset(int n) {
    m_state.m_offset.m_column = minMax(n, 0, getMaxLineOffset());
  }
  inline int getLineOffset() const {
    return getTopLeft().m_column;
  }
  inline int getRightOffset() const {
    return getLineOffset() + m_winSize.cx;
  }
  inline int getMaxLineOffset() const {
    return m_maxOffset.m_column;
  }
  inline void setPreferredColumn(int n) {
    m_state.m_preferredColumn = max(n, 0);
  }
  inline int getPreferredColumn() const {
    return m_state.m_preferredColumn;
  }
  void incrPreferredColumn() {
    setPreferredColumn(getPreferredColumn()+1);
  }
  void decrPreferredColumn() {
    setPreferredColumn(getPreferredColumn()-1);
  }

  void updateWindowSize(int cx, int cy);
  void updateWindowSize();
  void paintTextSegment(  CDC *pDC, int x, int y, int w, COLORREF tc, COLORREF bc, const TCHAR *s, int *textEndX = NULL);
  void paintTextLine(     CDC *pDC, int y,               COLORREF tc, COLORREF bc, const TCHAR *s, const LineSegments &lineSegments);
  void paintTextLine(     CDC *pDC, int y);
  void paintLineNumber(   CDC *pDC, int y, int lineno);
  void paintLineNumber(   CDC *pDC, int y);
  void paint(             CDC *pDC);
  void paintArrow(        CDC *pDC);
  void saveBackground(    CDC *pDC);
  void unpaintArrow(      CDC *pDC = NULL);
  inline void resetArrow() {
    m_arrowLine = -1;
  }
  inline bool hasArrow() const {
    return m_arrowLine >= 0;
  }
  inline int getArrowLine() const {
    return m_arrowLine;
  }
  CPoint getArrowPosition(int lineno) const;
  void setArrowSize(const CSize &size);

  inline void setCaretY(     int n) {
    m_state.m_caret.y = minMax(n, 0, getMaxCaretY());
  }

  void setCaret(const MousePosition &p);
  void setCaretSize(const CSize &size);
  void showCaret();
  void hideCaret();
  void createCaret();
  void destroyCaret();
  void displayCaret(       CDC *pDC = NULL);
public:
  inline const CPoint &getCaretPosition() const {
    return m_state.m_caret;
  }
private:
  void unMarkSelectedText( CDC *pDC);
  void clearDC(            CDC *pDC = NULL);
  void repaintAll();
  void repaintLines(       CDC *pDC, int from, int to);
  void markSelectedText(   CDC *pDC);
  void createWorkBitmap(int w, int h);
  void redrawBoth();
  LOGFONT getLogFont();
  static inline bool isAlnum(_TUCHAR ch) {
    return ::_istalnum(ch) || (ch == _T('_'));
  }

  static inline bool isSpace(_TUCHAR ch) {
    return ::_istspace(ch) || (ch == 183) || (ch == 187);
  }

  TextPosition  getTextPosFromScreenPos( const CPoint &p);
  MousePosition getCaretPosFromScreenPos(const CPoint &p);
  MousePosition getNearestCaretPos(      const CPoint &p);

  inline DiffLineAttribute getLineAttribute(int index) const {
    return m_diff->getDiffLines()[index].getAttr();
  }
  int  getProgressiveDistance(int distance);
  int  getCurrentLineLength(int offset = 0);
  TCHAR getCurrentChar();
  bool adjustLineOffset();
  int  searchNextWordPos();
  int  searchPrevWordPos();
  int  searchNextDiff();
  int  searchPrevDiff();
  void gotoPos(const TextPosition &pos);
  int  getMaxCaretX();
  int  getMaxCaretY();
  inline void setAnchor() {
    m_anchor = getCurrentPos();
  }

  void handleAnchor();
    ;
  LineSegments getLineSegments(int l) const;

  void scrollDown( int count);
  void scrollUp(   int count);
  void scrollRight(int count);
  void scrollLeft( int count);

  inline bool isCreated() const {
    return getFlagValue(VIEW_ISCREATED);
  }
  inline bool isActive() const {
    return getFlagValue(VIEW_ISACTIVE);
  }
  inline bool caretExist() const {
    return getFlagValue(CARET_EXIST);
  }
  inline bool isCaretVisible() const {
    return getFlagValue(CARET_VISIBLE);
  }
  bool getShow1000Separator() const {
    return getFlagValue(SHOW_1000SEPARATOR);
  }; 
  bool getHighLightCompareEqual() const {
    return getFlagValue(HIGHLIGhT_COMPAREEQUAL);
  }

protected:
  DECLARE_DYNCREATE(TextView)
public:
  TextView();
  void         Create(const CRect &r, CWnd *parent, Diff &diff);
  void         OnDraw(CDC *pDC);
  void         OnInitialUpdate();
  CWinDiffDoc *getDocument();
  TextView    *getPartner();
  CWinDiffView &getDiffView();

  void PostNcDestroy() {} // to override default, that calls delete this
  bool hasPartner();
  void savePositionState();
  void restorePositionState();
  
  inline void setSavedPositionState(const PositionState &state) {
    m_savedPositionState = state;
  }
  
  inline const PositionState &getSavedPositionState() const {
    return m_savedPositionState;
  }
  
  void refreshBoth();
  
  inline int getId() const {
    return m_state.m_id;
  }

  inline TextPosition getCurrentPos() const {
    return TextPosition(getCurrentLine(),getCurrentColumn());
  }
  inline const CSize &getCharacterSize() const { // in pixels
    return m_characterSize;
  }
  inline int getMargin() const { // in characters
    return m_margin;
  }
  
  String           getTextLines(const TextPositionPair &tp);
  String           getSelectedText();
  String           getFirstSelectedLine() const;
  TextPositionPair getSelectedRange() const;

  bool             hasSelection() const;

  inline int getCurrentLine() const {
    return m_state.getCurrentLine();
  }

  inline int getCurrentColumn() const {
    return m_state.getCurrentColumn();
  }

  const TCHAR *getCurrentString(int offset = 0) const;
  const TCHAR *getString(int index) const;
  int          getFirstNonSpacePosition() const; // returns index of the first nonspace charater of current string
  String       getCurrentWord();
  String       getOrigString(int index);
  String       getCurrentOrigString(int offset = 0);
  bool         attributeMatch(const FindParameters &param, int index) const;

  void nextDiff();
  void prevDiff();
  void selectAll();
  void activatePartner();
  void setActive(bool active);
  void ctrlHome();
  void ctrlEnd();
  void home();
  void end();
  void lineUp();
  void lineDown();
  void pageUp();
  void pageDown();
  void ctrlCharLeft();
  void ctrlCharRight();
  void charLeft();
  void charRight();
  bool setIgnoreCase(       bool newValue, bool repaint = true);
  bool setIgnoreWhiteSpace( bool newValue, bool repaint = true);
  bool setIgnoreComments(   bool newValue, bool repaint = true);
  bool setStripComments(    bool newValue, bool repaint = true);
  bool setIgnoreStrings(    bool newValue, bool repaint = true);
  bool setIgnoreColumns(    bool newValue, bool repaint = true);
  bool setIgnoreRegex(      bool newValue, bool repaint = true);
  bool setViewWhiteSpace(   bool newValue, bool repaint = true);
  bool setTabSize(          int  newValue, bool repaint = true);
  bool setFont(             const LOGFONT &newValue, bool repaint = true);
  bool setNameFontSizePct(  int  newValue, bool repaint = true);
  bool setShow1000Separator(bool newValue, bool repaint = true); 
  bool setHighLightCompareEqual(bool newValue, bool repaint = true);
#ifdef _DEBUG
  inline String flagValuesToString() const {
    return m_viewFlags.toString();
  }
#endif
  void gotoFoundPosition(const TextPositionPair &tp);
  void gotoLine(UINT line);
  int getLineCount() const {
    return m_lineCount;
  }
  void setOptions(const Options &options);
  void resetAnchor();
  void dropAnchor();

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP()

  friend class CWinDiffView;
};
