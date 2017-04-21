#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestBtree12 {

#include <UnitTestTraits.h>

  TEST_CLASS(UnitTest1) {
    public:

    TEST_METHOD(OutputSizes) {

#define printSize(t) OUTPUT(_T("sizeof(%-20s) = %d"), _T(#t), (UINT)sizeof(t))

      printSize(KeyFileHeader);
      printSize(KeyFileDefinition);
      printSize(KeyField);
      printSize(DbAddrFileFormat);
      printSize(DbAddr);
      printSize(DataFileHeader);
      printSize(FreeKey);
      printSize(FreePageItem);
      printSize(FreePage);
      printSize(KeyPageHeader);
      printSize(KeyPageItem);
      printSize(KeyPage);
    }
  };
}