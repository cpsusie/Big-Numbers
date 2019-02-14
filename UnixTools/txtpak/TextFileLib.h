#pragma once

#include "AbstractLib.h"

class TextFileLib : public AbstractLib {
private:
  FILE *m_libFile;
  size_t addFiles(const StringArray &names, const LibraryContent *content);
  void   addFile(const String &name);
  void   closeFile(FILE * &f, const FileInfo &info);

public:
  TextFileLib(const String &libName) : AbstractLib(libName) {
    m_libFile = NULL;
  }
  ~TextFileLib();
  void           open(OpenMode mode, bool checkSubDirCount = true);
  void           close();
  void           unpack(const TCHAR **argv, bool setTimestamp, bool setMode, bool verbose);

  bool           checkIntegrity(const Array<FileInfo> &list);
  static void    removeLib(const String &libName);
  // list can be NULL. return number of elements in lib, matching argv
  size_t         getInfoList(Array<FileInfo> *list) const;
  LibType        getType() const {
    return LT_TEXTFILE;
  }
};
