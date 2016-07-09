#include "stdafx.h"

static const ColorName colorNameArray[] = {
  _T("BLACK"       ) ,BLACK      
 ,_T("BLUE"        ) ,RGB(0,0,196)    
 ,_T("GREEN"       ) ,RGB(0,196,0)    
 ,_T("CYAN"        ) ,RGB(0,196,196)  
 ,_T("RED"         ) ,RGB(196,0,0)    
 ,_T("MAGENTA"     ) ,RGB(196,0,128)  
 ,_T("BROWN"       ) ,RGB(128,0,128)  
 ,_T("LIGHTGRAY"   ) ,RGB(200,200,200)
 ,_T("DARKGRAY"    ) ,RGB(128,128,128)
 ,_T("LIGHTBLUE"   ) ,RGB(0,0,255)    
 ,_T("LIGHTGREEN"  ) ,RGB(0,255,0)    
 ,_T("LIGHTCYAN"   ) ,RGB(0,255,255)  
 ,_T("LIGHTRED"    ) ,RGB(255,0,0)    
 ,_T("LIGHTMAGENTA") ,RGB(255,0,255)  
 ,_T("YELLOW"      ) ,RGB(255,255,0)  
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
  String result;
  for(int i = 0; i < ARRAYSIZE(colorNameArray); i++) {
    const ColorName &cn = colorNameArray[i];
    if(i > 0) result += ",";
    result += cn.m_name;
  }
  return result;
}

void usage() {
  errorMessage(_T("%s\nLegal colors are:%s")
              ,_T("Usage : Showgraf [options] file1 file2 ...\n"
                  "Options :\n"
                  "  -k : Paint data as crosses.\n"
                  "  -p : Paint data as points.\n"
                  "       (Default is connecting lines between data).\n"
                  "  -g : Paint grid.\n"
                  "  -c backgroundcolor axiscolor. (Default is WHITE and BLACK).\n"
                  "  -i : Ignore errors in input.\n"
                  "        (Illegal inputformat, Negative input on logaritmic scale,...).\n"
                  "  -r[size] : Calculates rolling average.\n"
                  "             Default rolling size = 10. Size = [1..10000].\n"
                  "  -l x-label y-label: Specify lablenames on x- and y-axis.\n"
                  "  -1 : Only 1 number on each inputline.\n"
                  "       (Used as y-value). X-value incremented by 1 for each inputline.\n"
                  "  -Iminx,maxx,miny,maxy : Explicit defined range on x- and y-axis.\n"
                  "  -Lx or -Ly or -Lxy    : Logaritmic scale of x and/or y-axis.\n"
                  "  -Dx or -Dy or -Dxy    : Date-time transformation of x and/or y-axis.\n"
                  "                          Dateformat is [yy]yyMMdd.hhmm[ss].\n"
                  "  -Nx or -Ny or -Nxy    : Use normal distribution scale on x-\n"
                  "                          and/or y-axis.\n"
                  "                          If no scale is specified, both axies are\n"
                  "                          linear with normal decimal numbers.\n"
                )
                ,getLegalColorNames().cstr()
                );
  exit(-1);
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
  int c1 = rand() % 256;
  int c2 = 255 - c1;
  int c3 = (c1+c2)/2;
  return rand()%2==0?RGB(c1,c2,c3):RGB(c1,c3,c2);
}

