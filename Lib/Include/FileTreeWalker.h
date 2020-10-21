#pragma once

#include "StringArray.h"
#include "Scandir.h"

class FileNameHandler {
public:
  virtual void handleFileName(const TCHAR *name, DirListEntry &entry) = 0;
  virtual void handleStartDir(const TCHAR *name) {};
  virtual void handleEndDir(  const TCHAR *name) {}
};

class FileTreeWalker {

public:
  static void walkFileTree(const TCHAR      *path, FileNameHandler &nameHandler, TCHAR **argv = nullptr);
  static void walkFileTree(const String     &path, FileNameHandler &nameHandler, TCHAR **argv = nullptr);
  static void walkDir(     const String     &path, FileNameHandler &nameHandler, TCHAR **argv = nullptr); // non recurse
  static void traverseArgv(const char      **argv, FileNameHandler &nameHandler, bool recurse = false);
  static void traverseArgv(const wchar_t   **argv, FileNameHandler &nameHandler, bool recurse = false);
  static void traverseArgv(const StringArray &a  , FileNameHandler &nameHandler, bool recurse = false);
};

