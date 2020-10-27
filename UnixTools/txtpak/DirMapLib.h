#pragma once

#include "AbstractLib.h"
#include "GuidNameMap.h"

class DirMapLib : public AbstractLib {
private:
  GuidNameMap  *m_guidMap;
  size_t addFiles(const StringArray &names, const LibraryContent *content);
  void   addFile(const String &name);
  static void copyFile(const String &srcName, const String &dstName);
public:
  DirMapLib(const String &libName) : AbstractLib(libName) {
    m_guidMap = nullptr;
  }
  ~DirMapLib();
  void           open(OpenMode mode, bool checkSubDirCount = true);
  void           close();
  void           unpack(const TCHAR      **argv , bool setTimestamp, bool setMode, bool verbose);

  bool           checkIntegrity(const Array<FileInfo> &list);
  static void    removeLib(const String &libName);

  // list can be nullptr. return number of elements in lib, matching argv
  size_t         getInfoList(Array<FileInfo> *list) const;
  size_t         getUnpackCount() const;
  LibType        getType() const {
    return LT_TEXTFILE;
  }
};
