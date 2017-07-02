#include "stdafx.h"
#include <Math.h>
#include <Random.h>

DataArray &DataArray::operator=(const DataArray &src) {
  ((ByteArray&)(*this)) = src;
  return *this;
}

void DataArray::setElement(BYTE *e, UINT value) {
  switch(getElementSize()) {
  case 1 : *e                    = (BYTE)value;           break;
  case 2 : *((unsigned short*)e) = (unsigned short)value; break;
  default: *((unsigned int*)e)   = value;                 break;
  }
}

UINT DataArray::operator[](size_t index) const {
  switch(getElementSize()) {
  case 1 : return ByteArray::operator[](index);
  case 2 : return *(unsigned short*)(ByteArray::getData() + index * sizeof(short));
  case 4 : return *(unsigned int  *)(ByteArray::getData() + index * sizeof(int));
  default: return *(unsigned int  *)(ByteArray::getData() + index * getElementSize());
  }
}

void DataArray::reset() {
  const size_t n      = m_param.m_elementCount;
  const size_t nBytes = n * m_param.m_elementSize;
  clear(nBytes);
  const BYTE zero = 0;
  for(size_t i = 0; i < nBytes; i++) {
    append(zero);
  }
}

void DataArray::init() {
  UINT i;
  reset();
  BYTE        *ep       = (BYTE*)getData();
  const size_t n        = size();
  const UINT   elemSize = getElementSize();

  UINT maxValue;
  switch(elemSize) {
  case 1 : maxValue = min((UINT)n, 0xff  ); break;
  case 2 : maxValue = min((UINT)n, 0xffff); break;
  default: maxValue = (UINT)n;              break;
  }
  switch(m_param.m_initMethod) {
  case IDC_RADIO_RANDOM:
    { switch(m_param.m_randomizationMethod) {
      case FIXED_SEED       : m_param.m_random.setSeed(m_param.m_seed);       break;
      case SAME_RANDOM_SEED : m_param.m_random.setSeed(m_param.m_randomSeed); break;
      case RANDOM_SEED      : m_param.m_random.randomize();                   break;
      }
      for(i = 0; i < n; i++, ep += elemSize) {
        setElement(ep, m_param.m_random.nextInt(maxValue));
      }
    }
    break;

  case IDC_RADIO_SORTED:
    for(i = 0; i < n; i++, ep += elemSize) {
      setElement(ep, (UINT)((i*maxValue)/n));
    }
    break;

  case IDC_RADIO_INVERSESORTED:
    for(i = 0; i < n; i++, ep += elemSize) {
      setElement(ep, (UINT)((n-i)*maxValue/n));
    }
    break;

  case IDC_RADIO_SINUS:
    for(i = 0; i < n; i++, ep += elemSize) {
      setElement(ep, (unsigned int)(maxValue * (0.5*(1.0+sin(M_PI*2*i / (n-1)*m_param.m_periodCount)))));
    }
    break;

  case IDC_RADIO_FILEDATA:
    for(i = 0; i < n; i++, ep += elemSize) {
      setElement(ep, m_param.m_fileData[i]);
    }
    break;

  default:
    throwException(_T("DataArray::init:Unknown initmethod (=%d)"), m_param.m_initMethod);
    break;
  }
}

bool DataArray::isSorted() const {
  const int n = size();
  if(n <= 1) {
    return true;
  }
  int v1 = (*this)[0];
  for(int i = 1; i < n; i++) {
    const int v2 = (*this)[i];
    if(v2 < v1) {
      return false;
    }
    v1 = v2;
  }
  return true;
}

unsigned int DataArray::getMaxValue() const {
  unsigned int maxv = 1; // not 0, just to avoid division by zero in drawElement
  const int n = size();
  for(int i = 0; i < n; i++) {
    const unsigned int v = (*this)[i];
    if(v > maxv) {
      maxv = v;
    }
  }
  return maxv;
}

BitSet &DataArray::getModified(BitSet &dst, const DataArray &a) const {
  dst.setCapacity(size());
  dst.clear();
  const int n           = size();
  const int elementSize = getElementSize();
  switch(elementSize) {
  case sizeof(BYTE):
    { const BYTE *p1 = (const BYTE*)getData();
      const BYTE *p2 = (const BYTE*)a.getData();
      for(int i = 0; i < n; i++) {
        if(*(p1++) != *(p2++)) {
          dst.add(i);
        }
      }
    }
    break;
  case sizeof(unsigned short):
    { const unsigned short *p1 = (const unsigned short*)getData();
      const unsigned short *p2 = (const unsigned short*)a.getData();
      for(int i = 0; i < n; i++) {
        if(*(p1++) != *(p2++)) {
          dst.add(i);
        }
      }
    }
    break;
  case sizeof(unsigned int):
    { const unsigned int *p1 = (const unsigned int*)getData();
      const unsigned int *p2 = (const unsigned int*)a.getData();
      for(int i = 0; i < n; i++) {
        if(*(p1++) != *(p2++)) {
          dst.add(i);
        }
      }
    }
    break;
  default:
    { const BYTE *p1 = (const BYTE*)getData();
      const BYTE *p2 = (const BYTE*)a.getData();
      for(int i = 0; i < n; i++, p1 += elementSize, p2 += elementSize) {
        if(*((unsigned int*)p1) != *((unsigned int*)p2)) {
          dst.add(i);
        }
      }
    }
    break;

  }
  return dst;
}

