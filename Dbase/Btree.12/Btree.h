#pragma once

#include <SqlApi.h>
#include <HashMap.h>
#include <Semaphore.h>
#include <Date.h>

#define BIG_DBADDR

#ifdef BIG_DBADDR

typedef UINT64 DbAddr;
typedef DbAddr KeyPageAddr;

#define DBADDRSIZE 6

// With 6 bytes we can access around 280 Tb which should be enough for now. Internal we use C++ native INT64 type
// and we have an easy way of converting them
class DbAddrFileFormat {
private:
  BYTE m_bytes[DBADDRSIZE];
public:
  DbAddrFileFormat &operator=(const DbAddr &addr);
  operator DbAddr() const;
};

inline String toString(const DbAddrFileFormat &addr) {
  return format(_T("%I64u"), (DbAddr)addr);
}

#else

typedef ULONG  DbAddr;
typedef DbAddr KeyPageAddr;
typedef DbAddr DbAddrFileFormat;

#define DBADDRSIZE sizeof(DbAddrFileFormat)

inline String toString(const DbAddrFileFormat &addr) {
  return format(_T("%lu"), (DbAddr)addr);
}

#endif

#define DB_NULLADDR ((DbAddr)0)

#define KEYPAGESIZE         4096
#define MAXKEYSIZE           256
#define MAXKEYFIELDCOUNT      64
// 3 characters is allocated for each column in SysTableIndexData. so the biggest columnindex is 999

#define CURSORBUFSIZE (MAXKEYSIZE * 100)

#define MAXRECSIZE          4096
#define MAXFILENAME          100
#define MAXTABLENAME          30
#define MAXINDEXNAME          30
#define MAXCOLUMNNAME         30
#define MAXUSERNAME           30
#define MAXPASSWORD           30
#define MAXDEFAULTVALUE       20
#define MAXVARCHARLEN    0x7fffffff
// 31 bits allocated in DataPage. See DataFile

#if(KEYPAGESIZE < 32)
#error("KEYPAGESIZE must be at least 32")
#endif

#if(KEYPAGESIZE < 4*MAXKEYSIZE)
#error("KEYPAGESIZE must be at least 4*MAXKEYSIZE");
#endif

#pragma warning(disable : 4146)

#define DBMINCHAR           -128.0
#define DBMAXCHAR            127.0
#define DBMINUCHAR             0.0
#define DBMAXUCHAR           255.0
#define DBMINSHORT        -32768.0
#define DBMAXSHORT         32767.0
#define DBMINUSHORT            0.0
#define DBMAXUSHORT        65535.0
#define DBMININT     -2147483648.0
#define DBMAXINT      2147483647.0
#define DBMINUINT              0.0
#define DBMAXUINT     4294967295.0
#define DBMINLONG    -2147483648.0
#define DBMAXLONG     2147483647.0
#define DBMINULONG             0.0
#define DBMAXULONG    4294967295.0
#define DBMININT64    (-0x7fffffffffffffff - 1)
#define DBMAXINT64    0x7fffffffffffffffi64
#define DBMINUINT64            0
#define DBMAXUINT64   0xffffffffffffffffui64


#define DBMINFLOAT      -FLT_MAX
#define DBMAXFLOAT       FLT_MAX
#define DBMINDOUBLE     -DBL_MAX
#define DBMAXDOUBLE      DBL_MAX

#define SETCONST(dst,src,type) { type *p = (type*)&(dst); *p = src; }
#define SETCONSTSHORT(dst,src) SETCONST(dst,src,USHORT)

class KeyFileDefinition;

class KeyType {
public:
  char m_data[MAXKEYSIZE];
  inline void init() { memset(m_data,0,sizeof(m_data)); }
  String toString(const KeyFileDefinition &keydef) const;
};

class RecordType {
public:
  char m_data[MAXRECSIZE];
  inline void init() { memset(m_data,0,sizeof(m_data)); }
};

typedef enum {
  SORT_ASCENDING
 ,SORT_DESCENDING
} SortDirection;

typedef enum {
  RELOP_GE
 ,RELOP_GT
 ,RELOP_LE
 ,RELOP_LT
 ,RELOP_EQ
 ,RELOP_NQ
 ,RELOP_TRUE
 ,RELOP_FALSE
} RelationType;

const TCHAR *relOpString(RelationType relop);
const TCHAR *sortDirString(SortDirection dir);
bool isInteger(  double d);
bool validChar(  double d);
bool validUchar( double d);
bool validShort( double d);
bool validUshort(double d);
bool validInt(   double d);
bool validUint(  double d);
bool validLong(  double d);
bool validUlong( double d);
bool validFloat( double d);

class KeyField {
private:
  UCHAR     m_type;   // type of keyfield
  bool      m_asc;    // is field ascending of descending
  USHORT    m_len;    // length of variable[i] excl. null-indicator in bytes
  USHORT    m_offset; // offset of var[i] in key in bytes.
public:
  KeyField(DbFieldType type, bool asc, USHORT len, USHORT offset);
  KeyField();
  inline DbFieldType getType()     const { return (DbFieldType)m_type;         }
  inline DbMainType  getMainType() const { return ::getMainType(getType());    }
  inline bool        isAscending() const { return m_asc;                       }
  inline bool        isNullable()  const { return DBTYPE_NULLABLE(m_type);     }
  // In bytes
  inline UINT        getLen()      const { return m_len;                       }
  inline UINT        getSize()     const { return (getMainType() == MAINTYPE_VARCHAR) ? (m_len + 1) : m_len; }
  // In characters. Only valid for m_type=DBTYPE_STRING/DBTYPE_STRINGN/DBTYPE_VARCHAR/DBTYPE_VARCHARN
  UINT               getMaxStringLen() const;
  inline UINT        getOffset()   const { return m_offset;                    }
};

class KeyFile;

class TupleField {
private:
  DbFieldType       m_type;
  bool              m_defined;
  union {
    char            m_char;
    short           m_short;
    int             m_int;
    long            m_long;
    INT64           m_int64;
    float           m_float;
    double          m_double;
    String         *m_string;
    varchar        *m_varchar;
    Date           *m_date;
    Time           *m_time;
    Timestamp      *m_timestamp;
  };
  void allocate(DbFieldType type);
  void cleanup();
  void setValue(double d);
  void setValue(const String    &str);
  void setValue(const varchar   &vch);
  void setValue(const Date      &dat);
  void setValue(const Time      &dat);
  void setValue(const Timestamp &dat);
public:
  TupleField();
  TupleField(const TupleField &rhs);
  TupleField(char             v);
  TupleField(UCHAR            v);
  TupleField(short            v);
  TupleField(USHORT           v);
  TupleField(int              v);
  TupleField(UINT             v);
  TupleField(long             v);
  TupleField(ULONG            v);
  TupleField(INT64            v);
  TupleField(UINT64           v);
  TupleField(float            v);
  TupleField(double           v);
  TupleField(const char      *v);
  TupleField(const wchar_t   *v);
  TupleField(const String    &v);
  TupleField(const varchar   &v);
  TupleField(const Date      &v);
  TupleField(const Time      &v);
  TupleField(const Timestamp &v);
  ~TupleField();
  TupleField &operator=(const SqlApiVarList &hv );
  TupleField &operator=(const TupleField    &rhs);
  void get(char             &v ) const;
  void get(UCHAR            &v ) const;
  void get(short            &v ) const;
  void get(USHORT           &v ) const;
  void get(int              &v ) const;
  void get(UINT             &v ) const;
  void get(long             &v ) const;
  void get(ULONG            &v ) const;
  void get(INT64            &v ) const;
  void get(UINT64           &v ) const;
  void get(float            &v ) const;
  void get(double           &v ) const;
  void get(String           &v ) const;
  void get(varchar          &v ) const;
  void get(Date             &v ) const;
  void get(Time             &v ) const;
  void get(Timestamp        &v ) const;
  void get(SqlApiVarList    &hv) const;

