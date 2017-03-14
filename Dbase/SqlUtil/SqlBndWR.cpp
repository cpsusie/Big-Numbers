#include "stdafx.h"
#include <ByteFile.h>
#include "SqlCom.h"

void sqlapi_bndprogramidfwrite(FILE *bndfile, const SqlApiBindProgramId &programid) {
  Packer p;
  p << programid;
  p.write(ByteOutputFile(bndfile));
}

bool sqlapi_bndprogramidfread(FILE *bndfile, SqlApiBindProgramId &programid) {
  Packer p;
  if(!p.read(ByteInputFile(bndfile))) {
    return false;
  }
  p >> programid;
  return true;
}

void sqlapi_bndstmtfwrite(FILE *bndfile, const SqlApiBindStmt &bndstmt) {
  Packer p;
  p << bndstmt;
  p.write(ByteOutputFile(bndfile));
}

bool sqlapi_bndstmtfread( FILE *bndfile, SqlApiBindStmt &bndstmt) {
  Packer p;
  if(!p.read(ByteInputFile(bndfile))) {
    return false;
  }
  p >> bndstmt;
  return true;
}

void SqlApiBindStmtHead::dump(FILE *f) const {
  _ftprintf(f,_T("%s nr:%d pos:(%d,%d) in:%d out:%d size:%d\n")
             ,m_sourceFile
             ,m_nr
             ,m_pos.m_line
             ,m_pos.m_column
             ,m_ninput
             ,m_noutput
             ,m_stmtSize
  );
}

void SqlApiBindStmt::dump( FILE *f ) const {
  size_t i;
  const HostVarDescription *var;
  m_stmtHead.dump(f);
  for(i = 0,var = m_inHost; i < m_stmtHead.m_ninput; i++, var++) {
    _ftprintf(f,_T("%-20s %4d\n")
               ,getTypeString(var->getType())
               ,var->sqllen
             );
  }
  for(i = 0, var = m_outHost; i < m_stmtHead.m_noutput; i++, var++) {
    _ftprintf(f,_T("%-20s %4d\n")
               ,getTypeString(var->getType())
               ,var->sqllen
             );
  }
  _ftprintf(f, _T("%s\n"), m_stmt);
}

