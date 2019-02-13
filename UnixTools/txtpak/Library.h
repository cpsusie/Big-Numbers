#pragma once

#include <WildCardRegex.h>
#include "GuidNameMap.h"

typedef enum {
  LT_UNKNOWN
 ,LT_TEXTFILE
 ,LT_DIRWITHMAP
} LibType;

typedef enum {
  READ_MODE
 ,WRITE_MODE
 ,UPDATE_MODE
} OpenMode;

class LibraryContent : public StringIHashMap<FileInfo> {
public:
  LibraryContent(const Array<FileInfo> &list);
};

class Library {
private:
  const String  m_libName;
  LibType       m_libType;
  FILE         *m_libFile;
  GuidNameMap  *m_guidMap;
  WildCardRegex m_wc;
  bool          m_setTimestamp, m_setMode, m_verbose, m_matchAll;

  void   openLib(OpenMode mode, bool checkSubDirCount = true);
  void   closeLib();

  void   closeFile(FILE * &f, const FileInfo &info);
  LibraryContent getAllContent();
  // if content == NULL, all files in names-array will be added. if != NULL, names contained in content will be skipped.
  // Return number of added files
  size_t packToTextFile(const StringArray &names, LibraryContent *content);
  void   unpackFromTextFile();
  bool   checkIntegrityTextFile(const Array<FileInfo> &list);
  size_t getInfoListFromTextFile(Array<FileInfo> *list) const; // list can be NULL. return number of elements in lib, matching argv
  size_t getUnpackCountTextFile() const;
  void   addFileToTextFile(const String &name);

  // if content == NULL, all files in names-array will be added. if != NULL, names contained in content will be skipped.
  // Return number of added files
  size_t packToDir(const StringArray &names, LibraryContent *content);
  void   unpackFromDir();
  bool   checkIntegrityFromDir(const Array<FileInfo> &list);
  size_t getInfoListFromDir(Array<FileInfo> *list) const;  // list can be NULL. return number of elements in lib, matching argv
  size_t getUnpackCountDir() const;
  void   addFileToDir(const String &name);

  void libTypeError(const TCHAR *method) const {
    throwException(_T("%s:Invalid libType:%d"), method, m_libType);
  }
  static String  createMapFileName(const String &libName);
  static void    checkFirstLineIsInfoLine(const String &fileName);

  void prepareArgvPattern(const TCHAR **argv);
  inline bool matchArgvPattern(const String &fileName) const {
    return m_matchAll || (m_wc.search(fileName) >= 0);
  }
  void restoreTimesAndMode(const FileInfo &info);
public:
  Library(const String &libName, LibType type);
  ~Library();
  void pack(  const StringArray &names, bool verbose);
  void update(const StringArray &names, bool verbose);
  void unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose);
  void list(  const TCHAR **argv, bool sorting);
  void checkIntegrity();
  static void    removeLib(const String &libName);
  inline LibType getLibType() const {
    return m_libType;
  }
  const String &getLibName() const {
    return m_libName;
  }
  static LibType getLibType(const String &libName, bool checkSubDirCount = true);
};
