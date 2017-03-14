#include "stdafx.h"

#ifdef BIG_DBADDR

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static DbAddrFileFormat &makeDbAddrFileFormatBigEndian(DbAddrFileFormat &dst, const DbAddr &src) {
  if(src >= (1ui64 << 48)) {
    throwException(_T("Address to big to fit in %d bytes:%s"),DBADDRSIZE, toString(src,0,0,std::ios::showbase|std::ios::hex).cstr());
  }
  return *(DbAddrFileFormat*)memcpy(&dst,((char*)&src)+2,DBADDRSIZE);
}

static DbAddr &makeDbAddrBigEndian(DbAddr &dst, const DbAddrFileFormat &src) {
  dst = 0;
  memcpy(((char*)&dst)+2,&src,6);
  return dst;
}

static DbAddrFileFormat &makeDbAddrFileFormatLittleEndian(DbAddrFileFormat &dst, const DbAddr &src) {
  if(src >= (1ui64 << 48)) {
    throwException(_T("Address to big to fit in %d bytes:%s"),DBADDRSIZE, toString(src,0,0,std::ios::showbase|std::ios::hex).cstr());
  }
  return *(DbAddrFileFormat*)memcpy(&dst,&src,DBADDRSIZE);
}

static DbAddr &makeDbAddrLittleEndian(DbAddr &dst, const DbAddrFileFormat &src) {
  dst = 0;
  memcpy(&dst,&src,DBADDRSIZE);
  return dst;
}

static DbAddrFileFormat &(*makeDbAddrFileFormat)(DbAddrFileFormat &dst, const DbAddr           &src);
static DbAddr           &(*makeDbAddr)(          DbAddr           &dst, const DbAddrFileFormat &src);

typedef union {
  UCHAR bytes[8];
  UINT64 value;
} byte8;

class ByteOrderChecker {
public:
  ByteOrderChecker();
};

ByteOrderChecker::ByteOrderChecker() {
  byte8 b;
  for(int i = 0; i < ARRAYSIZE(b.bytes); i++) {
    b.bytes[i] = i+1;
  }
  switch(b.value) {
  case 0x0102030405060708ui64:
    makeDbAddrFileFormat = makeDbAddrFileFormatBigEndian;
    makeDbAddr           = makeDbAddrBigEndian;
    break;

  case 0x0807060504030201ui64:
    makeDbAddrFileFormat = makeDbAddrFileFormatLittleEndian;
    makeDbAddr           = makeDbAddrLittleEndian;
    break;

  default:
    throwException(_T("Unknown byte order"));
  }
}

static ByteOrderChecker initializer;

DbAddrFileFormat &DbAddrFileFormat::operator=(const DbAddr &addr) {
  return makeDbAddrFileFormat(*this,addr);
}

DbAddrFileFormat::operator DbAddr() const {
  DbAddr addr;
  return makeDbAddr(addr,*this);
}

#endif
