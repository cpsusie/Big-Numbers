#pragma once

static void myVerify(bool b, TCHAR *str) {
  if (!b) {
    Assert::IsTrue(b, str);
  }
}

#ifdef verify
#undef verify
#endif
#define verify(expr) myVerify(expr, _T(#expr))

String getTestFileName(const String &fileName, const String extension = _T("dat")) {
  const String testDir = _T("c:\\temp\\TestDir");
  String fname = fileName;
  fname.replace(_T(':'), _T('_'));
  return FileNameSplitter(FileNameSplitter::getChildName(testDir, fname)).setExtension(extension).getFullPath();
}

void OUTPUT(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  Logger::WriteMessage(msg.cstr());
}

class DebugLogRedirector {
public:
  inline DebugLogRedirector() {
    if(!isDebugLogRedirected()) redirectDebugLog(true);
  }
};

static DebugLogRedirector _redirector;
