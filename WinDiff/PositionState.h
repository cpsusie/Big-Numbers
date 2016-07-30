#pragma once

#include "TextPosition.h"

class PositionState { // remember caret-position and scrolloffset of TextView
public:
  int          m_id;              // id of the panel
  TextPosition m_offset;          // Position of upper-left corner of visual area in units
  CPoint       m_caret;           // Current caret-position in window in units
  int          m_preferredColumn; // Current preferred column (in text) of caret in units

  inline PositionState() {
    reset();
  }

  inline void reset() {
    m_id = -1;
  }

  inline void init(int id) {
    m_id = id;
    init();
  }

  void init();

  void set(int id, const TextPosition &offset, const CPoint &caret, int preferredColumn);

  inline bool isEmpty() const {
    return m_id < 0;
  }

  inline int getCurrentLine() const {
    return m_offset.m_line + m_caret.y;
  }

  inline int getCurrentColumn() const {
    return m_offset.m_column + m_caret.x;
  }

  inline int getPreferredCaretX() const {
    return m_preferredColumn - m_offset.m_column;
  }

  String toString() const;
};

