#include "stdafx.h"
#include <limits.h>

String RandomData::nextString(int maxLen) {
  int len = nextInt() % (maxLen + 1);
  String s = spaceString(len);
  for(int i = 0; i < len; i++)
    s[i] = 'a' + nextInt() % ('z' - 'a' + 1);
//  _tprintf(_T("genereret streng:<%s>\n"),s.cstr());
  return s;
}

static String &incrString(String &s) {
  int len = (int)s.length();
  int index = len - 1;
  while(index >= 0) {
    TCHAR &ch = s[index];
    ch++;
    if(ch <= 'z') {
      return s;
    } else {
      ch = 'A';
      index--;
    }
  }

  // overflow.make a new String of AAAA... one longer than s
  return s = spaceString(len+1,'a');
}

char RandomData::nextChar() {
  return nextInt() % 128;
}

UCHAR RandomData::nextUChar() {
  return nextUInt() % 256;
}

short RandomData::nextShort() {
  return (short)nextInt();
}

USHORT RandomData::nextUShort() {
  return (USHORT)nextUInt();
}

UINT   RandomData::nextUInt() {
  return abs(nextInt());
}

UINT64 RandomData::nextUInt64() {
  return nextInt64();
}

Date RandomData::nextDate() {
  for(;;) {
    try {
      Date d(nextUShort() % 31+1,nextUShort() % 12+1, 1970 + nextShort() % 60);
      return d;
    } catch(Exception e) {
    }
  }
}

Time RandomData::nextTime() {
  return Time(nextUShort() % 24, nextUShort() % 60, nextUShort() % 60, nextUShort() % 1000);
}

Timestamp RandomData::nextTimestamp() {
  return Timestamp(nextDate(),nextTime());
}

// ------------------------------------------------------------------

RandomData TestFields::random;

void TestFields::initDefinition(bool unique, int flags) {
  if(flags & TESTFIELD_NOT_NULL) {
    if(flags & TESTFIELD_SIGNED) {
      addKeyField(SORT_ASCENDING,DBTYPE_CHAR    );
      addKeyField(SORT_ASCENDING,DBTYPE_SHORT   );
      addKeyField(SORT_ASCENDING,DBTYPE_INT     );
      addKeyField(SORT_ASCENDING,DBTYPE_LONG    );
      addKeyField(SORT_ASCENDING,DBTYPE_INT64   );
    }
    if(flags & TESTFIELD_UNSIGNED) {
      addKeyField(SORT_ASCENDING,DBTYPE_UCHAR    );
      addKeyField(SORT_ASCENDING,DBTYPE_USHORT   );
      addKeyField(SORT_ASCENDING,DBTYPE_UINT     );
      addKeyField(SORT_ASCENDING,DBTYPE_ULONG    );
      addKeyField(SORT_ASCENDING,DBTYPE_UINT64   );
    }

    addKeyField(SORT_ASCENDING,DBTYPE_FLOAT      );
    addKeyField(SORT_ASCENDING,DBTYPE_DOUBLE     );
    addKeyField(SORT_ASCENDING,DBTYPE_CSTRING,6  );
    addKeyField(SORT_ASCENDING,DBTYPE_WSTRING,6  );
    addKeyField(SORT_ASCENDING,DBTYPE_VARCHAR,6  );
    addKeyField(SORT_ASCENDING,DBTYPE_DATE       );
    addKeyField(SORT_ASCENDING,DBTYPE_TIME       );
    addKeyField(SORT_ASCENDING,DBTYPE_TIMESTAMP  );
  }
  if(flags & TESTFIELD_NULL) {
    if(flags & TESTFIELD_SIGNED) {
      addKeyField(SORT_ASCENDING,DBTYPE_CHARN    );
      addKeyField(SORT_ASCENDING,DBTYPE_SHORTN   );
      addKeyField(SORT_ASCENDING,DBTYPE_INTN     );
      addKeyField(SORT_ASCENDING,DBTYPE_LONGN    );
      addKeyField(SORT_ASCENDING,DBTYPE_INT64N   );
    }
    if(flags & TESTFIELD_UNSIGNED) {
      addKeyField(SORT_ASCENDING,DBTYPE_UCHARN   );
      addKeyField(SORT_ASCENDING,DBTYPE_USHORTN  );
      addKeyField(SORT_ASCENDING,DBTYPE_UINTN    );
      addKeyField(SORT_ASCENDING,DBTYPE_ULONGN   );
      addKeyField(SORT_ASCENDING,DBTYPE_UINT64N  );
    }
    addKeyField(SORT_ASCENDING,DBTYPE_FLOATN     );
    addKeyField(SORT_ASCENDING,DBTYPE_DOUBLEN    );
    addKeyField(SORT_ASCENDING,DBTYPE_CSTRINGN,6 );
    addKeyField(SORT_ASCENDING,DBTYPE_WSTRINGN,6 );
    addKeyField(SORT_ASCENDING,DBTYPE_VARCHARN,6 );
    addKeyField(SORT_ASCENDING,DBTYPE_DATEN      );
    addKeyField(SORT_ASCENDING,DBTYPE_TIMEN      );
    addKeyField(SORT_ASCENDING,DBTYPE_TIMESTAMPN );
  }
  addAddrField(unique);
}

