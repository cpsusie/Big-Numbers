#include "stdafx.h"

#define MAXRECORDS 1000
typedef struct {
  DbAddr         m_records[MAXRECORDS];
  int            m_count;
} testliste;

static void genRandomString(TCHAR *s, int maxlen) {
  int i, n = randInt(maxlen - 1);
  for( i = 0; i < n; i++) {
    s[i] = 'a' + randInt() % ('z'-'a');
  }
  s[i] = '\0';
}

static void genDataFile(Database &db, DataFile &dbf, testliste *liste) {
  TCHAR s[100];

  liste->m_count = 0;

  _tprintf(_T("Now inserting\n"));

  try {
    for(int i = 0; i < MAXRECORDS; i++) {
      db.trbegin();
      genRandomString(s,ARRAYSIZE(s)-1);
      liste->m_records[i] = dbf.insert(s,(ULONG)_tcslen(s) + 1);
      db.trcommit();
      liste->m_count++;
    }

    _tprintf(_T("Now deleting\n"));

    for(int i = 0; i < MAXRECORDS; i++) {
      db.trbegin();
      dbf.remove(liste->m_records[i]);
      db.trcommit();
    }
    for(int i = 0; i < MAXRECORDS; i++) {
      liste->m_records[i] = DB_NULLADDR;
    }
    liste->m_count = 0;
  } catch(sqlca ca) {
    ca.dump();
    db.trabort();
    exit(-1);
  }
}


DbAddr getdbaddr(TCHAR *prompt) {
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("%s"),prompt); GETS(line);
    DbAddr addr;
#if !defined(BIG_DBADDR)
    if(_stscanf(line,_T("%ld"),&addr) == 1) return addr;
#else
    if(_stscanf(line,_T("%I64d"),&addr) == 1) return addr;
#endif
  }
}

static testliste       newl,oldl;

int main(int argc, char **argv) {
  DataFile      *dbf;
  Database      *db;
  DbAddr         addr;
  TCHAR          s[100];
  TCHAR          line[100];
  int            i;
  FILE          *lf;
  TCHAR         *testfname = _T("c:\\fisk.dat");

  for(i=0;i<MAXRECORDS;i++) newl.m_records[i] = DB_NULLADDR;
  newl.m_count = 0;

  try {
    if(DbFile::exist(testfname))
      DbFile::destroy(testfname);
    DataFile::create( testfname );
    db  = new Database(_T("fisk"));
    dbf = new DataFile(testfname,DBFMODE_READWRITE,db->getLogFile());
    lf = FOPEN(_T("test.log"),_T("w"));
    oldl = newl;
  } catch(sqlca ca) {
    ca.dump();
    exit(-1);
  } catch(Exception e) {
    _tprintf(_T("%s\n"),e.what());
    exit(-1);
  }

  for(;;) {
    _tprintf(_T("Enter command (G)enerate/(L)ist/(B)egin,(C)ommit/(A)bort/(I)nsert/(D)elete,(R)ead/(Q)uit:"));
    GETS(line);
    try {
      switch(line[0]) {
        case 'b':
        case 'B':
          db->trbegin();
          break;
        case 'c':
        case 'C':
          db->trcommit();
          oldl = newl;
          break;
        case 'a':
        case 'A':
          db->trabort();
          newl = oldl;
          break;
        case 'g':
        case 'G':
          genDataFile(*db, *dbf, &newl );
          break;
        case 'I':
        case 'i':
          if(newl.m_count >= MAXRECORDS)
            _tprintf(_T("Cannot insert more than %d records\n"),MAXRECORDS);
          else {
            _tprintf(_T("Enter String to insert:")); GETS(s);
            addr = dbf->insert(s,(ULONG)_tcslen(s)+1);
            newl.m_count++;
            for(i=0;i<MAXRECORDS;i++)
              if(newl.m_records[i] == DB_NULLADDR) {
                newl.m_records[i] = addr;
                break;
              }
            _tprintf(_T("Record inserted at addr:%s\n"),toString(addr).cstr());
          }
          break;
        case 'd':
        case 'D':
          { addr = getdbaddr(_T("Enter addr to delete:"));
            int found = -1;
            for(i=0; found == -1 && i<MAXRECORDS;i++)
              if(newl.m_records[i] == addr)
                found = i;
            if(found < 0)
              _tprintf(_T("None existing address\n"));
            else {
              dbf->remove(addr);
              newl.m_records[found] = DB_NULLADDR;
              newl.m_count--;
              _tprintf(_T("Record deleted\n"));
            }
            break;
          }
        case 'r':
        case 'R':
          addr = getdbaddr(_T("Enter addr to read:"));
          dbf->readRecord( addr, s, ARRAYSIZE(s));
          _tprintf(_T("Read <%s>\n"),s);
          break;
        case 'L':
          dbf->freeTreeDump();
          dbf->freeTreeDump(lf);
          /* NB continue case */
        case 'l':
          _tprintf(_T("------------------- Contents ------------------------\n"));
          for(i=0;i<MAXRECORDS;i++) {
            if(newl.m_records[i] != DB_NULLADDR) {
              dbf->readRecord( newl.m_records[i], s, ARRAYSIZE(s));
              _ftprintf(stdout ,_T("Record at addr %s rec:<%s>\n"), toString(newl.m_records[i]).cstr(),s);
              _ftprintf(lf     ,_T("Record at addr %s rec:<%s>\n"), toString(newl.m_records[i]).cstr(),s);
            }
          }
          _tprintf(_T("----------------------------------------\n"));
          break;

        case 'q':
        case 'Q':
          delete dbf;
          delete db;
          exit(0);
      }
    } catch(sqlca ca) {
      ca.dump();
    }
  }
  return 0;
}

