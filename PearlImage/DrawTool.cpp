#include "stdafx.h"
#include "DrawTool.h"

void DrawTool::copy() {
  PixRect *pr = m_container->getImage();

  HBITMAP bitmap = *pr;
  putClipboard(AfxGetMainWnd()->m_hWnd,bitmap);
  DeleteObject(bitmap);
}
