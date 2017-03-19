#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestDbFile {

  TEST_CLASS(TestDbFile) {
    public:

#include "UnitTestTraits.h"

    TEST_METHOD(testDbFile) {
      Array<DbFile> fileTable;
      for(int i = 0; i < 30; i++) {
        fileTable.add(DbFile(format(_T("fisk%04d"),i),DBFMODE_READWRITE,NULL));
      }

      for(size_t i = 0; i < fileTable.size(); i++) {
        if(!DbFile::exist(fileTable[i].getName()))
          DbFile::create(fileTable[i].getName());
      }

      char buf[100];
      memset(buf,0,sizeof(buf));
      for(int i = 0; i < 2000; i++) {
        _tprintf(_T("write %d\r"),i);
        DbFile &f = fileTable[rand() % fileTable.size()];
        f.write(0,buf,sizeof(buf));
      }

      for(size_t i = 0; i < fileTable.size(); i++) {
        DbFile::destroy(fileTable[i].getName());
      }

      try {
        DbFile tmp(_T("test.dat"),DBFMODE_READONLY,NULL);
        tmp.write(0,buf,sizeof(buf));
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_WRITE_ERROR);
      }
    }
  };
}