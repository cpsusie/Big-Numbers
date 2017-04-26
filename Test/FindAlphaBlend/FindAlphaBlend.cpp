#include "stdafx.h"

static void usage() {
  _ftprintf(stderr, _T("Usage:FindAlphaBlend p01 p02 p1 p2\n"));
  exit(-1);
}

class AlphaMap : public MatrixTemplate<float> {
public:
  AlphaMap(const CSize &size) : MatrixTemplate<float>(size.cy, size.cx) {
  }
  float &operator()(const CPoint &p) {
    return MatrixTemplate<float>::operator()(p.y,p.x);
  }
  const float &operator()(const CPoint &p) const {
    return MatrixTemplate<float>::operator()(p.y,p.x);
  }
  inline CSize getSize() const {
    return CSize((int)getColumnCount(), (int)getRowCount());
  }
  float findAvgAlpha(const CPoint &p) const;
  void makeSmooth();
  String toString() const;
};

float AlphaMap::findAvgAlpha(const CPoint &p) const {
  float sum = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if((dx|dy)==0) continue;
      sum += MatrixTemplate::operator()(p.y+dy,p.x+dx);
    }
  }
  return sum/8.f;
}

void AlphaMap::makeSmooth() {
  AlphaMap &a = *this;
  CSize size = getSize();
  size.cx--; size.cy--;
  CPoint p;
  for(p.y = 1; p.y < size.cy; p.y++) {
    for(p.x = 1; p.x < size.cx; p.x++) {
      if(a(p) == 0.f) {
        float avgAlpha = findAvgAlpha(p);
        if(avgAlpha > 0.5f) {
          a(p) = avgAlpha;
        }
      }
    }
  }
}

String AlphaMap::toString() const {
  const AlphaMap &a = *this;
  String result = _T("    ");
  const CSize size = getSize();
  for (int x = 0; x < size.cx; x++) {
    result += format(_T("%5d"),x);
  }
  result += _T("\n");
  CPoint p;
  for(p.y = 0; p.y<size.cy; p.y++) {
    result += format(_T("%3d  |"), p.y);
    for(p.x = 0; p.x < size.cx; p.x++) {
      const double alpha = a(p);
      if(alpha == 0) {
        result += _T("     ");
      } else {
        result += format(_T("%.2lf "), alpha);
      }
    }
    result += format(_T(" | %3d\n"),p.y);
  }
  result += _T("    ");
  for(int x = 0; x < size.cx; x++) {
    result += format(_T("%5d"),x);
  }
  result += _T("\n");
  return result;
}

