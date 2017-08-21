#pragma once

#include <Timer.h>

class CharacterMarker : TimeoutHandler {
private:
  CDialog     *m_dlg;
  const int    m_ctrlId;
  const bool   m_above             : 1;
  bool         m_multiMarksAllowed : 1;
  bool         m_marksVisible      : 1;
  COLORREF     m_backgroundColor;
  CBitmap      m_markBitmap;
  CSize        m_bitmapSize;
  UINT         m_charWidth;
  size_t       m_textLength;
  CRect        m_ctrlRect;
  BitSet       m_markSet;
  Semaphore    m_gate;
  Timer       *m_blinkTimer;

  void   handleTimeout(Timer &timer);
  void   setMarksVisible(bool visible);
  void   paintAllMarkPositions(bool mark);
  void   paintMarkPosition(CDC &dc, size_t index, bool mark);
  void   saveCtrlRect();
  void   saveTextLength();
  CPoint getMarkPosition(size_t index) const;
public:
  CharacterMarker(CDialog *dlg, int ctrlId, int bitmapId, bool above);
  ~CharacterMarker();
  void setMultiMarksAllowed(bool multiMarksAllowed);
  inline bool isMultiMarksAllowed() const {
    return m_multiMarksAllowed;
  }
  void setBlinking(bool on, int msec = 500);
  bool isBlinking() const {
    return m_blinkTimer && m_blinkTimer->isRunning();
  }
  void unmarkAll();
  void setMarks(const BitSet &s);
  void setMark(size_t index);
};

class CharacterMarkerArray : public CompactArray<CharacterMarker*> {
public:
  ~CharacterMarkerArray() {
    clear();
  }
  void add(CharacterMarker *m) {
    TRACE_NEW(m);
    __super::add(m);
  }
  void clear() {
    for(size_t i = 0; i < size(); i++) {
      SAFEDELETE((*this)[i]);
    }
    __super::clear();
  }
};
