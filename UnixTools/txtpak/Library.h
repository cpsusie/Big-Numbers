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
} OpenMode;

class Library {
private:
  const String  m_libName;
  LibType       m_libType;
  FILE         *m_libFile;
  GuidNameMap  *m_guidMap;
  WildCardRegex m_wc;
  bool          m_setTimestamp, m_setMode, m_verbose, m_matchAll;

  void   openLib(OpenMode mode);
  void   closeLib();

  void   closeFile(FILE * &f, const FileInfo &info);
  void   packToTextFile(const StringArray &names);
  void   unpackFromTextFile();
  size_t getInfoListFromTextFile(Array<FileInfo> *list) const; // list can be NULL. return number of elements in lib, matching argv
  size_t getUnpackCountTextFile() const;

  void   packToDir(const StringArray &names);
  void   unpackFromDir();
  size_t getInfoListFromDir(Array<FileInfo> *list) const;  // list can be NULL. return number of elements in lib, matching argv
  size_t getUnpackCountDir() const;

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
  void pack(const StringArray &names, bool verbose);
  void unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose);
  void list(const TCHAR **argv, bool sorting);
  static void    removeLib(const String &libName);
  inline LibType getLibType() const {
    return m_libType;
  }

  static LibType getLibType(const String &libName);
};
