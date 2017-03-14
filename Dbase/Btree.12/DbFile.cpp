#include "stdafx.h"
#include <io.h>
#include <Console.h>

const char *dbVersion = "V1.1.0.2";

/* -------------------- LOG-FILE FUNCTIONS -------------------- */

typedef struct {
  ULONG  m_count;
  UINT64 m_end;
} LogFileHeader;

void LogFile::setCount(ULONG count, UINT64 end ) {
  LogFileHeader lh;

  lh.m_count = count;
  lh.m_end   = end;
  FSEEK( m_f, 0 );
  FWRITE(&lh,sizeof(LogFileHeader),1,m_f);
  m_count = count;
  m_end   = end;
}

void LogFile::getHead(ULONG &count, UINT64 &end) const {
  LogFileHeader lh;
  count = 0;
  end   = sizeof(LogFileHeader);
  FSEEK( m_f, 0 );
  if(fread(&lh, sizeof(LogFileHeader), 1, m_f) != 1) {
    return; /* no transaction logged */
  }
  count = lh.m_count;
  end   = lh.m_end;
}

typedef struct {
  TCHAR  m_fileName[MAXFILENAME];
  UINT64 m_offset;
  UINT   m_size;
  UINT64 m_prevhead;
} DbFileTransactionHead;

void LogFile::log(const DbFile &dbf, UINT64 offset, const void *buf, ULONG size) {
  if(!m_intmf) {
    throwSqlError(SQL_NO_TRANSACTION,_T("Cannot log. No tmf startet"));
  }
  if(GetCurrentThreadId() != m_threadid) {
    throwSqlError(SQL_NO_TRANSACTION,_T("Transaction not started by this thread"));
  }

  DbFileTransactionHead trhead;
  if(dbf.getName().length() >= ARRAYSIZE(trhead.m_fileName)) {
    throwSqlError(SQL_INVALID_FILENAME,_T("Filename <%s> too long"), dbf.getName().cstr());
  }
  _tcscpy(trhead.m_fileName,dbf.getName().cstr());
  trhead.m_offset   = offset;
  trhead.m_size     = size;
  trhead.m_prevhead = m_end;
  FSEEK(m_f, m_end);
  FWRITE(buf, size, 1, m_f);
  FWRITE(&trhead, sizeof(trhead), 1, m_f);
  setCount(m_count+1, m_end + sizeof(trhead) + size);
}

ULONG LogFile::getCount() const {
  ULONG  count;
  UINT64 end;
  getHead(count,end);
  return count;
}

bool LogFile::inTmf() const { 
  return m_intmf && (m_threadid == GetCurrentThreadId());
}

void LogFile::begin() {
  m_sem.wait();
  m_threadid = GetCurrentThreadId();
  try {
    setCount(0,sizeof(LogFileHeader));
    m_intmf = true;
  } catch(...) {
    m_threadid = 0;
    m_sem.signal();
    throw;
  }
}

void LogFile::commit() {
  if(!m_intmf) {
    throwSqlError(SQL_NO_TRANSACTION,_T("Cannot commit, No tmf started"));
  }
  if(GetCurrentThreadId() != m_threadid) {
    throwSqlError(SQL_NO_TRANSACTION,_T("Transaction not started by this thread"));
  }

  setCount(0,sizeof(LogFileHeader));
  m_intmf = false;
  m_threadid = 0;
  m_sem.signal();
}

