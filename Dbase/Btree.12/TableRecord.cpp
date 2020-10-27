#include "stdafx.h"

void TableRecord::initData() {
  m_varchar.clear();
  m_rec.init();
}

void TableRecord::init() {
  initData();
}

TableRecord::TableRecord(Database &db, ULONG sequenceNo) : m_db(db), m_tableInfo(m_db.getTableInfo(sequenceNo)) {
  init();
}

TableRecord::TableRecord(Database &db, const String &tableName)  : m_db(db), m_tableInfo(m_db.getTableInfo(tableName))  {
  init();
}

void TableRecord::putVarChar(USHORT col, const varchar &vchar) {
  const ColumnInfo &tc = m_tableInfo.getColumn(col);
  if(tc.getMainType() != MAINTYPE_VARCHAR) {
    throwSqlError(SQL_FATAL_ERROR,_T("Column %d not type varchar in table <%s>"),col,getTableName().cstr());
  }

  if(vchar.len() > tc.m_len) {
    throwSqlError(SQL_STRING_TOO_LONG,_T("Varchar too long to fit in column %d"),col);
  }

  varchar *vcp = m_varchar.get(col);
  if(vcp == nullptr) {
    m_varchar.put(col,vchar);
  } else {
    *vcp = vchar;
  }
}

const varchar &TableRecord::getVarChar(USHORT col) const {
  const varchar *vcp = m_varchar.get(col);
  if(vcp == nullptr) {
    throwSqlError(SQL_FATAL_ERROR,_T("No varchar defined for col %d"),col);
  }
  return *vcp;
}

varchar &TableRecord::getVarChar(USHORT col) {
  varchar *vcp = m_varchar.get(col);
  if(vcp == nullptr) {
    throwSqlError(SQL_FATAL_ERROR,_T("No varchar defined for col %d"),col);
  }
  return *vcp;
}

void TableRecord::setUndefined(USHORT col) {
  m_tableInfo.setUndefined(m_rec,col);
}

bool TableRecord::isDefined(USHORT col) const {
  return m_tableInfo.isDefined(m_rec,col);
}

KeyType &TableRecord::genKey(KeyType &key, UINT index, DbAddr addr) const {
  const IndexDefinition &indexDef = m_tableInfo.getIndex(index);
  const KeyFileDefinition keydef = m_tableInfo.getKeyFileDefinition(index);
  key.init();
  for(UINT j = 0; j < indexDef.getColumnCount(); j++) {
    UINT c = indexDef.m_columns[j].m_col;

    switch(m_tableInfo.getColumn(c).getType()) {
    case DBTYPE_VARCHAR : // it has been checked in putVarChar, that String fits
    case DBTYPE_VARCHARN:
      if(!m_tableInfo.isDefined(m_rec,c)) {
        keydef.setUndefined(key,j);
      } else {
        keydef.put(key,j,getVarChar(c));
      }
      break;
    default:
      if(!m_tableInfo.isDefined(m_rec,c)) {
        keydef.setUndefined(key,j);
      } else {
        TupleField t;
        get(c,t);
        keydef.put(key,j,t);
      }
      break;
    }
  }
  keydef.putRecordAddr(key,addr);
  return key;
}

DbAddr TableRecord::insert(const Tuple &tuple) { // this is almost the same as insertkey in usxcreat
  DataFile datafile(m_db,m_tableInfo.getFileName(),DBFMODE_READWRITE,false);

  put(tuple);
  DbAddr addr = datafile.insert(m_rec.m_data,m_tableInfo.getRecordSize());

  for(UINT i = 0; i < m_tableInfo.getIndexCount(); i++) {
    KeyType key;
    genKey(key,i,addr);
    KeyFile indexfile(m_db, m_tableInfo.getIndex(i).m_fileName, DBFMODE_READWRITE,false);
    indexfile.insert(key);
  }

  // now insert varchars
  bool needUpdate = false;
  for(UINT i = 0; i < m_tableInfo.getColumnCount(); i++) {
    switch(m_tableInfo.getColumn(i).getType()) {
    case DBTYPE_VARCHARN:
      if(!m_tableInfo.isDefined(m_rec,i)) {
        break;
      }
      // continue case
    case DBTYPE_VARCHAR :
      { needUpdate = true;
        const varchar &vc = getVarChar(i);
        DbAddr vcAddr = datafile.insert(vc.data(),vc.len());
        m_tableInfo.put(m_rec,i,vcAddr);
      }
      break;

    default:
      break;
    }
  }

  if(needUpdate) {
    datafile.update(addr,m_rec.m_data);
  }
  return addr;
}

