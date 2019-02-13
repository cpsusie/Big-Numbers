#include "StdAfx.h"
#include "Library.h"

static size_t getSubDirCount(const String &dir) {
  return getAllSubDir(dir).size();
}

void copyFile(const String &srcName, const String &dstName) {
  ByteOutputFile dst(dstName);
  ByteInputFile  src(srcName);
  BYTE buffer[4096];
  intptr_t n;
  while((n = src.getBytes(buffer, sizeof(buffer))) > 0) {
    dst.putBytes(buffer, n);
  }
}

LibraryContent::LibraryContent(const Array<FileInfo> &list) {
  const size_t n = list.size();
  for(size_t i = 0; i < n; i++) {
    const FileInfo &info = list[i];
    put(info.m_name, info);
  }
}

static void removeNamesWithLibPath(StringArray &names, const String &libName) {
  const size_t n = names.size();
  StringArray result;
  for(size_t i = 0; i < n; i++) {
    const String &name = names[i];
    FileNameSplitter sp1(name), sp2(name);
    sp2.setDir(libName);
    if(!sp1.getAbsolutePath().equalsIgnoreCase(sp2.getAbsolutePath())) {
      result.add(name);
    }
  }
  names = result;
}

static void printAddCountMessage(size_t addCount) {
  if(addCount == 0) {
    printMessage(_T("No files added to library"));
  } else if(addCount == 1) {
    printMessage(_T("1 file added to library"));
  } else {
    printMessage(_T("%zu files added to library"), addCount);
  }
}

// ---------------------------------------------------------------------------------------------------------------

#define BUFSIZE 0x10000

Library::Library(const String &libName, LibType type)
  : m_libName(libName)
  , m_libType(type)
{
  m_libFile = NULL;
  m_guidMap = NULL;
}

Library::~Library() {
  closeLib();
}

String Library::createMapFileName(const String &libName) { // static
  return FileNameSplitter(FileNameSplitter::getChildName(libName
    , FileNameSplitter(libName).getFileName())).setExtension(_T("txt")).getFullPath();
}

void Library::checkFirstLineIsInfoLine(const String &fileName) { // static
  FILE *f = FOPEN(fileName, _T("r"));
  try {
    if(!feof(f)) {
      String line;
      readLine(f, line);
      FileInfo info;
      info.extractFileInfo(line, 1);
    }
    fclose(f);
  }
  catch (...) {
    fclose(f);
    throw;
  }
}

LibType Library::getLibType(const String &libName, bool checkSubDirCount) { // static
  struct _stati64 st;
  STAT64(libName, st);
  if(st.st_mode & S_IFDIR) {
    const GuidNameMap map(createMapFileName(libName));
    if(!checkSubDirCount || (getSubDirCount(libName) == 0)) {
      return LT_DIRWITHMAP;
    }
  } else if(st.st_mode & S_IFREG) {
    checkFirstLineIsInfoLine(libName);
    return LT_TEXTFILE;
  }
  throwException(_T("%s is not a valid library"), libName.cstr());
  return LT_UNKNOWN;
}

void Library::removeLib(const String &libName) { // static
  switch(getLibType(libName)) {
  case LT_TEXTFILE:
    UNLINK(libName);
    break;
  case LT_DIRWITHMAP:
    { const DirList list = scandir(childName(libName, _T("*.*")), SELECTFILE);
      for(size_t i = 0; i < list.size(); i++) {
        UNLINK(childName(libName, list[i].name));
      }
      RMDIR(libName);
    }
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("libName=%s"), libName.cstr());
  }
}

