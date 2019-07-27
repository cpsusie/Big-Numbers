#include "stdafx.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <Tokenizer.h>
#include <io.h>

static void checkFileName(const String &path,const String &command) {
  String fullname = FileNameSplitter::getChildName(path,command);
  if(ACCESS(fullname,0) == 0) {
    _tprintf(_T("%s\n"), fullname.cstr());
  }
}

static void commandInPath(const String &path, const String &command, const String &pathext) {
  String ext = FileNameSplitter(command).getExtension();
  if(ext.length() == 0) {
    for(Tokenizer tok(pathext, _T(";")); tok.hasNext();) {
      checkFileName(path,command + tok.next());
    }
  } else {
    checkFileName(path,command);
  }

}

static void usage() {
  _ftprintf(stderr,_T("Usage:which command\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  if(argc != 2) {
    usage();
  }
  const String command = argv[1];
  const String path    = getenv("path");
  String       pathext = getenv("pathext");
  if(pathext.length() == 0) {
    pathext = _T(".com;.exe;.bat;.cmd");
  }
  if(path.length() == 0) {
    _ftprintf(stderr, _T("No path\n"));
    exit(-1);
  }

  commandInPath(_T("."),command,pathext);
  for(Tokenizer tok(path,_T(";")); tok.hasNext();) {
    String p = tok.next();
    if(p == _T(".") || p == _T(".\\")) {
      continue;
    }
    commandInPath(p,command,pathext);
  }
  return 0;
}
