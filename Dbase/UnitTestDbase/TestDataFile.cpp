#include "stdafx.h"
#include "DbFileTester.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestDataFile {

#include <UnitTestTraits.h>

#define TESTFILENAME _T("testDataFile.dat")

  class RecordData {
  private:
    char   m_record[MAXRECSIZE];
    int    m_size;
  public:
    DbAddr m_addr;
    RecordData() : m_size(0) {}
    RecordData(BYTE pattern, UINT size);
    int   getSize() const { return m_size; }
    void  setPattern(BYTE pattern);
    const char *getRecord();
    int   cmp(const char *rec);
  };

  RecordData::RecordData(BYTE pattern, UINT size) { 
    verify(size <= sizeof(m_record));
    m_size = size;
    setPattern(pattern);
  }

  void RecordData::setPattern(BYTE pattern) {
    memset(m_record,pattern,m_size);
  }

  const char *RecordData::getRecord() {
    return m_record;
  }

  int RecordData::cmp(const char *rec) {
    return memcmp(m_record,rec,m_size);
  }

  class DataFileTester : DbFileTester {
  private:
    DataFile         *m_dataFile;
    CompactArray<RecordData> m_dataList;
    CompactArray<RecordData> m_backup;
  public:
    DataFileTester(bool useTransaction = false);
    ~DataFileTester();
    void insert(BYTE pattern, UINT size);
    void update(UINT index, BYTE pattern);
    void remove(UINT index);
    void checkData();
    UINT size() const {
      return (UINT)m_dataList.size();
    }
    void beginTrans();
    void commitTrans();
    void abortTrans();
    UINT randomRecordSize() const;
  };

  DataFileTester::DataFileTester(bool useTransaction) : DbFileTester(useTransaction) {
    if(DbFile::exist(TESTFILENAME)) {
      DbFile::destroy(TESTFILENAME);
    }

    DataFile::create(TESTFILENAME);
    m_dataFile = new DataFile(TESTFILENAME,DBFMODE_READWRITE,m_logFile);
  }

  DataFileTester::~DataFileTester() {
    delete m_dataFile;
    DbFile::destroy(TESTFILENAME);
  }

  void DataFileTester::insert(BYTE pattern, UINT size) {
    RecordData rd(pattern,size);
    rd.m_addr = m_dataFile->insert(rd.getRecord(), rd.getSize());
    m_dataList.add(rd);
  }

  void DataFileTester::update(UINT index, BYTE pattern) {
    RecordData rd = m_dataList[index];
    rd.setPattern(pattern);
    m_dataFile->update(rd.m_addr,rd.getRecord());
    m_dataList[index] = rd;
  }

  void DataFileTester::remove(UINT index) {
    RecordData &rd = m_dataList[index];
    m_dataFile->remove(rd.m_addr);
    m_dataList.remove(index);
  }

  void DataFileTester::checkData() {
    for(size_t i = 0; i < m_dataList.size(); i++) {
      RecordData &rd = m_dataList[i];
      char rec[MAXRECSIZE];
      memset(rec,0,sizeof(rec));
      m_dataFile->readRecord(rd.m_addr,rec, sizeof(rec));
      verify(rd.cmp(rec) == 0);
    }
  }

  void DataFileTester::beginTrans() {
    DbFileTester::beginTrans();
    m_backup = m_dataList;
  }

  void DataFileTester::commitTrans() {
    DbFileTester::commitTrans();
    m_backup.clear();
  }

  void DataFileTester::abortTrans() {
    DbFileTester::abortTrans();
    m_dataList = m_backup;
  }

  UINT DataFileTester::randomRecordSize() const {
    return randInt(1, MAXRECSIZE/2);
  }

  TEST_CLASS(TestDataFile) {
    public:

#define PATTERN1 0xaa
#define PATTERN2 0x55

    TEST_METHOD(testSimpleDataFile) {
      DataFileTester test;
      UINT i;

      for(i = 0; i < 1000; i++) {
        test.insert(PATTERN1,test.randomRecordSize());
      }
      test.checkData();

      for(i = 0; i < test.size(); i += 2) {
        test.update(i, PATTERN2);
      }
      test.checkData();

      while(test.size() > 10) {
        const UINT n = test.size() / 4;
        for(i = 0; i < n; i++) {
          const int d = randInt(test.size());
          test.remove(d);
        }
        test.insert(PATTERN1,test.randomRecordSize());
        test.checkData();
      }
    }

    TEST_METHOD(testBackLoggedDataFile) {
      DataFileTester test(true);

      try {
        test.insert(PATTERN1,100);
        verify(false);
      } catch(sqlca ca) {
        verify(ca.sqlcode == SQL_NO_TRANSACTION);
      }
      test.checkData();

      test.beginTrans();
      for(UINT size = 0; size < MAXRECSIZE; size+=500) {
        test.insert(PATTERN1,size);
      }
      test.abortTrans();
      test.checkData();

      test.beginTrans();
      for(UINT size = 0; size < MAXRECSIZE; size+=500) {
        test.insert(PATTERN1,size);
      }
      test.commitTrans();
      test.checkData();

      test.beginTrans();
      for(UINT i = 0; i < test.size() / 2; i++) {
        test.remove(i);
      }
      test.abortTrans();
      test.checkData();

      test.beginTrans();
      for(size_t i = test.size(); i--;) {
        test.remove((UINT)i);
      }
      test.commitTrans();
      test.checkData();
    }
  };
}