void Library::openLib(OpenMode mode, bool checkSubDirCount) {
  DEFINEMETHODNAME;
  closeLib();
  if(getLibType() == LT_UNKNOWN) {
    m_libType = getLibType(m_libName, checkSubDirCount);
  }
  switch(mode) {
  case READ_MODE:
    switch(getLibType()) {
    case LT_TEXTFILE:
      m_libFile = FOPEN(m_libName, _T("r"));
      break;
    case LT_DIRWITHMAP:
      m_guidMap = new GuidNameMap(createMapFileName(m_libName));
      break;
    default:
      libTypeError(method);
    }
    break;
  case WRITE_MODE:
    switch(getLibType()) {
    case LT_TEXTFILE:
      m_libFile = FOPEN(m_libName, _T("w"));
      setvbuf(m_libFile, NULL, _IOFBF, BUFSIZE);
      break;
    case LT_DIRWITHMAP:
      { FILE *tmpFile = MKFOPEN(createMapFileName(m_libName), _T("w"));
        fclose(tmpFile);
        m_guidMap = new GuidNameMap;
      }
      break;
    default:
      libTypeError(method);
    }
    break;

  case UPDATE_MODE:
    switch(getLibType()) {
    case LT_TEXTFILE:
      m_libFile = FOPEN(m_libName, _T("a"));
      break;
    case LT_DIRWITHMAP:
      m_guidMap = new GuidNameMap(createMapFileName(m_libName));
      break;
    default:
      libTypeError(method);
    }
    break;
  default:
    throwInvalidArgumentException(method, _T("mode=%d"), mode);
  }
}

void Library::closeLib() {
  if(m_libFile != NULL) {
    fclose(m_libFile);
    m_libFile = NULL;
  }
  if(m_guidMap != NULL) {
    delete m_guidMap;
    m_guidMap = NULL;
  }
}

void Library::prepareArgvPattern(const TCHAR **argv) {
  if((argv == NULL) || (*argv == NULL)) {
    m_matchAll = true;
  } else {
    m_matchAll = false;
    m_wc.compile(argv);
  }
}

void Library::restoreTimesAndMode(const FileInfo &info) {
  if(m_setTimestamp) {
    try {
      info.setFileTimes();
    } catch(Exception e) {
      printException(e);
    }
  }
  if(m_setMode) {
    try {
      info.setMode();
    } catch (Exception e) {
      printException(e);
    }
  }
}

void Library::closeFile(FILE * &f, const FileInfo &info) {
  fclose(f);
  f = NULL;
  restoreTimesAndMode(info);
}

LibraryContent Library::getAllContent() {
  prepareArgvPattern(NULL);
  openLib(READ_MODE, false);
  Array<FileInfo> list;
  switch(getLibType()) {
  case LT_TEXTFILE  :
    getInfoListFromTextFile(&list);
    break;
  case LT_DIRWITHMAP:
    getInfoListFromDir(&list);
    break;
  default           :
    libTypeError(__TFILE__);
  }
  return LibraryContent(list);
}

// ---------------------------------------------------------------------------------------------------------------

void Library::pack(const StringArray &names, bool verbose) {
  m_verbose = verbose;
  openLib(WRITE_MODE);
  size_t addCount;
  switch(getLibType()) {
  case LT_TEXTFILE:
    addCount = packToTextFile(names, NULL);
    break;
  case LT_DIRWITHMAP:
    addCount = packToDir(names, NULL);
    break;
  default:
    libTypeError(__TFUNCTION__);
  }
  if(m_verbose) {
    printAddCountMessage(addCount);
  }
  closeLib();
}

void Library::update(const StringArray &names, bool verbose) {
  LibraryContent content = getAllContent();
  m_verbose = verbose;
  openLib(UPDATE_MODE);
  size_t addCount;
  switch(getLibType()) {
  case LT_TEXTFILE:
    addCount = packToTextFile(names, &content);
    break;
  case LT_DIRWITHMAP:
    { StringArray names1 = names;
      removeNamesWithLibPath(names1, getLibName());
      addCount = packToDir(names1, &content);
    }
    break;
  default:
    libTypeError(__TFUNCTION__);
  }
  if(m_verbose) {
    printAddCountMessage(addCount);
  }
  closeLib();
}


void Library::unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose) {
  m_setTimestamp = setTimestamp;
  m_setMode = setMode;
  m_verbose = verbose;

  prepareArgvPattern(argv);
  openLib(READ_MODE);
  switch(getLibType()) {
  case LT_TEXTFILE:
    unpackFromTextFile();
    break;
  case LT_DIRWITHMAP:
    unpackFromDir();
    break;
  default:
    libTypeError(__TFUNCTION__);
  }
  closeLib();
}

