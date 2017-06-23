#include "stdafx.h"

static const ColorName colorNameArray[] = {
  _T("BLACK"       ) ,BLACK      
 ,_T("BLUE"        ) ,RGB(0  ,0  ,196)    
 ,_T("GREEN"       ) ,RGB(0  ,196,0  )    
 ,_T("CYAN"        ) ,RGB(0  ,196,196)  
 ,_T("RED"         ) ,RGB(196,0  ,0  )    
 ,_T("MAGENTA"     ) ,RGB(196,0  ,128)  
 ,_T("BROWN"       ) ,RGB(128,0  ,128)  
 ,_T("LIGHTGRAY"   ) ,RGB(200,200,200)
 ,_T("DARKGRAY"    ) ,RGB(128,128,128)
 ,_T("LIGHTBLUE"   ) ,RGB(0  ,0  ,255)    
 ,_T("LIGHTGREEN"  ) ,RGB(0  ,255,0  )    
 ,_T("LIGHTCYAN"   ) ,RGB(0  ,255,255)  
 ,_T("LIGHTRED"    ) ,RGB(255,0  ,0  )    
 ,_T("LIGHTMAGENTA") ,RGB(255,0  ,255)  
 ,_T("YELLOW"      ) ,RGB(255,255,0  )  
};

int getColorCount() {
  return ARRAYSIZE(colorNameArray);
}

COLORREF getColor(int index) {
  return colorNameArray[index%getColorCount()].m_color;
}

const ColorName *findColorByName(const TCHAR *name) {
  for(int i = 0; i < ARRAYSIZE(colorNameArray); i++) {
    if(_tcsicmp(name, colorNameArray[i].m_name) == 0) {
      return &colorNameArray[i];
    }
  }
  return NULL;
}

String getLegalColorNames() {
  const int n          = ARRAYSIZE(colorNameArray);
  int       lineLength = 0;
  String    result;

  for(int i = 0; i < n; i++) {
    if(lineLength > 42) {
      result += _T("\n");
      lineLength = 0;
    }
    const ColorName &cn = colorNameArray[i];
    result += cn.m_name;
    lineLength += (int)_tcslen(cn.m_name);
    if(i < n-1) { result += _T(","); lineLength++; }
  }
  return result;
}

void usage() {
  throwException(_T("%s\nLegal colors are:\n%s")
                ,_T("Usage : Showgraf [options] file1 file2 ...\n"
                    "Options :\n"
                    "  -k : Paint data as crosses.\n"
                    "  -p : Paint data as points.\n"
                    "       (Default is connecting lines between data).\n"
                    "  -g : Paint grid.\n"
                    "  -c backgroundcolor axescolor. (Default is WHITE and BLACK).\n"
                    "  -i : Ignore errors in input.\n"
                    "       (Illegal inputformat, negative input on logarithmic scale,...).\n"
                    "  -1 : Only 1 number on each inputline.\n"
                    "       (Used as y-value). X-value incremented by 1 for each inputline.\n"
                    "  -r[size] : Calculates rolling average.\n"
                    "             Default rolling size = 10. Size = [1..10000].\n"
                    "  -l x-label y-label    : Specify lablenames on x- and y-axes.\n"
                    "  -Iminx,maxx,miny,maxy : Explicit defined range on x- and y-axes.\n"
                    "  -Lx or -Ly or -Lxy    : Logaritmic scale of x and/or y-axes.\n"
                    "  -Dx or -Dy or -Dxy    : Date-time transformation of x and/or y-axes.\n"
                    "                          Dateformat is [yy]yyMMdd.hhmm[ss].\n"
                    "  -Nx or -Ny or -Nxy    : Use normal distribution scale on\n"
                    "                          x- and/or y-axes.\n"
                    "  If no scale is specified, both axes are linear with normal\n"
                    "  decimal numbers.\n"
                   )
                  ,getLegalColorNames().cstr()
                );
}

void errorMessage(TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  AfxMessageBox(vformat(format, argptr).cstr(),MB_ICONSTOP);
  va_end(argptr);
  exit(-1);
}

CFont *scaleFont(CFont *src, double scale) {
  LOGFONT lf;
  src->GetLogFont(&lf);
  lf.lfHeight = (int)(scale * lf.lfHeight);
  lf.lfWidth  = (int)(scale * lf.lfWidth);
  CFont *nf = new CFont();
  nf->CreateFontIndirect(&lf);
  return nf;
}

COLORREF randomColor() {
  const int c1 = randInt(256);
  const int c2 = 255 - c1;
  const int c3 = (c1+c2)/2;
  return (randInt(2)==0)?RGB(c1,c2,c3):RGB(c1,c3,c2);
}