void LogFile::abort() {
  if(!m_intmf) {
    throwSqlError(SQL_NO_TRANSACTION,_T("Cannot abort, No tmf started"));
  }
  if(GetCurrentThreadId() != m_threadid) {
    throwSqlError(SQL_NO_TRANSACTION,_T("Transaction not started by this thread"));
  }

  DbFileTransactionHead trhead;
  DbFile               *dbf = NULL;
  ULONG                 count;
  UINT64                end;
  ULONG                 currentBufferSize = 4096;
  BYTE                 *buffer = new BYTE[currentBufferSize];

  getHead(count,end);
  for(ULONG i = 0; i < count; i++, end = trhead.m_prevhead) {
    if(_lseeki64(_fileno(m_f), end - sizeof(trhead), SEEK_SET) < 0) {
      goto cleanup;
    }

    if(fread(&trhead, sizeof(trhead), 1, m_f) != 1) {
      goto cleanup;
    }
    if(dbf && trhead.m_fileName == dbf->getName()) {
      delete dbf;
      dbf = NULL;
    }
    if(!dbf) {
      dbf = new DbFile(trhead.m_fileName, DBFMODE_READWRITE,NULL);
      if(dbf == NULL) {
        continue; // maybe removed
      }
    }
    if(trhead.m_size > currentBufferSize) { // resize buffer
      delete[] buffer;
      currentBufferSize = trhead.m_size;
      buffer = new BYTE[currentBufferSize];
    }
    if(_lseeki64(_fileno(m_f),end - sizeof(trhead) - trhead.m_size, SEEK_SET) < 0) {
      goto cleanup;
    }
    if(fread(buffer,trhead.m_size,1,m_f) != 1) {
      goto cleanup;
    }

/* write back blocks to their original position */
    dbf->write(trhead.m_offset, buffer, trhead.m_size);
  }

  setCount(0,sizeof(LogFileHeader));
  m_intmf = false;
cleanup:
  if(dbf) {
    delete dbf;
  }
  delete[] buffer;

  m_threadid = 0;
  m_sem.signal();
}

LogFile::LogFile(const String &fileName) {
  m_f = fopen(fileName,"r+b");
  if(!m_f) {
    throwSqlError(SQL_NO_LOG,_T("No logfile <%s>"),fileName.cstr());
  }
  getHead(m_count,m_end);
  m_intmf = m_count > 0;
  if(m_intmf) {
    m_threadid = GetCurrentThreadId();;
  }
}

LogFile::~LogFile() {
  m_sem.wait();
  if(m_f) fclose(m_f);
  m_f = NULL;
}

/* define how many open files we have open at any time. except logfiles */
#define MAXFILESLOT 20

class FileSlot {
public:
  FILE         *m_file;
  String        m_fileName;
  ULONG m_lasttrans;
  FileSlot(const String &fileName, ULONG lasttrans);
  bool open();
  void close();
  ~FileSlot();
};

FileSlot::FileSlot(const String &fileName, ULONG lasttrans) {
  m_fileName  = toUpperCase(fileName);
  m_file      = NULL;
  m_lasttrans = lasttrans;
}

bool FileSlot::open() {
  m_file = fopen(m_fileName,"r+b");
  return m_file != NULL;
}

void FileSlot::close() {
  if(m_file != NULL) {
    fclose(m_file);
    m_file = NULL;
  }
}

FileSlot::~FileSlot() {
  close();
}

class FileSlotTable : public StringHashMap<FileSlot> {
private:
  static ULONG transcount;
  ULONG nextTransCount();
  void releaseFileSlot(FileSlot *fileSlot);
  FileSlot *allocateFileSlot(const String &fileName);
public:
  FileSlot *getFileSlot(const String &fileName);
  void closeFileSlot(const String &fileName);
  void dump();
};

ULONG FileSlotTable::transcount = 0;

void FileSlotTable::releaseFileSlot(FileSlot *fileSlot) {
  fileSlot->close();
  remove(fileSlot->m_fileName);
}

ULONG FileSlotTable::nextTransCount() {
  ULONG result = transcount++;
  if(transcount < result) { // wrapped around. reset all counters
    for(Iterator<String> it = keySet().getIterator(); it.hasNext();) {
      FileSlot *fileSlot = get(it.next());
      fileSlot->m_lasttrans = 0;
    }
  }
  return result;
}

FileSlot *FileSlotTable::allocateFileSlot(const String &fileName) {
  if(size() >= MAXFILESLOT) {
    Iterator<Entry<String,FileSlot> > it = entrySet().getIterator();
    FileSlot *fileSlot = &it.next().getValue();
    while(it.hasNext()) {
      FileSlot &fs1 = it.next().getValue();
      if(fs1.m_lasttrans < fileSlot->m_lasttrans) {
        fileSlot = &fs1;
      }
    }
    releaseFileSlot(fileSlot);
  }
  FileSlot fileSlot(fileName,transcount);
  put(fileSlot.m_fileName,fileSlot);
  FileSlot *result = get(fileSlot.m_fileName);
  if(!result->open()) {
    remove(fileSlot.m_fileName);
    throwSqlError(SQL_FILE_OPEN_ERROR,_T("File <%s> cannot be opened"),fileSlot.m_fileName.cstr());
  }
//  dump();
  return result;
}

