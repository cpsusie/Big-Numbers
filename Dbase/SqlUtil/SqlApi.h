#pragma once

#include <MyUtil.h>

typedef enum {
  SQL_CALL_COMMIT    ,
  SQL_CALL_CONNECT   ,
  SQL_CALL_IMMEDIATE ,
  SQL_CALL_ROLLBACK  ,
  SQL_CALL_PREPARE   ,
  SQL_CALL_DESCRIBE  ,
  SQL_CALL_OPEN      ,
  SQL_CALL_CLOSE     ,
  SQL_CALL_FETCH     ,
  SQL_CALL_EXECUTE   ,
  SQL_CALL_BIND      ,
  SQL_CALL_UNBIND    ,
  SQL_CALL_CREATE    ,
  SQL_CALL_DROP
} SqlApiCallCode;

#define SQL_CALL_CONNECT_RESET 3
#define SQL_CALL_CONNECT_DB    4
#define SQL_CALL_CONNECT_USING 5

#define SQL_OK                            0
#define SQL_NOT_FOUND                   100
#define SQL_DUPKEY                    -8227
#define SQL_ERROR_NO_CONNECTION       -1000
#define SQL_ERROR_INVALID_APICALL     -1001
#define SQL_ERROR_ALREADY_CONNECTED   -1002
#define SQL_ERROR_NO_SERVER           -1003
#define SQL_SOCKET_ERROR              -1004
#define SQL_DBNOTOPEN                 -1005
#define SQL_NO_TRANSACTION            -1006
#define SQL_NESTED_TRANSACTION        -1007
#define SQL_DBCORRUPTED               -1008
#define SQL_ALREADY_OPEN              -1010
#define SQL_NO_LOG                    -1011
#define SQL_FILE_OPEN_ERROR           -1012
#define SQL_RECSIZE_TOO_BIG           -1013
#define SQL_CREATE_ERROR              -1014
#define SQL_WRITE_ERROR               -1015
#define SQL_INVALID_DELETE            -1016
#define SQL_BUF_TOO_BIG               -1017
#define SQL_INVALID_ADDRESS           -1018
#define SQL_KEY_TOO_BIG               -1020
#define SQL_NO_KEYFIELDS              -1021
#define SQL_KEYFIELD_AFTER_DATAFIELD  -1022
#define SQL_SEEK_ERROR                -1023
#define SQL_READ_ERROR                -1024
#define SQL_INVALID_KEYDEF            -1025
#define SQL_INVALID_FIELDTYPE         -1026
#define SQL_DUPLICATE_FIELDNAME       -1027
#define SQL_FILE_ALREADY_EXIST        -1028
#define SQL_INVALID_TABLENAME         -1029
#define SQL_INVALID_COLUMNNAME        -1030
#define SQL_INVALID_RECSIZE           -1031
#define SQL_INVALID_KEYCOUNT          -1032
#define SQL_FILE_DELETE_ERROR         -1033
#define SQL_INVALID_RELOP             -1034
#define SQL_INVALID_TABLETYPE         -1035
#define SQL_FILE_NOT_BACKLOGGED       -1036
#define SQL_INVALID_INDEXNAME         -1037
#define SQL_FILE_RENAME_ERROR         -1038
#define SQL_INDEX_ALREADY_EXIST       -1039
#define SQL_INVALID_USERNAME          -1040
#define SQL_INVALID_PASSWORD          -1041
#define SQL_INVALID_DBNAME            -1042
#define SQL_INVALID_FILEMODE          -1043
#define SQL_DB_ALREADY_EXIST          -1044
#define SQL_DB_NOT_EXIST              -1045
#define SQL_SYNTAX_ERROR              -1046
#define SQL_TOO_MANY_COLUMNS          -1047
#define SQL_KEYFIELD_NULL             -1048
#define SQL_TOO_COMPLEX               -1049
#define SQL_INVALID_INDEXTYPE         -1050
#define SQL_TIMESTAMP_MISMATCH        -1051
#define SQL_NO_PRIMARY_KEY            -1052
#define SQL_RELEASEERROR              -1053
#define SQL_NOINDEX                   -1054
#define SQL_DBNAME_TOO_LONG           -1055
#define SQL_UNKNOWN_PROGRAMID         -1056
#define SQL_TABLE_ALREADY_EXIST       -1057
#define SQL_INVALID_EXPRLIST          -1058
#define SQL_NODEFAULT_OR_NULLALLOWED  -1059
#define SQL_COLUMN_ALREADY_DEFINED    -1060
#define SQL_INVALID_DEFAULT           -1061
#define SQL_UNKNOWN_DBTYPE            -1062
#define SQL_INVALID_EXPR_TYPE         -1063
#define SQL_NO_HOSTVAR_DEFINED        -1064
#define SQL_TOO_MANY_HOSTVAR          -1065
#define SQL_NULLVALUE_NOT_ALLOWED     -1066
#define SQL_STRING_TOO_LONG           -1067
#define SQL_DIVISION_BY_ZERO          -1068
#define SQL_NULLVALUE_IN_EXPRESSION   -1069
#define SQL_DELETE_PRIMARY_INDEX      -1070
#define SQL_DOMAIN_ERROR              -1071
#define SQL_VARCHAR_TOO_LONG          -1072
#define SQL_INVALID_HOSTVAR_TYPE      -1073
#define SQL_INVALID_CORRELATION_NAME  -1074
#define SQL_AMBIGOUS_COLUMNNAME       -1075
#define SQL_INVALID_ORDERBY           -1076
#define SQL_INVALID_GROUPBY           -1077
#define SQL_INVALID_INTO_LIST         -1078
#define SQL_BUFFER_OVERFLOW           -1079
#define SQL_NAME_TOO_LONG             -1080
#define SQL_ARITHMETIC_OVERFLOW       -1081
#define SQL_INVALID_FILENAME          -1082
#define SQL_UNDECLARED_HOSTVAR        -1083
#define SQL_UNDECLARED_CURSOR         -1084
#define SQL_UNDECLARED_STATEMENT      -1085
#define SQL_STATEMENT_TOO_BIG         -1086
#define SQL_HOSTVAR_ALREADY_DECLARED  -1087
#define SQL_INVALID_DATATYPE          -1088
#define SQL_CURSOR_ALREADY_DECLARED   -1089
#define SQL_STATEMENT_ALREADY_DEFINED -1090
#define SQL_DUPLICATE_CORRELATIONNAME -1100
#define SQL_MODULUS_BY_ZERO           -1101
#define SQL_AGGR_IN_AGGR              -1102
#define SQL_NO_AGGR_NO_GROUPBY        -1103
#define SQL_AGGR_IN_WHERE             -1104
#define SQL_LIKEPATTERN_TOO_LONG      -1105
#define SQL_BUFFER_UNDERFLOW          -1106
#define SQL_NOINDICATOR               -1107
#define SQL_RANGEERROR                -1108
#define SQL_INVALIDCAST               -1109
#define SQL_TABLECURSORNOTOPENED      -1110
#define SQL_INVALIDDATE               -1111
#define SQL_CURSOR_ALREADY_OPEN       -1112
#define SQL_CURSOR_NOT_OPEN           -1113
#define SQL_INVALID_NO_OF_COLUMNS     -1114
#define SQL_TYPEMISMATCH              -1115
#define SQL_NOSUCHRECORD              -1116
#define SQL_INVALIDTIME               -1117
#define SQL_INVALIDTIMESTAMP          -1118
#define SQL_LOOSEPRECISION            -1119
#define SQL_WRONGDBVERSION            -1120
#define SQL_FETCHNEWPAGE_FAILED       -1121

