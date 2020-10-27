#include "stdafx.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <FileTreeWalker.h>
#include <ByteMemoryStream.h>
#include "MediaFile.h"

#define PROCESS_NONCONVERTED             0x0001
#define PROCESS_CONVERTED                0x0002
#define LIST_ALLTAGS                     0x0004
#define LIST_MOBILETAGS                  0x0008
#define LIST_SORT                        0x0010
#define LIST_QUOTED                      0x0020
#define LIST_VERTICALALIGN               0x0040
#define LIST_HEXDUMP                     0x0080
#define EXTENDNAME_WITH_CONVERTED        0x0100
#define VERBOSE                          0x0200

#define PROCESS_ALL          (PROCESS_NONCONVERTED | PROCESS_CONVERTED)
#define LISTFLAG(flag)       ((flag)&(LIST_ALLTAGS|LIST_MOBILETAGS))

static const String convStr(_T("-converted"));

static bool isConvertedFileName(const String &name) {
  const String        fileName = FileNameSplitter(name).getFileName();
  return right(fileName, convStr.length()) == convStr;
}

static bool needToProcessName(const TCHAR *stageText, const String &name, UINT flags, size_t i, size_t total) {
  const bool isConvName = isConvertedFileName(name);
  if( isConvName && ((flags&PROCESS_CONVERTED   )==0)) return false;
  if(!isConvName && ((flags&PROCESS_NONCONVERTED)==0)) return false;
  if(flags & VERBOSE) {
    _ftprintf(stderr, _T("%-10s:[%6.1lf%%] Processing %s                          \r"), stageText, PERCENT(i,total), name.cstr());
  }
  return true;
}

static String makeConvertedFileName(const String &name) {
  if(isConvertedFileName(name)) {
    return name;
  }
  FileNameSplitter sp(name);
  return sp.setFileName(sp.getFileName() +convStr).getFullPath();
}

class MediaCollection : public Array<MediaFile> {
private:
  const UINT m_flags;
public:
  MediaCollection(const StringArray &fileNames, UINT flags);
  void list() const;
  void extractImages() const;
  inline UINT getFlags() const {
    return m_flags;
  }
};

MediaCollection::MediaCollection(const StringArray &fileNames, UINT flags) : m_flags(flags) {
  const size_t n = fileNames.size();
  for(size_t i = 0; i < n; i++) {
    const String    &sourceName = fileNames[i];
    if(!needToProcessName(_T("Loading"), sourceName, m_flags, i, n)) {
      continue;
    }
    try {
      add(MediaFile(sourceName));
    } catch(Exception e) {
      _ftprintf(stderr, _T("%s:%s\n"), sourceName.cstr(), e.what());
    }
  }
}

void MediaCollection::list() const {
  const bool   hexdump = (m_flags & LIST_HEXDUMP) != 0;
  const size_t n       = size();
  for(size_t i = 0; i < n; i++) {
    const MediaFile &mmf = (*this)[i];
    _tprintf(_T("%s"), mmf.toString(hexdump).cstr());
  }
}

void MediaCollection::extractImages() const {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    const MediaFile &mf         = (*this)[i];
    const String    &sourceName = mf.getSourceURL();
    if(!needToProcessName(_T("Extracting"), sourceName, m_flags, i, n)) {
      continue;
    }
    const ByteArray *binData = mf.getTags().getFrameBinary(ID3FID_PICTURE);
    if(binData && (binData->size() > 40)) {
      try {
        CPicture pic;
        pic.load(ByteMemoryInputStream(*binData));
        if(pic.isLoaded()) {
          const String imageFilename = FileNameSplitter(sourceName).setDir(_T("images")).setExtension(_T("bmp")).getFullPath();
          FILE *f = MKFOPEN(imageFilename, _T("w"));
          fclose(f);
          pic.saveAsBitmap(imageFilename);
        }
      } catch(Exception e) {
        _ftprintf(stderr, _T("%s:%s\n"), sourceName.cstr(), e.what());
      }
    }
  }
}