FileSlot *FileSlotTable::getFileSlot(const String &fileName) {
  String tmp = toUpperCase(fileName);
  FileSlot *result = get(tmp);
  if(result == NULL) {
    result = allocateFileSlot(tmp);
  }
  result->m_lasttrans = nextTransCount();
  return result;
}

/* assert that no fileslots is using fileName */
void FileSlotTable::closeFileSlot(const String &fileName) {
  FileSlot *fileSlot = get(toUpperCase(fileName));
  if(fileSlot != NULL) {
    releaseFileSlot(fileSlot);
  }
}

void FileSlotTable::dump() {
  int x,y;
  Console::getCursorPos(x,y);

  int line = 1;
  Console::printf(40,line++,_T("slotTable.size:%2d"),size());
  for(Iterator<Entry<String,FileSlot> > it = entrySet().getIterator(); it.hasNext();line++) {
    FileSlot &slot = it.next().getValue();
    Console::printf(40,line,_T("%-20s lasttrans:%d  "),slot.m_fileName.cstr(),slot.m_lasttrans);
  }
  for(;line < MAXFILESLOT; line++) {
    Console::clearRect(40,line,80,line);
  }

//  getchar();
  Console::setCursorPos(x,y);
}

/* ------------------------------------------------------------ */

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static FileSlotTable slotTable;
static Semaphore m_filesem;

DbFile::DbFile(const String &fileName, DbFileMode mode, LogFile *lf) {
  init(fileName,mode,lf);
}

DbFile::DbFile(const Database &db, const String &fileName, DbFileMode mode, bool systemFile) {
  LogFile *lf = mode == DBFMODE_READWRITE ? db.getLogFile() : NULL;

  if(systemFile) {
    init(dbFileName(db.getPath(),fileName),mode,lf);
  } else {
    init(fileName,mode,lf);
  }
}

extern "C" {
WINBASEAPI BOOL WINAPI GetFileSizeEx(HANDLE hFile,PLARGE_INTEGER lpFileSize);
}

