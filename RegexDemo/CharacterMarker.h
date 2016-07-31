#pragma once

class CharacterMarker {
private:
  CDialog     *m_dlg;
  const int    m_ctrlId;
  const bool   m_above;
  bool         m_multiMarksAllowed;
  COLORREF     m_backgroundColor;
  CBitmap      m_markBitmap;
  CSize        m_bitmapSize;
  UINT         m_charWidth;
  size_t       m_textLength;
  CRect        m_ctrlRect;
  intptr_t     m_currentCharMark;
  BitSet       m_markSet;

  void   markPosition(CDC &dc, size_t index, bool mark);
  void   unmarkCurrentChar();
  void   saveCtrlRect();
  void   saveTextLength();
  CPoint getMarkPosition(size_t index) const;
public:
  CharacterMarker(CDialog *dlg, int ctrlId, int bitmapId, bool above);
  void setMultiMarksAllowed(bool multiMarksAllowed);
  inline bool isMultiMarksAllowed() const {
    return m_multiMarksAllowed;
  }

  void unmarkAll();
  void setMarks(const BitSet &s);
  void setMark(size_t index);
};

