#include "StdAfx.h"
#include "GuidNameMap.h"

void GuidNameMap::load(const String &fileName) {
  FILE *f = FOPEN(fileName, _T("r"));
  try {
    clear();
    String line;
    size_t lineNumber = 0;
    while(readLine(f, line)) {
      lineNumber++;
      FileInfo info;
      info.extractFileInfo(line, lineNumber);
      String packedName;
      if(!readLine(f, packedName)) {
        libError(packedName, lineNumber);
      }
      lineNumber++;
      put(packedName, info);
    }
    fclose(f);
  } catch (...) {
    fclose(f);
    throw;
  }
}

void GuidNameMap::save(const String &fileName) {
  FILE *f = FOPEN(fileName, _T("w"));
  try {
    for(Iterator<Entry<String, FileInfo> > it = getIterator(); it.hasNext();) {
      const Entry<String, FileInfo> &e = it.next();
      const String                  &packedName = e.getKey();
      const FileInfo                &info = e.getValue();
      _ftprintf(f, _T("%s\n%s\n"), info.toString().cstr(), packedName.cstr());
    }
    fclose(f);
  } catch (...) {
    fclose(f);
    throw;
  }
}

void GuidNameMap::add(const String &packedName, const FileInfo &info) {
  if(!put(packedName, info)) {
    throwException(_T("add(\"%s\",info(%s)) failed"), packedName.cstr(), info.m_name.cstr());
  }
}
