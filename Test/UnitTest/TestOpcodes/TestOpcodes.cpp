#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestOpcodes {		

#include <UnitTestTraits.h>

  static void vprint(const TCHAR *format, va_list argptr) {
    OUTPUT(_T("%s"),vformat(format,argptr).cstr());
  }

  TEST_CLASS(TestMachineCode) {
	public:
		

    TEST_METHOD(GenerateTestSequence) {
      try {
        generateTestSequence(vprint);
      } catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
      }
    }
    TEST_METHOD(CallAssemblerCode) {
      callAssemblerCode();
    }


	};
}