  void setType(DbFieldType newType);

  inline DbFieldType getType() const {
    return m_type;
  }

  inline DbMainType getMainType() const {
    return ::getMainType(m_type);
  }

  inline bool isDefined() const {
    return m_defined;
  }

  TupleField &setUndefined(); // return this
  String toString() const;
  void dump(FILE *f = stdout) const;
};

double sqlPow(double x, double y);                                 // handles negative x with y=integer
TupleField operator+( const TupleField &v1, const TupleField &v2); // arithmetic PLUS
TupleField operator-( const TupleField &v1, const TupleField &v2); // arithmetic binary MINUS
TupleField operator-( const TupleField &v);                        // arithmetic unary MINUS
TupleField operator*( const TupleField &v1, const TupleField &v2); // arithmetic MULTIPLICATION
TupleField operator/( const TupleField &v1, const TupleField &v2); // arithmetic DIVISION
TupleField operator%( const TupleField &v1, const TupleField &v2); // arithmetic MODULUS
TupleField sqlPow   ( const TupleField &v1, const TupleField &v2);

TupleField operator&&(const TupleField &v1, const TupleField &v2); // logical AND
TupleField operator||(const TupleField &v1, const TupleField &v2); // logical OR
TupleField operator!( const TupleField &v);                        // logical NOT
TupleField operator> (const TupleField &v1, const TupleField &v2);
TupleField operator>=(const TupleField &v1, const TupleField &v2);
TupleField operator< (const TupleField &v1, const TupleField &v2);
TupleField operator<=(const TupleField &v1, const TupleField &v2);
TupleField operator==(const TupleField &v1, const TupleField &v2);
TupleField operator!=(const TupleField &v1, const TupleField &v2);

TupleField concat(    const TupleField &v1, const TupleField &v2);
int compare(          const TupleField &v1, const TupleField &v2); // v1<v2:-1, v1>v2:1, v1==v2:0, v1.isnull || v2.isnull:-2

class KeyFileDefinition {
private:
  UCHAR     m_totalFieldCount;         // Total number of fields in key (ie #keyfields + #datafields)
  UCHAR     m_keyFieldCount;           // Number of fields, making the key unique. m_keyFieldCount <= m_totalFieldCount
  KeyField  m_field[MAXKEYFIELDCOUNT]; // Definition of the fields in the key
  USHORT    m_keySize;                 // Size of key (including datafields) in bytes

  KeyFileDefinition &addField(bool keyField, SortDirection sortdir, DbFieldType type, UINT length);
  KeyType           &setDefined(      KeyType &key, UINT n, bool defined ) const;
  void               getBytes(  const KeyType &key, UINT n,       void *v) const;
  void               putBytes(        KeyType &key, UINT n, const void *v) const;
  void               throwFieldNotVarChar(  UINT n    , const TCHAR *method) const;
  void               throwIndexOutOfRange(  UINT index, const TCHAR *method) const;
  void               throwInvalidFieldCount(UINT n    , const TCHAR *method) const;
  void               throwKeyFieldCount(    UINT n    , const TCHAR *method) const;
  void               throwFieldNotDbAddr(   UINT n    , const TCHAR *method) const;
  void               throwFieldNotString(   UINT n    , const TCHAR *method) const;
  void               throwInvalidFieldType( UINT n    , const TCHAR *method) const;
public:
  KeyFileDefinition() {
    init();
  }
  KeyFileDefinition(KeyFile &keyFile);
  KeyFileDefinition &init();
  // length only used for CSTRING(N),WSTRING(N) and VARCHAR(N).
  // For CSTRING and WSTRING length is in characters.
  // For VARCHAR length is in BYTES
  KeyFileDefinition &addKeyField( SortDirection sortDir, DbFieldType type, UINT length=0);
  KeyFileDefinition &addDataField(                       DbFieldType type, UINT length=0); // Must be at least one keyfield before adding datafields.
  // Cannot add keyfields when the first datafield has been added.

  KeyFileDefinition &addAddrField(bool unique);
  inline void        checkFieldIsVarChar(UINT n    , const TCHAR *method) const {
    const KeyField &f = m_field[n];
    if(f.getType() != DBTYPE_VARCHAR && f.getType() != DBTYPE_VARCHARN) {
      throwFieldNotVarChar(n, method);
    }
  }
  inline void        checkFieldIndex(    UINT index, const TCHAR *method) const {
    if(index >= m_totalFieldCount) throwIndexOutOfRange(index, method);
  }
  inline void        checkFieldCount(    UINT n    , const TCHAR *method) const {
    if(n     >  m_totalFieldCount) throwInvalidFieldCount(n, method);
  }
  inline void        checkKeyFieldCount( UINT n    , const TCHAR *method) const {
    if(n     >  m_keyFieldCount  ) throwKeyFieldCount(n, method);
  }
  inline void        checkFieldIsDbAddr( UINT n    , const TCHAR *method) const {
    if(m_field[n].getType() != DBTYPE_DBADDR) throwFieldNotDbAddr(n, method);
  }

  inline UINT getSize() const { // Total size of key in bytes
    return m_keySize;
  }

  inline UINT getFieldCount() const { // Total number of fields in key
    return m_totalFieldCount;
  }

  inline UINT getKeyFieldCount() const { // Number of fields defined the key-part of the KeyFileDefinition
    return m_keyFieldCount;
  }

  inline UINT getDataFieldCount() const {
    return m_totalFieldCount - m_keyFieldCount;
  }

  inline const KeyField &getFieldDef(UINT i) const {
    return m_field[i];
  }

//  bool isUnique() const;

  void      *getFieldAddr(const KeyType &key,  UINT   n) const;
  USHORT     getFieldOffset(                   UINT   n) const;
  USHORT     getIndicatorOffset(               UINT   n) const;

  void get(const KeyType &key, UINT n,       char       &v) const;
  void put(      KeyType &key, UINT n, const char       &v) const;
  void get(const KeyType &key, UINT n,       UCHAR      &v) const;
  void put(      KeyType &key, UINT n, const UCHAR      &v) const;
  void get(const KeyType &key, UINT n,       short      &v) const;
  void put(      KeyType &key, UINT n, const short      &v) const;
  void get(const KeyType &key, UINT n,       USHORT     &v) const;
  void put(      KeyType &key, UINT n, const USHORT     &v) const;
  void get(const KeyType &key, UINT n,       int        &v) const;
  void put(      KeyType &key, UINT n, const int        &v) const;
  void get(const KeyType &key, UINT n,       UINT       &v) const;
  void put(      KeyType &key, UINT n, const UINT       &v) const;
  void get(const KeyType &key, UINT n,       long       &v) const;
  void put(      KeyType &key, UINT n, const long       &v) const;
  void get(const KeyType &key, UINT n,       ULONG      &v) const;
  void put(      KeyType &key, UINT n, const ULONG      &v) const;
  void get(const KeyType &key, UINT n,       INT64      &v) const;
  void put(      KeyType &key, UINT n, const INT64      &v) const;
  void get(const KeyType &key, UINT n,       UINT64     &v) const;
  void put(      KeyType &key, UINT n, const UINT64     &v) const;
  void get(const KeyType &key, UINT n,       float      &v) const;
  void put(      KeyType &key, UINT n, const float      &v) const;
  void get(const KeyType &key, UINT n,       double     &v) const;
  void put(      KeyType &key, UINT n, const double     &v) const;
  void get(const KeyType &key, UINT n,       String     &v) const;
  void put(      KeyType &key, UINT n, const String     &v) const;
  void get(const KeyType &key, UINT n,       varchar    &v) const;
  void put(      KeyType &key, UINT n, const varchar    &v) const;
  void get(const KeyType &key, UINT n,       Date       &v) const;
  void put(      KeyType &key, UINT n, const Date       &v) const;
  void get(const KeyType &key, UINT n,       Time       &v) const;
  void put(      KeyType &key, UINT n, const Time       &v) const;
  void get(const KeyType &key, UINT n,       Timestamp  &v) const;
  void put(      KeyType &key, UINT n, const Timestamp  &v) const;
  void get(const KeyType &key, UINT n,       TupleField &v) const;
  void put(      KeyType &key, UINT n, const TupleField &v) const;
  KeyType  &initField(                       KeyType &key, UINT n) const;
  bool      isDefined(                 const KeyType &key, UINT n) const;
  inline KeyType  &setUndefined(             KeyType &key, UINT n) const {
    return setDefined(key,n,false);
  }
  DbAddr    getRecordAddr(             const KeyType &key) const;
  KeyType  &putRecordAddr(                   KeyType &key, const DbAddr &addr) const;

