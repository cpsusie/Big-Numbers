#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math/Int128.h>
#include <limits.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestInt128 {		

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  TEST_CLASS(TesSomeUnit) {
    public:

    TEST_METHOD(TestPrimiteveOperations) {
      int              minI32  = _I32_MIN;
      int              maxI32  = _I32_MAX;
      UINT             maxUI32 = _UI32_MAX;
      __int64          minI64  = _I64_MIN;
      __int64          maxI64  = _I64_MAX;
      unsigned __int64 maxUI64 = _UI64_MAX;

      _int128 i1 = minI32;
      verify(i1 == minI32);
      _int128 i2 = maxI32;
      verify(i2 == maxI32);

      _int128 i3 = minI64;
      verify(i3 == minI64);
      _int128 i4 = maxI64;
      verify(i4 == maxI64);


    }

  };
}