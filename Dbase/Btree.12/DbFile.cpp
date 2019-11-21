#include "stdafx.h"
#include <io.h>
#include <Console.h>

/* -------------------- LOG-FILE FUNCTIONS -------------------- */

LogFile::LogFile(const String &fileName) {
  m_f = fopen(fileName,_T("r+b"));
  if(!m_f) {
    throwSqlError(SQL_NO_LOG,_T("No logfile <%s>"),fileName.cstr());
  }
  getHead(m_count,m_end);
  m_inTMF = m_count > 0;
  if(m_inTMF) {
    m_threadId = GetCurrentThreadId();;
  }
}

LogFile::~LogFile() {
  m_sem.wait();
  if(m_f) fclose(m_f);
  m_f = NULL;
}

typedef struct {
  ULONG  m_count;
  UINT64 m_end;
} LogFileHeader;

void LogFile::setCount(ULONG count, UINT64 end ) {
  LogFileHeader lh;

  lh.m_count = count;
  lh.m_end   = end;
  FSEEK( m_f, 0);
  FWRITE(&lh, sizeof(LogFileHeader), 1, m_f);
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
  if(!m_inTMF) {
    throwSqlError(SQL_NO_TRANSACTION, _T("Cannot log. No tmf startet"));
  }
  if(GetCurrentThreadId() != m_threadId) {
    throwSqlError(SQL_NO_TRANSACTION, _T("Transaction not started by this thread"));
  }

  DbFileTransactionHead trhead;
  if(dbf.getName().length() >= ARRAYSIZE(trhead.m_fileName)) {
    throwSqlError(SQL_INVALID_FILENAME, _T("Filename <%s> too long"), dbf.getName().cstr());
  }
  _tcscpy(trhead.m_fileName, dbf.getName().cstr());
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

void LogFile::begin() {
  m_sem.wait();
  m_threadId = GetCurrentThreadId();
  try {
    setCount(0, sizeof(LogFileHeader));
    m_inTMF = true;
  } catch(...) {
    m_threadId = 0;
    m_sem.notify();
    throw;
  }
}

void LogFile::commit() {
  if(!m_inTMF) {
    throwSqlError(SQL_NO_TRANSACTION, _T("Cannot commit. No tmf started"));
  }
  checkThreadId();

  setCount(0,sizeof(LogFileHeader));
  m_inTMF    = false;
  m_threadId = 0;
  m_sem.notify();
}

void LogFile::abort() {
  if(!m_inTMF) {
    throwSqlError(SQL_NO_TRANSACTION, _T("Cannot abort. No tmf started"));
  }
  checkThreadId();

  DbFile *dbf               = NULL;
  ULONG   currentBufferSize = 4096;
  BYTE   *buffer            = new BYTE[currentBufferSize];
  String  errorMsg;

  try {
    DbFileTransactionHead trhead;
    ULONG                 count;
    UINT64                end;
    getHead(count,end);
    for(ULONG i = 0; i < count; i++, end = trhead.m_prevhead) {
      FSEEK(m_f, end - sizeof(trhead));
      FREAD(&trhead, sizeof(trhead), 1, m_f);
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
      FSEEK(m_f, end - sizeof(trhead) - trhead.m_size);
      FREAD(buffer,trhead.m_size,1,m_f);

  /* write back blocks to their original position */
      dbf->write(trhead.m_offset, buffer, trhead.m_size);
    }

    setCount(0,sizeof(LogFileHeader));
    m_inTMF = false;
  } catch(Exception e) {
    errorMsg = e.what();
  } catch (...) {
    errorMsg = _T("Unknown exception in abort");
  }
  if(dbf) {
    delete dbf;
  }
  delete[] buffer;
  m_threadId = 0;
  m_sem.notify();
  if (errorMsg.length() > 0) {
    throwSqlError(SQL_FATAL_ERROR, _T("%s"), errorMsg.cstr());
  }
}

void LogFile::throwInvalidThreadId() const {
  throwSqlError(SQL_NO_TRANSACTION, _T("Transaction not started by this thread"));
}

const char *LogFile::getDbProgramVersion() { // static
  static const char *dbVersion = "V2.1.0.3";
  return dbVersion;
}

void DbFileHeader::setFileVersion() {
  strcpy(m_version, LogFile::getDbProgramVersion());
}

void DbFileHeader::checkFileVersion(const String &fileName) const {
  if(strcmp(LogFile::getDbProgramVersion(), m_version) != 0) {
    throwSqlError(SQL_WRONGDBVERSION,_T("Wrong database version for file %s. File verison=<%s>, program version=<%s>")
                                    ,fileName.cstr()
                                    ,getDbFileVersion().cstr()
                                    ,LogFile::getDbProgramVersionString().cstr());
  }
}

/* define how many open files we have open at any time. except logfiles */
#define MAXFILESLOT 20

class FileSlot {
public:
  FILE         *m_file;
  String        m_fileName;
  ULONG         m_lastTrans;
  FileSlot(const String &fileName, ULONG lastTrans);
  ~FileSlot();
  bool open();
  void close();
};

FileSlot::FileSlot(const String &fileName, ULONG lastTrans) {
  m_fileName  = toUpperCase(fileName);
  m_file      = NULL;
  m_lastTrans = lastTrans;
}

bool FileSlot::open() {
  m_file = fopen(m_fileName,_T("r+b"));
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
  static ULONG s_transCount;
  ULONG     nextTransCount();
  void      releaseFileSlot(FileSlot *fileSlot);
  FileSlot *allocateFileSlot(const String &fileName);
public:
  FileSlot *getFileSlot(  const String &fileName);
  void      closeFileSlot(const String &fileName);
  void      dump();
};

ULONG FileSlotTable::s_transCount = 0;

void FileSlotTable::releaseFileSlot(FileSlot *fileSlot) {
  fileSlot->close();
  remove(fileSlot->m_fileName);
}

ULONG FileSlotTable::nextTransCount() {
  const ULONG result = s_transCount++;
  if(s_transCount < result) { // wrapped around. reset all counters
    for(Iterator<String> it = keySet().getIterator(); it.hasNext();) {
      FileSlot *fileSlot = get(it.next());
      fileSlot->m_lastTrans = 0;
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
      if(fs1.m_lastTrans < fileSlot->m_lastTrans) {
        fileSlot = &fs1;
      }
    }
    releaseFileSlot(fileSlot);
  }
  FileSlot fileSlot(fileName,s_transCount);
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
  const String tmp    = toUpperCase(fileName);
  FileSlot    *result = get(tmp);
  if(result == NULL) {
    result = allocateFileSlot(tmp);
  }
  result->m_lastTrans = nextTransCount();
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
    Console::printf(40,line,_T("%-20s lastTrans:%d  "),slot.m_fileName.cstr(),slot.m_lastTrans);
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

Semaphore DbFile::s_filesem;

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

UINT64 DbFile::getSize() {
  struct _stati64 st;
  return STAT64(m_fileName, st).st_size;
}

void DbFile::truncate() {
  HANDLE fh = CreateFile(getName().cstr(), GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
  if(fh == INVALID_HANDLE_VALUE) {
    throwSqlError(SQL_FATAL_ERROR, _T("DbFile::truncate:Cannot get handle to file %s:%s"), getName().cstr(), getLastErrorText().cstr());
  }
  CloseHandle(fh);
}

void DbFile::init(const String &fileName, DbFileMode mode, LogFile *lf) {
  if(mode != DBFMODE_READWRITE && mode != DBFMODE_READONLY) {
    throwSqlError(SQL_INVALID_FILEMODE, _T("Invalid filemode (=%d) on <%s>"), mode, fileName.cstr());
  }
  m_fileName = toUpperCase(fileName);
  m_mode     = mode;
  m_logFile  = lf;
}

void DbFile::write(UINT64 offset, const void *buffer, UINT size) const {
  if(m_mode != DBFMODE_READWRITE) {
    throwSqlError(SQL_WRITE_ERROR, _T("Writing to <%s>, Opened as readonly"), m_fileName.cstr());
  }
  if(isBackLogged() && !m_logFile->inTMF()) {
    throwSqlError(SQL_NO_TRANSACTION, _T("Cannot write to <%s>. No tmf startet"), m_fileName.cstr());
  }
  if(size == 0) {
    return;
  }
  String errorMsg;
  s_filesem.wait();
  try {
    FileSlot *fileSlot = slotTable.getFileSlot(m_fileName);
    FSEEK(fileSlot->m_file, offset);
    FWRITE(buffer, size, 1, fileSlot->m_file);
  } catch(Exception e) {
    errorMsg = e.what();
  } catch(...) {
    errorMsg = _T("Unknown exception");
  }
  s_filesem.notify();
  if (errorMsg.length() > 0) {
    throwSqlError(SQL_FATAL_ERROR, _T("%s"), errorMsg.cstr());
  }
}

void DbFile::read(UINT64 offset, void *buffer, UINT size) const {
  if(size == 0) {
    return;
  }
  String errorMsg;
  s_filesem.wait();
  try {
    FileSlot *fileSlot = slotTable.getFileSlot(m_fileName);
    FSEEK(fileSlot->m_file, offset);
    FREAD(buffer, size, 1, fileSlot->m_file);
  } catch(Exception e) {
    errorMsg = e.what();
  } catch(...) {
    errorMsg = _T("Unknown exception");
  }
  s_filesem.notify();
  if (errorMsg.length() > 0) {
    throwSqlError(SQL_FATAL_ERROR, _T("%s"), errorMsg.cstr());
  }
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
  FILE *f = NULL;
  try {
    f = MKFOPEN(fileName, _T("w+b"));
    fclose(f);
  } catch(Exception e) {
    throwSqlError(SQL_CREATE_ERROR,_T("%s"), e.what());
  }
}

void DbFile::destroy(const String &fileName) {
  s_filesem.wait();
  slotTable.closeFileSlot(fileName);
  String errorMsg;
  try {
    UNLINK(fileName);
  } catch (Exception e) {
    errorMsg = e.what();
  }
  s_filesem.notify();
  if(errorMsg.length() > 0) {
    throwSqlError(SQL_FILE_DELETE_ERROR,_T("%s"),errorMsg.cstr());
  }
}

bool DbFile::exist(const String &fileName) { // static
  return ACCESS(fileName,00)==0;
}

void DbFile::rename(const String &from, const String &to) {
  s_filesem.wait();

  slotTable.closeFileSlot(from);
  String errorMsg;
  try {
    RENAME(from,to);
  } catch(Exception e) {
    errorMsg = e.what();
  }
  s_filesem.notify();

  if(errorMsg.length() > 0) {
    throwSqlError(SQL_FILE_RENAME_ERROR,_T("%s"),errorMsg.cstr());
  }
}

String DbFile::dbFileName(const String &dbName, const String &fileName) {
  return toUpperCase(format(_T("%s\\%s"), dbName.cstr(), fileName.cstr()));
}

