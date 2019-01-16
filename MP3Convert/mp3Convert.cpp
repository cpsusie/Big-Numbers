#include "stdafx.h"
#include <FileTreeWalker.h>
#include "MediaFile.h"

#define PROCESS_NONCONVERTED 0x01
#define PROCESS_CONVERTED    0x02
#define LIST_ALLTAGS         0x04
#define LIST_MOBILETAGS      0x08
#define VERBOSE              0x10

#define PROCESS_ALL          (PROCESS_NONCONVERTED | PROCESS_CONVERTED)
#define LISTFLAG(flag)       ((flag)&(LIST_ALLTAGS|LIST_MOBILETAGS))

class MP3FileHandler : public FileNameHandler {
private:
  const UINT m_flags;
  static bool isConvertedFileName(const String &name);
public:
  MP3FileHandler(UINT flags) : m_flags(flags) {
  }
  void handleFileName(const TCHAR *name, DirListEntry &entry);
};

bool MP3FileHandler::isConvertedFileName(const String &name) { // static
  static const String conv(_T("-converted"));
  return right(FileNameSplitter (name).getFileName(), conv.length()) == conv;
}

void MP3FileHandler::handleFileName(const TCHAR *name, DirListEntry &entry) {
  try {
    const bool isConvName = isConvertedFileName(entry.name);
    if( isConvName && ((m_flags&PROCESS_CONVERTED   )==0)) return;
    if(!isConvName && ((m_flags&PROCESS_NONCONVERTED)==0)) return;
    FileNameSplitter sp(name);
    const String fileName = sp.getFileName();
    if(m_flags & VERBOSE) {
      _ftprintf(stderr, _T("Processing %s\n"), name);
    }

    MediaFile srcFile(name);
    switch(LISTFLAG(m_flags)) {
    case LIST_ALLTAGS   :
      _tprintf(_T("%s\n%s")    , fileName.cstr(), srcFile.getTags().toStringAllTags().cstr());
      break;
    case LIST_MOBILETAGS:
      _tprintf(_T("%-30s:%s\n"), fileName.cstr(), srcFile.getTags().toStringMobileTags().cstr());
      break;
    default             :
      throwException(_T("Unknown flags combination:%02X"), m_flags);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s:%s\n"), name, e.what());
  }
}

void listTags(const TCHAR **argv, bool recurse, BYTE flags) {
  FileTreeWalker::traverseArgv(argv, MP3FileHandler(flags), recurse);
}

void copyTags(const TCHAR **argv, bool recurse) {
  throwException(_T("copyTags not impoemented yet"));
}

typedef enum {
  CMD_UNKNOWN
 ,CMD_LIST
 ,CMD_COPYTAGS
} Command;

static void usage() {
  _ftprintf(stderr,_T("usage:mp3Convert [-C|L[a]] [-r] [-p[c]]\n"
                      "      -C : Copy all tags from non-converted.mp3 to *-converted.mp3\n"
                      "      -L[a] List tags\n"
                      "         a : list All tags\n"
                      "             Default: List only mobile tags\n"
                      "      -r : recurse subdirs\n"
                      "      -p[c] : Without 'c', then process all files\n"
                      "              if 'c' specified, process only converted\n"
                      "              Default: Process only non converted filed (ie. filenames not ending with \"-converted\"\n"
                      "      -v : Verbose\n"
                     )
           );
  exit(-1);
}


int _tmain(int argc, TCHAR **argv) {
  const TCHAR *cp;
  Command      cmd     = CMD_UNKNOWN;
  bool         recurse = false;
  UINT         flags   = PROCESS_NONCONVERTED | LIST_MOBILETAGS;
#define SETCOMMAND(command)         \
{ if((cmd) != CMD_UNKNOWN) usage(); \
  cmd = command;                    \
}

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'C':
        SETCOMMAND(CMD_COPYTAGS);
        continue;
      case 'L':
        SETCOMMAND(CMD_LIST);
        switch(cp[1]) {
        case 'a':
          flags &= ~LIST_MOBILETAGS;
          flags |= LIST_ALLTAGS;
          break;
        default : continue;
        }
        break;
      case 'p':
        switch(cp[1]) {
        case 'c':
          flags &= ~PROCESS_NONCONVERTED;
          flags |= PROCESS_CONVERTED;
          break;
        default :
          flags |= PROCESS_ALL;
          break;
        }
        break;
      case 'v':
        flags |= VERBOSE;
        continue;
      case 'r':
        recurse = true;
        continue;
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
    case CMD_LIST    :
      listTags((const TCHAR**)argv, recurse, flags);
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
