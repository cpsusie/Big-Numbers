#pragma once

#include "Scandir.h"

class FileNameHandler {
public:
  virtual void handleFileName(const TCHAR *name, DirListEntry &entry) = 0;
  virtual void handleStartDir(const TCHAR *name) {};
  virtual void handleEndDir(  const TCHAR *name) {}
};

class FileTreeWalker {
public:
  static void walkFileTree(const TCHAR *path, FileNameHandler &nameHandler, TCHAR **argv = NULL);
  static void walkFileTree(const String    &path, FileNameHandler &nameHandler, TCHAR **argv = NULL);
  static void walkDir(     const String    &path, FileNameHandler &nameHandler, TCHAR **argv = NULL); // non recurse
  static void traverseArgv(TCHAR **argv, FileNameHandler &nameHandler, bool recurse = false);
};

