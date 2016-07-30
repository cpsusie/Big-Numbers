#include "stdafx.h"
#include "PositionState.h"

void PositionState::set(int id, const TextPosition &offset, const CPoint &caret, int preferredColumn) {
  m_id              = id;
  m_offset          = offset;
  m_preferredColumn = preferredColumn;
  m_caret           = caret;
}

void PositionState::init() {
  m_offset.m_line   = 0;
  m_offset.m_column = 0;
  m_preferredColumn = m_caret.x = m_caret.y =  0;
}

String PositionState::toString() const {
  if(isEmpty()) {
    return _T("empty");
  }
  return format(_T("Id:%d, UL(line,column):(%3d,%3d), Caret(y,x):(%2d,%2d), pref.col:%2d")
               , m_id
               , m_offset.m_line, m_offset.m_column
               , m_caret.y,       m_caret.x
               , m_preferredColumn
               );
}
