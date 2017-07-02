#include "stdafx.h"
#include <MyUtil.h>
#include <sys\utime.h>
#include <Scandir.h>
#include <WildCardRegex.h>
#include <Date.h>
#include <Tokenizer.h>
#include <Conio.h>
#include <comdef.h>
#include <atlconv.h>

#define BUFSIZE 0x10000

static bool overwriteAll = false;

static bool canOverWrite(const String &fname) {
  if(overwriteAll || ACCESS(fname, 0) < 0) {
    return true;
  }

  for(;;) {
    _tprintf(_T("%s already exist. Overwrite (y/n) (a=for all files)? "), fname.cstr());
    const char ch = _getch();
    _tprintf(_T("\n"));
    switch(ch) {
    case 'y':
    case 'Y': return true;
    case 'n':
    case 'N': return false;
    case 'a':
    case 'A':
      overwriteAll = true;
      return true;
    case 'q':
    case 'Q':
      exit(0);
    }
  }
}

static int alphasort(const String &s1, const String &s2) {
  return _tcsicmp(s1.cstr(), s2.cstr());
}

static void libError(const TCHAR *line, int lineNumber) {
  throwException(_T("Illegal input line %d:%s\n"), lineNumber, line);
}

class FileInfo {
public:
  String         m_name;
  Timestamp      m_creationTime;
  Timestamp      m_lastAccessTime;
  Timestamp      m_lastWriteTime;
  unsigned short m_mode;
  FileInfo() {}
  FileInfo(const String &name, const Timestamp &createTime, const Timestamp &accessTime, const Timestamp &modTime, unsigned short mode);
  void setFileTimes();
  void setMode();
};

FileInfo::FileInfo(const String &name, const Timestamp &createTime, const Timestamp &accessTime, const Timestamp &modTime, unsigned short mode) {
  m_name           = name;
  m_creationTime   = createTime;
  m_lastAccessTime = accessTime;
  m_lastWriteTime  = modTime;
  m_mode           = mode;
}

void FileInfo::setMode() {
  if(chmod(m_name,m_mode) < 0) {
    _tperror(m_name.cstr());
  }
}

static FILETIME timestampToFileTime(const Timestamp &ts) {
  const SYSTEMTIME ust = ts;
  FILETIME ft;
  if(!SystemTimeToFileTime(&ust,&ft)) {
    throwException(_T("Cannot convert timestamp <%s> to FILETIME:%s"), ts.toString().cstr(), getLastErrorText().cstr());
  }
  return ft;
}

