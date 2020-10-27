#include "StdAfx.h"
#include "Library.h"

LibraryContent::LibraryContent(const Array<FileInfo> &list) {
  const size_t n = list.size();
  for(size_t i = 0; i < n; i++) {
    const FileInfo &info = list[i];
    put(info.m_name, info);
  }
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

Library::Library(const String &libName, LibType type)
  : m_libName(libName)
  , m_libType(type   )
  , m_lib(    nullptr   )
{
}

Library::~Library() {
  closeLib();
}

void Library::removeLib(const String &libName) { // static
  switch(getLibType(libName)) {
  case LT_TEXTFILE:
    TextFileLib::removeLib(libName);
    break;
  case LT_DIRWITHMAP:
    DirMapLib::removeLib(libName);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("libName=%s"), libName.cstr());
  }
}

void Library::openLib(OpenMode mode, bool checkSubDirCount) {
  closeLib();
  if(getLibType() == LT_UNKNOWN) {
    m_libType = getLibType(m_libName, checkSubDirCount);
  }
  switch(getLibType()) {
  case LT_TEXTFILE:
    m_lib = new TextFileLib(m_libName);
    break;
  case LT_DIRWITHMAP:
    m_lib = new DirMapLib(m_libName);
    break;
  default:
    throwException(_T("%s:Invalid libType:%d"), __TFUNCTION__, getLibType());
  }
  m_lib->open(mode, checkSubDirCount);
}

void Library::closeLib() {
  if(m_lib) {
    delete m_lib;
    m_lib = nullptr;
  }
}

void Library::pack(const StringArray &names, bool verbose) {
  openLib(WRITE_MODE);
  const size_t addCount = m_lib->pack(names, verbose);
  if(verbose) {
    printAddCountMessage(addCount);
  }
  closeLib();
}

void Library::update(const StringArray &names, bool verbose) {
  openLib(UPDATE_MODE);
  const size_t addCount = m_lib->update(names, verbose);
  if(verbose) {
    printAddCountMessage(addCount);
  }
  closeLib();
}

void Library::unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose) {
  openLib(READ_MODE);
  m_lib->unpack(argv, setTimestamp, setMode, verbose);
  closeLib();
}

void Library::list(const TCHAR **argv, bool sorting) {
  openLib(READ_MODE);
  m_lib->list(argv, sorting);
  closeLib();
}

void Library::checkIntegrity() {
  openLib(READ_MODE, false);
  bool ok = m_lib->checkIntegrity();
  closeLib();
  if(ok) {
    _tprintf(_T("All ok\n"));
  }
}

