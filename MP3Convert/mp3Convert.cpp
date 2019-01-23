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
#define LIST_VERTICALALIGN               0x40
#define VERBOSE                          0x80

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
  MediaCollection(const StringArray &fileNames, UINT flags);
  inline const Array<MediaFile> &getMediaFileArray() const {
    return m_handler.getMediaFileArray();
  }
  void list(UINT flags) const;
};

MediaCollection::MediaCollection(const StringArray &fileNames, UINT flags) : m_handler(flags) {
  const size_t n = fileNames.size();
  for(size_t i = 0; i < n; i++) {
    m_handler.handleFileName(fileNames[i]);
  }
}

void MediaCollection::list(UINT flags) const {
  const Array<MediaFile> &a = getMediaFileArray();
  const size_t            n = a.size();
  for(size_t i = 0; i < n; i++) {
    _tprintf(_T("%s"), a[i].toString().cstr());
  }
}

class MobileMediaCollection : public Array<MobileMediaFile> {
private:
  UINT m_columnWidth[6];
  void  findColumnWidth(UINT flags);
public:
  MobileMediaCollection(const MediaCollection &mc);
  // if(filename == EMPTYSTRING) read from stdin
  MobileMediaCollection(const String &fileName);
  void list(UINT flags, MobileMediaFileComparator &cmp);
};

MobileMediaCollection::MobileMediaCollection(const MediaCollection &mc) {
  const Array<MediaFile> &a = mc.getMediaFileArray();
  const size_t            n = a.size();
  for(size_t i = 0; i < n; i++) {
    add(MobileMediaFile(a[i]));
  }
}

MobileMediaCollection::MobileMediaCollection(const String &fileName) {
  FILE *input = NULL;
  try {
    input = fileName.isEmpty() ? stdin : FOPEN(fileName, _T("r"));
    String line;
    while(readLine(input, line)) {
      add(MobileMediaFile(trim(line)));
    }
    if(input && (input != stdin)) {
      fclose(input);
      input = NULL;
    }
  } catch(...) {
    if(input && (input != stdin)) {
      fclose(input);
      input = NULL;
    }
    throw;
  }
}

void MobileMediaCollection::findColumnWidth(UINT flags) {
  memset(m_columnWidth, 0, sizeof(m_columnWidth));
  const bool   addQuotes = (flags&LIST_QUOTED) != 0;
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

void MobileMediaCollection::list(UINT flags, MobileMediaFileComparator &cmp) {
  if(flags&LIST_ALLTAGS) {
    throwException(_T("Cannot list all tags from MobileMediaCollection"));
  }
  if(flags&LIST_SORT) {
    sort(cmp);
  }
  if(flags & LIST_VERTICALALIGN) {
    findColumnWidth(flags);
  }
  const bool   addQuotes = (flags & LIST_QUOTED       ) != 0;
  const UINT  *cw        = (flags & LIST_VERTICALALIGN) ? m_columnWidth : MobileMediaFile::getDefaultColumnWidth();
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    _tprintf(_T("%s\n"), (*this)[i].toString(addQuotes,cw).cstr());
  }
}

static void listTags(const MediaCollection &mc, UINT flags, MobileMediaFileComparator &cmp) {
  switch(LISTFLAG(flags)) {
  case LIST_ALLTAGS   :
    mc.list(flags);
    break;
  case LIST_MOBILETAGS:
    { MobileMediaCollection(mc).list(flags, cmp);
      break;
    }
  default             :
    throwException(_T("Unknown flags combination:%04X"), flags);
  }
}

static void copyTags(const StringArray &fileNames) {
  throwException(_T("copyTags not implemented yet"));
}

static void extractImages(const MediaCollection &mc, UINT flags) {
  const Array<MediaFile> &a = mc.getMediaFileArray();
  const size_t            n = a.size();
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
  FILE *input = NULL;
  try {
    input = fileName.isEmpty() ? stdin : FOPEN(fileName, _T("r"));
    String      line;
    StringArray result;
    while(readLine(input, line)) {
      result.add(trim(line));
    }
    if(input && (input != stdin)) {
      fclose(input);
      input = NULL;
    }
    return result;
  } catch(...) {
    if(input && (input != stdin)) {
      fclose(input);
      input = NULL;
    }
    throw;
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:mp3Convert [-L[a|v]|-I] [-s[fields]] [-rv] [-p[c]] [-m[textfile]|-f[textfile]|files....]\n"
                      "      -L[a] : List tags.\n"
                      "         a  : list All tags.\n"
                      "              Default: List only mobile tags.\n"
                      "         v  : Vertical align columns. Only aplicable without -a-option.\n"
                      "      -I    : Extract image if any. Image-files are saved in subDir images, with filename = sourcefile, extension .bmp\n"
                      "      -s[fields]: Sort list by artist,album,track,title,filename, before print to stdout. Only available for -L option.\n"
                      "              Sort order can be changed by specifying fields:[a=artist, l=album, n=trackno, t=title, y=year, g=genre].\n"
                      "              The last field, to compare, if no other fields differ, is always fileName.\n"
                      "      -q    : Strings are sourrounded by \"...\", and \" are escaped with \"\\\" (like C-strings).\n"
                      "      -r    : Recurse subdirs.\n"
                      "      -p[c] : Without 'c', then process all files.\n"
                      "              If 'c' specified, process only converted.\n"
                      "              Default: Process only non converted filed (ie. filenames not ending with \"-converted\".\n"
                      "      -v    : Verbose\n"
                      "      -f[textfile]: File contains filenames to process.\n"
                      "              If textfile if not specified, stdin is used. Input can be generated by grep -H or -h.\n"
                      "              Ex: grep -hr ABBA names.txt | cut -d: -f1 | mp3Convert -L -f\n"
                      "              will process all files, containing the text \"ABBA\", assuming that names.txt contains lines, with filename:.....ABBA\n"
                      "      -m[textfile]: Read textfile, assumed to be generated with command mp3convert -L -q...\n"
                      "              If textfile is not specified, stdin is used.\n"
                      )
           );
  exit(-1);
}


int _tmain(int argc, TCHAR **argv) {
  const TCHAR              *cp;
  Command                   cmd               = CMD_UNKNOWN;
  bool                      recurse           = false;
  bool                      fileIsNamesOnly   = false;
  const TCHAR              *fileName          = NULL;
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
        switch(cp[1]) {
        case 'a':
          flags &= ~LIST_MOBILETAGS;
          flags |= LIST_ALLTAGS;
          break;
        case 'v':
          flags |= LIST_VERTICALALIGN;
          break;
        default : continue;
        }
        break;
      case 'I':
        SETCOMMAND(CMD_EXTRACTIMAGE);
        continue;
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
    bool        allDone = false;
    if(fileName == NULL) {
      fileNames = findFiles(argv, recurse);
    } else if(fileIsNamesOnly) {
      fileNames = readFileNames(fileName);
    } else {
      MobileMediaCollection mmc(fileName);
      switch(cmd) {
      case CMD_LIST:
        mmc.list(flags, comparator);
        break;
      default:
        usage();
      }
      allDone = true;
    }

    if(!allDone) {
      MediaCollection mc(fileNames, flags);
      switch(cmd) {
      case CMD_COPYTAGS    :
        copyTags(fileNames);
        break;
      case CMD_LIST        :
        listTags(mc, flags, comparator);
        break;
      case CMD_EXTRACTIMAGE:
        extractImages(mc, flags);
        break;
      default:
        usage();
      }
    }
  } catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
