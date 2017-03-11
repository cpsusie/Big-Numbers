#include "stdafx.h"
#include "DrawTool.h"
#include "TextDlg.h"

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

TextTool::TextTool(PixRectContainer *container, bool bezier, FontParameters &fontParameters, String &text) : DrawTool(container), m_fontParameters(fontParameters), m_text(text) {
  m_bezier = bezier;
}

void TextTool::OnLButtonDown(UINT nFlags, const CPoint &point) {
  CTextDlg dlg(m_fontParameters,m_text);
  if(dlg.DoModal() == IDOK) {
    m_container->saveDocState();
    m_text           = dlg.m_text;
    m_fontParameters = dlg.m_fontParameters;
    PixRectFont font(theApp.m_device, m_fontParameters.m_logFont,m_fontParameters.m_orientation);
    if(m_bezier) {
      getImage()->drawText(point,(LPCTSTR)dlg.m_text,font,m_container->getColor());
    } else {
      getImage()->text(point,(LPCTSTR)dlg.m_text,font,m_container->getColor());
    }
    repaint();
  }
}