int _tmain(int argc, TCHAR **argv) {
  try {
    HWND hwnd = Console::getWindow();
    PixRectDevice device;
    device.attach(hwnd);
    argv++;
    PixRect *pr[4];
    CSize size;
    for (int i = 0; i < 4; i++) {
      if(!*argv) usage();
      pr[i] = PixRect::load(device, ByteInputFile(*(argv++)));
      if (i == 0) {
        size = pr[i]->getSize();
      } else {
        const CSize sz = pr[i]->getSize();
        if (sz != size) {
          throwException(_T("image %d has size (%d,%d), which is not the same size the first image (size=(%d,%d)")
                        ,sz.cx, sz.cy
                        ,size.cx, size.cy);
        }
      }
    }
#define B01 pr[0]
#define B02 pr[1]
#define B1  pr[2]
#define B2  pr[3]

    PixRect *result = new PixRect(device, PIXRECT_PLAINSURFACE, size,D3DPOOL_FORCE_DWORD, D3DFMT_A8R8G8B8);
    CPoint p;
    int caseCounter[3][5];
    BYTE iRED, iGREEN, iBLUE;
    memset(caseCounter, 0, sizeof(caseCounter));
    FILE *specf  = FOPEN("special.log", "w");

    AlphaMap alphaMap(size);

    for (p.y = 0; p.y < size.cy; p.y++) {
      for (p.x = 0; p.x < size.cx; p.x++) {
        const D3DCOLOR p01 = B01->getPixel(p);
        const D3DCOLOR p02 = B02->getPixel(p);
        const D3DCOLOR p1  = B1->getPixel(p);
        const D3DCOLOR p2  = B2->getPixel(p);

#define FINDALPHA(ch) {                                                             \
  const int c01 = ARGB_GET##ch(p01),c02 = ARGB_GET##ch(p02);                        \
  const int c1  = ARGB_GET##ch(p1 ),c2  = ARGB_GET##ch(p2 );                        \
  double a;                                                                         \
  if (c2 == c1) {                                                                   \
    a = (c1==c01)&&(c2==c02)?0:1;                                                   \
    i##ch = a?1:0;                                                                  \
  } else if (c02 == c01) {                                                          \
    a = 1;                                                                          \
    i##ch = 2;                                                                      \
  } else {                                                                          \
    a = 1.0 - (double)(c2-c1)/(c02-c01);                                            \
    i##ch = 3;                                                                      \
  }                                                                                 \
  a##ch = a;                                                                        \
}

#define FINDCHANNEL(ch) {                                                           \
  if(alpha == 0) {                                                                  \
    c##ch = 255;                                                                      \
  } else {                                                                          \
    if(alpha == 1) {                                                                \
      const int c1 = ARGB_GET##ch(p1);                                              \
      const int c2 = ARGB_GET##ch(p2);                                              \
      c##ch = (c1+c2)/2;                                                            \
    } else {                                                                        \
      const int    c01 = ARGB_GET##ch(p01);                                         \
      const int    c1  = ARGB_GET##ch(p1);                                          \
      const double v1  = (c1 - c01*(1.-alpha))/alpha;                               \
      const int    c02 = ARGB_GET##ch(p02);                                         \
      const int    c2  = ARGB_GET##ch(p2);                                          \
      const double v2  = (c2 - c02*(1.-alpha))/alpha;                               \
      c##ch = (v1+v2)/2;                                                            \
    }                                                                               \
  }                                                                                 \
}

        D3DCOLOR resultColor;
        double   alpha;
        if(p1 == p2) {
          resultColor = p1;
          alpha = 1;
          iRED = iGREEN = iBLUE = 4;
        } else {
          double aRED,aGREEN,aBLUE;
          FINDALPHA(RED  );
          FINDALPHA(GREEN);
          FINDALPHA(BLUE );
          alpha = minMax((aRED+aGREEN+aBLUE)/3,0.,1.);
          if (alpha > 0.6) {
            alpha = 1;
          }
        }
        alphaMap(p) = (float)alpha;
        caseCounter[0][iRED  ]++;
        caseCounter[1][iGREEN]++;
        caseCounter[2][iBLUE ]++;

      }
    }
    FILE *alphaf = FOPEN("alpha.log", "w");
    _ftprintf(alphaf, _T("Before smooth:\n%s"), alphaMap.toString().cstr());
    alphaMap.makeSmooth();
    _ftprintf(alphaf, _T("After smooth:\n%s"), alphaMap.toString().cstr());
    fclose(alphaf);

    for(p.y = 0; p.y < size.cy; p.y++) {
      for(p.x = 0; p.x < size.cx; p.x++) {
        const D3DCOLOR p01 = B01->getPixel(p);
        const D3DCOLOR p02 = B02->getPixel(p);
        const D3DCOLOR p1  = B1->getPixel(p);
        const D3DCOLOR p2  = B2->getPixel(p);

        double cRED,cGREEN,cBLUE;
        const double alpha = alphaMap(p);
        FINDCHANNEL(RED  );
        FINDCHANNEL(GREEN);
        FINDCHANNEL(BLUE );

        const BYTE a = (BYTE)(alpha*255);
        const BYTE r = (BYTE)minMax(cRED       ,0.,255.);
        const BYTE g = (BYTE)minMax(cGREEN     ,0.,255.);
        const BYTE b = (BYTE)minMax(cBLUE      ,0.,255.);
        D3DCOLOR resultColor = D3DCOLOR_ARGB(a,r,g,b);
        result->setPixel(p, resultColor);

        _ftprintf(specf
                 ,_T("(%2d,%2d):[%d,%d,%d], p01:(%3d,%3d,%3d) p1:(%3d,%3d,%3d) p02:(%3d,%3d,%3d) p2:(%3d,%3d,%3d). Result:(%3d,%3d,%3d,%3d) alpha:%.3lf\n")
                 ,p.x,p.y
                 ,iRED,iGREEN,iBLUE
                 ,ARGB_GETRED(p01),ARGB_GETGREEN(p01),ARGB_GETBLUE(p01)
                 ,ARGB_GETRED(p1 ),ARGB_GETGREEN(p1 ),ARGB_GETBLUE(p1 )
                 ,ARGB_GETRED(p02),ARGB_GETGREEN(p02),ARGB_GETBLUE(p02)
                 ,ARGB_GETRED(p2 ),ARGB_GETGREEN(p2 ),ARGB_GETBLUE(p2 )
                 ,ARGB_GETALPHA(resultColor),ARGB_GETRED(  resultColor),ARGB_GETGREEN(resultColor),ARGB_GETBLUE( resultColor)
                 ,alpha
                 );
      }
    }

    int total[ARRAYSIZE(caseCounter[0])];
    int colTotal[3];
    memset(total, 0, sizeof(total));
    memset(colTotal, 0, sizeof(colTotal));
    for(int j = 0; j < ARRAYSIZE(caseCounter[0]); j++) {
      for(int i = 0; i < 3; i++) {
        total[j] += caseCounter[i][j];
      }
    }
    const TCHAR colorChar[] = { 'R','G','B'};
    for (BYTE c = 0; c < 3; c++) {
      _ftprintf(specf, _T("%15c  "), colorChar[c]);
    }
    _ftprintf(specf, _T("\n"));
    for(int r = 0; r < ARRAYSIZE(caseCounter[0]); r++) {
      for(BYTE c = 0; c < 3; c++) {
        _ftprintf(specf, _T("%15s  ")
                ,format(_T("%.2lf%% (%5d)")
                       ,PERCENT(caseCounter[c][r],total[r])
                       ,caseCounter[c][r]
                       ).cstr()
                );
        colTotal[c] += caseCounter[c][r];
      }
      _ftprintf(specf, _T("  total:%6d\n"), total[r]);
    }
    _ftprintf(specf, _T("%s\n"), spaceString(17*3,'_').cstr());
    for (BYTE c = 0; c < 3; c++) {
      _ftprintf(specf, _T("%15d  "), colTotal[c]);
    }
    _ftprintf(specf, _T("\n"));
    fclose(specf);
    result->writeAsPNG(ByteOutputFile(_T("result.png")));
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

