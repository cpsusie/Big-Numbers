#include "stdafx.h"
#include <FileTreeWalker.h>

class FileVersionWithName : public FileVersion {
public:
  const String m_filename;
  FileVersionWithName(const String &filename) : FileVersion(filename), m_filename(filename) {
  }
  String toString(int fnamelen = 20, bool printLanguage = false, UINT index = 0) const;
};

String FileVersionWithName::toString(int fnamelen, bool printLanguage, UINT index) const {
  String result = format(_T("%-*.*s:%-16s:%-16s:")
                        ,fnamelen, fnamelen, m_filename.cstr()
                        ,getProductVersion().cstr()
                        ,getFileVersion().cstr()
                        );
  if(index < m_fileInfo.size()) {
    if(printLanguage) {
      result += format(_T("%-20.20s:"), m_fileInfo[index].getFileInfoTranslation().getLanguage().cstr());
    }
    result += format(_T("%-50s:%s"), m_fileInfo[index].fileDescription.cstr(), m_fileInfo[index].companyName.cstr());
  }
  return result;
}

class VersionList : public Array<FileVersionWithName>, public FileNameHandler {
private:
  bool m_verbose;
  bool m_warnings;
public:
  VersionList(bool verbose, bool warnings) {
    m_verbose  = verbose;
    m_warnings = warnings;
  }
  int getMaxFileNameLength() const;
  void addVersion(const TCHAR *name);
  void handleFileName(const TCHAR *name, DirListEntry &info);
};

int VersionList::getMaxFileNameLength() const {
  size_t maxlen = 0;
  for(size_t i = 0; i < size(); i++) {
    const String &name = (*this)[i].m_filename;
    if(name.length() > maxlen) {
      maxlen = name.length();
    }
  }
  return (int)maxlen;
}

void VersionList::addVersion(const TCHAR *name) {
  try {
    add(FileVersionWithName(name));
  } catch(Exception e) {
    if(m_warnings) {
      _ftprintf(stderr, _T("%s:%s\n"), name,e.what());
    }
  }
}

void VersionList::handleFileName(const TCHAR *name, DirListEntry &info) {
  addVersion(name);
}

static void usage() {
  _ftprintf(stderr,_T("GetVersion [options] files...\n"
                      "  Option: -v:verbose\n"
                      "          -w:suppress warnings\n"
                      "          -r:recurse subdirs\n"
                      "          -l:display language\n"
                      "          -L:display ressources for all languages\n")
           );
  exit(-1);
}

int main(int argc, const char **argv) {
  const char *cp;
  bool verbose       = false;
  bool warnings      = true;
  bool printLanguage = false;
  bool alllang       = false;
  bool recurse       = false;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'r':
        recurse = true;
        continue;
      case 'v':
        verbose  = true;
        continue;
      case 'w':
        warnings = false;
        continue;
      case 'L':
        alllang       = true;
        printLanguage = true;
        continue;
      case 'l':
        printLanguage = true;
        continue;
      default :
        usage();
      }
      break;
    }
  }

  if(*argv == nullptr) {
    usage();
  }
  VersionList versions(verbose, warnings);
  FileTreeWalker::traverseArgv(argv, versions, recurse);

  if(versions.size() > 0) {
    int fnamelen = 20; // versions.getMaxFileNameLength();
    TCHAR *languageheader = printLanguage ? _T("Language            :"):EMPTYSTRING;
    _tprintf(_T("%-*.*s:%-16s:%-16s:%s%-50s:%-s\n")
            ,fnamelen,fnamelen,_T("File")
            ,_T("Productversion"), _T("FileVersion")
            ,languageheader,_T("Description"),_T("Company"));
    for(size_t i = 0; i < versions.size(); i++) {
      FileVersionWithName &v = versions[i];
      if(alllang) {
        for(UINT l = 0; l < v.m_fileInfo.size(); l++) {
          _tprintf(_T("%s\n"), v.toString(fnamelen, printLanguage, l).cstr());
        }
      } else {
        _tprintf(_T("%s\n"), v.toString(fnamelen, printLanguage, 0).cstr());
      }
    }
  }
  return 0;
}
