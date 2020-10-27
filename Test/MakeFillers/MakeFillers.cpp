#include "stdafx.h"
#include <Random.h>
#include <Date.h>
#include <MathUtil.h>
#include <ThreadPool.h>

static void usage() {
  exit(-1);
}

#define WANTEDFILESIZE  (50*1024*1024)
#define MAXINDEX        (1024*1024)

class DiscInfo {
public:
  UINT64 m_bytesAvailable;
  UINT64 m_bytesTotal;
  UINT64 m_freeBytes;
  DiscInfo(const PULARGE_INTEGER bytesAvailable, const PULARGE_INTEGER bytesTotal, const PULARGE_INTEGER freeBytes);
  String toString() const;
};

UINT64 toUint64(const PULARGE_INTEGER x) {
  union {
    ULARGE_INTEGER uli;
    UINT64         ui64;
  } tmp;
  tmp.uli = *x;
  return tmp.ui64;
}

DiscInfo::DiscInfo(const PULARGE_INTEGER bytesAvailable, const PULARGE_INTEGER bytesTotal, const PULARGE_INTEGER freeBytes)
: m_bytesAvailable(toUint64(bytesAvailable))
, m_bytesTotal(    toUint64(bytesTotal    ))
, m_freeBytes(     toUint64(freeBytes     ))
{
}

String DiscInfo::toString() const {
  return format(_T("BytesTotal:%20s, BytesAvailable:%20s, FreeBytes:%20s")
               ,format1000(m_bytesTotal    ).cstr()
               ,format1000(m_bytesAvailable).cstr()
               ,format1000(m_freeBytes     ).cstr()
               );
}

DiscInfo getDiscInfo(TCHAR driveName) {
  String path = format(_T("%c:\\"), driveName);
  ULARGE_INTEGER bytesAvailable, bytesTotal, freeBytes;
  GetDiskFreeSpaceEx(path.cstr(), &bytesAvailable, &bytesTotal, &freeBytes);
  return DiscInfo(&bytesAvailable, &bytesTotal, &freeBytes);
}

class FileInfo {
public:
  size_t m_fileSize;   // in bytes
  float  m_writeTime;  // in msec

  inline FileInfo()
    : m_fileSize(0)
    , m_writeTime(0)
  {
  }
  inline FileInfo(size_t fileSize, float writeTime)
    : m_fileSize( fileSize )
    , m_writeTime(writeTime)
  {
  }
  inline FileInfo &clear() {
    m_fileSize  = 0;
    m_writeTime = 0;
    return *this;
  }
  FileInfo &operator+=(const FileInfo &i) {
    m_fileSize  += i.m_fileSize;
    m_writeTime += i.m_writeTime;
    return *this;
  }
  FileInfo &operator-=(const FileInfo &i) {
    m_fileSize  -= i.m_fileSize;
    m_writeTime -= i.m_writeTime;
    return *this;
  }
  inline bool isEmpty() const {
    return m_fileSize == 0;
  }
  inline double getAvgTimePerKb() const {
    return m_fileSize 
         ? ((double)m_writeTime / (double)m_fileSize * 1024.0)
         : 0.0;
  }
  String FileInfo::toString() const {
    return format(_T("%16s %8.3le  %.3le ms/Kb")
                 ,format1000(m_fileSize).cstr(), m_writeTime, getAvgTimePerKb()
                 );
  }
};

class FileTable {
private:
  const TCHAR            m_drive;
  const String           m_fileName;
  BitSet                 m_existingFiles;
  mutable JavaRandom     m_rnd;
  size_t                 m_count;
  FileInfo               m_sum;
  CompactArray<FileInfo> m_timeTable;
  void       findTimeSumAndCount();
  void       initTimeTable();
  FileTable &save(ByteOutputStream &out);
  FileTable &load(ByteInputStream  &in );
public:
  FileTable(TCHAR drive);
  ~FileTable();
  FileTable &load();
  FileTable &save();
  void listTimeTable(FILE *f = stdout) const;
  bool fillerFileExist(size_t n);
  inline TCHAR getDrive() const {
    return m_drive;
  }
  void addFile(size_t n);
  void removeFile(size_t n);
  void setInfo(size_t n, const FileInfo &info);
  inline double getAvgTime() const {
    return m_sum.getAvgTimePerKb();
  }
  // return timeusage to read file in msec/byte
  FileInfo writeFillerFile(size_t n, size_t wantedSize = WANTEDFILESIZE);
  Iterator<size_t> getIterator();
  String makeFileName(size_t n) const;
  inline DiscInfo getDiscInfo() const {
    return ::getDiscInfo(getDrive());
  }
  const FileInfo &getTableInfoSum() const {
    return m_sum;
  }
  const FileInfo getFileInfo(size_t n) const {
    return m_timeTable[n];
  }
  void showAllInfo(int line) const;
};

FileTable::FileTable(TCHAR drive)
: m_drive(drive)
, m_fileName(format(_T("%c:\\temp\\timings.dat"), drive))
, m_existingFiles(MAXINDEX)
{
  m_rnd.randomize();
  load();
}

