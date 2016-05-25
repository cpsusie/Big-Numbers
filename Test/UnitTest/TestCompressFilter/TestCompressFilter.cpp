#include "stdafx.h"
#include "CppUnitTest.h"
#include <ByteFile.h>
#include <CompressFilter.h>
#include <Random.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestCompressFilter
{		

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  static ByteArray randomByteArray(unsigned int size) {
    BYTE *buf = new BYTE[size];
    for (unsigned int i = 0; i < size; i++) {
      buf[i] = randInt() % 256;
    }
    ByteArray result;
    result.setData(buf, size);
    delete[] buf;
    return result;
  }

  static ByteArray niceByteArray(unsigned int size) {
    BYTE *buf = new BYTE[size];
    int k = 0;
    for (unsigned int i = 0, j = 0, dj = 1; i < size;) {
      buf[i++] = j % 256;
      j += dj;
      if (j == 0) {
        dj = ++k;
      }
      else if (j == 256) {
        dj = -(++k);
      }
      if (k == 128) {
        k = 0;
      }
    }
    ByteArray result;
    result.setData(buf, size);
    delete[] buf;
    return result;
  }


	TEST_CLASS(TestCompressFilter) {
    public:

    TEST_METHOD(testCompressFilter) {
      const double startTime = getProcessTime();
      bool ok = true;
      const String fileName = _T("c:\\temp\\compressedData.dat");
      try {
        for (int i = 0; i < 10; i++) {
          ByteArray a = niceByteArray(100000) + randomByteArray(100000);
          a.save(CompressFilter(ByteOutputFile(fileName)));
          const long fileSize = STAT(fileName).st_size;

          ByteArray b;
          b.load(DecompressFilter(ByteInputFile(fileName)));
          verify(a == b);
          OUTPUT(_T("Raw size:%8ld. Compressed size:%8ld CompressionRate:%8.2lf%%"), a.size(), fileSize, (double)fileSize / a.size() * 100.0);
        }
        unlink(fileName);
      }  catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
      OUTPUT(_T("CompressFilter  Timeusage:%.3lf sec"), (getProcessTime() - startTime) / 1000000);
    }
  };
}