  String sprintf(const KeyType &key, UINT fieldCount) const;

  inline String sprintf(const KeyType &key) const {
    return sprintf(key, m_totalFieldCount);
  }

  void fprintf(FILE *f, const KeyType &key, UINT fieldCount) const;

  inline void fprintf(FILE *f, const KeyType &key) const {
    fprintf(f, key, m_totalFieldCount);
  }

  inline void printf(const KeyType &key) const {
    fprintf(stdout, key);
  }

  inline void printf(const KeyType &key, UINT fieldCount) const {
    fprintf(stdout, key, fieldCount);
  }

  KeyType &sscanf(const String &str, KeyType &key) const;

  KeyType &scanf(KeyType &key, UINT fieldCount) const;

  inline KeyType &scanf(KeyType &key) const {
    return scanf(key, m_totalFieldCount);
  }

  int    keynCmp(                           const KeyType &key1, const KeyType &key2, UINT fieldCount) const;
  int    keyCmp(                            const KeyType &key1, const KeyType &key2) const;
  bool   keynCmpRelOp(  RelationType relop, const KeyType &key1, const KeyType &key2, UINT fieldCount) const;
  bool   keyCmpRelOp(   RelationType relop, const KeyType &key1, const KeyType &key2) const;
  String toString() const;
  void   dump(FILE *f = stdout) const;
};

#pragma pack(push,2)

class KeyPageHeader {
public:
  UINT      m_leafPage  : 1;
  UINT      m_itemCount : 15;
  String toString() const;
};

#define KEY_DATASIZE (KEYPAGESIZE-sizeof(KeyPageHeader))

class KeyPageItem {
public:
  DbAddrFileFormat  m_child; // Must be first element
  KeyType           m_key;   // Not every byte is written to file. sizeof(KeyType) is maximum keysize
  String toString(const KeyFileDefinition &keydef) const;
};

class KeyPageInfo {
public:
  USHORT m_keySize;
  USHORT m_halfMaxKeyCount;
  USHORT m_maxKeyCount;
  USHORT m_itemSize;
  USHORT m_halfMaxItemCount;
  USHORT m_maxItemCount;
  USHORT m_pageSize;
  KeyPageInfo() {
  };
  KeyPageInfo(USHORT keySize);
  void init(USHORT keySize);
  String toString() const;
};

class KeyPage {
private:
  KeyPageHeader        m_header;

  // m_h and m_items must be declared after each other. Written to file
  BYTE                 m_items[KEY_DATASIZE];

  const KeyPageInfo    m_pageInfo;                                 // These fields must be declared after m_items.
                                                                   // Everything after m_items is NOT written to file.

  KeyPageAddr          getP0() const;
  void                 setP0(const KeyPageAddr &addr);
public:
  KeyPage(USHORT keySize);
  KeyPage(const KeyPageInfo &pageInfo);

  // Initialize m_header and m_items
  void         init(bool leafPage);
  inline bool  isLeafPage()      const {
    return m_header.m_leafPage ? true : false;
  }
  inline UINT  getItemSize()     const {
    return m_pageInfo.m_itemSize;
  }
  inline UINT  getKeySize()      const {
    return m_pageInfo.m_keySize;
  }
  inline UINT  getHalfSize()     const {
    return isLeafPage() ? m_pageInfo.m_halfMaxKeyCount : m_pageInfo.m_halfMaxItemCount;
  }
  inline UINT  getPageSize()     const {
    return m_pageInfo.m_pageSize;
  }
  inline UINT  getMaxItemCount() const {
    return isLeafPage() ? m_pageInfo.m_maxKeyCount : m_pageInfo.m_maxItemCount;
  }
  inline const KeyPageInfo   &getPageInfo() const {
    return m_pageInfo;
  }

  // 1 <= i <= itemcount. For leafpages t.m_child must be DB_NULLADDR
  void                 setItem(UINT i, const KeyPageItem &t);
  KeyPageItem         &getItem(UINT i,       KeyPageItem &t) const;
  KeyPageItem         &getLastItem(          KeyPageItem &t) const;
  KeyType             &setKey( UINT i, const KeyType     &key);

  // 1 <= i <= itemcount
  const KeyType       &getKey( UINT i) const;
  inline const KeyType &getLastKey() const {
    return getKey(getItemCount());
  };

  // 0 <= i <= itemcount. For leafpages addr must be DB_NULLADDR
  void                 setChild( UINT i, KeyPageAddr addr );

  // 0 <= i <= itemcount. Return DB_NULLADDR for leaf-pages
  KeyPageAddr          getChild( UINT i) const;

  // Return DB_NULLADDR for leaf-pages
  inline KeyPageAddr          getLastChild() const {
    return getChild(getItemCount());
  }
  void                 copyItems(int from, int to, const KeyPage &src, int start); // from <= to && to-from+1 <= src.getItemCount()-start
  int                  searchMinIndex(const KeyType &key, const KeyFileDefinition &keydef, UINT fieldCount);
  int                  searchMaxIndex(const KeyType &key, const KeyFileDefinition &keydef, UINT fieldCount);
  void                 setItemCount(UINT value);                    // 0 <= value <= getMaxItemCount()
  inline USHORT        getItemCount() const {                       // Number of keys in page
    return m_header.m_itemCount;
  }

  // itemcount < getMaxItemCount()
  void                 itemCountIncr();

  // itemcount > 0. Return true if underflow, ie itemCount becomes < getHalfSize()
  bool                 itemCountDecr();

  // i <= itemcount < getMaxItemCount(). For leafpages t.m_child must be DB_NULLADDR
  void                 insertItem(UINT i, const KeyPageItem &t);

  // 1 <= i <= itemcount.Return true on underflow. See condition for itemCountDecr.
  bool                 removeItem(UINT i);
  inline bool          isFull() const {
    return getItemCount() >= getMaxItemCount();
  }
  KeyPageAddr          getNextFree() const;
  void                 setNextFree(KeyPageAddr addr);
  const TCHAR         *getTypeStr() const;
  String toString(const KeyFileDefinition &keydef) const;
};

class DbFileHeader {
protected:
  void setFileVersion();
public:
  char m_version[10];
  String getDbFileVersion() const {
    return m_version;
  }
  // will throw sqlError if version mismatch
  void checkFileVersion(const String &fileName) const;
};