FileTable::~FileTable() {
  save();
}

void FileTable::initTimeTable() {
  m_timeTable.setCapacity(MAXINDEX);
  m_timeTable.clear(-1);
  m_timeTable.insert(0,FileInfo(),MAXINDEX);
}

FileTable &FileTable::save() {
  return save(CompressFilter(ByteOutputFile(m_fileName)));
}

FileTable &FileTable::load() {
  initTimeTable();
  if(ACCESS(m_fileName,0) >= 0) {
    load(DecompressFilter(ByteInputFile(m_fileName)));
  }
  findTimeSumAndCount();
  return *this;
}

FileTable &FileTable::save(ByteOutputStream &out) {
  m_existingFiles.save(out);
  m_timeTable.save(out);
  return *this;
}

FileTable &FileTable::load(ByteInputStream &in) {
  m_existingFiles.load(in);
  m_timeTable.load(in);
  return *this;
}

void FileTable::findTimeSumAndCount() {
  m_count = m_existingFiles.size();
  FileInfo sum;
  if(m_count > 0) {
    for(Iterator<size_t> it = m_existingFiles.getIterator(); it.hasNext();) {
      sum += m_timeTable[it.next()];
    }
  }
  m_sum = sum;
}

void FileTable::addFile(size_t n) {
  FileInfo info = writeFillerFile(n);
  setInfo(n, info);
}

void FileTable::removeFile(size_t n) {
  if(!fillerFileExist(n)) {
    Console::printf(0, 20, _T("file %zu doesn't exist"), n);
    return;
  }
  const String name = makeFileName(n);
  UNLINK(name);
  setInfo(n, FileInfo());
}

void FileTable::setInfo(size_t n, const FileInfo &info) {
  if(m_existingFiles.contains(n)) {
    m_count--;
    m_sum -= m_timeTable[n];
    m_timeTable[n].clear();
    m_existingFiles.remove(n);
  }
  if(!info.isEmpty()) {
    m_count++;
    m_timeTable[n] = info;
    m_sum += info;
    m_existingFiles.add(n);
  }
}

FileInfo FileTable::writeFillerFile(size_t n, size_t wantedSize) {
  if(fillerFileExist(n)) {
    Console::printf(0,20,_T("file %zu already exist"), n);
    return FileInfo();
  }
  const String name = makeFileName(n);
  FILE        *f    = nullptr;
  try {
    f = MKFOPEN(name, _T("wb"));
    fclose(f);
  } catch(Exception e) {
    if(f) {
      fclose(f);
    }
    unlink(name);
    Console::printf(0, 20, _T("%-20s:Exception:%s"), __TFUNCTION__, e.what());
    return FileInfo();
  } catch(...) {
    if(f) {
      fclose(f);
    }
    throw;
  }

  int           handle = -1;
  UINT64       *buffer = nullptr;
  double        usedTime = 0;
  size_t        totalWritten = 0;
  try {
    buffer = new UINT64[wantedSize / sizeof(UINT64) + 1];
    const UINT64 *endbuf = (UINT64*)((BYTE*)buffer + wantedSize);
    for(UINT64 *p = buffer; p < endbuf;) *(p++) = m_rnd.nextInt64();
    handle = _wopen(name.cstr(), O_RDWR | O_RAW);
    if(handle < 0) {
      throwLastErrorOnSysCallNameException(name);
    }
    const Timestamp startTime;
    for(totalWritten = 0; totalWritten < wantedSize;) {
      const UINT wantwrite = (UINT)min(WANTEDFILESIZE, wantedSize - totalWritten);
      const int  written = _write(handle, (BYTE*)buffer + totalWritten, wantwrite);
      if(written < 0) {
        throwException(_T("write failed"));
      }
      totalWritten += written;
    }
    const double usedTime = Timestamp::diff(startTime, Timestamp(), TMILLISECOND);
    _close(handle); handle = -1;
    SAFEDELETEARRAY(buffer);
    return FileInfo(totalWritten, (float)usedTime);
  } catch(...) {
    if(handle >= 0) {
      _close(handle);
    }
    SAFEDELETEARRAY(buffer);
    throw;
  }
}

void FileTable::listTimeTable(FILE *f) const {
  _ftprintf(f,_T("Sum:%s"), m_sum.toString().cstr());
  const double avgSum = m_sum.getAvgTimePerKb();
  for(ConstIterator<size_t> it = m_existingFiles.getIterator(); it.hasNext();) {
    const size_t    i    = it.next();
    const FileInfo &info = m_timeTable[i];
    const double    avg  = info.getAvgTimePerKb();
    const double relativeDif = PERCENT(avg - avgSum, avgSum);
    _ftprintf(f, _T("%s:%s (%+.3lf)\n"), makeFileName(i).cstr(), m_timeTable[i].toString().cstr(), relativeDif);
  }
  _ftprintf(f, _T("Sum:%s"), m_sum.toString().cstr());
}

