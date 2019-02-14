#pragma once

#include <WildCardRegex.h>
#include <HashMap.h>
#include "FileInfo.h"

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

class AbstractLib {
private:
  WildCardRegex m_wc;
  bool          m_matchAll;
  const String  m_libName;
protected:
  bool          m_setTimestamp, m_setMode, m_verbose;

  void        prepareArgvPattern(const TCHAR **argv);
  inline bool isMatchAll() const {
    return m_matchAll;
  }
  inline bool matchArgvPattern(const String &fileName) const {
    return isMatchAll() || (m_wc.search(fileName) >= 0);
  }
  void restoreTimesAndMode(const FileInfo &info);
  static void checkFirstLineIsInfoLine(const String &fileName);
  // if content == NULL, all files in names-array will be added. if != NULL, names contained in content will be skipped.
  // Return number of added files
  virtual size_t addFiles(const StringArray &names, const LibraryContent *content) = 0;
  virtual bool   checkIntegrity(const Array<FileInfo> &list)                       = 0;

public:
  inline AbstractLib(const String &libName) : m_libName(libName) {
  }
  virtual ~AbstractLib() {
  }
  virtual void           open(OpenMode mode, bool checkSubDirCount = true)                         = 0;
  virtual void           close()                                                                   = 0;
  virtual void           unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose) = 0;
  // list can be NULL. return number of elements in lib, matching argv
  virtual size_t         getInfoList(Array<FileInfo> *list) const                                  = 0;
  virtual LibType        getType() const                                                           = 0;
  
  size_t                 pack(  const StringArray &names, bool verbose);
  size_t                 update(const StringArray &names, bool verbose);
  void                   list(  const TCHAR     **argv  , bool sorting);
  bool                   checkIntegrity();

  LibraryContent         getAllContent();
  // return number of elements in lib, matching argv
  virtual size_t         getUnpackCount() const {
    return getInfoList(NULL);
  }
  const String          &getLibName() const {
    return m_libName;
  }

  static LibType getLibType(const String &libName, bool checkSubDirCount);
};
