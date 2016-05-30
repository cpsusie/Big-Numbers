#pragma once

#include <afxext.h>
#include "WinTools.h"

class OBMButton : public CBitmapButton {
private:
  void loadBitmap(CBitmap &bm, int id);
public:
  void SetPosition(int x, int y, bool redraw = true);
  void Create(CWnd  *parent, int res, int resD, int resI, const CPoint &pos, UINT id, bool tabStop=false);
};

#define OBMIMAGE(id) OBM_##id, OBM_##id##D, OBM_##id##I

#define OBM_ZOOMI    0
#define OBM_REDUCEI  0
#define OBM_RESTOREI 0

/* Use OBMIMAGE together with predeined bitmaps:
  DNARROW
 ,LFARROW
 ,RGARROW
 ,UPARROW
 ,ZOOM
 ,REDUCE
 ,RESTORE
*/
