#include "stdafx.h"
#include <Console.h>

class DBFILE : public DbFile {
public:
  DBFILE( Database &db, const String &fname, DbFileMode mode);
  void write(ULONG offset, const void *buffer, UINT size) const {
    DbFile::write(offset,buffer,size);
  }
  void read(ULONG offset,       void *buffer, UINT size) const {
    DbFile::read(offset,buffer,size);
  }
};

DBFILE::DBFILE(Database &db, const String &fname, DbFileMode mode) : DbFile(db,fname,mode,true) {
}

class FileArray : public Array<DBFILE*> {
private:
  Database &m_db;
public:
  FileArray(Database &db) : m_db(db) {}
  void list();
  int find(const String &name) const;
};

void FileArray::list() {
  for(UINT i = 0; i < size(); i++) {
    DBFILE *f = (*this)[i];
    _tprintf(_T("%2d:<%-20s> : %s\n"),i,f->getName().cstr(),f->getMode()==DBFMODE_READWRITE?_T("R/W"):_T("R"));
  }
}

int FileArray::find(const String &name) const {
  for(UINT i = 0; i < size(); i++) {
    if((*this)[i]->getName().equalsIgnoreCase(name)) {
      return i;
    }
  }
  throwSqlError(SQL_INVALID_FILENAME,_T("Invalid filename:<%s>"),name.cstr());
  return -1;
}

String GETNAME() {
  Console::setCursorPos(1,23);
  return inputString(_T("Indtast navn:"));
}

int main(int argc, char **argv) {
  try {
    Database db(_T("fisk"));
    FileArray files(db);
    int index;

    for(;;) {
      try {
        files.list();
        char buf[100];

        String line;
        Console::setCursorPos(1,22);
        printf("Angiv kommando: (B)egin/(E)nd/(A)bort/(O)pen/(C)lose/(W)rite/(R)ead/(Q)uit:");
        switch(toupper(Console::getKey())) {
        case 'B':
          db.trbegin();
          break;
        case 'E':
          db.trcommit();
          break;
        case 'A':
          db.trabort();
          break;
        case 'O':
          { line = GETNAME();
            TCHAR modeline[10];
            printf("Indtast mode R/W:"); GETS(modeline);
            DbFileMode mode;
            switch(toupper(modeline[0])) {
            case 'W':
              mode = DBFMODE_READWRITE;
              break;
            case 'R':
              mode = DBFMODE_READONLY;
              break;
            default:
              printf("Forkert filemode.\n");
              continue;
            }
            files.add(new DBFILE(db,line,mode));
            break;
          }
        case 'C':
          line = GETNAME();
          index = files.find(line);
          printf("Lukker %d\n",index);
          delete files[index];
          files.removeIndex(index);
          break;
        case 'W':
          line = GETNAME();
          index = files.find(line);
          printf("Skriver til %d\n",index);
          files[index]->write(0,buf,sizeof(buf));
          break;
        case 'R':
          line = GETNAME();
          index = files.find(line);
          printf("Læser fra %d\n",index);
          files[index]->read(0,buf,sizeof(buf));
          break;
        case 'Q':
          exit(0);
        default:
          break;  
        }
      } catch(sqlca ca) {
        ca.dump();
      }
    }
  } catch(sqlca ca) {
    ca.dump();
    return -1;
  }
  return 0;
}
