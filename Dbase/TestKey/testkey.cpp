#include "stdafx.h"

int main(int argc, char **argv) {
  KeyFileDefinition keydef;
  TCHAR line[10];
  _tprintf(_T("Testtype (D)efined/(N)ulls/(B)oth:"));
  GETS(line);

  keydef.init();

  switch(line[0]) {
  case 'l':
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHAR,5);
    keydef.addDataField(DBTYPE_DBADDR);
    break;

  case 'd':
  case 'D':
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UCHAR     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORT     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORT    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_INT       );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINT      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_LONG      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_ULONG     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_FLOAT     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_DOUBLE    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CSTRING,5 );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRING,5 );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHAR,5 );
    keydef.addDataField(DBTYPE_DBADDR);
    break;
  case 'n':
  case 'N':
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHARN     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UCHARN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORTN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORTN   );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_INTN      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINTN     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_LONGN     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_ULONGN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_FLOATN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_DOUBLEN   );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CSTRINGN,5);
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRINGN,5);
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHARN,5);
    keydef.addDataField(DBTYPE_DBADDR);
    break;
  case 'b':
  case 'B' :
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHAR      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CHARN     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UCHAR     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UCHARN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORT     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_SHORTN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORT    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORTN   );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_INT       );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_INTN      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINT      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINTN     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_LONG      );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_LONGN     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_ULONG     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_ULONGN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_FLOAT     );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_FLOATN    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_DOUBLE    );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_DOUBLEN   );
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CSTRING ,5);
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_CSTRINGN,5);
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRING ,5);
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_WSTRINGN,5);
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHAR ,5);
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_VARCHARN,5);
    keydef.addDataField(DBTYPE_DBADDR);
    break;
  }

  keydef.dump();
    
  for(;;) {
    try {
      KeyType key1, key2;

      _tprintf(_T("Enter key1\n"));
      keydef.scanf(key1);
      _tprintf(_T("Key1="));
      keydef.printf(key1);
      _tprintf(_T("\nkey1.DBADDR:%s\n"),toString(keydef.getRecordAddr(key1)).cstr());

      _tprintf(_T("\nEnter key2\n"));
      keydef.scanf(key2);
      _tprintf(_T("Key2="));
      keydef.printf(key2);
      _tprintf(_T("\nkey2.DBADDR:%s\n"), toString(keydef.getRecordAddr(key2)).cstr());

      _tprintf(_T("\nkeyCmp( key1,key2 ) = %d\n"), keydef.keyCmp( key1, key2 ));
    } catch(sqlca ca) {
      ca.dump();
    }
  }
  return 0;
}
