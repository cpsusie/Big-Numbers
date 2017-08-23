#pragma once

class CDebugTextWindow : public CListBox {
private:
  CBitmap     m_lineMarkBitmap , m_breakPointBitmap, m_acceptMarkBitmap;
  CSize       m_arrowBitmapSize, m_breakPointBitmapSize;
  String      m_codeText;
  StringArray m_textLines;
  int         m_currentLine, m_lastAcceptLine;
  BitSet      m_breakPoints, m_possibleBreakPointLines;
  bool        m_hasFocus;
  int         m_itemHeight;
  bool        m_allowMarking;

  void redrawItem(UINT index);
  bool isItemVisible(int index);
  void setItemVisible(int index);
public:
  CDebugTextWindow();
  void substituteControl(CWnd *parent, int id);
  void clear();
  void setText(const String &text);
  const String &getText() const {
    return m_codeText;
  }
  void markCurrentLine(int line);
  void markLastAcceptLine(int line);
  void setAllowMarking(bool allowMarking, const BitSet *possibleBreakPointLines = NULL);
  void addBreakPoint(UINT line);
  void removeBreakPoint(UINT line);
  void scrollToBottom();
  const BitSet &getBreakPoints() const {
    return m_breakPoints;
  }
  bool isBreakPointLine(UINT line) const {
    return m_breakPoints.contains(line);
  }
  // return l so l = max(0..line) and m_possibleBreakPointLines.contains(l). -1 if none exist
  int  getHighestBreakPointLine(int line) const;
public:
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  virtual ~CDebugTextWindow();

protected:
  afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  afx_msg void OnSetfocus();
  afx_msg void OnKillfocus();

  DECLARE_MESSAGE_MAP()
};
