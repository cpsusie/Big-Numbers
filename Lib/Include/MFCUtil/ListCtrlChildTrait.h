#pragma once

#include "EditListCtrl.h"

inline String toString(const ListCell &cell) {
  return format(_T("(%2d,%2d)"), cell.y,cell.x);
}

#define USE_LISTCTRL_ACCESS_METHODS                                                             \
  inline CEditListCtrl *getListCtrl()       { return (CEditListCtrl*)GetParent();             } \
  inline ListCell       getCurrentCell()    { return getListCtrl()->getCurrentCell();         } \
  inline String         getCellStr()        { return toString(getCurrentCell()).cstr();       } \
  inline const CPoint  &getLastMouseClick() { return getListCtrl()->getLastMouseClickPoint(); }