#define SQL_FATAL_ERROR               -9999

class sqlca {
public:
  long  sqlcode;       /* SQL return code */
  TCHAR sqlerrmc[256]; /* Error message tokens */
  void init(long error = SQL_OK);
  sqlca(long error = SQL_OK) { init(error); }
  void seterror( long error) { init(error); }
  void vseterror(long error, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void seterror( long error, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  sqlca(         long error, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void dump(FILE *f = stdout);
  String toString() const;
};

#define SQLCODE sqlca.sqlcode

class SqlApiSqlName {
public:
  short int      length;
  TCHAR          data[30];
};

/* must match the definitions of dbtypestr.cpp */
typedef enum {
  DBTYPE_UNKNOWN    =  0
 ,DBTYPE_DBADDR     =  1
 ,DBTYPE_CHAR       =  3
 ,DBTYPE_CHARN      =  4
 ,DBTYPE_UCHAR      =  5
 ,DBTYPE_UCHARN     =  6
 ,DBTYPE_SHORT      =  7
 ,DBTYPE_SHORTN     =  8
 ,DBTYPE_USHORT     =  9
 ,DBTYPE_USHORTN    = 10
 ,DBTYPE_INT        = 11
 ,DBTYPE_INTN       = 12
 ,DBTYPE_UINT       = 13
 ,DBTYPE_UINTN      = 14
 ,DBTYPE_LONG       = 15
 ,DBTYPE_LONGN      = 16
 ,DBTYPE_ULONG      = 17
 ,DBTYPE_ULONGN     = 18
 ,DBTYPE_INT64      = 19
 ,DBTYPE_INT64N     = 20
 ,DBTYPE_UINT64     = 21
 ,DBTYPE_UINT64N    = 22
 ,DBTYPE_FLOAT      = 23
 ,DBTYPE_FLOATN     = 24
 ,DBTYPE_DOUBLE     = 25
 ,DBTYPE_DOUBLEN    = 26
 ,DBTYPE_CSTRING    = 27
 ,DBTYPE_CSTRINGN   = 28
 ,DBTYPE_WSTRING    = 29
 ,DBTYPE_WSTRINGN   = 30
 ,DBTYPE_VARCHAR    = 31
 ,DBTYPE_VARCHARN   = 32
 ,DBTYPE_DATE       = 33
 ,DBTYPE_DATEN      = 34
 ,DBTYPE_TIME       = 35
 ,DBTYPE_TIMEN      = 36
 ,DBTYPE_TIMESTAMP  = 37
 ,DBTYPE_TIMESTAMPN = 38
} DbFieldType;

#if !defined(UNICODE)
#define DBTYPE_TSTRING  DBTYPE_CSTRING
#define DBTYPE_TSTRINGN DBTYPE_CSTRINGN
#else
#define DBTYPE_TSTRING  DBTYPE_WSTRING
#define DBTYPE_TSTRINGN DBTYPE_WSTRINGN
#endif // UNICODE

// NB No DBADDRN ! DBTYPE_DBADDR must be odd !
// NB! The "NULL-allowed-types" must be "not-NULL-allowed-types" + 1
// see macros below

#define DBTYPE_NULLABLE(dbtype)              (((dbtype)&1)?false:true)
#define DBTYPE_WITH_NULL_ALLOWED(dbtype)     (DBTYPE_NULLABLE(dbtype) ?  dbtype    : (dbtype+1))
#define DBTYPE_WITH_NULL_NOT_ALLOWED(dbtype) (DBTYPE_NULLABLE(dbtype) ? (dbtype-1) :  dbtype  ))

#define INDICATOR_DEFINED   0
#define INDICATOR_UNDEFINED 1

typedef enum {
  MAINTYPE_NUMERIC
 ,MAINTYPE_STRING
 ,MAINTYPE_VARCHAR
 ,MAINTYPE_DATE
 ,MAINTYPE_TIME
 ,MAINTYPE_TIMESTAMP
 ,MAINTYPE_VOID
} DbMainType ;

class HostVarDescription {
public:
  USHORT  sqltype;
  ULONG   sqllen; // in bytes
  HostVarDescription() { sqltype = 0; sqllen = 0; }
  HostVarDescription(DbFieldType type, ULONG len); // len in bytes
  inline DbFieldType getType() const { return (DbFieldType)sqltype; }
  String toString() const;
};

class SqlApiVarList : public HostVarDescription {
public:
  void          *sqldata;
  short         *sqlind;
};

class SqlApiSqlVar : public SqlApiVarList {
public:
  SqlApiSqlName sqlname;
};

class SqlApiSqlDa {
public:
  short  int    sqln;
  short  int    sqld;
  SqlApiSqlVar *var[1];
};

class varchar {
private:
  // in bytes
  ULONG  m_len;
  BYTE  *m_data;

   // len in bytes
  void init(ULONG len);
  void clear();

public:
  varchar();
  // len in bytes
  varchar(ULONG len );
  // len in bytes
  varchar(ULONG len, const void *data);
  varchar(const varchar &src);
  varchar(const char    *src);
  varchar(const wchar_t *src);
  varchar(const String  &src);
  ~varchar();
  varchar &operator=(const varchar &rhs);
  varchar &operator=(const char    *rhs);
  varchar &operator=(const wchar_t *rhs);
  varchar &operator=(const String  &rhs);

  // len in bytes
  void setdata(ULONG len, const void *data);
        BYTE *data()        { return m_data; }
  const BYTE *data()  const { return m_data; }
  // length of data in bytes
  ULONG len() const         { return m_len;  }
  void dump(FILE *f = stdout) const;
};

bool operator==(const varchar &vc1, const varchar &vc2);
bool operator!=(const varchar &vc1, const varchar &vc2);
bool operator< (const varchar &vc1, const varchar &vc2);
bool operator<=(const varchar &vc1, const varchar &vc2);
bool operator> (const varchar &vc1, const varchar &vc2);
bool operator>=(const varchar &vc1, const varchar &vc2);
int  compare(   const varchar &vc1, const varchar &vc2);

class SqlApiBindProgramId {
public:
  TCHAR m_fileName[30];
  TCHAR m_timestamp[30];
  SqlApiBindProgramId()                { memset(this,0  ,sizeof(SqlApiBindProgramId)); }
  SqlApiBindProgramId(const void *src) { memcpy(this,src,sizeof(SqlApiBindProgramId)); }
};

bool operator==(const SqlApiBindProgramId &p1, const SqlApiBindProgramId &p2);

#define SQLAPI_MAXDBNAME   9
#define SQLAPI_MAXSTMTSIZE 10000
#define SQLAPI_MAXHOSTVAR  500

class SqlApiSourcePosition {
public:
  int   m_line;
  short m_column;
  SqlApiSourcePosition(int l=0, short c=0) { m_line = l; m_column = c; }
};

class SqlApiBindStmtHead {
public:
  TCHAR                m_sourceFile[100];
  int                  m_nr;
  SqlApiSourcePosition m_pos;
  UINT                 m_ninput;   /* max SQLAPI_MAXHOSTVAR  */
  UINT                 m_noutput;  /* max SQLAPI_MAXHOSTVAR  */
  UINT                 m_stmtSize; /* max SQLAPI_MAXSTMTSIZE */
  SqlApiBindStmtHead(int size = 0) { memset(this,0,sizeof(*this)); m_stmtSize = size; }
  void dump(FILE *f = stdout) const;
};

class SqlApiBindStmt {
public:
  SqlApiBindStmtHead     m_stmtHead;
  HostVarDescription     m_inHost[ SQLAPI_MAXHOSTVAR ];
  HostVarDescription     m_outHost[SQLAPI_MAXHOSTVAR ];
  TCHAR                  m_stmt[   SQLAPI_MAXSTMTSIZE];
  void dump(FILE *f = stdout) const;
  SqlApiBindStmt(const TCHAR *s = EMPTYSTRING) : m_stmtHead((UINT)_tcslen(s)) { _tcscpy(m_stmt,s); }
};

class HostVarDescriptionList : public Array<HostVarDescription> {
public:
  HostVarDescriptionList() {}
  HostVarDescriptionList(const SqlApiBindStmt &bndstmt, bool input, bool output);
  void dump(FILE *f = stdout) const;
};

typedef struct {
  TCHAR         m_dbname[SQLAPI_MAXDBNAME + 1]; /* must be the same length as dbname in btree.h */
  char          m_drive;
  unsigned char m_colseq[256];
} SqlApiCreateDb;

class SqlApiCom {
public:
  SqlApiCallCode      m_apicall;
  int                 m_apiopt;
  SqlApiBindProgramId m_programid;
  sqlca               m_ca;
  SqlApiCom() {}
  SqlApiCom(SqlApiCallCode             apicall
           ,int                        apiopt
           );
  SqlApiCom(SqlApiCallCode             apicall
           ,int                        apiopt
           ,const SqlApiBindProgramId &programid
           );
  SqlApiCom(SqlApiCallCode             apicall
           ,int                        apiopt
           ,const SqlApiBindProgramId &programid
           ,const sqlca               &ca       );

  void dump(FILE *f = stdout);
};

bool         isNullAllowed(       DbFieldType dbtype       );
bool         isNumericType(       DbFieldType dbtype       );
bool         isStringType(        DbFieldType dbtype       );
bool         isDateType(          DbFieldType dbtype       );
bool         isTimeType(          DbFieldType dbtype       );
bool         isTimestampType(     DbFieldType dbtype       );
bool         isIntegerType(       DbFieldType dbtype       );
bool         isCompatibleType(    DbMainType  mt1    , DbMainType mt2     );
bool         isCompatibleType(    DbFieldType dbtype1, DbFieldType dbtype2);
DbMainType   getMainType(         DbFieldType dbtype       );
int          getDbTypePrintWidth( DbFieldType dbtype       );
const TCHAR *getTypeString(       DbFieldType dbtype       );
const TCHAR *getMainTypeString(   DbMainType type          );

void sqlapi_bindda(   SqlApiSqlDa                 *sqlda);
void sqlapi_call(     int                          call     , // SqlApiCallCode
                      int                          opt      ,
                      void                        *programid,
                      int                          in       ,
                      int                          out      ,
                      SqlApiVarList               *varlist  ,
                      sqlca                       &cca
                );
void sqlapi_stop();
void sqlapi_bind(     const SqlApiBindProgramId   &programid,
                      const SqlApiBindStmt        &bndstmt  ,
                      StringArray                 &errmsg   ,
                      sqlca                       &cca
                );

void sqlapi_unbind(   const TCHAR                 *filename ,
                      sqlca                       &cca
                  );

void sqlapi_createdb( const SqlApiCreateDb        &crdb     ,
                      sqlca                       &cca
                    );

void sqlapi_dropdb(   const TCHAR                 *dbname   ,
                      sqlca                       &cca);

void throwSqlError(long error, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);

DbFieldType sqlapi_getFieldType(const TCHAR *typestring);

void sqlapi_bndprogramidfwrite(FILE *bndfile, const SqlApiBindProgramId &programid);
bool sqlapi_bndprogramidfread( FILE *bndfile,       SqlApiBindProgramId &programid);
void sqlapi_bndstmtfwrite(     FILE *bndfile, const SqlApiBindStmt      &bndstmt  );
bool sqlapi_bndstmtfread(      FILE *bndfile,       SqlApiBindStmt      &bndstmt  );

#define DBASEROOT "c:/mytools2015/DBase/"
#define DBASELIB DBASEROOT "Lib/"

#define DBASE_VERSION DBASELIB _PLATFORM_ _CONFIGURATION_

#if defined(GIVE_LINK_MESSAGE)
#pragma message("link with " DBASE_VERSION)
#endif
#pragma comment(lib, DBASE_VERSION "SqlUtil.lib")
