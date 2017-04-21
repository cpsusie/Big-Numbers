#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestDbAddrFileFormat {

#include <UnitTestTraits.h>

  TEST_CLASS(TestDbAddrFileFormat) {

    public:

      TEST_METHOD(testDbAddrFileFormat) {
#ifdef BIG_DBADDR
#define MAXADDR ((1ui64 << 48) - 1)
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
    };
}