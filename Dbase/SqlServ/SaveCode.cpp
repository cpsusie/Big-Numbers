#include "stdafx.h"

//#define DEBUGMODULE

static bool codelookup( Database       &db,
                        const String   &filename,
                        int             nr) {

  KeyFile        indexfile(db, SYSTEM_CODEKEY_FNAME ,DBFMODE_READONLY);
  KeyFileDefinition keydef(indexfile);
  KeyType key;
  keydef.put(key,0,filename);
  keydef.put(key,1,nr);
  return indexfile.searchMin(RELOP_EQ, key, 2);
}

void sqlSaveCode( Database            &db       ,
                  SqlApiBindProgramId &programid,
                  int                  nr       ,
                  VirtualCode         &vc       ) {
  SysTableCodeKey codekey;

  vc.setProgramId(programid);
  _tcscpy(codekey.m_filename,programid.m_fileName);
  codekey.m_nr    = nr;

  if(codelookup(db, programid.m_fileName, nr))
    db.sysTabCodeDelete(codekey);

#ifdef DEBUGMODULE
_tprintf(_T("now saving code for <%s> <%s> nr:%d totalsize:%d\n"),
      programid.filename,programid.Timestamp,nr,vc.totalSize());
//PAUSE();
#endif

  db.sysTabCodeInsert(codekey,&vc,sizeof(VirtualCodeHeader) + vc.totalSize());
}

void sqlLoadCode(  const Database            &db        ,
                   const SqlApiBindProgramId &programid ,
                   int                        nr        ,
                   VirtualCode               &vc        ) {


  DataFile datafile  ( db, SYSTEM_CODEDATA_FNAME, DBFMODE_READONLY);
  KeyFile  indexfile ( db, SYSTEM_CODEKEY_FNAME , DBFMODE_READONLY);

  KeyFileDefinition keydef(indexfile);
  KeyType key;
  keydef.put(key,0,String(programid.m_fileName));
  keydef.put(key,1,nr);

  bool found = indexfile.searchMin( RELOP_EQ, key, 2);

#ifdef DEBUGMODULE
  _tprintf(_T("loading code for <%s>,%d\n"),programid.filename,nr);
#endif

  if(!found)
    throwSqlError(SQL_UNKNOWN_PROGRAMID,_T("Unknown programfile:<%s>"),programid.m_fileName);
  datafile.readRecord( keydef.getRecordAddr(key), &vc, sizeof(VirtualCode));

  if(_tcscmp(vc.getProgramId().m_timestamp,programid.m_timestamp) != 0)
    throwSqlError(SQL_TIMESTAMP_MISMATCH,_T("Timestamp mismatch on <%s>"),programid.m_fileName);
}
