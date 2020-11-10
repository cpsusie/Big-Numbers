#include "stdafx.h"

const ByteCount ByteCount::s_pointerSize(4,8);

ByteCount ByteCount::getAlignedSize() const {
  const int restx86 = m_countx86%4, restx64 = m_countx64%8;
  return ByteCount(restx86 ? (m_countx86 + (4-restx86)) : m_countx86
                  ,restx64 ? (m_countx64 + (8-restx64)) : m_countx64);
}

String ByteCount::toString() const {
  return format(_T("%s(x86)/%s(x64) bytes")
                ,format1000(m_countx86).cstr()
                ,format1000(m_countx64).cstr());
}


static IntegerType findUintType(UINT maxValue) {
  const Options &options = Options::getInstance();
  assert(options.m_language == CPP);
  if(maxValue <= UCHAR_MAX) {
    return TYPE_UCHAR;
  } else if(maxValue <= USHRT_MAX) {
    return TYPE_USHORT;
  } else {
    return TYPE_UINT;
  }
}

IntegerType findIntType(int minValue, int maxValue) {
  const Options &options = Options::getInstance();
  switch(options.m_language) {
  case CPP:
    if(minValue >= 0) {
      return findUintType(maxValue);
    }
    // NB continue case
  case JAVA:
    if((minValue >= CHAR_MIN) && (maxValue <= CHAR_MAX)) {
      return TYPE_CHAR;
    } else if((minValue >= SHRT_MIN) && (maxValue <= SHRT_MAX)) {
      return TYPE_SHORT;
    } else {
      return TYPE_INT;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("language=%d, minValue=%d, maxValue=%d"), options.m_language, minValue, maxValue);
  return TYPE_INT;
}

const TCHAR *getTypeName(IntegerType type) { // static
  const Options &options = Options::getInstance();
  switch(options.m_language) {
  case CPP:
    switch(type) {
    case TYPE_CHAR  : return _T("char"          );
    case TYPE_UCHAR : return _T("unsigned char" );
    case TYPE_SHORT : return _T("short"         );
    case TYPE_USHORT: return _T("unsigned short");
    case TYPE_INT   : return _T("int"           );
    case TYPE_UINT  : return _T("unsigned int"  );
    }
    break;
  case JAVA:
    switch(type) {
    case TYPE_CHAR  : return _T("byte"          );
    case TYPE_SHORT : return _T("short"         );
    case TYPE_INT   : return _T("int"           );
    }
    break;
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("language=%d, type=%d"), options.m_language, type);
  return EMPTYSTRING;
}

UINT getTypeSize(IntegerType type) {
  switch(type) {
  case TYPE_CHAR  :
  case TYPE_UCHAR : return sizeof(char );
  case TYPE_SHORT :
  case TYPE_USHORT: return sizeof(short);
  case TYPE_INT   :
  case TYPE_UINT  : return sizeof(int  );
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
  return 0;
}
