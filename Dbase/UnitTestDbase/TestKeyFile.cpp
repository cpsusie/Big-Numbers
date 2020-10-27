#include "stdafx.h"
#include <FileNameSplitter.h>
#include <ProcessTools.h>
#include "DbFileTester.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestKeyFile {

#define TESTFILENAME _T("testKeyFile.key")

#include <UnitTestTraits.h>

  class KeyFileTester : DbFileTester {
  private:
    KeyFile              *m_keyFile;
    CompactArray<KeyType> m_keyList;
    CompactArray<KeyType> m_backup;
    KeyFileDefinition     m_keydef;
    KeyComparator         m_comparator;
    void sortList() {
      m_keyList.sort(m_comparator);
    }
    KeyType searchMin(RelationType relop, const KeyType &key);
    KeyType searchMax(RelationType relop, const KeyType &key);
  public:
    KeyFileTester(bool useTransaction, const KeyFileDefinition &keydef);
    ~KeyFileTester();
    void insert(const KeyType &key);
    void remove(UINT i);
    void checkEquals(int line, const KeyType &k1, const KeyType &k2);
    void checkData(int line);

    void testCursor(RelationType beginRelOp, size_t beginIndex, int beginFieldCount, RelationType endRelOp, size_t endIndex, int endFieldCount, SortDirection dir);
    void searchMinTest1();
    void searchMinTest2();
    void searchMinTest3();
    void searchMinTest4();
    void searchMinTest5();

    void searchMaxTest1();
    void searchMaxTest2();
    void searchMaxTest3();
    void searchMaxTest4();
    void searchMaxTest5();

    size_t size() const {
      return m_keyList.size();
    }
    void beginTrans();
    void commitTrans();
    void abortTrans();
    void dump(const String &fileName);
  };

  KeyFileTester::KeyFileTester(bool useTransaction, const KeyFileDefinition &keydef) : DbFileTester(useTransaction), m_comparator(keydef) {
    if(DbFile::exist(TESTFILENAME))
      DbFile::destroy(TESTFILENAME);

    KeyFile::create(TESTFILENAME,keydef);

    m_keyFile = new KeyFile(TESTFILENAME,DBFMODE_READWRITE,m_logFile);
    m_keydef = keydef;
  }

  KeyFileTester::~KeyFileTester() {
    delete m_keyFile;
    DbFile::destroy(TESTFILENAME);
  }

  void KeyFileTester::insert(const KeyType &key) {
    m_keyFile->insert(key);
    m_keyList.binaryInsert(key,m_comparator);
  }

  void KeyFileTester::remove(UINT i) {
    KeyType key = m_keyList[i];
    try {
      m_keyFile->remove(key);
    } catch(sqlca ca) {
      OUTPUT(_T("Remove %s failed:%s"), m_keydef.sprintf(key).cstr(), ca.toString().cstr());
      dump(_T("c:\\temp\\dump.txt"));
      verify(false);
    }
    m_keyList.remove(i);
  }

  KeyType KeyFileTester::searchMin(RelationType relop, const KeyType &key) {
    KeyType result(key);
    verify(m_keyFile->searchMin(relop,result,m_keydef.getKeyFieldCount()));
    return result;
  }

  KeyType KeyFileTester::searchMax(RelationType relop, const KeyType &key) {
    KeyType result(key);
    verify(m_keyFile->searchMax(relop,result,m_keydef.getKeyFieldCount()));
    return result;
  }

  void KeyFileTester::checkEquals(int line, const KeyType &k1, const KeyType &k2) {
    if(m_comparator.compare(k1, k2) != 0) {
      OUTPUT(_T("%s line %d\nkey1 = <%s> != key2 = <%s>"), __TFUNCTION__,line,m_keydef.sprintf(k1).cstr(), m_keydef.sprintf(k2).cstr());
      dump(_T("c:\\temp\\dump"));
      verify(false);
    }
  }

  void KeyFileTester::checkData(int line) {
    KeyCursor cursorDesc(*m_keyFile,RELOP_TRUE,nullptr,0,RELOP_TRUE,nullptr,0,SORT_DESCENDING);
    intptr_t i = m_keyList.size()-1;
    while(cursorDesc.hasNext()) {
      KeyType key;
      cursorDesc.next(key);
      verify(i >= 0);
      checkEquals(line, key,m_keyList[i]);
      i--;
    }
    verify(i == -1);
  }

  void KeyFileTester::beginTrans() {
    DbFileTester::beginTrans();
    m_backup = m_keyList;
  }

  void KeyFileTester::commitTrans() {
    DbFileTester::commitTrans();
    m_backup.clear();
  }

  void KeyFileTester::abortTrans() {
    DbFileTester::abortTrans();
    m_keyList = m_backup;
  }

  void KeyFileTester::dump(const String &fileName) {
    FileNameSplitter info(fileName);
    info.setExtension(_T("txt1"));
    FILE *f = MKFOPEN(info.getFullPath(),_T("w"));
    m_keyFile->dump(f,FULL_DUMP);
    fclose(f);
    info.setExtension(_T("txt2"));
    f = MKFOPEN(info.getFullPath(),_T("w"));
    for(size_t i = 0; i < m_keyList.size(); i++) {
      const KeyType &key = m_keyList[i];
      m_keydef.fprintf(f,key); _ftprintf(f,_T("\n"));
    }
    fclose(f);
  }

  void KeyFileTester::searchMinTest1() {
    OUTPUT(_T("searchMinTest1"));
    for(size_t i = 0; i < m_keyList.size(); i++) {
      const KeyType key = searchMin(RELOP_GE,m_keyList[i]);
      checkEquals(__LINE__,key,m_keyList[i]);
    }
  }

  void KeyFileTester::searchMinTest2() {
    OUTPUT(_T("searchMinTest2"));
    for(size_t i = 0; i < m_keyList.size() - 1; i++) {
      String keyiStr  = m_keydef.sprintf(m_keyList[i]);
      String keyi1Str = m_keydef.sprintf(m_keyList[i+1]);
      const KeyType key = searchMin(RELOP_GT,m_keyList[i]);
      checkEquals(__LINE__,key,m_keyList[i+1]);
    }
  }

  void KeyFileTester::searchMinTest3() {
    OUTPUT(_T("searchMinTest3"));
    for(size_t i = 0; i < m_keyList.size(); i++) {
      const KeyType key = searchMin(RELOP_LE,m_keyList[i]);
      checkEquals(__LINE__,key,m_keyList[0]);
    }
  }

  void KeyFileTester::searchMinTest4() {
    OUTPUT(_T("searchMinTest4"));
    for(size_t i = 1; i < m_keyList.size(); i++) {
      const KeyType key = searchMin(RELOP_LT,m_keyList[i]);
      checkEquals(__LINE__,key,m_keyList[0]);
    }
  }

  void KeyFileTester::searchMinTest5() {
    OUTPUT(_T("searchMinTest5"));
    KeyType key0 = m_keyList[0];
    verify(!m_keyFile->searchMin(RELOP_LT,key0,m_keydef.getKeyFieldCount()));
    KeyType keyN = m_keyList.last();
    verify(!m_keyFile->searchMin(RELOP_GT,keyN,m_keydef.getKeyFieldCount()));
  }

  void KeyFileTester::searchMaxTest1() {
    OUTPUT(_T("searchMaxTest1"));
    for(size_t i = 0; i < m_keyList.size(); i++) {
      const KeyType key = searchMax(RELOP_LE,m_keyList[i]);
      checkEquals(__LINE__,m_keyList[i],key);
    }
  }

  void KeyFileTester::searchMaxTest2() {
    OUTPUT(_T("searchMaxTest2"));
    for(size_t i = 1; i < m_keyList.size(); i++) {
      const KeyType key = searchMax(RELOP_LT,m_keyList[i]);
      checkEquals(__LINE__,key,m_keyList[i-1]);
    }
  }

  void KeyFileTester::searchMaxTest3() {
    OUTPUT(_T("searchMaxTest3"));
    for(size_t i = 0; i < m_keyList.size(); i++) {
      const KeyType key = searchMax(RELOP_GE,m_keyList[i]);
      checkEquals(__LINE__,key,m_keyList.last());
    }
  }

  void KeyFileTester::searchMaxTest4() {
    OUTPUT(_T("searchMaxTest4"));
    for(size_t i = 0; i < m_keyList.size() - 1; i++) {
      const KeyType key = searchMax(RELOP_GT,m_keyList[i]);
      checkEquals(__LINE__,key,m_keyList.last());
    }
  }

  void KeyFileTester::searchMaxTest5() {
    OUTPUT(_T("searchMaxTest5"));
    KeyType key0 = m_keyList[0];
    verify(!m_keyFile->searchMax(RELOP_LT,key0,m_keydef.getKeyFieldCount()));
    KeyType keyN = m_keyList.last();
    verify(!m_keyFile->searchMax(RELOP_GT,keyN,m_keydef.getKeyFieldCount()));
  }

  void KeyFileTester::testCursor(RelationType beginRelOp, size_t beginIndex, int beginFieldCount, RelationType endRelOp, size_t endIndex, int endFieldCount, SortDirection dir) {
    OUTPUT(_T("testCursor:%-5s,%5zu,%3d,%-5s,%5zu,%3d %s")
            ,relOpString(beginRelOp),beginIndex,beginFieldCount
            ,relOpString(endRelOp)  ,endIndex  ,endFieldCount
            ,sortDirString(dir));
    KeyType *beginKey = nullptr;
    KeyType *endKey   = nullptr;

    if(m_keyList.size() > 0) {
      beginKey = &m_keyList[beginIndex];
      endKey   = &m_keyList[endIndex];
    }

    intptr_t start,end;
    try {
      switch(dir) {
      case SORT_ASCENDING :
        { switch(beginRelOp) {
          case RELOP_GE    : start = beginIndex;       break;
          case RELOP_GT    : start = beginIndex + 1;   break;
          case RELOP_EQ    : start = beginIndex;       break;
          case RELOP_TRUE  : start = 0;                break;
          case RELOP_FALSE : start = 1;                break;
          default          : start = -1;               break; // Exception expected.
          }
          switch(endRelOp) {
          case RELOP_LE    : end   = endIndex;         break;
          case RELOP_LT    : end   = endIndex - 1;     break;
          case RELOP_EQ    : end   = beginIndex;       break;
          case RELOP_TRUE  : end   = m_keyList.size()-1; break;
          case RELOP_FALSE : end   = 0;                break;
          default          : end   = -1;               break; // Exception expected.
          }
          intptr_t i = start;
          KeyCursor cursor(*m_keyFile,beginRelOp,beginKey,beginFieldCount, endRelOp,endKey,endFieldCount, dir);
          KeyType key;
          while(cursor.hasNext()) {
            cursor.next(key);
            verify(i <= end);
            checkEquals(__LINE__,key,m_keyList[i]);
            i++;
          }
          verify(i == end+1);
        }
        break;
      case SORT_DESCENDING:
        { switch(beginRelOp) {
          case RELOP_LE    : start = beginIndex;       break;
          case RELOP_LT    : start = beginIndex - 1;   break;
          case RELOP_EQ    : start = beginIndex;       break;
          case RELOP_TRUE  : start = m_keyList.size()-1; break;
          case RELOP_FALSE : start = 1;                break;
          default          : start = -1;               break; // Exception expected.
          }
          switch(endRelOp) {
          case RELOP_GE    : end   = endIndex;         break;
          case RELOP_GT    : end   = endIndex + 1;     break;
          case RELOP_EQ    : end   = beginIndex;       break;
          case RELOP_TRUE  : end   = 0;                break;
          case RELOP_FALSE : end   = 2;                break;
          default          : end   = -1;               break; // Exception expected.
          }
          intptr_t i = start;
          KeyCursor cursor(*m_keyFile,beginRelOp,beginKey,beginFieldCount, endRelOp,endKey,endFieldCount, dir);
          KeyType key;
          while(cursor.hasNext()) {
            cursor.next(key);
            verify(i >= end);
            checkEquals(__LINE__,key,m_keyList[i]);
            i--;
          }
          verify(i == end-1);
        }
        break;
      }
    } catch(sqlca ca) {
      switch(ca.sqlcode) {
      case SQL_FATAL_ERROR      :
        verify((start == -1) || (end == -1));
        break;
      case SQL_INVALID_KEYCOUNT :
        verify((beginFieldCount > (int)m_keydef.getKeyFieldCount()) || (endFieldCount > (int)m_keydef.getKeyFieldCount()));
        break;
      default:
        ca.dump();
        verify(false);
        break;
      }
    }
  }

  TEST_CLASS(TestKeyFile) {
  public:
    TEST_METHOD(testSimpleKeyFile) {
      TestFields fields(false);

      OUTPUT(_T("Inserting data"));

      KeyFileTester test(false,fields);
      for(int i = 0; i < 10000; i++) {
        KeyType key = fields.getRandomKey();
        test.insert(key);
    //    fields.keyprintf(key); OUTPUT(_T("\n"));
      }
      OUTPUT(_T("10000 keys inserted"));
      test.checkData(__LINE__);

      test.searchMinTest1();
      test.searchMinTest2();
      test.searchMinTest3();
      test.searchMinTest4();
      test.searchMinTest5();

      test.searchMaxTest1();
      test.searchMaxTest2();
      test.searchMaxTest3();
      test.searchMaxTest4();
      test.searchMaxTest5();

      int fieldCount = fields.getKeyFieldCount();

      test.testCursor(RELOP_TRUE ,0            ,0         ,RELOP_TRUE ,0            ,0         ,SORT_ASCENDING );
      test.testCursor(RELOP_GE   ,1            ,fieldCount,RELOP_TRUE ,0            ,0         ,SORT_ASCENDING );
      test.testCursor(RELOP_TRUE ,0            ,0         ,RELOP_LE   ,test.size()-1,fieldCount,SORT_ASCENDING );
      test.testCursor(RELOP_GE   ,1            ,fieldCount,RELOP_LE   ,test.size()-1,fieldCount,SORT_ASCENDING );
      test.testCursor(RELOP_GT   ,1            ,fieldCount,RELOP_LT   ,test.size()-1,fieldCount,SORT_ASCENDING );
      test.testCursor(RELOP_EQ   ,1            ,fieldCount,RELOP_EQ   ,1            ,fieldCount,SORT_ASCENDING );
      test.testCursor(RELOP_FALSE,1            ,fieldCount,RELOP_FALSE,1            ,fieldCount,SORT_ASCENDING );
      test.testCursor(RELOP_LE   ,1            ,fieldCount,RELOP_TRUE ,1            ,fieldCount,SORT_ASCENDING );

      test.testCursor(RELOP_TRUE ,0            ,0         ,RELOP_TRUE ,0            ,0         ,SORT_DESCENDING);
      test.testCursor(RELOP_LE   ,1            ,fieldCount,RELOP_TRUE ,0            ,0         ,SORT_DESCENDING);
      test.testCursor(RELOP_TRUE ,test.size()-1,0         ,RELOP_GE   ,0            ,fieldCount,SORT_DESCENDING);
      test.testCursor(RELOP_LE   ,test.size()-1,fieldCount,RELOP_GE   ,1            ,fieldCount,SORT_DESCENDING);
      test.testCursor(RELOP_LT   ,test.size()-1,fieldCount,RELOP_GT   ,1            ,fieldCount,SORT_DESCENDING);
      test.testCursor(RELOP_EQ   ,1            ,fieldCount,RELOP_EQ   ,1            ,fieldCount,SORT_DESCENDING);
      test.testCursor(RELOP_FALSE,1            ,fieldCount,RELOP_FALSE,1            ,fieldCount,SORT_DESCENDING);
      test.testCursor(RELOP_GE   ,1            ,fieldCount,RELOP_TRUE ,1            ,fieldCount,SORT_DESCENDING);

      while(test.size() > 0) {
        OUTPUT(_T("testKeyFile keyList.size:%u"),(UINT)test.size());
        int n = min((int)test.size(),1000);
        for(int i = 0; i < n; i++) {
          const UINT index = randInt((UINT)test.size());
          test.remove(index);
          if(test.size() == 0) {
            break;
          }
        }
        test.checkData(__LINE__);
      }
      test.testCursor(RELOP_TRUE ,0            ,0         ,RELOP_TRUE ,0            ,0         ,SORT_ASCENDING );
      test.testCursor(RELOP_TRUE ,0            ,0         ,RELOP_TRUE ,0            ,0         ,SORT_DESCENDING);
    }

    TEST_METHOD(testBackLoggedKeyFile) {
      TestFields fields(false);

      OUTPUT(_T("Inserting data"));

      KeyFileTester test(true, fields);

      try {
        test.insert(fields.getRandomKey());
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_NO_TRANSACTION);
      }
      test.checkData(__LINE__);

      test.beginTrans();
      int i;
      for(i = 0; i < 100; i++) {
        KeyType key = fields.getRandomKey();
        test.insert(key);
      }
      test.abortTrans();
      test.checkData(__LINE__);

      test.beginTrans();
      for(i = 0; i < 100; i++) {
        KeyType key = fields.getRandomKey();
        test.insert(key);
      }
      test.commitTrans();
      test.checkData(__LINE__);

      test.beginTrans();
      while(test.size() > 0) {
        int index = rand() % test.size();
        test.remove(index);
      }
      test.commitTrans();
      test.checkData(__LINE__);
    }

    TEST_METHOD(testKeyFile) {
      double start = getProcessTime();
      testSimpleKeyFile();
      testBackLoggedKeyFile();
      OUTPUT(_T("Process time:%6.3lf"), (getProcessTime() - start) / 1000000);
    }
  };
}
