#include "stdafx.h"
#include "DrawTool.h"

FontParameters::FontParameters() {
  m_orientation = 0;
  CFont f;
  f.CreateFont( 10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                         DEFAULT_PITCH | FF_MODERN,
                         _T("Times new roman")
                        );
  f.GetLogFont(&m_logFont);
  f.DeleteObject();
}
