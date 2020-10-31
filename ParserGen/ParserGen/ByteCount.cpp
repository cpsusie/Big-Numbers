#include "stdafx.h"
#include "ByteCount.h"

const ByteCount ByteCount::s_pointerSize(4,8); // sizeof(void*) in x86 and x64

IntegerType findUintType(UINT maxValue) { // static
  if(maxValue <= UCHAR_MAX) {
    return TYPE_UCHAR;
  } else if(maxValue <= USHRT_MAX) {
    return TYPE_USHORT;
  } else {
    return TYPE_UINT;
  }
}

const TCHAR *getTypeName(IntegerType type) { // static
  switch(type) {
  case TYPE_CHAR  : return _T("char"          );
  case TYPE_UCHAR : return _T("unsigned char" );
  case TYPE_SHORT : return _T("short"         );
  case TYPE_USHORT: return _T("unsigned short");
  case TYPE_INT   : return _T("int"           );
  case TYPE_UINT  : return _T("unsigned int"  );
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
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
