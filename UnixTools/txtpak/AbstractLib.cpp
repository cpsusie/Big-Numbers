#include "stdafx.h"
#include "AbstractLib.h"
#include "GuidNameMap.h"
#include "Util.h"

void AbstractLib::prepareArgvPattern(const TCHAR **argv) {
  if((argv == nullptr) || (*argv == nullptr)) {
    m_matchAll = true;
  } else {
    m_matchAll = false;
    m_wc.compile(argv);
  }
}

void AbstractLib::restoreTimesAndMode(const FileInfo &info) {
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

LibType AbstractLib::getLibType(const String &libName, bool checkSubDirCount) { // static
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

void AbstractLib::checkFirstLineIsInfoLine(const String &fileName) { // static
  FILE *f = FOPEN(fileName, _T("r"));
  try {
    if(!feof(f)) {
      String line;
      readLine(f, line);
      FileInfo info;
      info.extractFileInfo(line, 1);
    }
    fclose(f);
  } catch (...) {
    fclose(f);
    throw;
  }
}

LibraryContent AbstractLib::getAllContent() {
  open(READ_MODE, false);
  prepareArgvPattern(nullptr);
  Array<FileInfo> list;
  getInfoList(&list);
  close();
  return LibraryContent(list);
}

size_t AbstractLib::pack(const StringArray &names, bool verbose) {
  m_verbose = verbose;
  return addFiles(names, nullptr);
}

size_t AbstractLib::update(const StringArray &names, bool verbose) {
  m_verbose = verbose;
  const LibraryContent content = getAllContent();
  open(UPDATE_MODE);
  return addFiles(names, &content);
}

void AbstractLib::list(const TCHAR **argv, bool sorting) {
  prepareArgvPattern(argv);

  Array<FileInfo> list;
  getInfoList(&list);
  if(sorting) {
    list.sort(fileInfoCmp);
  }
  for(size_t i = 0; i < list.size(); i++) {
    const FileInfo &f = list[i];
    _tprintf(_T("%-70s %s\n"), f.m_name.cstr(), Timestamp::cctime(f.m_mtime.gettime_t()).cstr());
  }
}

bool AbstractLib::checkIntegrity() {
  prepareArgvPattern(nullptr);
  Array<FileInfo> list;
  getInfoList(&list);
  return checkIntegrity(list);
}