class KeyFileHeader : public DbFileHeader {
public:
  DbAddrFileFormat  m_root;
  DbAddrFileFormat  m_freeList;
  ULONG             m_freeListSize;
  DbAddrFileFormat  m_last;
  KeyFileDefinition m_keydef;

  // #elements in file.
  UINT64            m_keyCount;

  void init(const KeyFileDefinition &keydef);
};

#pragma pack(pop)

typedef enum {
  DBFMODE_READWRITE
 ,DBFMODE_READONLY
} DbFileMode;

class DbFile;

class LogFile {
private:
  Semaphore m_sem;
  ULONG     m_threadId;
  FILE     *m_f;
  bool      m_inTMF;
  ULONG     m_count;
  UINT64    m_end;
  void setCount( ULONG  count, UINT64 end );
  void getHead(  ULONG &count, UINT64 &end) const;
  void throwInvalidThreadId() const;
  inline void checkThreadId() const {
    if(GetCurrentThreadId() != m_threadId) throwInvalidThreadId();
  }
public:
  LogFile(const String &fname);
  ~LogFile();
  void abort();
  void commit();
  void begin();
  inline bool inTMF() const {
    return m_inTMF && (m_threadId == GetCurrentThreadId());
  }
  void log(const DbFile &dbf, UINT64 offset, const void *buf, ULONG size);
  ULONG getCount() const;
  static const char *getDbProgramVersion();
  static String      getDbProgramVersionString() {
    return getDbProgramVersion();
  }
};

class Database;

class DbFile {
private:
  static Semaphore  s_filesem;
  LogFile          *m_logFile;
  String            m_fileName;
  DbFileMode        m_mode;
  void init(const String &fileName, DbFileMode mode, LogFile *lf);
protected:
  void appendLog(UINT64 offset, const void *buffer, ULONG size) const;
  friend class LogFile;
public:
  DbFile(const String &fileName, DbFileMode mode, LogFile *lf); // lf can be NULL
  DbFile(const Database &db, const String &fileName, DbFileMode mode, bool systemFile);
  const String &getName() const { return m_fileName;        }
  DbFileMode    getMode() const { return m_mode;            }

  // size of file in bytes
  UINT64        getSize();
  void          truncate();
  inline bool   isBackLogged()       const { return m_logFile != NULL; }

  // write size bytes at offset from buffer to file
  void          write(UINT64 offset, const void *buffer, UINT size) const;

  // read size bytes at offset from file into buffer
  void          read( UINT64 offset,       void *buffer, UINT size) const;
  static void   create( const String &fileName);
  static void   destroy(const String &fileName);
  static bool   exist(  const String &fileName);
  static void   rename( const String &from, const String &to);
  static String dbFileName(const String &dbName, const String &fileName);
};

#pragma pack(push,2)

class DataFileHeader : public DbFileHeader {
public:
  DbAddrFileFormat  m_freeTree;
  DbAddrFileFormat  m_freeList;
  DbAddrFileFormat  m_end;
  void init();
};

class FreeKey {
public:
  ULONG            m_recSize;
  DbAddrFileFormat m_addr;
};

class FreePageItem {
public:
  FreeKey           m_key;
  DbAddrFileFormat  m_child;
};

#define FREEPAGEHALFSIZE 128
#define FREEPAGEMAXCOUNT (2*(FREEPAGEHALFSIZE))

class FreePage {
private:
  USHORT            m_itemCount;
  DbAddrFileFormat  m_p0;
  FreePageItem      m_e[FREEPAGEMAXCOUNT];
public:
  void                        init() { memset(this,0,sizeof(FreePage)); }
  void                        setItem(UINT i, const FreePageItem &t);
  const        FreePageItem  &getItem(UINT i) const;
  inline const FreePageItem  &getLastItem()  const { return getItem(getItemCount()); }
  void                        setChild( UINT i, DbAddr addr);
  DbAddr                      getChild( UINT i) const;
  inline const FreeKey       &getKey( UINT i) const { return getItem(i).m_key; };
  UINT                        getItemCount() const { return m_itemCount;      }
  void                        itemCountIncr();
  void                        itemCountDecr();
  void                        setItemCount(int value);
  void                        copyItems(int from, int to, const FreePage &src, int start); // from <= to && to-from+1 <= src.getItemCount()-start
  void                        insertItem(UINT i, const FreePageItem &t);
  void                        removeItem(UINT i);
};

#pragma pack(pop)

class DataFile : public DbFile {
private:
  DataFileHeader    m_dh;
  void   init();
  void   writeHead(const DataFileHeader &dh);
  void   readHead(       DataFileHeader &dh, bool log) const;
  void   listFreeList();
  DbAddr getFreeList() const { return m_dh.m_freeList; }
  DbAddr getFreeTree() const { return m_dh.m_freeTree; }
  DbAddr end()         const { return m_dh.m_end;      }
  DbAddr dataFetchFreePage();
  void   dataReleaseFreePage(DbAddr addr);
  int    freeKeyCmp(    const FreeKey &key1, const FreeKey &key2) const;
  int    freeKeyCmpIns( const FreeKey &key1, const FreeKey &key2) const;
  void   freePageInsert(          DbAddr        a      ,
                                  FreeKey      &key    ,
                                  bool         &h      ,
                                  FreePageItem &v      );
  void   freePageUnderflow(       DbAddr        a      ,
                                  FreePage     &c      ,
                                  int           s      ,
                                  bool         &h      );
  void   freePageDel(             DbAddr        p      ,
                                  FreePage     &a      ,
                                  int           r      ,
                                  bool         &h      );
  void   freePageDelete(          DbAddr        a      ,
                                  FreeKey      &key    ,
                                  bool         &h      );
  void   freeTreeInsert(          DbAddr        addr   ,
                                  ULONG         recSize);
  DbAddr freeTreeSearchDelete(    ULONG         recSize);
  void   freePageDump(            DbAddr        addr   ,
                                  FreePage     &page   ,
                                  int           level  ,
                                  FILE         *f      ) const;
  void   freeAddrDump(            DbAddr        addr   ,
                                  int           level  ,
                                  FILE         *f      ) const;

public:
  DataFile(const Database &db, const String &fileName, DbFileMode mode, bool systemFile = true);
  DataFile(                    const String &fileName, DbFileMode mode, LogFile *lf);
  static void create( const String &fileName);
  DbAddr  insert(const void *rec, ULONG recSize);
  void    remove(     DbAddr addr);

  // remove all data!!! be careful
  void    truncate();

 // maxSize used to check that read recordsize <= maxSize
  void    readRecord( DbAddr addr, void *rec, ULONG maxSize);
  void    update(     DbAddr addr, const void *rec);
  void    readVarChar(DbAddr addr, varchar &vchar) const;
  void    freeTreeDump(FILE *f = stdout) const;
};

class KeyFileScanner {
public:
  virtual bool handleKey(const KeyType &key) = 0;
};

class KeyFilePageScanner {
public:
  virtual bool handlePage(KeyPageAddr addr, int level, const KeyPage &page) = 0;
};

class KeyFileInfo {
public:

  // Size of file in bytes
  UINT64            m_size;
  KeyFileHeader     m_header;
  KeyPageInfo       m_pageInfo;
  // Height of btree.
  UINT              m_treeHeight;
  // Number of pages in tree (containing data)
  UINT              m_dataPageCount;
  // Number of items in rootpage
  UINT              m_rootPageItemCount;
  // Index of rootpage [starting at 1]
  KeyPageAddr       m_rootPageIndex;
  // Byte offset of rootpage
  KeyPageAddr       m_rootPageAddress;
  // #keys / (#pages * maxItemsPerPage) * 100
  double            m_utilizationRate;
  CompactIntArray   m_pageCountPerLevel;
  String toString() const;
};


