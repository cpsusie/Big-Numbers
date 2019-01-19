#include "stdafx.h"
#include <FileTreeWalker.h>
#include <ByteMemoryStream.h>
#include "MediaFile.h"

#define PROCESS_NONCONVERTED             0x01
#define PROCESS_CONVERTED                0x02
#define LIST_ALLTAGS                     0x04
#define LIST_MOBILETAGS                  0x08
#define LIST_SORT                        0x10
#define LIST_QUOTED                      0x20
#define VERBOSE                          0x40

#define PROCESS_ALL          (PROCESS_NONCONVERTED | PROCESS_CONVERTED)
#define LISTFLAG(flag)       ((flag)&(LIST_ALLTAGS|LIST_MOBILETAGS))


class MP3FileHandler {
private:
  const UINT       m_flags;
  Array<MediaFile> m_mediaFileArray;
  static bool isConvertedFileName(const String &name);
public:
  MP3FileHandler(UINT flags) : m_flags(flags) {
  }
  void handleFileName(const String &name);
  inline const Array<MediaFile> &getMediaFileArray() const {
    return m_mediaFileArray;
  }
};

bool MP3FileHandler::isConvertedFileName(const String &fileName) { // static
  static const String conv(_T("-converted"));
  return right(fileName, conv.length()) == conv;
}

void MP3FileHandler::handleFileName(const String &name) {
  try {
    FileNameSplitter sp(name);
    const String fileName = sp.getFileName();
    const bool isConvName = isConvertedFileName(fileName);
    if( isConvName && ((m_flags&PROCESS_CONVERTED   )==0)) return;
    if(!isConvName && ((m_flags&PROCESS_NONCONVERTED)==0)) return;
    if(m_flags & VERBOSE) {
      _ftprintf(stderr, _T("Processing %s                               \r"), name.cstr());
    }
    m_mediaFileArray.add(MediaFile(name));
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s:%s\n"), name.cstr(), e.what());
  }
}

class MediaCollection {
private:
  MP3FileHandler m_handler;
public:
  MediaCollection(const StringArray &fileNames, BYTE flags);
  inline const Array<MediaFile> &getMediaFileArray() const {
    return m_handler.getMediaFileArray();
  }
};

MediaCollection::MediaCollection(const StringArray &fileNames, BYTE flags) : m_handler(flags) {
  const size_t n = fileNames.size();
  for(size_t i = 0; i < n; i++) {
    m_handler.handleFileName(fileNames[i]);
  }
}

static void listTags(const StringArray &fileNames, BYTE flags) {
  const MediaCollection   mc(fileNames, flags);
  const Array<MediaFile> &a = mc.getMediaFileArray();

  switch(LISTFLAG(flags)) {
  case LIST_ALLTAGS   :
    { const size_t n = a.size();
      for(size_t i = 0; i < n; i++) {
        const MediaFile &mf = a[i];
        _tprintf(_T("%s\n%s"), mf.getSourceURL().cstr(), mf.getTags().toString().cstr());
      }
    }
    break;
  case LIST_MOBILETAGS:
    { const size_t n = a.size();
      Array<MobileMediaFile> mma(n);
      for(size_t i = 0; i < n; i++) {
        mma.add(MobileMediaFile(a[i]));
      }
      if(flags&LIST_SORT) {
        mma.sort(mobileMediaFileCmp);
      }
      const bool addQuotes = (flags&LIST_QUOTED) != 0;
      for(size_t i = 0; i < n; i++) {
        const MobileMediaFile &mmf = mma[i];
        _tprintf(_T("%-30s:%s\n"), mmf.getSourceURL().cstr(), mmf.toString(addQuotes).cstr());
      }
    }
    break;
    default             :
      throwException(_T("Unknown flags combination:%02X"), flags);
    }
}

static void copyTags(const StringArray &fileNames) {
  throwException(_T("copyTags not implemented yet"));
}

static void extractImages(const StringArray &fileNames, UINT flags) {
  const MediaCollection   mc(fileNames, flags);
  const Array<MediaFile> &a = mc.getMediaFileArray();
  const size_t n = a.size();
  for(size_t i = 0; i < n; i++) {
    const MediaFile &mf = a[i];
    const ByteArray *binData = mf.getTags().getFrameBinary(ID3FID_PICTURE);
    if(binData && (binData->size() > 40)) {
      try {
        CPicture pic;
        pic.load(ByteMemoryInputStream(*binData));
        if(pic.isLoaded()) {
          const String imageFilename = FileNameSplitter(mf.getSourceURL()).setDir(_T("images")).setExtension(_T("bmp")).getFullPath();
          FILE *f = MKFOPEN(imageFilename, _T("w"));
          fclose(f);
          pic.saveAsBitmap(imageFilename);
        }
      } catch(Exception e) {
        _ftprintf(stderr, _T("%s:%s\n"), mf.getSourceURL().cstr(), e.what());
      }
    }
  }
}