Iterator<size_t> FileTable::getIterator() {
  return m_existingFiles.getIterator();
}

String FileTable::makeFileName(size_t n) const {
  return format(_T("%c:\\temp\\Fillers\\%08zX\\filler%03zu.dat"), m_drive, n>>8, n&0xff);
}

inline bool FileTable::fillerFileExist(size_t n) {
  const String fileName = makeFileName(n);
  const bool   ffExist  = ACCESS(fileName, 0) >= 0;
  const bool   ftExist  = m_existingFiles.contains(n);
  if(ffExist) {
    if(!ftExist) {
      Console::printf(0, 20, _T("%s exist, but not in table"), __TFUNCTION__, fileName.cstr());
      return false;
    }
    return true;
  } else { // !ffExist
    if(ftExist) {
      setInfo(n, FileInfo());
    }
    return false;
  }
}

void FileTable::showAllInfo(int line) const {
  const DiscInfo discInfo = getDiscInfo();
  Console::printf(0, line  , _T("DiskInfo:%s"), discInfo.toString().cstr());
  Console::printf(0, line+1, _T("Info:%s"    ), getTableInfoSum().toString().cstr());
}

// ---------------------------------------------------------

class FillerFiles {
private:
  FileTable m_fileTable;
  size_t    m_writtenCounter;

public:
  FillerFiles(TCHAR drive);
  ~FillerFiles();
  void fillUpDrive();
  void unlinkFastFiles(float threshold);
  void unlinkAll();
  void save();
  void listTable();
  void showAllInfo(int line) const;
  void showStartInfo() {
    showAllInfo(0);
  }
  void showFillUpInfo(size_t foundCounter) const;
};

FillerFiles::FillerFiles(TCHAR drive)
: m_fileTable(drive)
{
  m_writtenCounter = 0;
}

FillerFiles::~FillerFiles() {
}

void FillerFiles::fillUpDrive() {
  size_t foundCounter   = 0;
  while(m_writtenCounter < MAXINDEX) {
    if(m_fileTable.fillerFileExist(m_writtenCounter)) {
      foundCounter++;
      continue;
    }
    if(Console::keyPressed()) {
      const int ch = Console::getKey();
      if((ch == 'b') || (ch == 'B')) {
        break;
      }
    }
    showFillUpInfo(foundCounter);
    const DiscInfo discInfo = m_fileTable.getDiscInfo();
    if(discInfo.m_freeBytes < 60000000) {
      break;
    }
    m_fileTable.addFile(m_writtenCounter++);
  }
}

void FillerFiles::unlinkFastFiles(float threshold) {
  for(Iterator<size_t> it = m_fileTable.getIterator(); it.hasNext();) {
    const size_t i = it.next();
    const FileInfo &fi = m_fileTable.getFileInfo(i);
    if(fi.m_writeTime < threshold) {
      m_fileTable.removeFile(i);
    }
  }
}

void FillerFiles::unlinkAll() {
  for(Iterator<size_t> it = m_fileTable.getIterator(); it.hasNext();) {
    const size_t i = it.next();
    m_fileTable.removeFile(i);
  }
}

void FillerFiles::save() {
  m_fileTable.save();
}

void FillerFiles::listTable() {
  showStartInfo();
  FILE *f = MKFOPEN(_T("C:\\temp\\Fillers.log"), _T("w"));
  m_fileTable.listTimeTable(f);
  fclose(f);
}

void FillerFiles::showAllInfo(int line) const {
  m_fileTable.showAllInfo(line);
}

void FillerFiles::showFillUpInfo(size_t foundCounter) const {
  showAllInfo(2);
  Console::printf(0,4,_T("Written:%10s, Found:%10s")
                 ,format1000(m_writtenCounter).cstr()
                 ,format1000(foundCounter).cstr());
}

int main(int argc, char **argv) {
  try {
    FillerFiles ff('c');
    ff.showStartInfo();
    float threshold = 1200;
    for(bool done = false; !done;) {
      try {
        Console::printf(0, 21, _T("(F)ill/(S)ave/(U)nlink fast files/(B)reak/Unlink (A)ll/(L)ist/(Q)uit"));
        char ch = (char)Console::getKey();
        ch = tolower(ch);
        switch(ch) {
        case 'f':
          ff.fillUpDrive();
          break;
        case 's':
          ff.save();
          break;
        case 'u':
          ff.unlinkFastFiles(threshold);
          break;
        case 'a':
          ff.unlinkAll();
          break;
        case 'l':
          ff.listTable();
          break;
        case 'q':
          done = true;
          break;
        }
      } catch(Exception e) {
        Console::printf(0,21,_T("Exception:%s\n"), e.what());
      } catch(...) {
        Console::printf(0,21, _T("Unknown exception\n"));
      }
    }
  } catch(Exception e) {
    Console::printf(0,21,_T("Exception:%s\n"), e.what());
  } catch(...) {
    Console::printf(0,21, _T("Unknown exception\n"));
  }
  return 0;
}