String initMethodToName(int initMethod) {
  switch(initMethod) {
  case IDC_RADIO_RANDOM       : return _T("random");
  case IDC_RADIO_SORTED       : return _T("sorted");
  case IDC_RADIO_INVERSESORTED: return _T("inversesorted");
  case IDC_RADIO_SINUS        : return _T("sinus");
  case IDC_RADIO_FILEDATA     : return _T("file");
  default                     : return _T("random");
  }
}

int nameToInitMethod(const String &s) {
  if(s == _T("random")       ) return IDC_RADIO_RANDOM;
  if(s == _T("sorted")       ) return IDC_RADIO_SORTED;
  if(s == _T("inversesorted")) return IDC_RADIO_INVERSESORTED;
  if(s == _T("sinus")        ) return IDC_RADIO_SINUS;
  if(s == _T("file")         ) return IDC_RADIO_FILEDATA;
  return IDC_RADIO_RANDOM;
}

static const TCHAR *getRandomizationStr(RandomizationMethod method) {
  switch(method) {
  case FIXED_SEED      : return _T("Fixed seed");
  case SAME_RANDOM_SEED: return _T("Same seed");
  case RANDOM_SEED     : return _T("Random seed");
  default              : return _T("?seed");
  }
}

InitializeParameters::InitializeParameters() {
  setDefault();
}

String InitializeParameters::getInitString() const {
  switch(m_initMethod) {
  case IDC_RADIO_RANDOM       : return format(_T("Random(%s)"), getRandomizationStr(m_randomizationMethod));
  case IDC_RADIO_SORTED       : return _T("sorted");
  case IDC_RADIO_INVERSESORTED: return _T("inversesorted");
  case IDC_RADIO_SINUS        : return format(_T("Sinus(%d periods)"), m_periodCount);
  case IDC_RADIO_FILEDATA     : return format(_T("file(%s)"), m_fileName.cstr());
  default                     : return format(_T("Unknown initmet:%d"), m_initMethod);
  }
}

void InitializeParameters::setDefault() {
  m_elementCount        = 75;
  m_elementSize         = sizeof(int);
  m_seed                = 0;
  m_periodCount         = 1;
  m_randomizationMethod = SAME_RANDOM_SEED;
  m_initMethod          = IDC_RADIO_RANDOM;
  m_fileData.clear();
}

void InitializeParameters::nextRandomSeed() {
  randomize();
  m_randomSeed = randInt();
}

RegistryKey InitializeParameters::getKey() { // static
  return RegistryKey(HKEY_CURRENT_USER,_T("Software")).createOrOpenKey(_T("JGMData\\SortDemo\\Settings"));
}

void InitializeParameters::load() {
  try {
    InitializeParameters defaultValues;
    RegistryKey key = getKey();
    m_elementCount        =  key.getInt(   _T("elementcount")    , (UINT)defaultValues.m_elementCount);
    m_elementSize         =  key.getInt(   _T("elementsize")     , defaultValues.m_elementSize );
    String initMethodName =  key.getString(_T("initmethod")      , _T("random")                    );
    m_initMethod = nameToInitMethod(initMethodName);

    m_randomizationMethod = (RandomizationMethod)key.getInt(    _T("randomize")       , defaultValues.m_randomizationMethod);
    m_seed                = key.getUint(   _T("seed")            , defaultValues.m_seed        );
    m_periodCount         = key.getUint(   _T("periodcount")     , defaultValues.m_periodCount );
    m_fileName            = key.getString( _T("filename")        , defaultValues.m_fileName    );
    if(m_initMethod == IDC_RADIO_FILEDATA) {
      readTextFile(m_fileName);
    }
  } catch(Exception e) {
    setDefault();
  }
}

void InitializeParameters::save() {
  RegistryKey key = getKey();

  key.setValue(_T("elementcount")    ,(UINT)m_elementCount          );
  key.setValue(_T("elementsize")     ,m_elementSize                 );
  key.setValue(_T("initmethod")      ,initMethodToName(m_initMethod));
  key.setValue(_T("randomize")       ,m_randomizationMethod         );
  key.setValue(_T("seed")            ,m_seed                        );
  key.setValue(_T("periodcount")     ,m_periodCount                 );
  key.setValue(_T("filename")        ,m_fileName                    );
}

void InitializeParameters::readTextFile(const String &fileName) {
  FILE *f = FOPEN(fileName, _T("r"));

  TCHAR line[100];
  CompactIntArray data;
  try {
    int lineCount = 0;
    while(FGETS(line, ARRAYSIZE(line), f)) {
      lineCount++;
      if(data.size() >= MAXDATACOUNT) {
        throwException(_T("Too many data. Max = %d"), MAXDATACOUNT);
      }
      double x;
      int i;
      if(_stscanf(line,_T("%le"),&x) == 1 && x >= 0) {
        if(x > INT_MAX) {
          i = INT_MAX;
        } else {
          i = (int)floor(x);
        }
        data.add(i);
      } else {
        throwException(_T("Illegal input in line %d:<%s>. Must be non-negative number."), lineCount, line);
      }
    }
    if(data.size() == 0) {
      throwException(_T("No data read"));
    }
    fclose(f);
    f = NULL;
    m_fileData     = data;
    m_elementCount = m_fileData.size();
    m_fileName     = fileName;
  } catch(...) {
    if(f) {
      fclose(f);
    }
    throw;
  }
}