typedef enum {
  CMD_UNKNOWN
 ,CMD_LIST
 ,CMD_COPYTAGS
 ,CMD_EXTRACTIMAGE
} Command;

inline String childName(const String &path, const String &fname) {
  return (path == _T(".")) ? fname : FileNameSplitter::getChildName(path, fname);
}

inline DirList getAllSubDir(const String &dir) {
  return scandir(childName(dir, _T("*.*")), SELECTSUBDIR);
}

inline int alphasort(const String &s1, const String &s2) {
  return _tcsicmp(s1.cstr(), s2.cstr());
}

static void recurseSubDir(const String &dir, const String &pattern, StringArray &result) {
  DirList list = scandir(childName(dir, pattern), SELECTFILE);
  for(size_t i = 0; i < list.size(); i++) {
    result.add(childName(dir,list[i].name));
  }
  list = getAllSubDir(dir);
  for(size_t i = 0 ; i < list.size(); i++) {
    recurseSubDir(childName(dir, list[i].name), pattern, result);
  }
}

static StringArray findFiles(TCHAR **argv, bool recurse) {
  StringArray result;
  if(!recurse) {
    for(;*argv; argv++) {
      const DirList list = scandir(*argv);
      for(size_t i = 0; i < list.size(); i++) {
        result.add(list[i].name);
      }
    }
  } else {
    for(;*argv; argv++) {
      recurseSubDir(_T("."), *argv, result);
    }
    result.sort(alphasort);
  }
  return result;
}

static StringArray readFileNames(const String &fileName) {
  String line;
  StringArray result;
  FILE *input = fileName.length() == 0 ? stdin : FOPEN(fileName, _T("r"));
  while(readLine(input, line)) {
    result.add(trim(line));
  }
  if(input != stdin) {
    fclose(input);
  }
  return result;
}

static void usage() {
  _ftprintf(stderr,_T("usage:mp3Convert [-C|-L[a]|-I] [-s] [-rv] [-p[c]] [-f[file]]\n"
                      "      -C    : Copy all tags from non-converted.mp3 to *-converted.mp3\n"
                      "      -L[a] : List tags.\n"
                      "         a  : list All tags.\n"
                      "             Default: List only mobile tags.\n"
                      "      -I    : Extract image if any. Image-files are saved in subDir images, with filename = sourcefile, extension .bmp\n"
                      "      -s    : Sort list by artist,album,track,title,filename, before print to stdout. Only available for -L option.\n"
                      "      -q    : Strings are sourrounded by \"...\", and \" are escaped with \"\\\" (like C-strings).\n"
                      "      -r    : Recurse subdirs.\n"
                      "      -p[c] : Without 'c', then process all files.\n"
                      "              If 'c' specified, process only converted.\n"
                      "              Default: Process only non converted filed (ie. filenames not ending with \"-converted\".\n"
                      "      -v    : Verbose\n"
                      "      -f[file]: File contains filenames to process.\n"
                      "                If file if not specified, stdin is used. Input can be generated by grep -H or -h.\n"
                      "                Ex: grep -hr ABBA names.txt | cut -d: -f1 | mp3Convert -L -f\n"
                      "                will process all files, containing the text \"ABBA\", assuming that names.txt contains lines, with filename:.....ABBA\n"
                      )
           );
  exit(-1);
}


int _tmain(int argc, TCHAR **argv) {
  const TCHAR *cp;
  Command      cmd      = CMD_UNKNOWN;
  bool         recurse  = false;
  const TCHAR *fileName = NULL;
  UINT         flags    = PROCESS_NONCONVERTED | LIST_MOBILETAGS;

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
      case 'I':
        SETCOMMAND(CMD_EXTRACTIMAGE);
        continue;
      case 'f':
        fileName     = cp+1;
        break;
      case 's':
        flags |= LIST_SORT;
        continue;
      case 'q':
        flags |= LIST_QUOTED;
        continue;
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

    StringArray fileNames;
    if(fileName == NULL) {
      fileNames = findFiles(argv, recurse);
    } else {
      fileNames = readFileNames(fileName);
    }

    switch(cmd) {
    case CMD_COPYTAGS    :
      copyTags(fileNames);
      break;
    case CMD_LIST        :
      listTags(fileNames, flags);
      break;
    case CMD_EXTRACTIMAGE:
      extractImages(fileNames, flags);
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