class MobileMediaCollection : public Array<MobileMediaFile> {
private:
  const UINT m_flags;
  UINT       m_columnWidth[6];
  void  findColumnWidth();
public:
  MobileMediaCollection(const MediaCollection &mc);
  MobileMediaCollection(const StringArray &fileNames, UINT flags);
  // if(filename == EMPTYSTRING) read from stdin
  MobileMediaCollection(const String &fileName, UINT flags);
  void list(MobileMediaFileComparator &cmp);
  void putTags();
  void buildMusicDirTree(const String &dstDir) const;
};

MobileMediaCollection::MobileMediaCollection(const MediaCollection &mc) : m_flags(mc.getFlags()) {
  const size_t n = mc.size();
  for(size_t i = 0; i < n; i++) {
    add(MobileMediaFile(mc[i]));
  }
}

MobileMediaCollection::MobileMediaCollection(const StringArray &fileNames, UINT flags) : m_flags(flags) {
  const size_t n = fileNames.size();
  for(size_t i = 0; i < n; i++) {
    const String &sourceName = fileNames[i];
    if(!needToProcessName(_T("Loading"), sourceName, m_flags, i, n)) {
      continue;
    }
    try {
      add(MediaFile(sourceName));
    } catch(Exception e) {
      _ftprintf(stderr, _T("%s:%s\n"), sourceName.cstr(), e.what());
    }
  }
}

MobileMediaCollection::MobileMediaCollection(const String &fileName, UINT flags) : m_flags(flags) {
  FILE *input = nullptr;
  try {
    input = fileName.isEmpty() ? stdin : FOPEN(fileName, _T("r"));
    String line;
    while(readLine(input, line)) {
      add(MobileMediaFile(trim(line)));
    }
    if(input && (input != stdin)) {
      fclose(input);
      input = nullptr;
    }
  } catch(...) {
    if(input && (input != stdin)) {
      fclose(input);
      input = nullptr;
    }
    throw;
  }
}

void MobileMediaCollection::findColumnWidth() {
  memset(m_columnWidth, 0, sizeof(m_columnWidth));
  const bool   addQuotes = (m_flags&LIST_QUOTED) != 0;
  const size_t n         = size();
  for(int f = 0; f <= TAG_LASTFIELD; f++) {
    StringArray sa;
    for(size_t i = 0; i < n; i++) {
      MobileMediaFile &mmf = (*this)[i];
      sa.add(mmf.toString((MobileMediaField)f, addQuotes));
    }
    m_columnWidth[f] = (UINT)sa.maxLength();
  }
}

void MobileMediaCollection::list(MobileMediaFileComparator &cmp) {
  if(m_flags & LIST_ALLTAGS) {
    throwException(_T("Cannot list all tags from MobileMediaCollection"));
  }
  if(m_flags & LIST_SORT) {
    sort(cmp);
  }
  if(m_flags & LIST_VERTICALALIGN) {
    findColumnWidth();
  }
  const bool   addQuotes = (m_flags & LIST_QUOTED       ) != 0;
  const UINT  *cw        = (m_flags & LIST_VERTICALALIGN) ? m_columnWidth : MobileMediaFile::getDefaultColumnWidth();
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    _tprintf(_T("%s\n"), (*this)[i].toString(addQuotes,cw).cstr());
  }
}

void MobileMediaCollection::putTags() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    const MobileMediaFile &mmf        = (*this)[i];
    const String          &sourceName = mmf.getSourceURL();
    if(!needToProcessName(_T("Updating"), sourceName, m_flags, i, n)) {
      continue;
    }
    try {
      String newUrl;
      if((m_flags & EXTENDNAME_WITH_CONVERTED) && !isConvertedFileName(sourceName)) {
        newUrl = makeConvertedFileName(sourceName);
        if(ACCESS(newUrl, 0) < 0) {
          newUrl = sourceName;
        }
      } else {
        newUrl = sourceName;
      }
      MediaFile(newUrl).removeAllFrames().updateMobileFrames(mmf);
    } catch(Exception e) {
      _ftprintf(stderr, _T("%s:%s\n"), sourceName.cstr(), e.what());
    }
  }
}

