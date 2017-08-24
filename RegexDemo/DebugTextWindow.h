#pragma once

#include <PropertyContainer.h>

class CDebugTextWindow : public CListBox, public PropertyChangeListener {
private:
  CBitmap     m_lineMarkBitmap , m_breakPointBitmap, m_acceptMarkBitmap;
  CSize       m_arrowBitmapSize, m_breakPointBitmapSize;
  String      m_codeText;
  StringArray m_textLines;
  int         m_currentLine, m_lastAcceptLine;
  int         m_itemHeight;
  BitSet      m_breakPoints, m_possibleBreakPointLines;
  bool        m_hasFocus        : 1;
  bool        m_allowMarking    : 1;
  bool        m_blinkersVisible : 1;

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
  void unmarkCurrentLine();
  void markLastAcceptLine(int line);
  void unmarkLastAcceptLine();
  bool hasLastAcceptLine() const {
    return m_lastAcceptLine >= 0;
  }
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
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
  virtual ~CDebugTextWindow();

protected:
  afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  afx_msg void OnSetfocus();
  afx_msg void OnKillfocus();

  DECLARE_MESSAGE_MAP()
};