UINT64 DbFile::getSize() {
  HANDLE fh = CreateFile(getName().cstr(), GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
  if(fh == INVALID_HANDLE_VALUE) {
    throwSqlError(SQL_FATAL_ERROR, _T("DbFile::getSize:Cannot get handle to file %s:%s"), getName().cstr(), getLastErrorText().cstr());
  }
  LARGE_INTEGER li;
  if(GetFileSizeEx(fh,&li) == 0) {
    String errText = getLastErrorText();
    CloseHandle(fh);
    throwSqlError(SQL_FATAL_ERROR, _T("DbFile::getSize:GetFileSizeEx failed on %s:%s"), getName().cstr(), errText.cstr());
  }
  UINT64 result = li.QuadPart;
  CloseHandle(fh);
  return result;
}

void DbFile::truncate() {
  HANDLE fh = CreateFile(getName().cstr(), GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
  if(fh == INVALID_HANDLE_VALUE) {
    throwSqlError(SQL_FATAL_ERROR, _T("DbFile::truncate:Cannot get handle to file %s:%s"), getName().cstr(), getLastErrorText().cstr());
  }
  CloseHandle(fh);
}

void DbFile::init(const String &fileName, DbFileMode mode, LogFile *lf) {
  String tmp = toUpperCase(fileName);

  if(mode != DBFMODE_READWRITE && mode != DBFMODE_READONLY) {
    throwSqlError(SQL_INVALID_FILEMODE,_T("Invalid filemode (=%d) on <%s>"),mode,tmp.cstr());
  }

  m_fileName = tmp;
  m_mode     = mode;
  m_logFile  = lf;
}

void DbFile::write(UINT64 offset, const void *buffer, UINT size) const {
  if(m_mode != DBFMODE_READWRITE) {
    throwSqlError(SQL_WRITE_ERROR,_T("Writing to <%s>, Opened as readonly"),m_fileName.cstr());
  }

  if(isBackLogged() && !m_logFile->inTmf()) {
    throwSqlError(SQL_NO_TRANSACTION,_T("Cannot write to <%s>. No tmf startet"), m_fileName.cstr());
  }

  if(size == 0) {
    return;
  }

  m_filesem.wait();
  try {
    FileSlot *fileSlot = slotTable.getFileSlot(m_fileName);

    if(_lseeki64(_fileno(fileSlot->m_file), offset, SEEK_SET) < 0) {
      throwSqlError(SQL_SEEK_ERROR,_T("_lseeki64(%I64u) failed in <%s>:%s"),offset,m_fileName.cstr(),getErrnoText().cstr());
    }

    int bytesWritten;
    if((bytesWritten = _write(_fileno(fileSlot->m_file), buffer, size)) != size) {
      if(bytesWritten < 0) {
        throwSqlError(SQL_WRITE_ERROR,_T("_write failed in <%s>:%s"),m_fileName.cstr(), getErrnoText().cstr());
      } else {
        throwSqlError(SQL_WRITE_ERROR,_T("_write %d bytes at offset %s to <%s> failed. Could only write %d bytes")
                     ,size, format1000(offset).cstr(), m_fileName.cstr(), bytesWritten);
      }
    }

//    if(_commit(fileno(fileSlot->m_file)) < 0) {
//      throwSqlError(SQL_WRITE_ERROR,_T("_commit failed in <%s>:%s"),m_fileName.cstr(),getErrnoText().cstr();
//    }
  } catch(...) {
    m_filesem.signal();
    throw;
  }
  m_filesem.signal();
}

void DbFile::read(UINT64 offset, void *buffer, UINT size) const {
  if(size == 0) {
    return;
  }
  m_filesem.wait();
  try {
    FileSlot *fileSlot = slotTable.getFileSlot(m_fileName);

    if(_lseeki64(_fileno(fileSlot->m_file), offset, SEEK_SET) < 0) {
      throwSqlError(SQL_SEEK_ERROR,_T("_lseeki64(%I64u) failed in <%s>:%s"),offset,m_fileName.cstr(),getErrnoText().cstr());
    }
    int bytesRead;
    if((bytesRead = _read(_fileno(fileSlot->m_file),buffer, size)) != size) {
      if(bytesRead < 0) {
        throwSqlError(SQL_READ_ERROR,_T("_read failed in <%s>:%s"),m_fileName.cstr(), getErrnoText().cstr());
      } else {
        throwSqlError(SQL_READ_ERROR,_T("_read %d bytes at offset %s from <%s> failed. Could only read %d bytes")
                     ,size, format1000(offset).cstr(), m_fileName.cstr(), bytesRead);
      }
    }
  } catch(...) {
    m_filesem.signal();
    throw;
  }
  m_filesem.signal();
}

void DbFile::appendLog(UINT64 offset, const void *buf, ULONG size) const {
  if(!isBackLogged()) {
    throwSqlError(SQL_FILE_NOT_BACKLOGGED,_T("Trying to log <%s> which is not isBackLogged\n"),m_fileName.cstr());
  }

  m_logFile->log(*this,offset,buf,size);
}

void DbFile::create(const String &fileName) {
  if(exist(fileName)) {
    throwSqlError(SQL_CREATE_ERROR,_T("File <%s> already exists"),fileName.cstr());
  }
  FILE *f = mkfopen(fileName,"w+b");
  if(f != NULL) {
    fclose(f);
  } else {
    throwSqlError(SQL_CREATE_ERROR,_T("Cannot create file <%s>"),fileName.cstr());
  }
}

void DbFile::destroy(const String &fileName) {
  m_filesem.wait();
  slotTable.closeFileSlot(fileName);
  if(unlink(fileName) < 0) {
    m_filesem.signal();
    throwSqlError(SQL_FILE_DELETE_ERROR,_T("Cannot delete <%s> %s"),fileName.cstr(),getErrnoText().cstr());
  }
  m_filesem.signal();
}

bool DbFile::exist(const String &fileName) { // static
  return ACCESS(fileName,00)==0;
}

void DbFile::rename(const String &from, const String &to) {
  m_filesem.wait();
  slotTable.closeFileSlot(from);
  if(::rename(from,to) < 0) {
    m_filesem.signal();
    throwSqlError(SQL_FILE_RENAME_ERROR,_T("Cannot rename <%s> -> <%s>"),from.cstr(),to.cstr());
  }
  m_filesem.signal();
}

String DbFile::dbFileName(const String &dbName, const String &fileName) {
  return toUpperCase(format(_T("%s\\%s"), dbName.cstr(), fileName.cstr()));
}

