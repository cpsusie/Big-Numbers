#include "stdafx.h"
#include "CppUnitTest.h"
#include <MemSwap.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSwap {

#include <UnitTestTraits.h>

  TEST_CLASS(TestSwapBuffers) {
  public:

    TEST_METHOD(SwapBuffers) {
      BYTE data1[256], data2[256];
#define BUFIZE sizeof(data1)
      for(int i = 0; i < BUFIZE; i++) {
        data1[i] = i;
        data2[i] = 255-i;
      }

      for (size_t i = 1; i <= BUFIZE; i++) {
        BYTE buf1[256], buf2[256];
        memcpy(buf1, data1, BUFIZE);
        memcpy(buf2, data2, BUFIZE);
        memSwap(buf1, buf2, i);
        verify(memcmp(buf1, data2, i) == 0);
        verify(memcmp(buf2, data1, i) == 0);
        if(i < BUFIZE) {
          verify(memcmp(buf1+i,data1+i,BUFIZE-i) == 0);
          verify(memcmp(buf2+i,data2+i,BUFIZE-i) == 0);
        }
      }
    }
  };
}