class KeyFile : public DbFile {
private:
  const KeyFileDefinition m_keydef;
  const KeyPageInfo       m_pageInfo;

  friend class GameFileIterator;

  void         init();
  void         writePage(       KeyPageAddr addr, const KeyPage &page);
  void         logPage(         KeyPageAddr addr, const KeyPage &page);
  void         readHead(              KeyFileHeader &header, bool log);
  void         writeHead(       const KeyFileHeader &header);
  void         dumpPage(        const KeyPage &page, FILE *f = stdout) const;
  void         pageUnderflow(   KeyPageAddr a, KeyPage &c, int s  , bool &h);
  void         pageDel(         KeyPageAddr p, KeyPage &a, int r  , bool &h);
  void         pageDelete(      KeyPageAddr a, const KeyType &key , bool &h);
  void         pageInsert(      KeyPageAddr a, const KeyType &key , bool &h, KeyPageItem &v);
  void         pageUpdate(      KeyPageAddr a, const KeyType &key);
  void         dbAddrDump(      KeyPageAddr addr, int level, FILE *f, int flags);
  bool         pageSearchMaxLE( KeyPageAddr addr, KeyType &key, UINT fieldCount);
  bool         pageSearchMaxLT( KeyPageAddr addr, KeyType &key, UINT fieldCount);
  bool         pageSearchLast(  KeyPageAddr addr, KeyType &key);
  bool         pageSearchMinGE( KeyPageAddr addr, KeyType &key, UINT fieldCount);
  bool         pageSearchMinGT( KeyPageAddr addr, KeyType &key, UINT fieldCount);
  bool         pageSearchFirst( KeyPageAddr addr, KeyType &key);

  bool         pageScanAsc(     KeyPageAddr         addr
                               ,RelationType        beginrelop
                               ,const KeyType      *beginkey
                               ,UINT                beginfieldcount
                               ,KeyFileScanner     &scanner);

  bool         pageScanDesc(    KeyPageAddr         addr
                               ,RelationType        beginrelop
                               ,const KeyType      *beginkey
                               ,UINT                beginfieldcount
                               ,KeyFileScanner     &scanner);

  bool         pageScan(        KeyPageAddr         addr
                               ,int                 level
                               ,KeyFilePageScanner &scanner
                               ,int                 maxLevel = -1);

  KeyPageAddr  getFreeList();
  void         setFreeList(     KeyPageAddr addr, int dsize); // dsize = +/-1
  KeyPageAddr  getLast();
  void         setLast(         KeyPageAddr addr);
  void         setRoot(         KeyPageAddr addr, int delta);
  KeyPageAddr  fetchNewPage(bool leafPage);
  void         releasePage(     KeyPageAddr addr);
public:
  KeyFile(const Database &db, const String &fileName, DbFileMode mode, bool systemFile = true);
  KeyFile(                    const String &fileName, DbFileMode mode, LogFile *lf);
  static void  create(const String &fileName, const KeyFileDefinition &keydef);

  void         insert(        const KeyType &key);
  void         remove(        const KeyType &key);
  // update data part of the key. if keypart needs to be updated, use remove and insert
  void         update(        const KeyType &key);

  // remove all keys !!! be careful
  void         truncate();
  void         readPage(      KeyPageAddr addr, KeyPage &page);

  bool         searchMin(     RelationType relop, KeyType &key, UINT fieldCount);
  bool         searchMax(     RelationType relop, KeyType &key, UINT fieldCount);

  void         scanAsc(       RelationType    beginRelOp
                             ,const KeyType  *beginKey
                             ,UINT            beginFieldCount
                             ,KeyFileScanner &scanner );

  void         scanDesc(      RelationType    beginRelOp
                             ,const KeyType  *beginKey
                             ,UINT            beginFieldCount
                             ,KeyFileScanner &scanner );

  const KeyFileDefinition &getDefinition() const;

  // Size of 1 page in bytes
  UINT                     getPageSize() const;

  // Number of released pages (length of freelist)
  UINT                     getFreePageCount();

  // Number of pages in btree, Containing data.
  UINT                     getPageCount();
  KeyFileInfo              getInfo();
  UINT64                   getKeyCount();
  KeyPageAddr              getPageAddr(const KeyType &key);

  inline KeyPageAddr getPageOffset(const KeyPageAddr &addr) const {
    return (KeyPageAddr)sizeof(KeyFileHeader) + m_pageInfo.m_pageSize * (addr-1);
  }
  void dump(FILE *f, int flags);
};

class KeyCursor : public KeyFileScanner {
private:
  String            m_fileName;
  KeyFileDefinition m_keydef;
  char              m_buffer[CURSORBUFSIZE];
  KeyType           m_beginKey;
  KeyType           m_endKey;
  KeyType           m_lastKey;
  RelationType      m_beginRelOp;
  RelationType      m_endRelOp;
  int               m_beginFieldCount;
  int               m_endFieldCount;
  SortDirection     m_dir;
  UINT              m_bufCount;
  UINT              m_bufSize;
  UINT              m_bufPointer;
  bool              m_firstTime;
  bool              m_lastWasFull;
  bool              m_hasNext;

  bool      bufferIsFull() const;
  bool      bufferIsEmpty() const;
  KeyType  &bufferFetch(KeyType &key);
  void      bufferInit();
  void      bufferFill(KeyFile &dbf);
  bool      handleKey(const KeyType &key);
public:
  KeyCursor(KeyFile       &keyFile
           ,RelationType   beginRelOp
           ,const KeyType *beginKey
           ,UINT           beginFieldCount
           ,RelationType   endRelOp
           ,const KeyType *endKey
           ,UINT           endFieldCount
           ,SortDirection  dir
           );

 ~KeyCursor();
  // return key;
  KeyType &next(KeyType &key);
  bool hasNext() const;
  const KeyFileDefinition &getKeyDef() const {
    return m_keydef;
  }
};

#define DUMP_ALL      0x01
#define DUMP_NEWLINE  0x04
#define DUMP_HEADER   0x08
#define DUMP_FREELIST 0x10
#define FULL_DUMP     0xff

class DbNames {
public:
  TCHAR TABLEDATA_FNAME[20];
  TCHAR TABLEKEY1_FNAME[20];
  TCHAR TABLEKEY2_FNAME[20];
  TCHAR COLUMNDATA_FNAME[20];
  TCHAR COLUMNKEY_FNAME[20];
  TCHAR INDEXDATA_FNAME[20];
  TCHAR INDEXKEY1_FNAME[20];
  TCHAR INDEXKEY2_FNAME[20];
  TCHAR USERDATA_FNAME[20];
  TCHAR USERKEY_FNAME[20];
  TCHAR CODEDATA_FNAME[20];
  TCHAR CODEKEY_FNAME[20];
  TCHAR STATDATA_FNAME[20];
  TCHAR STATKEY_FNAME[20];
  TCHAR LOG_FNAME[60];
  TCHAR DBDATA_FNAME[60];
  TCHAR DBKEY_FNAME[60];
  TCHAR DBLOG_FNAME[60];
  TCHAR SYSTABLE_LOGICALNAME[20];
  TCHAR SYSCOLUMN_LOGICALNAME[20];
  TCHAR SYSINDEX_LOGICALNAME[20];
  TCHAR SYSUSER_LOGICALNAME[20];
  TCHAR SYSCODE_LOGICALNAME[20];
  TCHAR SYSSTAT_LOGICALNAME[20];
  DbNames();
};

extern DbNames systemFileNames;