static void copyFile(const String &srcName, const String &dstName) {
  ByteOutputFile dst(dstName);
  ByteInputFile  src(srcName);
  BYTE buffer[4096];
  intptr_t n;
  while((n = src.getBytes(buffer, sizeof(buffer))) > 0) {
    dst.putBytes(buffer, n);
  }
}

void MobileMediaCollection::buildMusicDirTree(const String &dstDir) const {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    const MobileMediaFile &mmf        = (*this)[i];
    const String          &sourceName = mmf.getSourceURL();
    if(!needToProcessName(_T("Copying"), sourceName, m_flags, i, n)) {
      continue;
    }
    try {
      const String pathAndName = mmf.buildPath();
      if(pathAndName.isEmpty()) {
        throwException(_T("No frames to build path"));
      }
      const String dstPath = FileNameSplitter::getChildName(dstDir, pathAndName);
      const String dstName = FileNameSplitter(dstPath).setExtension(FileNameSplitter(sourceName).getExtension()).getFullPath();
      if(ACCESS(dstName,0) == 0) {
        throwException(_T("%s already exist"), dstName.cstr());
      }
      copyFile(sourceName, dstName);
    } catch(Exception e) {
      _ftprintf(stderr, _T("%s:%s\n"), sourceName.cstr(), e.what());
    }
  }
}

typedef enum {
  CMD_UNKNOWN
 ,CMD_LIST
 ,CMD_PUTTAGS
 ,CMD_MAKETREE
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
  FILE *input = nullptr;
  try {
    input = fileName.isEmpty() ? stdin : FOPEN(fileName, _T("r"));
    String      line;
    StringArray result;
    while(readLine(input, line)) {
      result.add(trim(line));
    }
    if(input && (input != stdin)) {
      fclose(input);
      input = nullptr;
    }
    result.sort(alphasort);
    return result;
  } catch(...) {
    if(input && (input != stdin)) {
      fclose(input);
      input = nullptr;
    }
    throw;
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:mp3Convert [-L[a|A|vq]|-C|-I|-P[c]|-Tdir] [-s[fields]] [-rv] [-p[c]] [-m[textfile]|-f[textfile]|files....]\n"
                      "      -L[a|A]: List tags.\n"
                      "         a   : List All tags.\n"
                      "         A   : List All tags, wih hexdump of binary- and text fields"
                      "               Default: List only mobile tags.\n"
                      "         v   : Vertical align columns. Only aplicable without -a or -A-option.\n"
                      "         q   : Strings are sourrounded by \"...\", and \" are escaped with \"\\\" (like C-strings).\n"
                      "      -I     : Extract image if any. Image-files are saved in subDir images, with filename = sourcefile, extension .bmp\n"
                      "      -P[c]  : Put mobile-tags, read from input to corresponding mp3-file.\n"
                      "               If c-option specified, use corresponding mp3-file, with filename extended with \"-converted\", if it exist.\n"
                      "      -Tdir  : Make tree structure, where files are put in path <dir>\\<artist>\\<album>\\<title>.ext"
                      "      -s[fields]: Sort list by artist,album,track,title,filename, before print to stdout. Only available for -L option.\n"
                      "               Sort order can be changed by specifying fields:[a=artist, l=album, n=trackno, t=title, y=year, g=genre].\n"
                      "               The last field, to compare, if no other fields differ, is always fileName.\n"
                      "               Only aplicable with -L option.\n"
                      "      -r     : Recurse subdirs.\n"
                      "      -p[c]  : Without 'c', then process all files.\n"
                      "               If 'c' specified, process only converted.\n"
                      "               Default: Process only non converted filed (ie. filenames not ending with \"-converted\".\n"
                      "      -v     : Verbose\n"
                      "      -f[textfile]: File contains filenames to process.\n"
                      "               If textfile if not specified, stdin is used. Input can be generated by grep -H or -h.\n"
                      "               Ex: grep -hr ABBA names.txt | cut -d: -f1 | mp3Convert -L -f\n"
                      "               will process all files, containing the text \"ABBA\", assuming that names.txt contains lines, with filename:.....ABBA\n"
                      "      -m[textfile]: Read textfile, assumed to be generated with command mp3convert -L -q...\n"
                      "               If textfile is not specified, stdin is used.\n"
                      )
           );
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  const TCHAR              *cp;
  Command                   cmd               = CMD_UNKNOWN;
  bool                      recurse           = false;
  bool                      fileIsNamesOnly   = false;
  const TCHAR              *fileName          = nullptr;
  String                    dstDir;
  UINT                      flags             = PROCESS_NONCONVERTED | LIST_MOBILETAGS;
  MobileMediaFileComparator comparator;

