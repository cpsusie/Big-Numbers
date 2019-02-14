#pragma once

#include <HashMap.h>
#include "FileInfo.h"

// mapping from <packed name> -> FileInfo
// Packed name is generated as new guid + original files extension
class GuidNameMap : public StringHashMap<FileInfo> {
public:
  GuidNameMap() {
  }
  GuidNameMap(const String &fileName) {
    load(fileName);
  }
  void load(const String &fileName);
  void save(const String &fileName);
  void add(const String &packedName, const FileInfo &info);
};
