#include "StdAfx.h"
#include "TextFileLib.h"

TextFileLib::~TextFileLib() {
  close();
}

#define BUFSIZE 0x10000

void TextFileLib::open(OpenMode mode, bool checkSubDirCount) {
  close();
  const String &libName = getLibName();
  switch(mode) {
  case READ_MODE:
    m_libFile = FOPEN(libName, _T("r"));
    break;
  case WRITE_MODE:
    m_libFile = FOPEN(libName, _T("w"));
    setvbuf(m_libFile, nullptr, _IOFBF, BUFSIZE);
    break;
  case UPDATE_MODE:
    m_libFile = FOPEN(libName, _T("a"));
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("mode=%d"), mode);
  }
}

void TextFileLib::close() {
  if(m_libFile != nullptr) {
    fclose(m_libFile);
    m_libFile = nullptr;
  }
}

size_t TextFileLib::addFiles(const StringArray &names, const LibraryContent *content) {
  const size_t n             = names.size();
  const String &libName      = getLibName();
  size_t        addedCounter = 0;
  for(size_t i = 0; i < n; i++) {
    try {
      const String &name = names[i];
      if(name.equalsIgnoreCase(libName)) {
        continue; // do not process library !!
      }
      if(content && (content->get(name) != nullptr)) {
        continue;
      }
      if(m_verbose) {
        printMessage(_T("(%5zu/%5zu) Packing %s"), i + 1, n, name.cstr());
      }
      addedCounter++;
      addFile(name);
    } catch(Exception e) {
      printException(e);
    }
  }
  return addedCounter;
}

void TextFileLib::unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose) {
  prepareArgvPattern(argv);
  m_setTimestamp = setTimestamp; m_setMode = setMode; m_verbose = verbose;

  const size_t totalCount = m_verbose ? getUnpackCount() : 0;
  size_t       count = 0;
  String       line;
  size_t       lineNumber = 0;
  FileInfo     info;
  FILE        *f = nullptr;
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

void TextFileLib::closeFile(FILE * &f, const FileInfo &info) {
  fclose(f);
  f = nullptr;
  restoreTimesAndMode(info);
}

bool TextFileLib::checkIntegrity(const Array<FileInfo> &list) {
  return true;
}

void TextFileLib::removeLib(const String &libName) { // static
  UNLINK(libName);
}

// list can be nullptr. return number of elements in lib, matching argv
size_t TextFileLib::getInfoList(Array<FileInfo> *list) const {
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

void TextFileLib::addFile(const String &name) {
  FILE *f = nullptr;
  try {
    const FileInfo info(name);
    f = FOPEN(name, _T("r"));
    _ftprintf(m_libFile, _T("%s\n"), info.toString().cstr());
    String line;
    while(readLine(f, line)) {
      _ftprintf(m_libFile, _T("%s\n"), line.cstr());
    }
    fclose(f); f = nullptr;
  } catch(...) {
    if(f) {
      fclose(f); f = nullptr;
    }
    throw;
  }
}
