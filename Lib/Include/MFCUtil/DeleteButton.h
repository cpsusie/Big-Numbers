#pragma once

#include <afxext.h>
#include "WinTools.h"

class CDeleteButton : public CBitmapButton {
private:
  void loadBitmap(CBitmap &bm, int id);
public:
  void SetPosition(int x, int y, bool redraw = true);
  void Create(CWnd *parent, const CPoint &pos, UINT id, bool tabStop=false);
};