void FileInfo::setFileTimes() {
  HANDLE f = CreateFile(m_name.cstr()
                        ,FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(f == INVALID_HANDLE_VALUE) {
    _ftprintf(stderr,_T("%s:%s\n"), m_name.cstr(), getLastErrorText().cstr());
    return;
  }

  try {
    const FILETIME ctime = timestampToFileTime(m_creationTime  );
    const FILETIME atime = timestampToFileTime(m_lastAccessTime);
    const FILETIME mtime = timestampToFileTime(m_lastWriteTime );
    if(!SetFileTime(f, &ctime, &atime, &mtime)) {
      _ftprintf(stderr,_T("%s:SetFileTime failed. %s\n"), m_name.cstr(), getLastErrorText().cstr());
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s:%s\n"), m_name.cstr(), e.what());
  }

  CloseHandle(f);
}

class Library {
private:
  static const TCHAR *skillLine1, *skillLine2, *skillLine3;
  static const size_t skillLength1;
  static const size_t skillLength2;

  String m_libName;
  FILE  *m_libFile;
  void openLib(const TCHAR *mode);
  void closeLib();

  static bool     isHeaderLine(   const TCHAR *line);
  static String   makeHeaderLine( const String &fname, const struct _stat &st);
  static FileInfo extractFileInfo(const TCHAR *line, int lineNumber);

public:
  Library(const String &libName);
  ~Library();
  void pack(  const StringArray &names, bool verbose);
  void unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose);
  void list(  const TCHAR **argv, bool sorting);
};

Library::Library(const String &libName) {
  m_libName = libName;
  m_libFile = NULL;
}

Library::~Library() {
  closeLib();
}

void Library::openLib(const TCHAR *mode) {
  closeLib();
  m_libFile = FOPEN(m_libName, mode);
  setvbuf(m_libFile, NULL, _IOFBF, BUFSIZE);
}

void Library::closeLib() {
  if(m_libFile != NULL) {
    fclose(m_libFile);
    m_libFile = NULL;
  }
}

void Library::pack(const StringArray &names, bool verbose) {
  openLib(_T("w"));
  for(size_t i = 0; i < names.size(); i++ ) {
    try {
      const String &name = names[i];
      if(name.equalsIgnoreCase(m_libName)) {
        continue; // do not process library !!
      }
      if(verbose) {
        _ftprintf(stderr, _T("Packing %s\n"), name.cstr());
      }
      const struct _stat st = STAT(name);
      FILE *f = FOPEN(name, _T("r"));
      _ftprintf(m_libFile, _T("%s\n"), makeHeaderLine(name, st).cstr());
      TCHAR line[10000];
      while(FGETS(line, ARRAYSIZE(line), f)) {
        _ftprintf(m_libFile, _T("%s\n"), line);
      }
      fclose(f);
    } catch(Exception e) {
      _ftprintf(stderr, _T("%s\n"), e.what());
    }
  }
  closeLib();
}

#define CLOSEFILE(f, fileInfo)                                        \
{ fclose(f);                                                          \
  if(setTimestamp) {                                                  \
    fileInfo.setFileTimes();                                          \
  }                                                                   \
  if(setMode) {                                                       \
    fileInfo.setMode();                                               \
  }                                                                   \
  f = NULL;                                                           \
}

void Library::unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose) {
  FileInfo      fileInfo;
  WildCardRegex wc;
  int           lineNumber = 0;
  bool          matchAll;

  if(*argv == NULL) {
    matchAll = true;
  } else {
    matchAll = false;
    wc.compile(argv);
  }
  openLib(_T("r"));

  FILE *f = NULL;
  TCHAR line[10000];
  while(FGETS(line, ARRAYSIZE(line), m_libFile)) {
    lineNumber++;
    if(!isHeaderLine(line)) {
      if(!f) {
        continue;
      }
      _ftprintf(f, _T("%s\n"), line);
    } else {
      if(f) {
        CLOSEFILE(f, fileInfo);
      }
      fileInfo = extractFileInfo(line, lineNumber);
      if(!matchAll && wc.search(fileInfo.m_name) < 0) {
        continue;
      }
      if(!canOverWrite(fileInfo.m_name)) {
        continue;
      }
      if((f = mkfopen(fileInfo.m_name, _T("w"))) == NULL) {
        _tperror(fileInfo.m_name.cstr());
        continue;
      }
      if(verbose) {
        _ftprintf(stderr, _T("Unpacking %s\n"), fileInfo.m_name.cstr());
      }
    }
  }
  if(f) {
    CLOSEFILE(f, fileInfo);
  }
  closeLib();
}

const TCHAR *Library::skillLine1   = _T("/* ####----####+-+-+-+-+");
const TCHAR *Library::skillLine2   = _T("+-+-+-+-+####-+-+-+-+-+");
const TCHAR *Library::skillLine3   = _T("+-+-+-+-+####----#### */");
const size_t Library::skillLength1 = _tcslen( Library::skillLine1 );
const size_t Library::skillLength2 = _tcslen( Library::skillLine2 );

bool Library::isHeaderLine(const TCHAR *line) {
  return _tcsnccmp(line, skillLine1, skillLength1) == 0;
}

