#include "stdafx.h"

#define printSize(t) _tprintf(_T("sizeof(%-20s) = %d\n"), _T(#t), (UINT)sizeof(t))

void testBtree() {
  try {
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

    testDbAddrFileFormat();
    testTupleField();
    testKeyDef();
    testKeyPage();
    testDbFile();
    testKeyFile();
    testDataFile();
    testDatabase();
    testUserRecord();

  } catch(Exception e) {
    _ftprintf(stderr,_T("\n%s\n"),e.what());
    PAUSE();
  } catch(sqlca ca) {
    _ftprintf(stderr,_T("\n"));
    ca.dump(stderr);
    PAUSE();
  }
}

int _tmain(int argc, TCHAR **argv) {
  testBtree();
  return 0;

}