void TableRecord::update(DbAddr addr, const Tuple &tuple) {
  TableRecord oldRec(m_db,m_tableInfo.getSequenceNo());
  oldRec.read(addr);
  put(tuple);
  for(UINT i = 0; i < m_tableInfo.getIndexCount(); i++) {
    const KeyFileDefinition keydef = m_tableInfo.getKeyFileDefinition(i);
    KeyType oldKey,newKey;
    oldRec.genKey(oldKey,i,addr);
    genKey(newKey,i,addr);
    if(keydef.keyCmp(oldKey,newKey) != 0) {
      KeyFile indexfile(m_db, m_tableInfo.getIndex(i).m_fileName, DBFMODE_READWRITE,false);
      indexfile.remove(oldKey);
      indexfile.insert(newKey);
    }
  }

  DataFile datafile(m_db,m_tableInfo.getFileName(),DBFMODE_READWRITE,false);
  for(UINT i = 0; i < m_tableInfo.getColumnCount(); i++) {
    switch(m_tableInfo.getColumn(i).getType()) {
    case DBTYPE_VARCHARN:
      { bool oldDefined = m_tableInfo.isDefined(oldRec.m_rec,i);
        bool newDefined = m_tableInfo.isDefined(m_rec,i);
        if(!oldDefined) {
          if(!newDefined) {
            continue;
          } else { // newDefined
            const varchar &newVarchar = getVarChar(i);
            DbAddr newAddr = datafile.insert(newVarchar.data(),newVarchar.len());
            m_tableInfo.put(m_rec,i,newAddr);
            continue;
          }
        } else { // oldDefined
          if(!newDefined) {
            DbAddr oldAddr;
            m_tableInfo.get(oldRec.m_rec,i,oldAddr);
            datafile.remove(oldAddr);
            continue;
          }
        }
      }
      // both defined. continue case
    case DBTYPE_VARCHAR :
      { const varchar &oldVarchar = oldRec.getVarChar(i);
        const varchar &newVarchar = getVarChar(i);
        DbAddr newAddr;
        if(oldVarchar == newVarchar) {
          m_tableInfo.get(oldRec.m_rec,i,newAddr);
        } else if(oldVarchar.len() != newVarchar.len()) {
          DbAddr oldAddr;
          m_tableInfo.get(oldRec.m_rec,i,oldAddr);
          datafile.remove(oldAddr);
          newAddr = datafile.insert(newVarchar.data(),newVarchar.len());
        } else {
          m_tableInfo.get(oldRec.m_rec,i,newAddr);
          datafile.update(newAddr,newVarchar.data());
        }
        m_tableInfo.put(m_rec,i,newAddr);
        break;
      }

    default:
      break;
    }
  }
  if(!m_tableInfo.recordsEqual(m_rec,oldRec.m_rec)) {
    datafile.update(addr,m_rec.m_data);
  }
}

