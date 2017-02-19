#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "ChessGraphics.h"

void dtextOut(HDC hdc, int x, int y, const String &str, COLORREF backColor, COLORREF textColor) {
  SetBkMode(   hdc, OPAQUE   );
  SetTextColor(hdc, textColor);
  SetBkColor(  hdc, backColor);
  textOut(     hdc, x, y, str);
}

void dtextOut(HDC hdc, const CPoint &pos, const String &str, COLORREF backColor, COLORREF textColor) {
  dtextOut(hdc, pos.x,pos.y,str,backColor,textColor);
}

int dtextOut(HDC hdc, int x, int y, TextAlignment align, const String &str, COLORREF backColor, COLORREF textColor) {
  const int textWidth = getTextExtent(hdc, str).cx;
  switch(align) {
  case ALIGN_LEFT  :
    dtextOut(hdc, x,y,             str,backColor,textColor);
    break;

  case ALIGN_RIGHT :
    dtextOut(hdc, x-textWidth,y,   str,backColor,textColor);
    break;

  case ALIGN_CENTER:
    dtextOut(hdc, x-textWidth/2,y, str,backColor,textColor);
    break;
  }
  return textWidth;
}

int dtextOut(HDC hdc, const CPoint &pos, TextAlignment align, const String &str, COLORREF backColor, COLORREF textColor) {
  return dtextOut(hdc, pos.x,pos.y,align,str,backColor,textColor);
}

void ColoredTextFields::print(HDC hdc, int x, int y, TextAlignment align) const {
  const size_t n = size();
  switch(align) {
  case ALIGN_LEFT  :
    { for(size_t i = 0; i < n; i++) {
        const ColoredText &str = (*this)[i];
        x += dtextOut(hdc, x,y,align, str, str.m_backColor,str.m_textColor);
      }
    }
    break;

  case ALIGN_RIGHT :
    { for(int i = (int)n; i-- >= 0;) {
        const ColoredText &str = (*this)[i];
        x -= dtextOut(hdc,x,y,align,str,str.m_backColor,str.m_textColor);
      }
    }
    break;

  case ALIGN_CENTER:
    { String resultStr;
      for(size_t i = 0; i < n; i++) {
        resultStr += (*this)[i];
      }
      const int textWidth = getTextExtent(hdc, resultStr).cx;
      x -= textWidth/2;
      for(size_t i = 0; i < n; i++) {
        const ColoredText &str = (*this)[i];
        x += dtextOut(hdc,x,y,ALIGN_LEFT,str,str.m_backColor,str.m_textColor);
      }
    }
    break;
  }
}
