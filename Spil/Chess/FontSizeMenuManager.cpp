#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "ChessGraphics.h"

#define FONTSIZE_RESOURCEID(pct) ID_FONTSIZE_##pct

typedef struct {
  int m_pct;
  int m_command;
} FontSizeMenuItem;

#define FI(pct) pct, FONTSIZE_RESOURCEID(pct)

static const FontSizeMenuItem fontMenuItems[] = {
  FI(75 )
 ,FI(100)
 ,FI(125)
 ,FI(150)
 ,FI(175)
 ,FI(200)
};

void FontSizeMenuManager::setFontSize(CWnd *wnd, int pct) {
  for(int i = 0; i < ARRAYSIZE(fontMenuItems); i++) {
    checkMenuItem(wnd, fontMenuItems[i].m_command, fontMenuItems[i].m_pct == pct);
  }
}

