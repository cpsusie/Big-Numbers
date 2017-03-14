#include "stdafx.h"
#include <WinSock.h>
#include <assert.h>
#include "SqlCom.h"

Packer &operator<<(Packer &p, const SqlApiCreateDb &crdb) { 
  p << crdb.dbname 
    << crdb.drive;
  for(int i = 0; i < ARRAYSIZE(crdb.colseq); i++) {
    p << crdb.colseq[i];
  }
  return p;
}

Packer &operator>>(Packer &p, SqlApiCreateDb &crdb) { 
  p >> crdb.dbname 
    >> crdb.drive;
  for(int i = 0; i < ARRAYSIZE(crdb.colseq); i++) {
    p >> crdb.colseq[i];
  }
  return p;
}

Packer &operator<<(Packer &p, const sqlca &ca) {
  p << ca.sqlcode
    << ca.sqlerrmc;
  return p;
}

Packer &operator>>(Packer &p, sqlca &ca) {
  p >> ca.sqlcode
    >> ca.sqlerrmc;
  return p;
}

Packer &operator<<(Packer &p, const SqlApiCom &com) {
  p << com.m_apicall
    << com.m_apiopt
    << com.m_programid
    << com.m_ca;
  return p;
}

Packer &operator>>(Packer &p, SqlApiCom &com) {
  p >> com.m_apicall
    >> com.m_apiopt
    >> com.m_programid
    >> com.m_ca;
  return p;
}

Packer &operator<<(Packer &p, const SqlApiBindProgramId &programid) { 
  p << programid.fileName 
    << programid.Timestamp;
  return p;
}

Packer &operator>>(Packer &p, SqlApiBindProgramId &programid) { 
  p >> programid.fileName 
    >> programid.Timestamp;
  return p;
}

Packer &operator<<(Packer &p, const SqlApiBindStmtHead &head) {
  p << head.m_sourceFile
    << head.m_pos.m_line
    << head.m_pos.m_column
    << head.m_ninput
    << head.m_noutput
    << head.m_nr
    << head.m_stmtSize;
  return p;
}

Packer &operator>>(Packer &p, SqlApiBindStmtHead &head) {
  p >> head.m_sourceFile
    >> head.m_pos.m_line
    >> head.m_pos.m_column
    >> head.m_ninput
    >> head.m_noutput
    >> head.m_nr
    >> head.m_stmtSize;
  return p;
}

Packer &operator<<(Packer &p, const SqlApiBindStmt &stmt) {
  p << stmt.m_stmtHead
    << stmt.m_stmt;
  for(size_t i = 0; i < stmt.m_stmtHead.m_ninput; i++) {
    p << stmt.m_inHost[i];
  }
  for(size_t i = 0; i < stmt.m_stmtHead.m_noutput; i++) {
    p << stmt.m_outHost[i];
  }
  return p;
}

Packer &operator>>(Packer &p, SqlApiBindStmt &stmt) {
  p >> stmt.m_stmtHead
    >> stmt.m_stmt;
  for(size_t i = 0; i < stmt.m_stmtHead.m_ninput; i++) {
    p >> stmt.m_inHost[i];
  }
  for(size_t i = 0; i < stmt.m_stmtHead.m_noutput; i++) {
    p >> stmt.m_outHost[i];
  }
  return p;
}

bool operator==(const SqlApiBindProgramId &p1, const SqlApiBindProgramId &p2) {
  return (_tcscmp(p1.fileName,p2.fileName) == 0) && (_tcscmp(p1.Timestamp,p2.Timestamp) == 0);
}