String Library::makeHeaderLine(const String &fname, const struct _stat &st) {
  Timestamp ctime(st.st_ctime);
  Timestamp atime(st.st_atime);
  Timestamp mtime(st.st_mtime);
  return format(_T("%s%s%s<%s><%s><%s><%04X>%s")
               ,skillLine1
               ,fname.cstr()
               ,skillLine2
               ,ctime.toString(ddMMyyyyhhmmss).cstr()
               ,atime.toString(ddMMyyyyhhmmss).cstr()
               ,mtime.toString(ddMMyyyyhhmmss).cstr()
               ,st.st_mode
               ,skillLine3);
}

FileInfo Library::extractFileInfo(const TCHAR *line, int lineNumber) {
  String copy(line);
  String fileName;

  TCHAR *sp1 = copy.cstr() + skillLength1;
  TCHAR *sp2;
  if((sp2 = _tcsstr(sp1, skillLine2)) == NULL) {
    libError(line, lineNumber);
  } else {
    const TCHAR ch = *sp2;
    *sp2     = _T('\0');
    fileName = sp1;
    *sp2     = ch;
  }
  sp2 += skillLength2;
  Tokenizer tok(sp2, _T("<>"));
  if(!tok.hasNext()) {
    libError(line, lineNumber);
  }
  Timestamp ctime(tok.next());
  if(!tok.hasNext()) {
    libError(line, lineNumber);
  }
  Timestamp atime(tok.next());
  if(!tok.hasNext()) {
    libError(line, lineNumber);
  }
  Timestamp mtime(tok.next());
  if(!tok.hasNext()) {
    libError(line, lineNumber);
  }
  unsigned short mode;
  const String modeStr = tok.next();
  if(_stscanf(modeStr.cstr(), _T("%hX"), &mode) != 1) {
    libError(line, lineNumber);
  }
  return FileInfo(fileName, ctime, atime, mtime, mode);
}

static int fileInfoSort(const FileInfo &f1, const FileInfo &f2) {
  return alphasort(f1.m_name, f2.m_name);
}

void Library::list(const TCHAR **argv, bool sorting) {
  TCHAR           line[10000];
  Array<FileInfo> result;
  WildCardRegex   wc;
  bool            matchAll;
  int             lineNumber = 0;

  if(*argv == NULL) {
    matchAll = true;
  } else {
    matchAll = false;
    wc.compile(argv);
  }

  openLib(_T("r"));
  while(FGETS(line, ARRAYSIZE(line), m_libFile)) {
    lineNumber++;
    if(!isHeaderLine(line)) {
      continue;
    }
    const FileInfo fileInfo = extractFileInfo(line, lineNumber);
    if(!matchAll && wc.search(fileInfo.m_name) < 0) {
      continue;
    }
    result.add(fileInfo);
  }
  closeLib();

  if(sorting) {
    result.sort(fileInfoSort);
  }

  for(size_t i = 0; i < result.size(); i++) {
    const FileInfo &f = result[i];
    _tprintf(_T("%-70s %s\n"), f.m_name.cstr(), Timestamp::cctime(f.m_lastWriteTime.gettime_t()).cstr());
  }
}

static void printBanner() {
  static bool done = false;
  if(!done) {
    _ftprintf(stderr, _T(" Txtpak Version 1.6  Copyright (c) 1999-2017 Jesper G. Mikkelsen\n\n"));
  }
  done = true;
}

typedef enum {
  NO_COMMAND
 ,CMD_UNPACK
 ,CMD_PACK
 ,CMD_LIST
} CommandType;

static String childName(const String &path, const String &fname) {
  return (path == ".") ? fname : FileNameSplitter::getChildName(path, fname);
}

static void recurseSubDir(const String &dir, const String &pattern, StringArray &result) {
  DirList list = scandir(childName(dir, pattern), SELECTFILE);
  for(size_t i = 0; i < list.size(); i++) {
    result.add(childName(dir,list[i].name));
  }
  list = scandir(childName(dir, _T("*.*")), SELECTSUBDIR);
  for(size_t i = 0 ; i < list.size(); i++) {
    recurseSubDir(childName(dir, list[i].name), pattern, result);
  }
}

