#include "stdafx.h"

void testTupleField() {
  try {
    TupleField f;
    f = _T("123.2435");
    f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_CSTRING  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_WSTRING  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_DOUBLE   ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_CSTRING  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_WSTRING  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_INT      ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_VARCHAR  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_INT      ); f.dump(); _tprintf(_T("\n")    );
    f = Date();
    f.setType(DBTYPE_VARCHAR  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_CSTRING  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_WSTRING  ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_DATE     ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_DOUBLE   ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_DATE     ); f.dump(); _tprintf(_T("\n")    );
    f.setType(DBTYPE_INT      ); f.dump(); _tprintf(_T("\n")    );
  } catch(sqlca ca) {
    ca.dump();
  }
  exit(0);
}
