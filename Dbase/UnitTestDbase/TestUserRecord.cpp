#include "stdafx.h"
#include <TreeMap.h>
#include "DbFileTester.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestUserRecord {

#include <UnitTestTraits.h>

#define DBNAME    _T("TEST")
#define TABLENAME _T("testTable")

  bool equals(const Tuple &t1, const Tuple &t2) {
    const size_t size = t1.size();
    if(size != t2.size()) {
      return false;
    }
    for(UINT i = 0; i < size; i++) {
      const TupleField &f1 = t1[i];
      const TupleField &f2 = t2[i];
      if(f1.isDefined() != f2.isDefined()) {
        return false;
      }
      if(f1.isDefined() && compare(f1,f2) != 0) {
        return false;
      }
    }
    return true;
  }

  class IndexComparator : public Comparator<KeyType> {
  private:
    Database &m_db;
    IndexDefinition m_indexDef;
    int m_indexNo;
    KeyComparator *m_comparator;
  public:
    IndexComparator(Database &db, const String &tableName, const String &indexName = EMPTYSTRING);
    IndexComparator(const IndexComparator &src);
   ~IndexComparator();
    int compare(const KeyType &k1, const KeyType &k2) {
      return m_comparator->compare(k1,k2);
    }
    const KeyFileDefinition &getKeydef() const {
      return m_comparator->getKeydef();
    }
    int getIndexNo() const {
      return m_indexNo;
    }
    void setIndexNo(int value) {
      m_indexNo = value;
    }
    const String &getTableName() const {
      return getIndexDefinition().m_tableName;
    }
    const String &getIndexName() const {
      return getIndexDefinition().m_indexName;
    }
    const IndexDefinition &getIndexDefinition() const {
      return m_indexDef;
    }
    AbstractComparator *clone() const {
      return new IndexComparator(m_db,getTableName(),getIndexName());
    }
  };

  IndexComparator::IndexComparator(Database &db, const String &tableName, const String &indexName) : m_db(db) {
    const TableDefinition &tableDef   = m_db.getTableDefinition(tableName);
    const IndexArray      &indexArray = m_db.getIndexDefinitions(tableName);
    if(indexName == EMPTYSTRING) {
      for(UINT i = 0; i < indexArray.size(); i++) {
        const IndexDefinition &indexDef = indexArray[i];
        if(indexDef.m_indexType == INDEXTYPE_PRIMARY) {
          m_indexNo    = i;
          m_indexDef   = indexDef;
          m_comparator = new KeyComparator(tableDef.getKeyFileDefinition(m_indexDef));
          return;
        }
      }
      throwException(_T("No primary index on table <%s>"), tableName.cstr());
    } else {
      for(UINT i = 0; i < indexArray.size(); i++) {
        const IndexDefinition &indexDef = indexArray[i];
        if(indexDef.m_indexName == indexName) {
          m_indexNo    = i;
          m_indexDef   = indexDef;
          m_comparator = new KeyComparator(tableDef.getKeyFileDefinition(m_indexDef));
          return;
        }
      }
      throwException(_T("No index <%s> on table <%s>"), indexName.cstr(),tableName.cstr());
    }
  }

  IndexComparator::IndexComparator(const IndexComparator &src) : m_db(src.m_db) {
    m_indexDef   = src.m_indexDef;
    m_indexNo    = src.m_indexNo;
    m_comparator = new KeyComparator(src.getKeydef());
  }

  IndexComparator::~IndexComparator() {
    delete m_comparator;
  }

  class TableIndex {
  private:
    IndexComparator m_comparator;
  public:
    TreeMap<KeyType,DbAddr> *m_keyMap;
    TableIndex(Database &db, const String &tableName, const String &indexName = EMPTYSTRING);
    TableIndex(const TableIndex &src);
    ~TableIndex();
    const KeyFileDefinition &getKeydef() const {
      return m_comparator.getKeydef();
    }

    int getIndexNo() const {
      return m_comparator.getIndexNo();
    }

    void  setIndexNo(int value) {
      m_comparator.setIndexNo(value);
    }

    const String &getTableName() const {
      return m_comparator.getTableName();
    }

    const String &getIndexName() const {
      return m_comparator.getIndexName();
    }

    const IndexDefinition &getIndexDefinition() const {
      return m_comparator.getIndexDefinition();
    }

    Iterator<KeyType> getIterator() const {
      return m_keyMap->keySet().getIterator();
    }

    DbAddr getAddr(const KeyType &key) const {
      return *m_keyMap->get(key);
    }
  };

  TableIndex::TableIndex(Database &db, const String &tableName, const String &indexName) : m_comparator(db,tableName,indexName) {
    m_keyMap = new TreeMap<KeyType,DbAddr>(m_comparator);
  }

  TableIndex::~TableIndex() {
    delete m_keyMap;
  }

  TableIndex::TableIndex(const TableIndex &src) : m_comparator(src.m_comparator) {
    m_keyMap = new TreeMap<KeyType,DbAddr>(m_comparator);
  }

  static unsigned long dbAddrHash(const DbAddr &a) {
    return int64Hash(a);
  }

  static int dbAddrHashCmp(const DbAddr &a1, const DbAddr &a2) {
    return sign((INT64)a1 - (INT64)a2);
  }

  template <class T> class DbAddrHashMap : public HashMap<DbAddr,T> {
  public:
    DbAddrHashMap(unsigned long capacity=31) : HashMap<DbAddr,T>(dbAddrHash,dbAddrHashCmp,capacity) {
    }
  };

  class TableMap {
  private:
    Database            &m_db;
    TableDefinition      m_tableDef;
    TableRecord         *m_tableRecord;
    Array<TableIndex>    m_indexArray;
    DbAddrHashMap<Tuple> m_addrMap;
    void reallocateUserRecord();
    bool hasUniqueKey(const Tuple &tuple);
  public:
    TableMap(Database &db, const String &tableName);
    ~TableMap();
    void insert(const Tuple &tuple);
    void remove(DbAddr addr);
    void update(DbAddr addr, const Tuple &tuple);
    void indexCreate(IndexDefinition &indexDef);
    void indexDrop(const String &name);
    void trbegin();
    void trcommit();
    void trabort();
    void checkData();
    Iterator<DbAddr>      getAddressIterator();
    Iterator<KeyType>     getKeyIterator(int index);
    CompactArray<DbAddr>  getAddressArray();
    CompactArray<KeyType> getKeyArray(int index);
    Database &getDatabase() {
      return m_db;
    }

    KeyFileDefinition getKeydef(int index) const {
      return m_indexArray[index].getKeydef();
    }

    const DbAddr getAddr(int index, const KeyType &key) const {
      return  m_indexArray[index].getAddr(key);
    }

    const Tuple &getTuple(DbAddr addr) const {
      return *m_addrMap.get(addr);
    }

    const Tuple &getTuple(int index, const KeyType &key) const {
      return getTuple(getAddr(index,key));
    }
    const TableRecord &getTableRecord() const {
      return *m_tableRecord;
    }

    const String &getTableName() const {
      return  m_tableDef.getTableName();
    }
  };

  TableMap::TableMap(Database &db, const String &tableName) : m_db(db), m_tableDef(db.getTableDefinition(tableName)) {
    m_tableRecord = new TableRecord(db,tableName);
    m_indexArray.add(TableIndex(m_db,tableName));
  }

  void TableMap::reallocateUserRecord() {
    delete m_tableRecord;
    m_tableRecord = new TableRecord(m_db,m_tableDef.getTableName());
    for(size_t i = 0; i < m_indexArray.size(); i++) {
      TableIndex &tableIndex = m_indexArray[i];
      tableIndex.setIndexNo(m_tableRecord->getTableInfo().findIndexNo(tableIndex.getIndexName()));
    }
  }

  bool TableMap::hasUniqueKey(const Tuple &tuple) {
    TableRecord tmp(m_db,getTableName());
    for(size_t i = 0; i < m_indexArray.size(); i++) {
      const TableIndex      &tableIndex = m_indexArray[i];
      const IndexDefinition &index      = tableIndex.getIndexDefinition();
      if(!index.isUnique()) {
        continue;
      }
      KeyType key;
      tmp.genKey(key,tableIndex.getIndexNo(),DB_NULLADDR);
      if(tableIndex.m_keyMap->get(key) != nullptr) {
        return true;
      }
    }
    return false;
  }

  TableMap::~TableMap() {
    delete m_tableRecord;
  }

  void TableMap::insert(const Tuple &tuple) {
    try {
      DbAddr addr = m_tableRecord->insert(tuple);
      for(size_t i = 0; i < m_indexArray.size(); i++) {
        KeyType key;
        m_tableRecord->genKey(key,m_indexArray[i].getIndexNo(),addr);
        verify(m_indexArray[i].m_keyMap->put(key,addr));
      }
      verify(m_addrMap.put(addr,tuple));
    } catch(sqlca ca) {
      verify(ca.sqlcode == SQL_DUPKEY);
      verify(hasUniqueKey(tuple));
    }
  }

  void TableMap::update(DbAddr addr, const Tuple &tuple) {
    try {
      m_tableRecord->update(addr, tuple);

      TableRecord oldRec(m_db,getTableName());
      oldRec.put(getTuple(addr));
      for(size_t i = 0; i < m_indexArray.size(); i++) {
        KeyType oldKey,newKey;
        oldRec.genKey(       oldKey,m_indexArray[i].getIndexNo(),addr);
        m_tableRecord->genKey(newKey,m_indexArray[i].getIndexNo(),addr);

        verify(m_indexArray[i].m_keyMap->remove(oldKey));
        verify(m_indexArray[i].m_keyMap->put(newKey,addr));
      }
      verify(m_addrMap.remove(addr));
      verify(m_addrMap.put(addr,tuple));
    } catch(sqlca ca) {
      verify(ca.sqlcode == SQL_DUPKEY);
      verify(hasUniqueKey(tuple));
    }
  }

  void TableMap::remove(DbAddr addr) {
    m_tableRecord->remove(addr);
    for(size_t i = 0; i < m_indexArray.size(); i++) {
      KeyType key;
      m_tableRecord->genKey(key,m_indexArray[i].getIndexNo(),addr);
      verify(m_indexArray[i].m_keyMap->remove(key));
    }
    verify(m_addrMap.remove(addr));
  }

  void TableMap::trbegin() {
    m_db.trbegin();
  }

  void TableMap::trcommit() {
    m_db.trcommit();
  }

  void TableMap::trabort() {
    m_db.trabort();
  }

  Iterator<DbAddr> TableMap::getAddressIterator() {
    return m_addrMap.keySet().getIterator();
  }

  Iterator<KeyType> TableMap::getKeyIterator(int index) {
    return m_indexArray[index].m_keyMap->keySet().getIterator();
  }

  CompactArray<DbAddr> TableMap::getAddressArray() {
    Iterator<DbAddr> it = getAddressIterator();
    CompactArray<DbAddr> result(m_addrMap.size());
    while(it.hasNext()) {
      result.add(it.next());
    }
    result.shuffle();
    return result;
  }

  CompactArray<KeyType> TableMap::getKeyArray(int index) {
    Iterator<KeyType> it = getKeyIterator(index);
    CompactArray<KeyType> result(m_indexArray[index].m_keyMap->size());
    while(it.hasNext()) {
      result.add(it.next());
    }
    result.shuffle();
    return result;
  }

  void TableMap::indexCreate(IndexDefinition &indexDef) {
    trbegin();
    try {
      getDatabase().indexCreate(indexDef);
      trcommit();
    } catch(...) {
      trabort();
      throw;
    }

    reallocateUserRecord();

    m_indexArray.add(TableIndex(m_db,getTableName(),indexDef.m_indexName));
    int indexNo = m_indexArray.last().getIndexNo();
    Iterator<DbAddr> it = getAddressIterator();
    while(it.hasNext()) {
      DbAddr addr = it.next();
      Tuple tuple = getTuple(addr);
      m_tableRecord->put(tuple);
      KeyType key;
      m_tableRecord->genKey(key,indexNo,addr);
      verify(m_indexArray.last().m_keyMap->put(key,addr));
    }
  }

  void TableMap::indexDrop(const String &indexName) {
    trbegin();
    getDatabase().indexDrop(indexName);
    trcommit();

    for(UINT i = 0; i < m_indexArray.size(); i++) {
      TableIndex &index = m_indexArray[i];
      if(index.getIndexName().equalsIgnoreCase(indexName)) {
        m_indexArray.removeIndex(i);
        break;
      }
    }
    reallocateUserRecord();
  }

  void TableMap::checkData() {
    const TableInfo &tableInfo = getTableRecord().getTableInfo();

    for(UINT i = 0; i < m_indexArray.size(); i++) {
      const TableIndex &tableIndex = m_indexArray[i];
  //    KeyDefinition keydef        = tableIndex.getKeydef();

      TableCursorParam cursorParam;
      cursorParam.m_sequenceNo      = tableInfo.getSequenceNo();
      cursorParam.m_indexName       = tableIndex.getIndexName();
      cursorParam.m_indexOnly       = false;
      cursorParam.m_beginRelOp      = RELOP_TRUE;
      cursorParam.m_beginFieldCount = 0;
      cursorParam.m_endRelOp        = RELOP_TRUE;
      cursorParam.m_endFieldCount   = 0;
      cursorParam.m_dir             = SORT_ASCENDING;
      cursorParam.m_fieldSet        = getTableRecord().getAllFields();

      TableCursor cursor(m_db,cursorParam,nullptr,nullptr);
      Iterator<KeyType> it = tableIndex.getIterator();
      Tuple dbTuple((UINT)cursorParam.m_fieldSet.size());

      while(it.hasNext()) {
        static int callCount = 0;
        callCount++;
        verify(cursor.hasNext());
        const KeyType mapKey   = it.next();
        const Tuple   mapTuple = getTuple(i,mapKey);
        cursor.next(dbTuple);

        verify(equals(dbTuple,mapTuple));
      }
      verify(!cursor.hasNext());
    }
  }

  void createDatabase(TestFields &testFields) {
    if(Database::exist(DBNAME)) {
      Database::destroy(DBNAME);
    }

    ::createDatabase(DBNAME);

    Database db(DBNAME);

    TableDefinition tableDef(TABLETYPE_USER,TABLENAME);

    for(UINT i = 0; i < testFields.getFieldCount(); i++) {
      const KeyField &keyField = testFields.getFieldDef(i);
      DbFieldType type = keyField.getType();
      if(type == DBTYPE_DBADDR) {
        continue;
      }

      tableDef.addColumn(ColumnDefinition(type,format(_T("column%02d"),tableDef.getColumnCount()+1),keyField.getLen()));
    }

    IndexDefinition indexDef;
    indexDef.m_tableName = tableDef.getTableName();
    indexDef.m_indexType = INDEXTYPE_PRIMARY;
    for(UINT i = 0; i < tableDef.getColumnCount(); i++) {
      const ColumnDefinition &col = tableDef.getColumn(i);
      if(isNullAllowed(col.getType())) {
        continue;
      }
      indexDef.m_columns.add(IndexColumn(i,true));
    }

    db.trbegin();
    db.tableCreate(tableDef);
    db.indexCreate(indexDef);
    db.trcommit();
  }

  void testInsert(TestFields &testFields, TableMap &tableMap) {
    for(int i = 0; i < 100; i++) {
      tableMap.trbegin();
      Tuple tuple = testFields.getRandomTuple();
      tableMap.insert(tuple);
      tableMap.trcommit();
    }

    tableMap.checkData();
  }

  void testRemove(TableMap &tableMap) {
    CompactArray<DbAddr> addressArray = tableMap.getAddressArray();

    for(size_t i = 0; i < addressArray.size(); i+=2) {
      tableMap.trbegin();
      tableMap.remove(addressArray[i]);
      tableMap.trcommit();
    }
    tableMap.checkData();
  }

  void testUpdate(TestFields &testFields, TableMap &tableMap) {
    CompactArray<DbAddr> addressArray = tableMap.getAddressArray();

    for(size_t i = 0; i < addressArray.size(); i+=3) {
      tableMap.trbegin();
      Tuple tuple = testFields.getRandomTuple();
      DbAddr addr = addressArray[i];
      tableMap.update(addr,tuple);
      tableMap.trcommit();
    }
    tableMap.checkData();
  }

  IndexDefinition genRandomIndexDefinition(TestFields &testFields, TableMap &tableMap, const String &indexName) {
    IndexDefinition indexDef;
    indexDef.m_tableName = tableMap.getTableName();
    indexDef.m_indexName = indexName;
    indexDef.m_indexType = ((testFields.getRandom().nextUChar() % 2) == 0) ? INDEXTYPE_UNIQUE : INDEXTYPE_NON_UNIQUE;

    CompactIntArray indexFields(testFields.getKeyFieldCount());
    for(UINT i = 0; i < testFields.getKeyFieldCount(); i++) {
      indexFields.add(i);
    }
    indexFields.shuffle();

    const int fieldCount = testFields.getRandom().nextUInt() % indexFields.size() + 1;

    for(int i = 0; i < fieldCount; i++) {
      indexDef.m_columns.add(IndexColumn(indexFields[i],true));
    }
    return indexDef;
  }

  TEST_CLASS(TestUserRecord) {
    public:

    TEST_METHOD(testUserRecord) {
      TestFields testFields(true,TESTFIELD_ALL);

      createDatabase(testFields);
      Database db(DBNAME);

      TableMap tableMap(db,TABLENAME);

    #define LOOPCOUNT 3

      for(int i = 0; i < LOOPCOUNT; i++) {
        int k;
        for(k = 0; k < 2; k++) {
          OUTPUT(_T("testUserRecord %d,%d"),i,k);
          testInsert(testFields, tableMap);
          testRemove(tableMap);
          testUpdate(testFields,tableMap);
        }

        for(;;) {
          try {
            IndexDefinition indexDef = genRandomIndexDefinition(testFields,tableMap, format(_T("%s%d%d"),tableMap.getTableName().cstr(),i,k));
            tableMap.indexCreate(indexDef);
            break;
          } catch(sqlca ca) {
            verify(ca.sqlcode == SQL_DUPKEY);
          }
        }
        tableMap.checkData();
      }
      verify(tableMap.getTableRecord().getTableInfo().getIndexCount() == LOOPCOUNT + 1);
      const String &indexName = tableMap.getTableRecord().getTableInfo().getIndex(LOOPCOUNT).m_indexName;
      tableMap.indexDrop(indexName);

      db.close();
      Database::destroy(DBNAME);

    }
  };
}

