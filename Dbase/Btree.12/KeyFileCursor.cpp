#include "stdafx.h"

//
//#define DEBUG

bool KeyCursor::bufferIsFull() const {
  return m_bufSize + m_keydef.getSize() > sizeof(m_buffer);
}

bool KeyCursor::bufferIsEmpty() const {
  return m_bufCount == 0;
}

// Called from KeyFilescan_asc/desc
bool KeyCursor::handleKey(const KeyType &key) {
  if(bufferIsFull()) return false;

  if(!m_keydef.keynCmpRelOp(m_endRelOp, key, m_endKey, m_endFieldCount)) {
#if defined(DEBUG)
    _tprintf(_T("stopped filescan because of end-cond. key:"));
    m_keydef.keyprintf(key);
    _tprintf(_T(" endkey:"));
    m_keydef.keynprintf(endkey, endfieldcount);
    _tprintf(_T("\n"));
#endif
    return false;
  }

  memcpy(m_buffer + m_bufSize, key.m_data, m_keydef.getSize());
  m_bufSize += m_keydef.getSize();
  m_bufCount++;
  if(bufferIsFull()) {
    m_lastKey = key;
    return false;
  }
  return true;
}

KeyType &KeyCursor::bufferFetch(KeyType &key) {
  if(bufferIsEmpty()) {
    throwSqlError(SQL_BUFFER_UNDERFLOW,_T("Buffer is empty in bufferFetch"));
  }

  memcpy(key.m_data, m_buffer + m_bufPointer, m_keydef.getSize());

  m_bufPointer += m_keydef.getSize();
  m_bufCount--;

  return key;
}

void KeyCursor::bufferInit() {
  m_bufCount   = 0;
  m_bufSize    = 0;
  m_bufPointer = 0;
}

void KeyCursor::bufferFill(KeyFile &file) {
  bufferInit();
  switch(m_dir) {
    case SORT_ASCENDING:
      if(m_firstTime) {
        file.scanAsc(  m_beginRelOp,
                      &m_beginKey,
                       m_beginFieldCount,
                      *this);
      } else {
        file.scanAsc(  RELOP_GT,
                      &m_lastKey,
                       m_keydef.getKeyFieldCount(),
                      *this);
      }
      break;

    case SORT_DESCENDING:
      if(m_firstTime) {
        file.scanDesc( m_beginRelOp,
                      &m_beginKey,
                       m_beginFieldCount,
                      *this);
      } else {
        file.scanDesc( RELOP_LT,
                      &m_lastKey,
                       m_keydef.getKeyFieldCount(),
                      *this);
      }
      break;
  }

  m_firstTime   = false;
  m_lastWasFull = bufferIsFull();

#if defined(DEBUG)
_tprintf(_T("fillbuffer finished:bufcount:%d   \n"),m_bufCount);
#endif

}

KeyCursor::KeyCursor(KeyFile       &keyFile,
                     RelationType   beginRelOp,
                     const KeyType *beginKey,
                     UINT   beginFieldCount,
                     RelationType   endRelOp,
                     const KeyType *endKey,
                     UINT   endFieldCount,
                     SortDirection  dir
                     ) {

  KeyFileDefinition keydef(keyFile);

  if(beginFieldCount > keydef.getKeyFieldCount())  {
    throwSqlError(SQL_INVALID_KEYCOUNT,_T("Invalid beginfieldcount (=%d). #keyfields=%d"),beginFieldCount,keydef.getKeyFieldCount());
  }
  if(endFieldCount   > keydef.getKeyFieldCount()) {
    throwSqlError(SQL_INVALID_KEYCOUNT,_T("Invalid endfieldcount (=%d). #keyfields=%d"), endFieldCount, keydef.getKeyFieldCount());
  }

  m_fileName        = keyFile.getName();
  m_keydef          = keydef;
  m_beginRelOp      = beginRelOp;
  m_endRelOp        = endRelOp;
  m_beginFieldCount = beginFieldCount;
  m_endFieldCount   = endFieldCount;
  m_dir             = dir;
  m_firstTime       = true;
  bufferInit();

  if(beginKey) {
    m_beginKey = *beginKey;
  } else {
    m_beginKey.init();
  }

  if(endKey) {
    m_endKey   = *endKey;
  } else {
    m_endKey.init();
  }

  if(beginRelOp != RELOP_FALSE && endRelOp != RELOP_FALSE) {
    bufferFill(keyFile);
  }

  m_hasNext = !bufferIsEmpty();
}

KeyCursor::~KeyCursor() {
}

KeyType &KeyCursor::next(KeyType &key) {
  if(!m_hasNext) {
    throwSqlError(SQL_NOSUCHRECORD,_T("KeyCursor::next:No souch record"));
  }

  bufferFetch(key);
  if(bufferIsEmpty()) {
    try {
      if(m_lastWasFull) {
        KeyFile file(m_fileName, DBFMODE_READONLY, nullptr);
        bufferFill(file);
      }
      m_hasNext = !bufferIsEmpty();
    } catch(sqlca) {
      m_hasNext = false;
    }
  }
  return key;
}

/*
void KeyCursor::getCurrentKey(KeyType &key) const {
  if(!hasCurrent())
    throwSqlError(SQL_NOCURRENTRECORD,_T("No current key"));
  key = m_lastKey; // keydef.keycpy(key,m_lastKey);
}
*/

bool KeyCursor::hasNext() const {
  return m_hasNext;
}
