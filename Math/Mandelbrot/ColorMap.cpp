#include "StdAfx.h"
#include "ColorMap.h"

ColorMapInterpolationPoint::ColorMapInterpolationPoint(const String &str) {
  if(_stscanf(str.cstr(), _T("pos:%f C:(r:%f,g:%f,b:%f)")
             , &m_pos
             , &m_rgb.m_red, &m_rgb.m_green, &m_rgb.m_blue)
      != 4
    ) {
    throwInvalidArgumentException(__TFUNCTION__,_T("Invalid input:\"%s\""), str.cstr());
  }
  validate();
}

#define VALIDATIONERROR(...) \
{ if(throwOnError) throwException(__VA_ARGS__); else return false; }

bool ColorMapInterpolationPoint::validate(bool throwOnError) const {
  if(m_pos < 0 || m_pos > 1) {
    VALIDATIONERROR(_T("%s::Pos=%f. Valid range:[0..1]"), __TFUNCTION__,m_pos);
  }
  if(!m_rgb.validate(throwOnError)) {
    return false;
  }
  return true;
}

static int posCmp(const ColorMapInterpolationPoint &p1, const ColorMapInterpolationPoint &p2) {
  return sign(p1.getPos() - p2.getPos());
}

int ColorMapData::insertPoint(const ColorMapInterpolationPoint &cip) {
  if(!cip.validate(false)) {
    return -1;
  }
  for(size_t i = 0; i < size(); i++) {
    if(cip.getPos() == (*this)[i].getPos()) {
      return -1;
    }
  }
  add(cip);
  sort(posCmp);
  for(UINT i = 0; i < size(); i++) {
    if(cip.getPos() == (*this)[i].getPos()) {
      return i;
    }
  }
  return -1;
}

bool ColorMapData::deletePoint(UINT index) {
  if(size() <= 2) {
    return false;
  }
  remove(index);
  if(first().getPos() != 0) {
    first().setPos(0);
  }
  if(last().getPos() != 1) {
    last().setPos(1);
  }
  return true;
}

static String makeFileName() {
  FileNameSplitter fs(getModuleFileName());
  return fs.setFileName(_T("ColorMapDefault")).setExtension(_T("dat")).getFullPath();
}

void ColorMapData::setAsDefault() const {
  validate();
  const String fileName = makeFileName();
  FILE *f = MKFOPEN(fileName,_T("w"));
  _ftprintf(f, _T("%s"), toString().cstr());
  fclose(f);
}

ColorMapData ColorMapData::loadDefaultFile() { // static
  const String fileName = makeFileName();
  FILE *f = NULL;
  ColorMapData result;
  try {
    f = FOPEN(fileName, _T("r"));
    String line;
    int lineCount = 0;
    while(readLine(f, line)) {
      lineCount++;
      if(lineCount == 1) {
        if(line.length() && (line[0] == '(')) line = line.cstr()+1;
      }
      ColorMapInterpolationPoint p(line);
      result.add(p);
    }
    fclose(f); f = NULL;
  } catch (...) {
    if(f) { fclose(f); f = NULL; }
    throw;
  }
  result.validate();
  return result;
}

ColorMapData ColorMapData::getDefault() { // static
  const String fileName = makeFileName();
  if(ACCESS(fileName, 0) < 0) {
    ColorMapData result;
    result.add(ColorMapInterpolationPoint(BLUE , 0    ));
    result.add(ColorMapInterpolationPoint(GREEN, 0.33f));
    result.add(ColorMapInterpolationPoint(RED  , 1    ));
    return result;
  } else {
    return loadDefaultFile();
  }
}

bool ColorMapData::validate(bool throwOnError) const {
  DEFINEMETHODNAME;
  if(isEmpty()) return true;
  if(!first().validate(throwOnError)) {
    return false;
  }
  if(size() >= 2) {
    if(first().getPos() != 0) {
      VALIDATIONERROR(_T("%s:First interpolationpoint must have pos=0 (=%f"),method, first().getPos());
    }
    for(size_t i = 1; i < size(); i++) {
      const ColorMapInterpolationPoint &cip = (*this)[i];
      if(!cip.validate(throwOnError)) {
        return false;
      }
      if(cip.getPos() <= (*this)[i - 1].getPos()) {
        VALIDATIONERROR(_T("%s:e[%zu] and e[%zu] has wrong order"), method, i-1,i);
      }
    }
    if(size() >= 2) {
      if(last().getPos() != 1) {
        VALIDATIONERROR(_T("%s:Last interpolationpoint must have pos=1 (=%f"),method, last().getPos());
      }
    }
  }
  return true;
}

ColorMap::ColorMap(const ColorMapData &cdm, UINT maxCount) {
  m_cdm = cdm;
  initColors(maxCount);
}

void ColorMap::initColors(UINT maxCount) {
  __super::clear(maxCount+1);
  if(m_cdm.size() == 1) {
    insert(0, ColorMapEntry(m_cdm[0].getD3DColor()), maxCount);
  } else if(m_cdm.size() >= 2) {
    for(UINT index = 1; index < m_cdm.size(); index++) {
      const ColorMapInterpolationPoint *p1 = &m_cdm[index-1], *p2 = &m_cdm[index];
      const UINT count = (index == m_cdm.size()-1) ? (maxCount - (UINT)size()) : (UINT)((p2->getPos() - p1->getPos()) * maxCount);
      if(count == 1) {
        add(ColorMapEntry(p2->getD3DColor()));
      } else {
        for(UINT i = 0; i < count; i++) {
          add(ColorMapEntry(RGBColor::blendColor(p1->getRGB(), p2->getRGB(), (float)i/(count-1))));
        }
      }
    }
  }
  insert(size(), ColorMapEntry(D3D_BLACK), maxCount+1-size());
  assert(getMaxCount() == maxCount);
}

void ColorMap::setColorMapData(const ColorMapData &cdm) {
  if(cdm != m_cdm) {
    m_cdm = cdm;
    initColors(getMaxCount());
  }
}

void ColorMap::setMaxCount(UINT maxCount) {
  if(maxCount != getMaxCount()) {
    initColors(maxCount);
  }
}

void ColorMap::clear(intptr_t capacity) {
  __super::clear(capacity);
  m_cdm.clear();
}
