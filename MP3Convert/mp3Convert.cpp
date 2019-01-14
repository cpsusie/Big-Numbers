#include "stdafx.h"
#include <FileTreeWalker.h>
#include "MediaDatabase.h"

class TagsCopy : public FileNameHandler {
public:
  void handleFileName(const TCHAR *name, DirListEntry &entry);
};

void TagsCopy::handleFileName(const TCHAR *name, DirListEntry &entry) {
  try {
    FileNameSplitter sp(name);
    const String fileName = sp.getFileName();
    static const String conv(_T("-converted"));
    if(right(fileName, conv.length()) == conv) {
      return;
    }
    _tprintf(_T("Processing %s\n"), name);
    MediaFile srcFile(name);
    const String dstFileName = sp.setFileName(fileName + conv).getFullPath();
    MediaFile dstFile(dstFileName);
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s:%s\n"), name, e.what());
  }
}

void copyTags(const TCHAR **argv, bool recurse) {
  FileTreeWalker::traverseArgv(argv, TagsCopy(), recurse);
}

typedef enum {
  CMD_UNKNOWN
 ,CMD_COPYTAGS
} Command;

static void usage() {
  _ftprintf(stderr,_T("usage:mp3Convert [-C] [-r]\n"
                      "      -C : Copy all tags from non-converted.mp3 to *-converted.mp3\n"
                      "      -r : recurse subdirs\n"
                     )
           );
  exit(-1);
}


int _tmain(int argc, TCHAR **argv) {
  const TCHAR *cp;
  Command cmd     = CMD_UNKNOWN;
  bool    recurse = false;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'C': cmd = CMD_COPYTAGS; continue;
      case 'r': recurse = true;     continue;
      default : usage();
      }
      break;
    }
  }
  try {
    switch(cmd) {
    case CMD_COPYTAGS:
      copyTags((const TCHAR**)argv, recurse);
      break;
    default:
      usage();
    }
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