void TableRecord::remove(DbAddr addr) {
  read(addr);
  DataFile datafile(m_db,m_tableInfo.getFileName(),DBFMODE_READWRITE,false);
  for(UINT col = 0; col < m_tableInfo.getColumnCount(); col++) {
    switch(m_tableInfo.getColumn(col).getType()) {
    case DBTYPE_VARCHARN:
      if(!m_tableInfo.isDefined(m_rec,col)) {
        break;
      }
      // continue case
    case DBTYPE_VARCHAR :
      { DbAddr vcAddr;
        m_tableInfo.get(m_rec,col,vcAddr);
        if(vcAddr == DB_NULLADDR)
          throwSqlError(SQL_DBCORRUPTED,_T("Datafile <%s> corrupted"),m_tableInfo.getFileName().cstr() );
        datafile.remove(vcAddr);
      }
    }
  }
  datafile.remove(addr);
  for(UINT i = 0; i < m_tableInfo.getIndexCount(); i++) {
    KeyType key;
    genKey(key,i,addr);
    KeyFile indexfile(m_db, m_tableInfo.getIndex(i).m_fileName, DBFMODE_READWRITE,false);
    indexfile.remove(key);
  }
}

void TableRecord::read(DbAddr addr, const FieldSet &fields) {
  DataFile datafile(m_db,m_tableInfo.getFileName(),DBFMODE_READONLY,false);
  datafile.readRecord(addr, m_rec.m_data, m_tableInfo.getRecordSize());
  for(size_t i = 0; i < fields.size(); i++) {
    int col = fields[i];
    switch(m_tableInfo.getColumn(col).getType()) {
    case DBTYPE_VARCHARN:
      if(!m_tableInfo.isDefined(m_rec,col)) {
        break;
      }
      // continue case
    case DBTYPE_VARCHAR :
      { DbAddr vcAddr = 0;
        m_tableInfo.get(m_rec,col,vcAddr);
        if(vcAddr == DB_NULLADDR) {
          throwSqlError(SQL_DBCORRUPTED,_T("Datafile <%s> corrupted"),m_tableInfo.getFileName().cstr() );
        }
        varchar vc;
        datafile.readVarChar(vcAddr,vc);
        putVarChar(col,vc);
      }
      break;
    default:
      break; // field already read (member of m_rec)
    }
  }
}

void TableRecord::read(DbAddr addr, const FieldSet &fields, Tuple &tuple) {
  read(addr,fields);
  get(tuple, fields);
}

void TableRecord::read(DbAddr addr) {
  read(addr,getAllFields());
}

void TableRecord::read(DbAddr addr, Tuple &tuple) {
  read(addr);
  get(tuple);
}

void TableRecord::put(USHORT col, const TupleField &v) {
  if(!v.isDefined()) {
    setUndefined(col);
    return;
  }
  switch(m_tableInfo.getColumn(col).getType()) {
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    { varchar vc;
      v.get(vc);
      putVarChar(col,vc);
      m_tableInfo.put(m_rec,col,DB_NULLADDR); // this will be changed when we write to file
    }
    break;
  default:
    m_tableInfo.put(m_rec,col,v);
    break;
  }
}

void TableRecord::get(USHORT col, TupleField &v) const {
  if(!isDefined(col)) {
    v.setUndefined();
    return;
  }
  switch(m_tableInfo.getColumn(col).getType()) {
  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    v = getVarChar(col);
    break;
  default:
    m_tableInfo.get(m_rec,col,v);
    break;
  }
}

void TableRecord::put(const Tuple &tuple) {
  for(USHORT i = 0; i < tuple.size(); i++) {
    put(i,tuple[i]);
  }
}

void TableRecord::get(Tuple &tuple) const {
  for(USHORT i = 0; i < tuple.size(); i++) {
    get(i,tuple[i]);
  }
}

void TableRecord::get(Tuple &tuple, const FieldSet &fields) {
  for(size_t i = 0; i < fields.size(); i++) {
    const short col = fields[i];
    get(col,tuple[i]);
  }
}

String TableRecord::toString() const {
  Tuple tuple((UINT)getAllFields().size());
  return tuple.toString();
}

void TableRecord::dump(FILE *f) const {
  _ftprintf(f,_T("TableRecord:<%s>\n"),toString().cstr());
}