#define SYSTEM_TABLEDATA_FNAME    systemFileNames.TABLEDATA_FNAME
#define SYSTEM_TABLEKEY1_FNAME    systemFileNames.TABLEKEY1_FNAME
#define SYSTEM_TABLEKEY2_FNAME    systemFileNames.TABLEKEY2_FNAME
#define SYSTEM_COLUMNDATA_FNAME   systemFileNames.COLUMNDATA_FNAME
#define SYSTEM_COLUMNKEY_FNAME    systemFileNames.COLUMNKEY_FNAME
#define SYSTEM_INDEXDATA_FNAME    systemFileNames.INDEXDATA_FNAME
#define SYSTEM_INDEXKEY1_FNAME    systemFileNames.INDEXKEY1_FNAME
#define SYSTEM_INDEXKEY2_FNAME    systemFileNames.INDEXKEY2_FNAME
#define SYSTEM_USERDATA_FNAME     systemFileNames.USERDATA_FNAME
#define SYSTEM_USERKEY_FNAME      systemFileNames.USERKEY_FNAME
#define SYSTEM_CODEDATA_FNAME     systemFileNames.CODEDATA_FNAME
#define SYSTEM_CODEKEY_FNAME      systemFileNames.CODEKEY_FNAME
#define SYSTEM_STATDATA_FNAME     systemFileNames.STATDATA_FNAME
#define SYSTEM_STATKEY_FNAME      systemFileNames.STATKEY_FNAME
#define SYSTEM_LOG_FNAME          systemFileNames.LOG_FNAME
#define SYSTEM_DBDATA_FNAME       systemFileNames.DBDATA_FNAME
#define SYSTEM_DBKEY_FNAME        systemFileNames.DBKEY_FNAME
#define SYSTEM_DBLOG_FNAME        systemFileNames.DBLOG_FNAME
#define SYSTABLE                  systemFileNames.SYSTABLE_LOGICALNAME
#define SYSCOLUMN                 systemFileNames.SYSCOLUMN_LOGICALNAME
#define SYSINDEX                  systemFileNames.SYSINDEX_LOGICALNAME
#define SYSUSER                   systemFileNames.SYSUSER_LOGICALNAME
#define SYSCODE                   systemFileNames.SYSCODE_LOGICALNAME
#define SYSSTAT                   systemFileNames.SYSSTAT_LOGICALNAME

typedef enum {
  TABLETYPE_SYSTEM     = 'S'
 ,TABLETYPE_USER       = 'U'
} TableType;

typedef enum {
  INDEXTYPE_PRIMARY    = 'P'
 ,INDEXTYPE_UNIQUE     = 'U'
 ,INDEXTYPE_NON_UNIQUE = 'N'
} IndexType;

class SysTableTableData {
public:
  TCHAR          m_tableName[MAXTABLENAME+1];
  TCHAR          m_fileName[ MAXFILENAME +1];
  ULONG          m_sequenceNo;
  char           m_tableType;
  USHORT         m_recSize;
};

class SysTableColumnData {
public:
  TCHAR          m_tableName[   MAXTABLENAME+1];
  USHORT         m_colNo;
  TCHAR          m_columnName[  MAXCOLUMNNAME+1];
  TCHAR          m_dbType[10];
  ULONG          m_offset;
  // in bytes
  ULONG          m_len;
  char           m_nulls;
  TCHAR          m_defaultValue[MAXDEFAULTVALUE+1];
};

class SysTableUserData {
public:
  TCHAR          m_username[MAXUSERNAME+1];
  TCHAR          m_password[MAXPASSWORD+1];
};

class SysTableCodeKey {
public:
  TCHAR          m_filename[30];
  int            m_nr;
};

class SysTableStatData {
public:
  TCHAR          m_indexName[MAXINDEXNAME+1];
  double         m_totalCount;
  double         m_field1;
  double         m_field2;
  double         m_field3;
  double         m_field4;
  void dump(FILE *f = stdout) const;
  double selectivity(int fieldCount) const;
};

class SysTableStatKey {
public:
  TCHAR          m_indexName[MAXINDEXNAME+1];
};

class SysTableDbData {
public:
   // must be the same length as dbName in sqlapi.h
  TCHAR          m_dbName[SQLAPI_MAXDBNAME+1];
  TCHAR          m_path[30];
  UCHAR          m_colseq[256];
};

class IndexDefinition;

class SysTableIndexData {
private:
  TCHAR          m_tableName[MAXTABLENAME+1];
  TCHAR          m_indexName[MAXINDEXNAME+1];
  TCHAR          m_fileName[ MAXFILENAME+1];
  TCHAR          m_indexType;
  USHORT         m_colCount;
  TCHAR          m_columns[  MAXKEYFIELDCOUNT*4 + 1];
  TCHAR          m_ascending[MAXKEYFIELDCOUNT+1];
public:
  SysTableIndexData() {
    memset(this, 0, sizeof(SysTableIndexData));
  }
  SysTableIndexData(const IndexDefinition &indexDef);
  friend class IndexDefinition;
  friend class Database;
};

class IndexColumn {
public:
  USHORT m_col;
  bool   m_asc;
  IndexColumn() {}
  IndexColumn(USHORT col, bool asc) { m_col = col; m_asc = asc; }
};

class IndexDefinition {
public:
  String                    m_tableName;
  String                    m_indexName;
  String                    m_fileName;
  IndexType                 m_indexType;
  CompactArray<IndexColumn> m_columns;
  IndexDefinition() {}
  IndexDefinition(const SysTableIndexData &indexData);
  inline UINT               getColumnCount()  const { return (UINT)m_columns.size(); }
  inline IndexType          getIndexType()    const { return m_indexType;      }
  inline const IndexColumn &getColumn(UINT i) const { return m_columns[i];     }

   // returns true if tablefield colindex is in this
  bool                      columnIsMember(UINT colindex) const;
  inline bool               isUnique()       const { return getIndexType() == INDEXTYPE_PRIMARY || getIndexType() == INDEXTYPE_UNIQUE; }

  // returns the index of tablefield colindex in this, -1 if not member
  int                       getFieldIndex(UINT colindex) const;
  String toString() const;
  void dump(FILE *f = stdout) const;
};
Packer &operator<<(Packer &p, const IndexDefinition &indexDef);
Packer &operator>>(Packer &p,       IndexDefinition &indexDef);

typedef Array<IndexDefinition> IndexArray;

// Minimized version of ColumnDefinition.
// Don't add any membervariables with non-trivial constructor (used in CompactArray)
class ColumnInfo {
public:
  DbFieldType    m_type;
  ULONG          m_offset;
  // in bytes
  ULONG          m_len;
  inline DbFieldType getType()         const { return m_type;                }
  inline DbMainType  getMainType()     const { return ::getMainType(m_type); }
  ULONG       size()            const;
   // In characters
  UINT        getMaxStringLen() const;
  String      toString()        const;
};

class ColumnDefinition : public ColumnInfo {
public:
  String         m_name;
  bool           m_nullAllowed;
  String         m_defaultValue;
  ColumnDefinition(const SysTableColumnData &colData);
  ColumnDefinition(DbFieldType type, const String &name, ULONG len = 0, const String defaultValue = EMPTYSTRING);
  ColumnDefinition() {}
  String toString() const;
};