static StringArray findFiles(const TCHAR **argv, bool recurse) {
  StringArray result;
  if(!recurse) {
    for(;*argv; argv++) {
      const DirList list = scandir(*argv);
      for(size_t i = 0; i < list.size(); i++) {
        result.add(list[i].name);
      }
    }
  } else {
    for(;*argv; argv++) {
      recurseSubDir(_T("."), *argv, result);
    }
    result.sort(alphasort);
  }
  return result;
}

static StringArray readFileNames(const String &fileName) {
  TCHAR line[256];
  StringArray result;
  FILE *input = fileName.length() == 0 ? stdin : FOPEN(fileName, _T("r"));
  while(FGETS(line, ARRAYSIZE(line), input)) {
    result.add(strTrim(line));
  }
  if(input != stdin) {
    fclose(input);
  }
  return result;
}

static void usage() {
  printBanner();
  _ftprintf(stderr
           ,_T("Usage: txtpak a archive [modifiers] [files...]\n"
               "       txtpak e archive [modifiers] [files...]\n"
               "       txtpak l archive [modifiers] [files...]\n"
               "Commands     a : Archive files.\n"
               "             e : Extract files.\n"
               "             l : List contents of archive.\n"
               "Modifiers : -s : Sort files by name\n"
               "            -w : Overwrite files when unpacking. Default is ask for overwrite.\n"
               "            -b : Don't print banner.\n"
               "            -t : Don't set creation-,modification- or access-time when unpacking.\n"
               "                 Default these 3 timestamps will be set the value specified in the archive.\n"
               "            -m : Don't set read/write mode when unpacking.\n"
               "                 Default the mode will be set to the value specified in the archive.\n"
               "            -v : Verbose.\n"
               "            -f[file]: file contains filenames to pack.\n"
               "                      If file if not specified, stdin is used. Input can be generated by grep -H\n"
               "                      Ex: grep -Hr addAttribute *.cpp | txtpak a files -f\n"
               "                      will pack all cpp-files (in current directory, and subdirectories), containing the text \"addAttribute\"\n")
           );
  exit(-1);
}

int main(int argc, const char **argv) {
  const char *cp;
  bool        recurse      = false;
  bool        setTimestamp = true;
  bool        setMode      = true;
  bool        showBanner   = true;
  bool        sorting      = false;
  bool        verbose      = false;
  const char *fileName     = NULL;

  CommandType command = NO_COMMAND;

  argv++;
  if(!*argv) {
    usage();
  }
  switch(**argv) {
  case 'a': command = CMD_PACK  ; break;
  case 'e': command = CMD_UNPACK; break;
  case 'l': command = CMD_LIST  ; break;
  default : usage();
  }
  argv++;

  if(!*argv) {
    usage();
  }
  const char *libName = *argv;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'w': overwriteAll = true;  continue;
      case 's': sorting      = true;  continue;
      case 'b': showBanner   = false; continue;
      case 'r': recurse      = true;  continue;
      case 't': setTimestamp = false; continue;
      case 'm': setMode      = false; continue;
      case 'f':
        fileName     = cp+1;
        break;
      case 'v': verbose      = true;  continue;
      default : usage();
      }
      break;
    }
  }

  if(showBanner) {
    printBanner();
  }

  USES_CONVERSION;
  Library library(libName);
  try {
    switch(command) {
    case CMD_UNPACK:
      library.unpack(argv2targv(argv), setTimestamp, setMode, verbose);
      break;
    case CMD_PACK:
      { StringArray fileNames;
        if(fileName == NULL) {
          fileNames = findFiles(argv2targv(argv), recurse);
        } else {
          fileNames = readFileNames(fileName);
        }
        if(!canOverWrite(libName)) {
          break;
        }
        if(sorting) {
          fileNames.sort(alphasort);
        }
        library.pack(fileNames, verbose);
      }
      break;
    case CMD_LIST:
      library.list(argv2targv(argv), sorting);
      break;
    default:
      usage();
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("txtpak:%s\n"), e.what());
    return -1;
  }
  return 0;
}
