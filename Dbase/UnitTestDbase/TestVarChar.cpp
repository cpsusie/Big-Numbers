#include "stdafx.h"
#include "CppUnitTest.h"
#include <Btree.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestVarChar {

#include <UnitTestTraits.h>

  TEST_CLASS(TestVarChar) {

    public:
      TEST_METHOD(TestVarCharPrimitives) {
        const char *cstr1 = "fisk";
        varchar cvchar(cstr1);
        verify((cvchar.len() == strlen(cstr1)) && (strcmp((char*)cvchar.data(), cstr1) == 0));
        const char *cstr2 = "sdfljkghsdfg";
        cvchar = cstr2;
        verify((cvchar.len() == strlen(cstr2)) && (strcmp((char*)cvchar.data(), cstr2) == 0));

        const wchar_t *wstr1 = L"fisk";
        varchar wvchar(wstr1);
        verify((wvchar.len() == wcslen(wstr1)*sizeof(wchar_t)) && (wcscmp((wchar_t*)wvchar.data(), wstr1) == 0));
        const wchar_t *wstr2 = L"sdfljkghsdfg";
        wvchar = wstr2;
        verify((wvchar.len() == wcslen(wstr2)*sizeof(wchar_t)) && (wcscmp((wchar_t*)wvchar.data(), wstr2) == 0));
      }
    };
}
