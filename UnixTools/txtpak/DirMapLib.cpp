#include "StdAfx.h"
#include "DirMapLib.h"

DirMapLib::~DirMapLib() {
  close();
}

void DirMapLib::open(OpenMode mode, bool checkSubDirCount) {
  close();
  const String mapFileName = createMapFileName(getLibName());
  switch(mode) {
  case READ_MODE:
    m_guidMap = new GuidNameMap(mapFileName);
    break;
  case WRITE_MODE:
    { FILE *tmpFile = MKFOPEN(mapFileName, _T("w"));
      fclose(tmpFile);
      m_guidMap = new GuidNameMap;
    }
    break;
  case UPDATE_MODE:
    m_guidMap = new GuidNameMap(mapFileName);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("mode=%d"), mode);
  }
}

void DirMapLib::close() {
  if(m_guidMap != NULL) {
    delete m_guidMap;
    m_guidMap = NULL;
  }
}

void DirMapLib::copyFile(const String &srcName, const String &dstName) { // static
  ByteOutputFile dst(dstName);
  ByteInputFile  src(srcName);
  BYTE buffer[4096];
  intptr_t n;
  while((n = src.getBytes(buffer, sizeof(buffer))) > 0) {
    dst.putBytes(buffer, n);
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

size_t DirMapLib::addFiles(const StringArray &names, const LibraryContent *content) {
  StringArray names1 = names;
  if(content != NULL) { // we are doing an update
    removeNamesWithLibPath(names1, getLibName());
  }

  const size_t  n            = names1.size();
  const String &libName      = getLibName();
  size_t        addedCounter = 0;
  for(size_t i = 0; i < n; i++) {
    try {
      const String &name = names1[i];
      if(name.equalsIgnoreCase(libName)) {
        continue; // do not process library !!
      }
      if(content && (content->get(name) != NULL)) {
        continue;
      }
      if(m_verbose) {
        printMessage(_T("(%5zu/%5zu) Packing %s"), i+1, n, name.cstr());
      }
      addedCounter++;
      addFile(name);
    } catch(Exception e) {
      printException(e);
    }
  }
  m_guidMap->save(createMapFileName(libName));
  return addedCounter;
}

void DirMapLib::addFile(const String &name) {
  const FileInfo info(name);
  const String   extension = FileNameSplitter(name).getExtension();
  TCHAR guid[100];
  newGUID(guid);
  const String packedName = FileNameSplitter(guid).setExtension(extension).getFullPath();
  const String dstName    = FileNameSplitter::getChildName(getLibName(), packedName);
  copyFile(name, dstName);
  m_guidMap->add(packedName, info);
}

void DirMapLib::unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose) {
  prepareArgvPattern(argv);
  m_setTimestamp = setTimestamp; m_setMode = setMode; m_verbose = verbose;
  const String &libName    = getLibName();
  const size_t  totalCount = m_verbose ? getUnpackCount() : 0;
  size_t        count      = 0;
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
      const String srcName = FileNameSplitter::getChildName(libName, packedName);
      copyFile(srcName, info.m_name);
      restoreTimesAndMode(info);
    } catch(Exception e) {
      printException(e);
    }
  }
}


bool DirMapLib::checkIntegrity(const Array<FileInfo> &list) {
  bool          ok           = true;
  intptr_t      missingCount = 0;
  const String &libName      = getLibName();
  for(Iterator<Entry<String, FileInfo> > it = m_guidMap->entrySet().getIterator(); it.hasNext();) {
    const Entry<String, FileInfo> &entry      = it.next();
    const String                  &packedName = entry.getKey();
    const FileInfo                &info       = entry.getValue();
    const String                   srcName    = FileNameSplitter::getChildName(libName, packedName);
    if(ACCESS(srcName,0) < 0) {
      _tprintf(_T("Missing file for map-entry <%s>-><%s>\n"), packedName.cstr(), info.m_name.cstr());
      missingCount++;
      ok = false;
    }
  }
  const intptr_t knownLibFileCount = m_guidMap->size() + 1 - missingCount;
  const intptr_t unknownFileCount  = getAllFiles(libName).size() - knownLibFileCount;
  if(unknownFileCount  != 0) {
    _tprintf(_T("%zd unknonwn files exist in %s\n"), unknownFileCount, libName.cstr());
    ok = false;
  }
  const size_t subDirCount = getSubDirCount(libName);
  if(subDirCount != 0) {
    _tprintf(_T("%zu sub directories exist in %s\n"), subDirCount, libName.cstr());
    ok = false;
  }
  return ok;
}

void DirMapLib::removeLib(const String &libName) { // static
  const DirList list = scandir(childName(libName, _T("*.*")), SELECTFILE);
  for(size_t i = 0; i < list.size(); i++) {
    UNLINK(childName(libName, list[i].name));
  }
  RMDIR(libName);
}

// list can be NULL. return number of elements in lib, matching argv
size_t DirMapLib::getInfoList(Array<FileInfo> *list) const {
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

size_t DirMapLib::getUnpackCount() const {
  return isMatchAll() ? m_guidMap->size() : getInfoList(NULL);
}
