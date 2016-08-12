#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math/FFT.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestFFT {		

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }


	TEST_CLASS(TestFFT)	{
    public:

    TEST_METHOD(FFTTest) {
#define EPS 1e-14

        CompactArray<Complex> data;

        data.add(Complex(1, 0));
        data.add(Complex(1, 1));
        data.add(Complex(0, 0));
        data.add(Complex(1, -1));
        data.add(Complex(0, 0));
        data.add(Complex(1, 1));
        data.add(Complex(0, 0));
        data.add(Complex(1, -1));

        CompactArray<Complex> expected(data.size());

        expected.add(Complex(5, 0));
        expected.add(Complex(1, 0));
        expected.add(Complex(-3, 0));
        expected.add(Complex(1, 0));
        expected.add(Complex(-3, 0));
        expected.add(Complex(1, 0));
        expected.add(Complex(5, 0));
        expected.add(Complex(1, 0));

        CompactArray<Complex> copy = data;

        // test DFT
        fft(data, true);

        verify(data.size() == expected.size());
        for (size_t i = 0; i < data.size(); i++) {
          verify(arg(data[i] - expected[i]) < EPS);
        }

        fft(data, false);

        verify(data.size() == copy.size());
        for (size_t i = 0; i < data.size(); i++) {
          verify(arg(data[i] - copy[i]) < EPS);
        }

        // test DFT
        data = copy;

        dft(data, true);

        verify(data.size() == expected.size());
        for (size_t i = 0; i < data.size(); i++) {
          verify(arg(data[i] - expected[i]) < EPS);
        }

        dft(data, false);

        verify(data.size() == copy.size());
        for (size_t i = 0; i < data.size(); i++) {
          verify(arg(data[i] - copy[i]) < EPS);
        }
      }
  };
}