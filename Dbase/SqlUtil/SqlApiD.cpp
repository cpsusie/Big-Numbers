#include "stdafx.h"

void sqlapi_dumpbuffer(SqlApiCom *b)  {
  _tprintf(_T("combuffer.sqlca.sqlcode:%ld\n"),b->m_ca.sqlcode);
  _tprintf(_T("combuffer.programid:<%s,%s>\n"),
          b->m_programid.fileName,b->m_programid.Timestamp);
  _tprintf(_T("combuffer.apicall,apiopt:%d,%d\n"), b->m_apicall,b->m_apiopt);
}