#define SETCOMMAND(command)         \
{ if((cmd) != CMD_UNKNOWN) usage(); \
  cmd = command;                    \
}

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'L':
        SETCOMMAND(CMD_LIST);
        for(cp++; *cp; cp++) {
          switch(*cp) {
          case 'A':
            flags |= LIST_HEXDUMP;
            // NB! continue case
          case 'a':
            flags &= ~LIST_MOBILETAGS;
            flags |= LIST_ALLTAGS;
            continue;
          case 'v':
            flags |= LIST_VERTICALALIGN;
            continue;
          case 'q':
            flags |= LIST_QUOTED;
            continue;
          default :
            usage();
          }
          break;
        }
        break;
      case 'I':
        SETCOMMAND(CMD_EXTRACTIMAGE);
        break;
      case 'P':
        SETCOMMAND(CMD_PUTTAGS);
        for(cp++; *cp; cp++) {
          switch (*cp) {
          case 'c':
            flags |= EXTENDNAME_WITH_CONVERTED;
            continue;
          default:
            usage();
          }
          break;
        }
        break;
      case 'T':
        SETCOMMAND(CMD_MAKETREE);
        dstDir = cp+1;
        if(dstDir.isEmpty()) usage();
        break;
      case 'm':
        fileIsNamesOnly = false;
        fileName        = cp+1;
        break;
      case 'f':
        fileIsNamesOnly = true;
        fileName        = cp+1;
        break;
      case 's':
        { flags |= LIST_SORT;
          for(cp++;*cp;cp++) {
            switch(*cp) {
            case 'a': comparator.addField(TAG_ARTIST     ); break;
            case 'l': comparator.addField(TAG_ALBUM      ); break;
            case 'n': comparator.addField(TAG_TRACK      ); break;
            case 't': comparator.addField(TAG_TITLE      ); break;
            case 'y': comparator.addField(TAG_YEAR       ); break;
            case 'g': comparator.addField(TAG_CONTENTTYPE); break;
            default : usage();
            }
          }
          if(comparator.isEmpty()) comparator.setDefault();
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
      default :
        usage();
      }
      break;
    }
  }
  if((flags & LIST_ALLTAGS) && (flags & LIST_VERTICALALIGN)) {
    usage();
  }
  if((cmd != CMD_LIST) && (flags & LIST_SORT)) {
    usage();
  }

  try {
    StringArray fileNames;
    bool        allDone = false;
    if(fileName == nullptr) {
      fileNames = findFiles(argv, recurse);
    } else if(fileIsNamesOnly) {
      fileNames = readFileNames(fileName);
    } else {
      MobileMediaCollection mmc(fileName, flags);
      switch(cmd) {
      case CMD_LIST   :
        mmc.list(comparator);
        break;
      case CMD_PUTTAGS:
        mmc.putTags();
        break;
      default:
        usage();
      }
      allDone = true;
    }

    if(!allDone) {
      switch(cmd) {
      case CMD_LIST         :
        switch(LISTFLAG(flags)) {
        case LIST_ALLTAGS   :
          MediaCollection(fileNames, flags).list();
          break;
        case LIST_MOBILETAGS:
          MobileMediaCollection(fileNames, flags).list(comparator);
          break;
        default             :
          throwException(_T("Unknown flags combination:%04X"), flags);
        }
        break;
      case CMD_EXTRACTIMAGE :
        MediaCollection(fileNames, flags).extractImages();
        break;
      case CMD_MAKETREE     :
        MobileMediaCollection(fileNames, flags).buildMusicDirTree(dstDir);
        break;
      default               :
        usage();
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
