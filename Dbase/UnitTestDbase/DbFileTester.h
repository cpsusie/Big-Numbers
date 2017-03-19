#pragma once

#include <Random.h>

class RandomData : public Random {
public:
  String    nextString(int maxLen);
  char      nextChar();
  UCHAR     nextUChar();
  short     nextShort();
  USHORT    nextUShort();
  UINT      nextUInt();
  UINT64    nextUInt64();
  Date      nextDate();
  Time      nextTime();
  Timestamp nextTimestamp();
};

#define TESTFIELD_NOT_NULL 0x1
#define TESTFIELD_NULL     0x2
#define TESTFIELD_SIGNED   0x4
#define TESTFIELD_UNSIGNED 0x8

#define TESTFIELD_ALL      (TESTFIELD_NOT_NULL | TESTFIELD_NULL | TESTFIELD_SIGNED | TESTFIELD_UNSIGNED)

class TestFields : public KeyFileDefinition {
private:
  void setRandom();
  void initDefinition(bool unique, int flags);
  KeyType m_key;
  static RandomData random;
public:
  TestFields(bool unique = false, int flags = TESTFIELD_NOT_NULL | TESTFIELD_SIGNED | TESTFIELD_UNSIGNED);
  TestFields(const KeyFileDefinition &keydef);

  const KeyType &getKey() const {
    return m_key;
  }
  
  TupleField getRandomField(UINT field);
  KeyType    getRandomKey();
  Tuple      getRandomTuple();

  void incrField(UINT field);
  void incrAll();

  static RandomData &getRandom() {
    return random;
  }
};

class DbFileTester {
protected:
  LogFile *m_logFile;
public:
  DbFileTester(bool useTransaction = false);
  ~DbFileTester();
  void beginTrans();
  void commitTrans();
  void abortTrans();
};

class KeyComparator : public Comparator<KeyType> {
private:
  KeyFileDefinition m_keydef;
public:
  KeyComparator(const KeyFileDefinition &keydef) {
    m_keydef = keydef;
  }

  int compare(const KeyType &k1, const KeyType &k2) {
    return m_keydef.keyCmp(k1,k2);
  }

  AbstractComparator *clone() const {
    return new KeyComparator(m_keydef);
  }

  const KeyFileDefinition &getKeydef() const {
    return m_keydef;
  }
};

void createDatabase(const String &dbname);

void testTupleField();
void testKeyDef();
void testKeyPage();
void testDbFile();
void testKeyFile();
void testDataFile();
void testLogFile();
void testDatabase();
void testBtree();
void testUserRecord();

void testDbAddrFileFormat();

void createDatabase(const String &dbname);
