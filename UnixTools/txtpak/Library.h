#pragma once

#include "TextFileLib.h"
#include "DirMapLib.h"

class Library {
private:
  const String  m_libName;
  LibType       m_libType;
  AbstractLib  *m_lib;

  void   openLib(OpenMode mode, bool checkSubDirCount = true);
  void   closeLib();

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
  static inline LibType getLibType(const String &libName, bool checkSubDirCount = true) {
    return AbstractLib::getLibType(libName, checkSubDirCount);
  }
};