class TableDefinition {
private:
  String                  m_tableName;
  String                  m_fileName;
  ULONG                   m_sequenceNo;
  TableType               m_tableType;
  UINT                    m_recSize;
  Array<ColumnDefinition> m_columns;
  UINT adjustOffset(ColumnDefinition &colDef);
public:
  TableDefinition(TableType type, const String &tableName, const String &fileName = EMPTYSTRING);
  TableDefinition(const SysTableTableData &tableData);
  TableDefinition() { init(TABLETYPE_USER,EMPTYSTRING); }
  void init(TableType type, const String &tableName, const String &fileName = EMPTYSTRING);
  void addColumn(const ColumnDefinition   &colDef );
  void addColumn(const SysTableColumnData &colData);
  int                            findColumnIndex(const String &columnName) const;
  KeyFileDefinition              getKeyFileDefinition(const IndexDefinition &indexDef) const;
  inline UINT                    getRecordSize()   const { return m_recSize; }
  inline const String           &getTableName()    const { return m_tableName;      }
  inline TableType               getTableType()    const { return m_tableType;      }
  const inline String           &getFileName()     const { return m_fileName;       }
  inline ULONG                   getSequenceNo()   const { return m_sequenceNo;     }
  inline const ColumnDefinition &getColumn(UINT i) const { return m_columns[i];     }
  inline UINT                    getColumnCount()  const { return (UINT)m_columns.size(); }
  String toString() const;
  void dump(FILE *f = stdout) const;
  friend Packer &operator<<(Packer &p, const TableDefinition &tableDef);
  friend Packer &operator>>(Packer &p,       TableDefinition &tableDef);
};

typedef CompactShortArray FieldSet;

// Minimized version of TableDefinition
class TableInfo {
private:
  ULONG                      m_sequenceNo;
  String                     m_fileName;
  UINT                       m_recSize;
  CompactArray<ColumnInfo>   m_columns;
  IndexArray                 m_indexArray;
  FieldSet                   m_allFields;

  ULONG getIndicatorOffset(UINT n) const;
  void init(const TableDefinition &tableDef);
  void             setDefined(         RecordType &rec, UINT n, bool defined)  const;
  void            *getFieldAddr( const RecordType &rec, UINT n)                const;
  void             getBytes(     const RecordType &rec, UINT n,       void *v) const;
  void             putBytes(           RecordType &rec, UINT n, const void *v) const;
  void             throwIndexOutOfRange(  UINT index     , const TCHAR *method) const;
  void             throwInvalidFieldCount(UINT fieldCount, const TCHAR *method) const;
  void             throwInvalidFieldType( UINT index     , const TCHAR *method) const;
  void             throwFieldNotString(   UINT index     , const TCHAR *method) const;
public:
  void checkFieldIndex(UINT index, const TCHAR *method) const {
    if(index >= m_columns.size()) throwIndexOutOfRange(index, method);
  }
  void checkFieldCount(UINT fieldCount, const TCHAR *method) const {
    if(fieldCount > m_columns.size()) throwInvalidFieldCount(fieldCount, method);
  }
//  TableInfo();
  TableInfo(const TableDefinition &tableDef, const IndexArray &indexArray);

  void get(const RecordType &rec, UINT n,       char       &v) const;
  void put(      RecordType &rec, UINT n, const char       &v) const;
  void get(const RecordType &rec, UINT n,       UCHAR      &v) const;
  void put(      RecordType &rec, UINT n, const UCHAR      &v) const;
  void get(const RecordType &rec, UINT n,       short      &v) const;
  void put(      RecordType &rec, UINT n, const short      &v) const;
  void get(const RecordType &rec, UINT n,       USHORT     &v) const;
  void put(      RecordType &rec, UINT n, const USHORT     &v) const;
  void get(const RecordType &rec, UINT n,       int        &v) const;
  void put(      RecordType &rec, UINT n, const int        &v) const;
  void get(const RecordType &rec, UINT n,       UINT       &v) const;
  void put(      RecordType &rec, UINT n, const UINT       &v) const;
  void get(const RecordType &rec, UINT n,       long       &v) const;
  void put(      RecordType &rec, UINT n, const long       &v) const;
  void get(const RecordType &rec, UINT n,       ULONG      &v) const;
  void put(      RecordType &rec, UINT n, const ULONG      &v) const;
  void get(const RecordType &rec, UINT n,       INT64      &v) const;
  void put(      RecordType &rec, UINT n, const INT64      &v) const;
  void get(const RecordType &rec, UINT n,       UINT64     &v) const;
  void put(      RecordType &rec, UINT n, const UINT64     &v) const;
  void get(const RecordType &rec, UINT n,       float      &v) const;
  void put(      RecordType &rec, UINT n, const float      &v) const;
  void get(const RecordType &rec, UINT n,       double     &v) const;
  void put(      RecordType &rec, UINT n, const double     &v) const;
  void get(const RecordType &rec, UINT n,       String     &v) const;
  void put(      RecordType &rec, UINT n, const String     &v) const;
  void get(const RecordType &rec, UINT n,       Date       &v) const;
  void put(      RecordType &rec, UINT n, const Date       &v) const;
  void get(const RecordType &rec, UINT n,       Time       &v) const;
  void put(      RecordType &rec, UINT n, const Time       &v) const;
  void get(const RecordType &rec, UINT n,       Timestamp  &v) const;
  void put(      RecordType &rec, UINT n, const Timestamp  &v) const;
  void get(const RecordType &rec, UINT n,       TupleField &v) const;
  void put(      RecordType &rec, UINT n, const TupleField &v) const;

  void                          initField(          RecordType &rec, UINT n)        const;
  bool                          isDefined(    const RecordType &rec, UINT n)        const;
  void                          setUndefined(       RecordType &rec, UINT n)        const;
  bool                          recordsEqual( const RecordType &rec1, const RecordType &rec2) const;
  inline const ColumnInfo      &getColumn(UINT i)  const { return m_columns[i];                }
  inline const FieldSet        &getAllFields()     const { return m_allFields;                 }
  inline const IndexDefinition &getIndex( UINT i)  const { return m_indexArray[i];             }
  inline const String          &getTableName()     const { return m_indexArray[0].m_tableName; }
  inline const String          &getFileName()      const { return m_fileName;                  }
  inline int                    getSequenceNo()    const { return m_sequenceNo;                }
  inline UINT                   getColumnCount()   const { return (UINT)m_columns.size();      }
  inline UINT                   getIndexCount()    const { return (UINT)m_indexArray.size();   }
  inline UINT                   getRecordSize()    const { return m_recSize;                   }
  int                    findIndexNo(const String &indexName) const;
  int                    findPrimaryIndex() const;

  // index is pointer into m_indexArray
  KeyFileDefinition      getKeyFileDefinition(UINT index) const;

  // index is pointer into m_indexArray
  FieldSet               genFieldSet(UINT index, const FieldSet &fields) const;
  String toString() const;
  void dump(FILE *f = stdout) const;
};

class DataFileScanner {
public:
  virtual bool handleData(DataFile &file, DbAddr addr, RecordType &rec) = 0;
};

class Database {
private:
  String        m_dbName;
  String        m_path;
  UCHAR         m_colseq[256];
  LogFile      *m_logFile;

  mutable StringHashMap<TableDefinition>  m_tableDefTableNameCache;  // tableName       ->  TableDefinition
  mutable LongHashMap<TableDefinition*>   m_tableDefSequenceCache;   // tablesequenceno -> *TableDefinition
  mutable StringHashMap<IndexArray>       m_indexTableNameCache;     // tableName       ->  IndexArray
  mutable StringHashMap<IndexDefinition*> m_indexDefIndexNameCache;  // indexName       -> *IndexDefinition
  mutable StringHashMap<TableInfo>        m_tableInfoTableNameCache; // tableName       ->  TableInfo
  mutable LongHashMap<TableInfo*>         m_tableInfoSequenceCache;  // tablesequenceno -> *TableInfo