TestFields::TestFields(bool unique, int flags) {
  initDefinition(unique, flags);
  setRandom();
}

TestFields::TestFields(const KeyFileDefinition &keydef) : KeyFileDefinition(keydef) {
  setRandom();
}

void TestFields::incrField(UINT field) {
  switch(getFieldDef(field).getType()) {
  case DBTYPE_CHAR      :
  case DBTYPE_CHARN     :
    { char ch;
      get(m_key,field,ch);
      ch++;
      put(m_key,field,ch);
    }
    break;

  case DBTYPE_UCHAR     :
  case DBTYPE_UCHARN    :
    { UCHAR ch;
      get(m_key,field,ch);
      ch++;
      put(m_key,field,ch);
    }
    break;

  case DBTYPE_SHORT     :
  case DBTYPE_SHORTN    :
    { short s;
      get(m_key,field,s);
      s++;
      put(m_key,field,s);
    }
    break;

  case DBTYPE_USHORT    :
  case DBTYPE_USHORTN   :
    { USHORT s;
      get(m_key,field,s);
      s++;
      put(m_key,field,s);
    }
    break;

  case DBTYPE_INT       :
  case DBTYPE_INTN      :
  case DBTYPE_LONG      :
  case DBTYPE_LONGN     :
    { int i;
      get(m_key,field,i);
      i++;
      put(m_key,field,i);
    }
    break;

  case DBTYPE_UINT      :
  case DBTYPE_UINTN     :
  case DBTYPE_ULONG     :
  case DBTYPE_ULONGN    :
    { UINT i;
      get(m_key,field,i);
      i++;
      put(m_key,field,i);
    }
    break;

  case DBTYPE_INT64     :
  case DBTYPE_INT64N    :
    { INT64 i;
      get(m_key,field,i);
      i++;
      put(m_key,field,i);
    }
    break;

  case DBTYPE_UINT64    :
  case DBTYPE_UINT64N   :
    { UINT64 i;
      get(m_key,field,i);
      i++;
      put(m_key,field,i);
    }
    break;

  case DBTYPE_FLOAT     :
  case DBTYPE_FLOATN    :
    { float f;
      get(m_key,field,f);
      f++;
      put(m_key,field,f);
    }
    break;

  case DBTYPE_DOUBLE    :
  case DBTYPE_DOUBLEN   :
    { double d;
      get(m_key,field,d);
      d++;
      put(m_key,field,d);
    }
    break;

  case DBTYPE_CSTRING   :
  case DBTYPE_CSTRINGN  :
  case DBTYPE_WSTRING   :
  case DBTYPE_WSTRINGN  :
    { String s;
      get(m_key,field,s);
      incrString(s);
      put(m_key,field,s);
    }
    break;

  case DBTYPE_VARCHAR   :
  case DBTYPE_VARCHARN  :
    { varchar v;
      get(m_key,field,v);
      String s = (TCHAR*)v.data();
      incrString(s);
      v = s.cstr();
      put(m_key,field,v);
    }
    break;

  case DBTYPE_DATE      :
  case DBTYPE_DATEN     :
    { Date d;
      get(m_key,field,d);
      d.add(TDAYOFMONTH,1);
      put(m_key,field,d);
    }
    break;

  case DBTYPE_TIME      :
  case DBTYPE_TIMEN     :
    { Time t;
      get(m_key,field,t);
      t.add(TSECOND,1);
      put(m_key,field,t);
    }
    break;

  case DBTYPE_TIMESTAMP :
  case DBTYPE_TIMESTAMPN:
    { Timestamp t;
      get(m_key,field,t);
      t.add(TSECOND,1);
      put(m_key,field,t);
    }
    break;

  case DBTYPE_DBADDR    :
    { DbAddr addr;
      get(m_key,field,addr);
      addr++;
      put(m_key,field,addr);
    }
    break;

  default:
    throwException(_T("%s:Invalid fieldtype. Field=%d:type=%d")
                  ,__TFUNCTION__,field,getFieldDef(field).getType());
  }
}

