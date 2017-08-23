#pragma once

#include <PropertyContainer.h>
#include <Semaphore.h>

class CharacterMarker {
private:
  CDialog     *m_dlg;
  const int    m_ctrlId;
  const bool   m_above             : 1;
  bool         m_multiMarksAllowed : 1;
  bool         m_marksVisible      : 1;
  bool         m_blinking          : 1;
  COLORREF     m_backgroundColor;
  CBitmap      m_markBitmap;
  CSize        m_bitmapSize;
  UINT         m_charWidth;
  size_t       m_textLength;
  CRect        m_ctrlRect;
  BitSet       m_markSet;
  Semaphore    m_gate;

  void   paintAllMarkPositions(bool mark);
  void   paintMarkPosition(CDC &dc, size_t index, bool mark);
  void   saveCtrlRect();
  void   saveTextLength();
  CPoint getMarkPosition(size_t index) const;
public:
  CharacterMarker(CDialog *dlg, int ctrlId, int bitmapId, bool above);
  void setMultiMarksAllowed(bool multiMarksAllowed);
  inline bool isMultiMarksAllowed() const {
    return m_multiMarksAllowed;
  }
  void setBlinking(bool on);
  inline bool isBlinking() const {
    return m_blinking;
  }
  void setMarksVisible(bool visible);
  void unmarkAll();
  void setMarks(const BitSet &s);
  void setMark(size_t index);
};

class CharacterMarkerArray : public CompactArray<CharacterMarker*>, public PropertyChangeListener {
private:
  Semaphore m_gate;
public:
  ~CharacterMarkerArray() {
    clear();
  }
  void add(CharacterMarker *m);
  void clear();
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
};