  TableDefinition readTableDefinition(   const String            &tableName ) const;
  TableDefinition readTableDefinition(   ULONG            sequenceNo) const;

  TableDefinition readColumnDefinitions( const SysTableTableData &tabdata)    const;

  void readIndexDefinitions(        const String            &tableName
                                   ,IndexArray              &indexArray   ) const;
  void sysTabTableInsert(           const String            &tableName
                                   ,const String            &fileName
                                   ,ULONG                    sequenceNo
                                   ,char                     tableType
                                   ,USHORT                   recSize      );

  void sysTabColumnInsert(          const String            &tableName
                                   ,USHORT                   colno
                                   ,const String            &columnName
                                   ,DbFieldType              dbtype
                                   ,ULONG                    offset
                                   ,ULONG                    len
                                   ,char                     nulls
                                   ,const String            &defaultValue = EMPTYSTRING);

  void sysTabIndexInsert(           const IndexDefinition   &index      );
  void removeFromCache(             const String            &tableName  );
  static void destroySystemFiles(SysTableDbData &dbdef);
public:
  Database(const String &dbName);
  ~Database();
  static void createDbDb();
  static void create(SysTableDbData &dbdef);
  static void destroy(       const String   &dbName);
  static bool exist(         const String   &dbName);
  static void sysTabDbDelete(const String   &dbName);
  static void sysTabDbInsert(SysTableDbData &dbdef );
  static bool searchDbName(  const String   &dbName, SysTableDbData &dbdef );
  void trbegin();
  void trcommit();
  void trabort();
  inline bool inTMF() const { return m_logFile->inTMF(); }
  void close();
  void dataFileDump( const String &tableName, FILE *f = stdout);
  void KeyFiledump(  const String &indexName, FILE *f = stdout);
  void dump(         FILE *f = stdout, bool dumpall = false);
  LogFile *getLogFile() const { return m_logFile; }
  const String &getPath() const { return m_path;    }

  void  dataFileScan(    const String         &tableName
                        ,DataFileScanner      &scanner      );

  String getNewFileName( const String         &extension    ) const;

  ULONG getMaxTableSequenceNo() const;

  void sysTabUserInsert( const String         &userName
                        ,const String         &password     );

  void sysTabCodeInsert( SysTableCodeKey      &key
                        ,void                 *rec
                        ,UINT                  size         );

  void sysTabCodeDelete( const String         &fileName     );

  void sysTabCodeDelete( const SysTableCodeKey &key         );

  void sysTabStatInsert( const SysTableStatData &stat       ); // ie update

  void sysTabStatDelete( const String         &indexName    );

  bool sysTabStatRead(   const String         &indexName
                        ,SysTableStatData     &stat         ) const;

  void sysTabReadUser(   const String         &userName
                        ,SysTableUserData     &rec          ) const;

  void tableCreate(      const TableDefinition &tableDef    );

  void tableCreateLike(  const String         &tableName
                        ,const String         &likeName
                        ,const String         &fileName     );

  void tableDrop(        const String         &tableName    );

  void indexCreate(      IndexDefinition      &indexDef     );

  void indexDrop(        const String         &indexName
                        ,bool  forceDeletePrimary = false   );

  const TableDefinition &getTableDefinition( const String  &tableName ) const;
  const TableDefinition &getTableDefinition( ULONG          sequenceNo) const;

  const TableInfo       &getTableInfo(       const String  &tableName ) const;
  const TableInfo       &getTableInfo(       ULONG          sequenceNo) const;

  const IndexArray      &getIndexDefinitions(const String  &tableName ) const;
  const IndexDefinition &getIndexDefinition( const String  &indexName ) const;

  void updateStatistics( const IndexDefinition &indexDef  );
  void updateStatistics( const String          &tableName );
  void updateStatistics();

  UCHAR getMinColSeq() const; // returns the character with the lowest colseq
  UCHAR getMaxColSeq() const; // returns the character with the higest colseq
};

class TuplePrintDefinition : private Array<UINT> {
public:
  TuplePrintDefinition(const HostVarDescriptionList &desc);
  friend class Tuple;
};

class Tuple : public Array<TupleField> {
public:
  Tuple(UINT size);
  Tuple() {};
  String    toString() const;
  String    toString(const TuplePrintDefinition &def) const;
  void dump(     FILE *f = stdout) const;
  void dumpTypes(FILE *f = stdout) const;
};
bool operator==(const Tuple &t1, const Tuple &t2);
bool operator!=(const Tuple &t1, const Tuple &t2);

class TableRecord {
private:
  Database             &m_db;
  TableInfo             m_tableInfo;
  RecordType            m_rec;
  ShortHashMap<varchar> m_varchar;
  const varchar &getVarChar( USHORT col) const;
        varchar &getVarChar( USHORT col);
  void  putVarChar(   USHORT col, const varchar &vchar );
  void  initData();
  void  init();
  void  put(          USHORT col, const TupleField &v);
  void  get(          USHORT col,       TupleField &v) const;
  void  setUndefined( USHORT col);
  bool  isDefined(    USHORT col) const;
  void  get(   Tuple &tuple) const;
  void  get(   Tuple &tuple,  const FieldSet &fields);
  void  read(  DbAddr addr, const FieldSet &fields);
  void  read(  DbAddr addr);
public:
  TableRecord(Database &db, ULONG sequenceNo);
  TableRecord(Database &db, const String &tableName);
  ~TableRecord() {}
  KeyType &genKey(KeyType &key, UINT index, DbAddr addr) const;
  int findPrimaryIndex()                  const { return m_tableInfo.findPrimaryIndex(); }
  const TableInfo &getTableInfo()         const { return m_tableInfo; }
  const FieldSet  &getAllFields()         const { return m_tableInfo.getAllFields(); }
  const String    &getTableName()         const { return m_tableInfo.getTableName(); }
  const String    &getFileName()          const { return m_tableInfo.getFileName();  }
  void   read(  DbAddr addr, const FieldSet &fields, Tuple &tuple);
  void   read(  DbAddr addr, Tuple &tuple);
  void   remove(DbAddr addr);
  void   update(DbAddr addr, const Tuple &tuple);
  void   put(   const Tuple &tuple);
  DbAddr insert(const Tuple &tuple);
  String toString() const;
  void   dump(FILE *f = stdout) const;
};

class TableCursorParam {
public:
  ULONG          m_sequenceNo;  // tablesequenceno
  String         m_indexName;
  bool           m_indexOnly;
  RelationType   m_beginRelOp;
  UINT           m_beginFieldCount;
  RelationType   m_endRelOp;
  UINT           m_endFieldCount;
  SortDirection  m_dir;
  FieldSet       m_fieldSet;
  String toString() const;
  void dump(FILE *f = stdout) const;
};

Packer &operator<<(Packer &p, const TableCursorParam &param);
Packer &operator>>(Packer &p,       TableCursorParam &param);

class TableCursor {
private:
  Database      &m_db;
  ULONG          m_sequenceNo;
  KeyCursor     *m_keyCursor;
  bool           m_indexOnly;
  FieldSet       m_fieldSet;
public:
  TableCursor(Database               &db
             ,const TableCursorParam &param
             ,const Tuple            *beginKey
             ,const Tuple            *endKey  );
  ~TableCursor();
  void next(Tuple &tuple);
  bool hasNext() const;
  const KeyFileDefinition &getKeyDef() const {
    return m_keyCursor->getKeyDef();
  }
};

#ifdef GIVE_LINK_MESSAGE
#pragma message("link with " DBASE_VERSION)
#endif
#pragma comment(lib, DBASE_VERSION "Btree12.lib")
