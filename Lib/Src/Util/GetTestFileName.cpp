#include "pch.h"
#include <DebugLog.h>
#include <FileNameSplitter.h>

String getTestFileName(const String &fileName, const String &extension) {
  const String testDir = _T("c:\\temp\\TestDir");
  String fname = fileName;
  fname.replace(_T(':'), _T('_'));
  return FileNameSplitter(FileNameSplitter::getChildName(testDir, fname)).setExtension(extension).getFullPath();
}
