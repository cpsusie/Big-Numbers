#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "ChessGraphics.h"

void dtextOut(HDC dc, int x, int y, const String &str, COLORREF backColor, COLORREF textColor) {
  SetTextColor(dc, textColor);
  SetBkColor(  dc, backColor);
  TextOut(     dc, x,y, str.cstr(),str.length());
}

void dtextOut(HDC dc, const CPoint &pos, const String &str, COLORREF backColor, COLORREF textColor) {
  dtextOut(dc, pos.x,pos.y,str,backColor,textColor);
}

int dtextOut(HDC dc, int x, int y, TextAlignment align, const String &str, COLORREF backColor, COLORREF textColor) {
  const int textWidth = getTextExtent(dc, str).cx;
  switch(align) {
  case ALIGN_LEFT  :
    dtextOut(dc, x,y,             str,backColor,textColor);
    break;

  case ALIGN_RIGHT :
    dtextOut(dc, x-textWidth,y,   str,backColor,textColor);
    break;

  case ALIGN_CENTER:
    dtextOut(dc, x-textWidth/2,y, str,backColor,textColor);
    break;
  }
  return textWidth;
}

int dtextOut(HDC dc, const CPoint &pos, TextAlignment align, const String &str, COLORREF backColor, COLORREF textColor) {
  return dtextOut(dc, pos.x,pos.y,align,str,backColor,textColor);
}

void dtextOut(HDC dc, int x, int y, TextAlignment align, const Array<ColoredText> &strings) {
  switch(align) {
  case ALIGN_LEFT  :
    { for(size_t i = 0; i < strings.size(); i++) {
        const ColoredText &str = strings[i];
        x += dtextOut(dc, x,y,align, str, str.m_backColor,str.m_textColor);
      }
    }
    break;

  case ALIGN_RIGHT :
    { for(int i = strings.size() - 1; i >= 0; i--) {
        const ColoredText &str = strings[i];
        x -= dtextOut(dc,x,y,align,str,str.m_backColor,str.m_textColor);
      }
    }
    break;

  case ALIGN_CENTER:
    { String resultStr;
      for(size_t i = 0; i < strings.size(); i++) {
        resultStr += strings[i];
      }
      const int textWidth = getTextExtent(dc, resultStr).cx;
      x -= textWidth/2;
      for(size_t i = 0; i < strings.size(); i++) {
        const ColoredText &str = strings[i];
        x += dtextOut(dc,x,y,ALIGN_LEFT,str,str.m_backColor,str.m_textColor);
      }
    }
    break;
  }
}

void dtextOut(HDC dc, const CPoint &pos, TextAlignment align, const Array<ColoredText> &strings) {
  dtextOut(dc, pos.x,pos.y, align, strings);
}