#define RANDOMNULL { if(random.nextChar() % 3 == 0) return TupleField(); }

TupleField TestFields::getRandomField(UINT field) {
  switch(getFieldDef(field).getType()) {
  case DBTYPE_CHARN     : RANDOMNULL;
  case DBTYPE_CHAR      : return random.nextChar();

  case DBTYPE_UCHARN    : RANDOMNULL;
  case DBTYPE_UCHAR     : return random.nextUChar();

  case DBTYPE_SHORTN    : RANDOMNULL;
  case DBTYPE_SHORT     : return random.nextShort();

  case DBTYPE_USHORTN   : RANDOMNULL;
  case DBTYPE_USHORT    : return random.nextUShort();

  case DBTYPE_INTN      : 
  case DBTYPE_LONGN     : RANDOMNULL;
  case DBTYPE_INT       : 
  case DBTYPE_LONG      : return random.nextInt();
  
  case DBTYPE_UINTN     : 
  case DBTYPE_ULONGN    : RANDOMNULL;
  case DBTYPE_UINT      : 
  case DBTYPE_ULONG     : return random.nextUInt();

  case DBTYPE_INT64N    : RANDOMNULL;
  case DBTYPE_INT64     : return random.nextInt64();
  
  case DBTYPE_UINT64N   : RANDOMNULL;
  case DBTYPE_UINT64    : return random.nextUInt64();

  case DBTYPE_FLOATN    : RANDOMNULL;
  case DBTYPE_FLOAT     : return (float)random.nextDouble(-1e10,1e10);

  case DBTYPE_DOUBLEN   : RANDOMNULL;
  case DBTYPE_DOUBLE    : return random.nextDouble(-1e10,1e10);

  case DBTYPE_CSTRINGN  : RANDOMNULL;
  case DBTYPE_CSTRING   : return random.nextString(getFieldDef(field).getMaxStringLen());

  case DBTYPE_WSTRINGN  : RANDOMNULL;
  case DBTYPE_WSTRING   : return random.nextString(getFieldDef(field).getMaxStringLen());

  case DBTYPE_VARCHARN  : RANDOMNULL;
  case DBTYPE_VARCHAR   : return varchar(random.nextString(getFieldDef(field).getMaxStringLen()));

  case DBTYPE_DATEN     : RANDOMNULL;
  case DBTYPE_DATE      : return random.nextDate();

  case DBTYPE_TIMEN     : RANDOMNULL;
  case DBTYPE_TIME      : return random.nextTime();

  case DBTYPE_TIMESTAMPN: RANDOMNULL;
  case DBTYPE_TIMESTAMP : return random.nextTimestamp();

  case DBTYPE_DBADDR    : return random.nextUInt();

  default:
    throwException(_T("TestFields::getRandomField:Invalid fieldtype. Field=%d:type=%d"),field,getFieldDef(field).getType());
    return 0;
  }
}

void TestFields::setRandom() {
  for(UINT field = 0; field < getFieldCount(); field++) {
    put(m_key,field,getRandomField(field));
  }
}

void TestFields::incrAll() {
  for(UINT field = 0; field < getFieldCount(); field++) {
    incrField(field);
  }
}

KeyType TestFields::getRandomKey() {
  setRandom();
  return getKey();
}

Tuple TestFields::getRandomTuple() {
  Tuple tuple;
  for(UINT field = 0; field < getFieldCount(); field++) {
    const KeyField &keyField = getFieldDef(field);
    if(keyField.getType() == DBTYPE_DBADDR) {
      continue;
    }
    tuple.add(getRandomField(field));
  }
  return tuple;
}
