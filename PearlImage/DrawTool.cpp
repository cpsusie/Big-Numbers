#include "stdafx.h"
#include "DrawTool.h"

void DrawTool::copy() {
  PixRect *pr = m_container->getPixRect();

  HBITMAP bitmap = *pr;
  putClipboard(AfxGetMainWnd()->m_hWnd,bitmap);
  DeleteObject(bitmap);
}
