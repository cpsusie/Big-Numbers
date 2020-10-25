#include "pch.h"
#include <MyUtil.h>
#include <WildCardRegex.h>
#include <HashMap.h>
#include <FileNameSplitter.h>
#include <FileTreeWalker.h>

static void traverseDir(const String &path, FileNameHandler &nameHandler, WildCardRegex *filter, bool recurse) {
  DirList list = scandir(FileNameSplitter::getChildName(path,_T("*.*")),SELECTALLBUTDOT,SORTDIRLASTALPHA);

  nameHandler.handleStartDir(path.cstr());
  for(size_t i = 0; i < list.size(); i++) {
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
  for(size_t i = 0; i < list.size(); i++) {
    DirListEntry &e = list[i];
    nameHandler.handleFileName(fileName.cstr(),e);
  }
}

static void handleDir(const String &dir, FileNameHandler &nameHandler, TCHAR **argv, bool recurse) {
  if(argv == nullptr) {
    traverseDir(dir, nameHandler, nullptr, recurse);
  } else {
    WildCardRegex filter((const TCHAR**)argv);
#if defined(_DEBUG)
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

static bool hasWildCard(const String &name) {
  return name.find('*') >= 0 || name.find('?') >= 0;
}

class IndexedStringArray : public StringArray {
private:
  const int    m_index;
  const String m_path;
  const bool   m_isDir;
  mutable CompactStrArray m_argv;
public:
  IndexedStringArray(const String &path, int index, bool isDir) : m_index(index), m_path(path), m_isDir(isDir) {
  }
  inline const String &getPath() const {
    return m_path;
  }
  inline bool isDir() const {
    return m_isDir;
  }
  inline int getIndex() const {
    return m_index;
  }
  operator TCHAR**() const;
};

IndexedStringArray::operator TCHAR**() const {
  if(m_argv.isEmpty() && !isEmpty()) {
    for(size_t i = 0; i < size(); i++) {
      const TCHAR * const s = (*this)[i].cstr();
      m_argv.add((TCHAR * const)s);
    }
    m_argv.add(nullptr);
  }
  return (TCHAR**)m_argv.getBuffer();
}

static int compareByIndex(IndexedStringArray * const &a1, IndexedStringArray * const &a2) {
  return a1->getIndex() - a2->getIndex();
}

void FileTreeWalker::traverseArgv(const char **argv, FileNameHandler &nameHandler, bool recurse) { // static
  traverseArgv(StringArray(argv), nameHandler, recurse);
}

void FileTreeWalker::traverseArgv(const wchar_t **argv, FileNameHandler &nameHandler, bool recurse) { // static
  traverseArgv(StringArray(argv), nameHandler, recurse);
}

void FileTreeWalker::traverseArgv(const StringArray &a, FileNameHandler &nameHandler, bool recurse) { // static
  StringHashMap<IndexedStringArray> dirMap;
  int arrayIndex = 0;

  for(size_t i = 0; i < a.size(); i++) {
    String name = a[i];
    if(name[name.length()-1] == '\\') {
      name = left(name,name.length()-1);
    }
    if(hasWildCard(name)) {
      FileNameSplitter info(name);
      String path = toLowerCase(info.getDrive() + info.getDir());
      String wildCard = toLowerCase(info.getFileName() + info.getExtension());

      IndexedStringArray *wcArray = dirMap.get(path);
      if(wcArray == nullptr) {
        IndexedStringArray ia(path, arrayIndex++, true);
        ia.add(wildCard);
        dirMap.put(path,ia);
      } else {
        wcArray->add(wildCard);
      }
    } else { // no wildCards
      struct _stat st;
      if(stat(name,st) < 0) {
        continue;
      }
      dirMap.put(name,IndexedStringArray(name, arrayIndex++, (st.st_mode & _S_IFDIR) ? true : false));
    }
  }
  CompactArray<IndexedStringArray*> a1;
  for(auto it = dirMap.values().getIterator(); it.hasNext();) {
    a1.add(&it.next());
  }

  a1.sort(compareByIndex);
  for(size_t i = 0; i < a1.size(); i++) {
    const IndexedStringArray *ia = a1[i];
    if(ia->isDir()) {
      handleDir(ia->getPath(), nameHandler, *ia, recurse);
    } else {
      handleSingleFile(ia->getPath(), nameHandler);
    }
  }
}
