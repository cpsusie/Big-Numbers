#include "stdafx.h"

#define MAXADDR ((1ui64 << 48) - 1)

void testDbAddrFileFormat() {

#ifdef BIG_DBADDR

  DbAddrFileFormat addrff;
  KeyPageAddr addr;
  KeyPageAddr addr1;
  for(addr = 1; addr <= MAXADDR; addr *= 3) {
    addrff = addr;
    addr1  = addrff;
    verify(addr1 == addr);
  }
  addr = 0xf1e2d3c4b5a6ui64;
  addrff = addr;
  addr1  = addrff;
  verify(addr1 == addr);

  addr = MAXADDR + 1;
  try {
    addrff = addr;
    verify(false);
  } catch(Exception e) {
    // ok
  }

#endif

}

