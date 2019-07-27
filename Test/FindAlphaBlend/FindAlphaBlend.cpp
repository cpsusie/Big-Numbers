#include "stdafx.h"
#include <FileNameSplitter.h>

static void usage() {
  _ftprintf(stderr, _T("Usage:FindAlphaBlend p01 p02 p1 p2\n"));
  exit(-1);
}

template<class T> class PointMatrix : public MatrixTemplate<T> {
public:
  PointMatrix(const CSize &size) : MatrixTemplate<T>(size.cy, size.cx) {
  }
  inline T &operator()(const CPoint &p) {
    return MatrixTemplate::operator()(p.y,p.x);
  }
  inline const T &operator()(const CPoint &p) const {
    return MatrixTemplate::operator()(p.y,p.x);
  }
  inline CSize getSize() const {
    return CSize((int)getColumnCount(), (int)getRowCount());
  }
};

class Alpha4 {
public:
  float m_alpha;
  float m_aRED,m_aGREEN,m_aBLUE;
  Alpha4(int dummy=0) {}
  Alpha4(float aR, float aG, float aB)
    : m_aRED(  minMax(aR,0.f,1.f))
    , m_aGREEN(minMax(aG,0.f,1.f))
    , m_aBLUE( minMax(aB,0.f,1.f)) {
    m_alpha = (m_aRED+m_aGREEN+m_aBLUE)/3;
    if(m_alpha > 0.8f) m_alpha = 1;
    else if(m_alpha < 1.f/255) m_alpha=0;
  }
};

inline String toString(const Alpha4 &a) {
  return ::toString(a.m_alpha);
}

class AlphaMap : public PointMatrix<Alpha4> {
private:
  float findAvgAlphaNeighborRow(const CPoint &p, int dy) const;
public:
  AlphaMap(const CSize &size) : PointMatrix(size) {
  }
  float findAvgAlpha(const CPoint &p) const;
  inline float findAvgAlpha1Up(const CPoint &p) const {
    return findAvgAlphaNeighborRow(p, -1);
  }
  inline float findAvgAlpha1Down(const CPoint &p) const {
    return findAvgAlphaNeighborRow(p, 1);
  }
  void makeSmooth();
  String toString() const;
};

float AlphaMap::findAvgAlpha(const CPoint &p) const {
  const AlphaMap &a     = *this;
  const UINT      mx    = getSize().cx;
  const UINT      my    = getSize().cy;
  float           sum   = 0;
  int             count = 0;

  for (int dy = -1; dy <= 1; dy++) {
    if((UINT)(p.y + dy) >= my) continue;
    for (int dx = -1; dx <= 1; dx++) {
      if(((dx|dy)==0) || ((UINT)(p.x + dx) >= mx)) continue;
      sum += MatrixTemplate::operator()(p.y+dy,p.x+dx).m_alpha;
      count++;
    }
  }
  return sum/count;
}

float AlphaMap::findAvgAlphaNeighborRow(const CPoint &p, int dy) const {
  const AlphaMap &a     = *this;
  const int       mx    = min(getSize().cx-1, p.x+1);
  float           sum   = 0;
  int             count = 0;
  CPoint          p1    = p;
  p1.y += dy;
  for(p1.x = max(0,p.x-1); p1.x <= mx; p1.x++) {
    sum += a(p1).m_alpha;
    count++;
  }
  return sum/count;
}

