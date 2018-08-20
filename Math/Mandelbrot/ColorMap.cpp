#include "StdAfx.h"
#include "ColorMap.h"

String ColorMapData::getDistributionName(ColorDistribution cd) { // static
  switch (cd) {
  case CD_LINEAR  : return _T("LINEAR");
  case CD_SQRROOT : return _T("SQRROOT");
  case CD_LOG     : return _T("LOG");
  default         : return format(_T("Unknown colordistribution:%d"), cd);
  }
}

static int newRandomStep(int old) {
  const int result = randInt(2, 16);
  if(old > 0) {
    return -result;
  }
  return result;
}

ColorMap::ColorMap(const ColorMapData &d) {
  clear(d.getMaxCount());
  if(d.m_randomSeed) {
    randomize();
  } else {
    _standardRandomGenerator->setSeed(d.m_seed);
  }

  int r  = randInt(0,255);
  int g  = randInt(0,255);
  int b  = randInt(0,255);
  int dr = newRandomStep(randInt(-5,5));
  int dg = newRandomStep(randInt(-5,5));
  int db = newRandomStep(randInt(-5,5));
  if(dr == 0) dr =  3;
  if(dg == 0) dg = -7;
  if(db == 0) db = 13;
  UINT i;
  for(i = 0; i < d.getMaxCount(); i++) {
    add(D3DCOLOR_XRGB(r,g,b));

    if(r+dr > 255 || r+dr < 0) dr = newRandomStep(dr);
    if(g+dg > 255 || g+dg < 0) dg = newRandomStep(dg);
    if(b+db > 255 || b+db < 0) db = newRandomStep(db);
    r += dr;
    g += dg;
    b += db;
  }
  add(D3D_BLACK);
}
