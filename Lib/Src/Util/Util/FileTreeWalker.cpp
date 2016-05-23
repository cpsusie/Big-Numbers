#include "pch.h"
#include <WildCardRegex.h>
#include <HashMap.h>
#include <FileTreeWalker.h>

static void traverseDir(const String &path, FileNameHandler &nameHandler, WildCardRegex *filter, bool recurse) {
  DirList list = scandir(FileNameSplitter::getChildName(path,_T("*.*")),SELECTALLBUTDOT,SORTDIRLASTALPHA);

  nameHandler.handleStartDir(path.cstr());
  for(int i = 0; i < list.size(); i++) {
    DirListEntry &e = list[i];
    if(e.attrib & _A_SUBDIR) {
      if(recurse) {
        traverseDir(FileNameSplitter::getChildName(path,e.name), nameHandler, filter, recurse);
      }
    } else {
      if(filter && !filter->match(e.name)) {
        continue;
      }
      nameHandler.handleFileName(FileNameSplitter::getChildName(path,e.name).cstr(),e);
    }
  }
  nameHandler.handleEndDir(path.cstr());
}

static void handleSingleFile(const String &fileName, FileNameHandler &nameHandler) {
  DirList list = scandir(fileName,SELECTALLBUTDOT,SORTDIRLASTALPHA);
  for(int i = 0; i < list.size(); i++) {
    DirListEntry &e = list[i];
    nameHandler.handleFileName(fileName.cstr(),e);
  }
}

static void handleDir(const String &dir, FileNameHandler &nameHandler, TCHAR **argv, bool recurse) {
  if(argv == NULL) {
    traverseDir(dir, nameHandler, NULL, recurse);
  } else {
    WildCardRegex filter((const TCHAR**)argv);
#ifdef _DEBUG
    filter.dump();
#endif
    traverseDir(dir, nameHandler, &filter, recurse);
  }
}

void FileTreeWalker::walkFileTree(const TCHAR *path, FileNameHandler &nameHandler, TCHAR **argv) {    // static 
  handleDir(path, nameHandler, argv, true);
}

void FileTreeWalker::walkFileTree(const String &path, FileNameHandler &nameHandler, TCHAR **argv) {  // static 
  handleDir(path, nameHandler, argv, true);
}

void FileTreeWalker::walkDir(const String &path, FileNameHandler &nameHandler, TCHAR **argv) {      // static non recurse
  handleDir(path, nameHandler, argv, false);
}

class Argv {
private:
  StringArray m_a;
  TCHAR **m_argv;
public:
  Argv();
  Argv(const StringArray &a);
  ~Argv();
  operator TCHAR**() {
    return m_argv;
  }
  int getArgc() {
    return m_a.size();
  }
};

Argv::Argv() {
  m_argv = NULL;
}

Argv::Argv(const StringArray &a) : m_a(a) {
  if(m_a.size() == NULL) {
    m_argv = NULL;
  } else {
    m_argv = new TCHAR*[m_a.size()+1];
    int i;
    for(i = 0; i < m_a.size(); i++) {
      m_argv[i] = m_a[i].cstr();
    }
    m_argv[i] = NULL;
  }
}

Argv::~Argv() {
  if(m_argv != NULL) {
    delete[] m_argv;
  }
}

static bool hasWildCard(const String name) {
  return name.find('*') >= 0 || name.find('?') >= 0;
}

class IndexedStringArray : public StringArray {
private:
  static int m_currrentIndex;
public:
  const int    m_index;
  const String m_path;
  const bool   m_isDir;
  IndexedStringArray(const String &path, bool isDir=true) : m_index(m_currrentIndex++), m_path(path), m_isDir(isDir) {
  }
};

int IndexedStringArray::m_currrentIndex = 0;

static int compareByIndex(IndexedStringArray * const &a1, IndexedStringArray * const &a2) {
  return a1->m_index - a2->m_index;
}

void FileTreeWalker::traverseArgv(TCHAR **argv, FileNameHandler &nameHandler, bool recurse) { // static
  StringHashMap<IndexedStringArray> dirMap;
  for(;*argv; argv++) {
    String name = *argv;
    if(name[name.length()-1] == '\\') {
      name = left(name,name.length()-1);
    }
    if(hasWildCard(name)) {
      FileNameSplitter info(name);
      String path = toLowerCase(info.getDrive() + info.getDir());
      String wildCard = toLowerCase(info.getFileName() + info.getExtension());

      IndexedStringArray *wcArray = dirMap.get(path);
      if(wcArray == NULL) {
        IndexedStringArray a(path);
        a.add(wildCard);
        dirMap.put(path,a);
      } else {
        wcArray->add(wildCard);
      }
    } else { // no wildCards
      struct _stat st;
      if(stat(name,st) < 0) {
        continue;
      }
      dirMap.put(name,IndexedStringArray(name, (st.st_mode & _S_IFDIR) ? true : false));
    }
  }
  Array<IndexedStringArray*> a;
  for(Iterator<IndexedStringArray> it = dirMap.values().getIterator(); it.hasNext();) {
    a.add(&it.next());
  }

  a.sort(compareByIndex);
  for(int i = 0; i < a.size(); i++) {
    IndexedStringArray *ia = a[i];
    if(ia->m_isDir) {
      handleDir(ia->m_path, nameHandler, Argv(*ia), recurse);
    } else {
      handleSingleFile(ia->m_path, nameHandler);
    }
  }
}