void Library::list(const TCHAR **argv, bool sorting) {
  prepareArgvPattern(argv);
  openLib(READ_MODE);

  Array<FileInfo> list;
  switch(getLibType()) {
  case LT_TEXTFILE:
    getInfoListFromTextFile(&list);
    break;
  case LT_DIRWITHMAP:
    getInfoListFromDir(&list);
    break;
  default:
    libTypeError(__TFUNCTION__);
  }
  closeLib();

  if(sorting) {
    list.sort(fileInfoCmp);
  }

  for(size_t i = 0; i < list.size(); i++) {
    const FileInfo &f = list[i];
    _tprintf(_T("%-70s %s\n"), f.m_name.cstr(), Timestamp::cctime(f.m_mtime.gettime_t()).cstr());
  }
}

void Library::checkIntegrity() {
  prepareArgvPattern(NULL);
  openLib(READ_MODE, false);

  Array<FileInfo> list;
  bool            ok;
  switch(getLibType()) {
  case LT_TEXTFILE:
    getInfoListFromTextFile(&list);
    ok = checkIntegrityTextFile(list);
    break;
  case LT_DIRWITHMAP:
    getInfoListFromDir(&list);
    ok = checkIntegrityFromDir(list);
    break;
  default:
    libTypeError(__TFUNCTION__);
  }
  closeLib();
  if(ok) {
    _tprintf(_T("All ok\n"));
  }
}

// ---------------------------------------------------------------------------------------------------------------

size_t Library::packToTextFile(const StringArray &names, LibraryContent *content) {
  const size_t n = names.size();
  size_t addedCounter = 0;
  for(size_t i = 0; i < n; i++) {
    try {
      const String &name = names[i];
      if(name.equalsIgnoreCase(m_libName)) {
        continue; // do not process library !!
      }
      if(content && (content->get(name) != NULL)) {
        continue;
      }
      if(m_verbose) {
        printMessage(_T("(%5zu/%5zu) Packing %s"), i + 1, n, name.cstr());
      }
      addedCounter++;
      addFileToTextFile(name);
    } catch(Exception e) {
      printException(e);
    }
  }
  return addedCounter;
}

void Library::addFileToTextFile(const String &name) {
  FILE *f = NULL;
  try {
    const FileInfo info(name);
    f = FOPEN(name, _T("r"));
    _ftprintf(m_libFile, _T("%s\n"), info.toString().cstr());
    String line;
    while(readLine(f, line)) {
      _ftprintf(m_libFile, _T("%s\n"), line.cstr());
    }
    fclose(f); f = NULL;
  } catch(...) {
    if(f) {
      fclose(f); f = NULL;
    }
    throw;
  }
}

size_t Library::getUnpackCountTextFile() const {
  return getInfoListFromTextFile(NULL);
}

void Library::unpackFromTextFile() {
  const size_t totalCount = m_verbose ? getUnpackCountTextFile() : 0;
  size_t       count = 0;
  String       line;
  size_t       lineNumber = 0;
  FileInfo     info;
  FILE        *f = NULL;
  while(readLine(m_libFile, line)) {
    lineNumber++;
    try {
      if(!FileInfo::isInfoLine(line)) {
        if(!f) {
          continue;
        }
        _ftprintf(f, _T("%s\n"), line.cstr());
      } else {
        if(f) {
          closeFile(f, info);
        }
        info.extractFileInfo(line, lineNumber);
        const String &fileName = info.m_name;
        if(!matchArgvPattern(fileName)) {
          continue;
        }
        count++;
        if(!canOverWrite(fileName)) {
          continue;
        }
        f = MKFOPEN(fileName, _T("w"));
        if(m_verbose) {
          printMessage(_T("(%5zu/%5zu) Unpacking %s"), count, totalCount, fileName.cstr());
        }
      }
    } catch(Exception e) {
      printException(e);
    }
  }
  if(f) {
    closeFile(f, info);
  }
}

bool Library::checkIntegrityTextFile(const Array<FileInfo> &list) {
  return true;
}

size_t Library::getInfoListFromTextFile(Array<FileInfo> *list) const {
  const INT64 pos = GETPOS(m_libFile);
  if(list) list->clear();
  String line;
  size_t lineNumber = 0;
  size_t count      = 0;
  while(readLine(m_libFile, line)) {
    lineNumber++;
    if(!FileInfo::isInfoLine(line)) {
      continue;
    }
    FileInfo info;
    info.extractFileInfo(line, lineNumber);
    if(matchArgvPattern(info.m_name)) {
      count++;
      if(list) list->add(info);
    }
  }
  FSEEK(m_libFile, pos);
  return count;
}