void AlphaMap::makeSmooth() {
  AlphaMap &a = *this;
  CSize size = getSize();
  int cxm1 = size.cx-1, cym1 = size.cy-1;

  CPoint p;
  for(p.y = 1; p.y < cym1; p.y++) {
    for(p.x = 1; p.x < cxm1; p.x++) {
      if(a(p).m_alpha == 0.f) {
        const float avgAlpha = findAvgAlpha(p);
        if(avgAlpha > 0.5f) a(p).m_alpha = avgAlpha;
      }
    }
  }

  p.y = 0;
  for(p.x = 0; p.x < size.cx; p.x++) {
    if(a(p).m_alpha == 1.f) {
      const float avgAlpha = findAvgAlpha1Down(p);
      if(avgAlpha < 0.5f) a(p).m_alpha = avgAlpha;
    }
  }
  p.y = cym1;
  for(p.x = 0; p.x < size.cx; p.x++) {
    if(a(p).m_alpha == 1.f) {
      const float avgAlpha = findAvgAlpha1Up(p);
      if(avgAlpha < 0.5f) a(p).m_alpha = avgAlpha;
    }
  }
  for(p.x = 0; p.x < size.cx; p.x += cxm1) {
    for(p.y = 1; p.y < cym1; p.y++) {
      if(a(p).m_alpha == 1.f) {
        const float avgAlpha = findAvgAlpha(p);
        if(avgAlpha < 0.5f) a(p).m_alpha = avgAlpha;
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
      const double alpha = a(p).m_alpha;
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

class CaseMatrix : public PointMatrix<USHORT> {
private:
  int findMaxCase() const;
public:
  CaseMatrix(const CSize &size) : PointMatrix(size) {
  }
  String toString() const;
};

#define PACKCASES(r,g,b) ((USHORT)(((r)<<8)|((g)<<4)|(b)))
#define GETRCASE(s)      ((BYTE)( (s)>>8))
#define GETGCASE(s)      ((BYTE)(((s)>>4)&0xf))
#define GETBCASE(s)      ((BYTE)( (s)&0xf))

int CaseMatrix::findMaxCase() const {
  const CaseMatrix &a = *this;
  const CSize size = getSize();
  CPoint p;
  int max = 0;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const USHORT v = a(p);
      const BYTE r = GETRCASE(v);
      const BYTE g = GETGCASE(v);
      const BYTE b = GETBCASE(v);
      if(r > max) max = r;
      if(b > max) max = g;
      if(r > max) max = b;
    }
  }
  return max;
}

String CaseMatrix::toString() const {
  const int rowCount = findMaxCase() + 1;
  CompactIntArray rCount(rowCount), gCount(rowCount), bCount(rowCount);
  CompactIntArray rowTotal(rowCount);
  for(int r = 0; r < rowCount; r++) {
    rCount.add(0);
    gCount.add(0);
    bCount.add(0);
    rowTotal.add(0);
  }
  const CaseMatrix &a = *this;
  const CSize size = getSize();
  CPoint p;
  for(p.y = 0; p.y < size.cy; p.y++) {
    for(p.x = 0; p.x < size.cx; p.x++) {
      const USHORT v = a(p);
      rCount[GETRCASE(v)]++;
      gCount[GETGCASE(v)]++;
      bCount[GETBCASE(v)]++;
    }
  }
  for (size_t r = 0; r < rowTotal.size(); r++) {
    rowTotal[r] += rCount[r] + gCount[r] + bCount[r];
  }
  String result;
  int colTotal[3];
  const TCHAR colorChar[] = { 'R','G','B'};
  for (BYTE c = 0; c < 3; c++) {
    result +=format(_T("%15c  "), colorChar[c]);
    colTotal[c] = 0;
  }
  result += _T("\n");
  CompactIntArray *counter[3] = { &rCount, &gCount, &bCount };

  for(size_t r = 0; r < rowTotal.size(); r++) {
    for(BYTE c = 0; c < 3; c++) {
      const int v = (*counter[c])[r];
      result += format(_T("%15s  ")
                      ,format(_T("%.2lf%% (%5d)")
                             ,PERCENT(v,rowTotal[r])
                             ,v
                             ).cstr()
                      );
      colTotal[c] += v;
    }
    result += format(_T("  total:%6d\n"), rowTotal[r]);
  }
  result += format(_T("%s\n"), spaceString(17*3+8,'_').cstr());
  for (BYTE c = 0; c < 3; c++) {
    result += format(_T("%15d  "), colTotal[c]);
  }
  result += _T("\n");
  return result;
}

static void stop(const CPoint &p, D3DCOLOR p01, D3DCOLOR p02, D3DCOLOR p1, D3DCOLOR p2, const Alpha4 &a4, double v1, double v2, const char *channel) {
  int fisk = 1;
}

int _tmain(int argc, TCHAR **argv) {
  try {
    String resultName,alphaName, detailName;
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
    if (*argv == NULL) {
      resultName = _T("result");
    } else {
      resultName = *argv;
    }

    FileNameSplitter fs(resultName);
    resultName = fs.setExtension(_T("png")).getFullPath();
    const String name = fs.getFileName();
    alphaName  = fs.setFileName(name+_T("_Alpha" )).setExtension(_T("txt")).getFullPath();
    detailName = fs.setFileName(name+_T("_Detail")).setExtension(_T("txt")).getFullPath();

#define B01 pr[0]
#define B02 pr[1]
#define B1  pr[2]
#define B2  pr[3]

    PixRect *result = new PixRect(device, PIXRECT_PLAINSURFACE, size,D3DPOOL_FORCE_DWORD, D3DFMT_A8R8G8B8);
    CPoint p;

    AlphaMap    alphaMap(size);
    CaseMatrix  pointCase(size);

    for (p.y = 0; p.y < size.cy; p.y++) {
      for (p.x = 0; p.x < size.cx; p.x++) {
        const D3DCOLOR p01 = B01->getPixel(p);
        const D3DCOLOR p02 = B02->getPixel(p);
        const D3DCOLOR p1  = B1->getPixel(p);
        const D3DCOLOR p2  = B2->getPixel(p);

#define FINDALPHA(ch) {                                                             \
  const int c01 = ARGB_GET##ch(p01),c02 = ARGB_GET##ch(p02);                        \
  const int c1  = ARGB_GET##ch(p1 ),c2  = ARGB_GET##ch(p2 );                        \
  double    a;                                                                      \
  if (c2 == c1) {                                                                   \
    a     = (c1==c01)&&(c2==c02)?0:1;                                                   \
    i##ch = a?1:0;                                                                  \
  } else if (c02 == c01) {                                                          \
    a     = 1;                                                                        \
    i##ch = 2;                                                                      \
  } else {                                                                          \
    a     = 1.0 - (double)(c2-c1)/(c02-c01);                                            \
    i##ch = 3;                                                                      \
  }                                                                                 \
  a##ch = (float)a;                                                                 \
}

#define FINDCHANNEL(alpha,ch) {                                                     \
  if(alpha == 0) {                                                                  \
    c##ch = 255;                                                                      \
  } else {                                                                          \
    const int c1 = ARGB_GET##ch(p1);                                                \
    const int c2 = ARGB_GET##ch(p2);                                                \
    if(alpha == 1) {                                                                \
      c##ch = (float)((double)(c1+c2)/2);                                           \
    } else {                                                                        \
      const int    c01 = ARGB_GET##ch(p01);                                         \
      const int    c02 = ARGB_GET##ch(p02);                                         \
      const double v1  = (c1 - c01*(1.-alpha))/alpha;                               \
      const double v2  = (c2 - c02*(1.-alpha))/alpha;                               \
      if(v1 < 0 || v2 < 0 || v1 > 255 || v2 > 255) {                                \
        stop(p,p01,p02,p1,p2,a4,v1,v2,#ch);                                         \
      }                                                                             \
      c##ch = (float)((v1+v2)/2);                                                   \
    }                                                                               \
  }                                                                                 \
}

        D3DCOLOR resultColor;
        BYTE     iRED, iGREEN, iBLUE;
        float    aRED, aGREEN, aBLUE;
        if(p1 == p2) {
          resultColor = p1;
          aRED = aGREEN = aBLUE = 1.f;
          iRED = iGREEN = iBLUE = 4;
        } else {
          FINDALPHA(RED  );
          FINDALPHA(GREEN);
          FINDALPHA(BLUE );
        }
        alphaMap(p)  = Alpha4(   aRED,aGREEN,aBLUE);
        pointCase(p) = PACKCASES(iRED,iGREEN,iBLUE);
      }
    }
    FILE *alphaFile = FOPEN(alphaName, _T("w"));
    _ftprintf(alphaFile, _T("Before smooth:\n%s"), alphaMap.toString().cstr());
    alphaMap.makeSmooth();
    _ftprintf(alphaFile, _T("After smooth:\n%s"), alphaMap.toString().cstr());
    fclose(alphaFile);

    FILE *detailFile = FOPEN(detailName, _T("w"));
    for(p.y = 0; p.y < size.cy; p.y++) {
      for(p.x = 0; p.x < size.cx; p.x++) {
        const D3DCOLOR p01 = B01->getPixel(p);
        const D3DCOLOR p02 = B02->getPixel(p);
        const D3DCOLOR p1  = B1->getPixel(p);
        const D3DCOLOR p2  = B2->getPixel(p);

        float cRED,cGREEN,cBLUE;
        const Alpha4 &a4    = alphaMap(p);
        const USHORT pc     = pointCase(p);
        const BYTE   iRED   = GETRCASE(pc);
        const BYTE   iGREEN = GETGCASE(pc);
        const BYTE   iBLUE  = GETBCASE(pc);
//        const float alpha   = a4.m_alpha;

        FINDCHANNEL(a4.m_aRED  , RED  );
        FINDCHANNEL(a4.m_aGREEN, GREEN);
        FINDCHANNEL(a4.m_aBLUE , BLUE );

        const BYTE a = (BYTE)(a4.m_alpha*255);
        const BYTE r = (BYTE)minMax(cRED       ,0.f,255.f);
        const BYTE g = (BYTE)minMax(cGREEN     ,0.f,255.f);
        const BYTE b = (BYTE)minMax(cBLUE      ,0.f,255.f);
        D3DCOLOR resultColor = D3DCOLOR_ARGB(a,r,g,b);
        result->setPixel(p, resultColor);
//        const USHORT pc = pointCase(p);
        _ftprintf(detailFile
                 ,_T("(%2d,%2d):[%d,%d,%d], p01:(%3d,%3d,%3d) p1:(%3d,%3d,%3d) p02:(%3d,%3d,%3d) p2:(%3d,%3d,%3d). Result:(%3d,%3d,%3d,%3d) alpha:%.3lf %.3lf %.3lf %.3lf\n")
                 ,p.x,p.y
                 ,GETRCASE(pc),GETGCASE(pc),GETBCASE(pc)
                 ,ARGB_GETRED(p01),ARGB_GETGREEN(p01),ARGB_GETBLUE(p01)
                 ,ARGB_GETRED(p1 ),ARGB_GETGREEN(p1 ),ARGB_GETBLUE(p1 )
                 ,ARGB_GETRED(p02),ARGB_GETGREEN(p02),ARGB_GETBLUE(p02)
                 ,ARGB_GETRED(p2 ),ARGB_GETGREEN(p2 ),ARGB_GETBLUE(p2 )
                 ,ARGB_GETALPHA(resultColor),ARGB_GETRED(  resultColor),ARGB_GETGREEN(resultColor),ARGB_GETBLUE( resultColor)
                 ,a4.m_alpha
                 ,a4.m_aRED,a4.m_aGREEN,a4.m_aBLUE
                 );
      }
    }
    _ftprintf(detailFile, _T("%s"), pointCase.toString().cstr());
    fclose(detailFile);
    result->writeAsPNG(ByteOutputFile(resultName));
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}