// ---------------------------------------------------------------------------------------------------------------

size_t Library::packToDir(const StringArray &names, LibraryContent *content) {
  const size_t n = names.size();
  size_t addedCounter = 0;
  for(size_t i = 0; i < n; i++) {
    try {
      const String &name = names[i];
      if(name.equalsIgnoreCase(m_libName)) {
        continue; // do not process library !!
      }
      if(content && (content->get(name) != NULL)) {
        continue;
      }
      if(m_verbose) {
        printMessage(_T("(%5zu/%5zu) Packing %s"), i+1, n, name.cstr());
      }
      addedCounter++;
      addFileToDir(name);
    } catch(Exception e) {
      printException(e);
    }
  }
  m_guidMap->save(createMapFileName(m_libName));
  return addedCounter;
}

void Library::addFileToDir(const String &name) {
  const FileInfo info(name);
  const String   extension = FileNameSplitter(name).getExtension();
  TCHAR guid[100];
  newGUID(guid);
  const String packedName = FileNameSplitter(guid).setExtension(extension).getFullPath();
  const String dstName    = FileNameSplitter::getChildName(m_libName, packedName);
  copyFile(name, dstName);
  m_guidMap->add(packedName, info);
}

size_t Library::getUnpackCountDir() const {
  return m_matchAll ? m_guidMap->size() : getInfoListFromDir(NULL);
}

void Library::unpackFromDir() {
  const size_t totalCount = m_verbose ? getUnpackCountDir() : 0;
  size_t       count      = 0;
  for(Iterator<Entry<String, FileInfo > > it = m_guidMap->entrySet().getIterator(); it.hasNext();) {
    try {
      const Entry<String, FileInfo > &e = it.next();
      const String                   &packedName = e.getKey();
      const FileInfo                 &info = e.getValue();
      if(!matchArgvPattern(info.m_name)) {
        continue;
      }
      count++;
      if(!canOverWrite(info.m_name)) {
        continue;
      }
      if(m_verbose) {
        printMessage(_T("(%5zu/%5zu) Unpacking %s"), count, totalCount, info.m_name.cstr());
      }
      const String srcName = FileNameSplitter::getChildName(m_libName, packedName);
      copyFile(srcName, info.m_name);
      restoreTimesAndMode(info);
    } catch(Exception e) {
      printException(e);
    }
  }
}

bool Library::checkIntegrityFromDir(const Array<FileInfo> &list) {
  bool   ok           = true;
  intptr_t missingCount = 0;
  for(Iterator<Entry<String, FileInfo> > it = m_guidMap->entrySet().getIterator(); it.hasNext();) {
    const Entry<String, FileInfo> &entry      = it.next();
    const String                  &packedName = entry.getKey();
    const FileInfo                &info       = entry.getValue();
    const String                   srcName    = FileNameSplitter::getChildName(m_libName, packedName);
    if(ACCESS(srcName,0) < 0) {
      _tprintf(_T("Missing file for map-entry <%s>-><%s>\n"), packedName.cstr(), info.m_name.cstr());
      missingCount++;
      ok = false;
    }
  }
  const intptr_t knownLibFileCount = m_guidMap->size() + 1 - missingCount;
  const intptr_t unknownFileCount  = getAllFiles(m_libName).size() - knownLibFileCount;
  if(unknownFileCount  != 0) {
    _tprintf(_T("%zd unknonwn files exist in %s\n"), unknownFileCount, m_libName.cstr());
    ok = false;
  }
  const size_t subDirCount = getSubDirCount(m_libName);
  if(subDirCount != 0) {
    _tprintf(_T("%zu sub directories exist in %s\n"), subDirCount, m_libName.cstr());
    ok = false;
  }
  return ok;
}

size_t Library::getInfoListFromDir(Array<FileInfo> *list) const {
  if(list) list->clear();
  size_t count = 0;
  for(Iterator<FileInfo> it = m_guidMap->values().getIterator(); it.hasNext();) {
    const FileInfo &info = it.next();
    if(matchArgvPattern(info.m_name)) {
      count++;
      if(list) list->add(info);
    }
  }
  return count;
}
