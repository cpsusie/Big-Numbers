#pragma once

#include <Registry.h>

String convertEscape(const String &s);
String expandEscape( const String &s);

class ColumnInterval {
public:
  UINT m_from,m_to;
  ColumnInterval() : m_from(0), m_to(0) {
  }
  ColumnInterval(int from, int to);
  ColumnInterval(const String &s);
  bool overlap(const ColumnInterval &iv) const;
  bool operator==(const ColumnInterval &ci) const;
  bool operator!=(const ColumnInterval &ci) const {
    return !(*this == ci);
  }
  String toString() const;
};

class FileFormat {
private:
  static RegistryKey getRootKey();
  String columnsToString() const;
  void   columnsFromString(const String &s);
  String concatFields(const StringArray &fields) const; // assume m_columns.size > 0
  void setDefault();
public:
  String                       m_name;
  CompactArray<ColumnInterval> m_columns;
  bool                         m_delimited;
  String                       m_delimiters;
  _TUCHAR                      m_textQualifier;
  bool                         m_multipleDelimiters;

  FileFormat();

  void addInterval(const ColumnInterval &ci);
  bool save() const;
  bool load(const String &name);
  static void saveData(RegistryKey &key, const FileFormat &param);
  static void loadData(RegistryKey &key,       FileFormat &param);
  static bool remove(const String &name);
  static StringArray getExistingNames();
  static Array<FileFormat> getExisting();
  bool validate() const;
  bool isEmpty() const {
    return m_columns.isEmpty();
  }
  void clear();
  bool operator==(const FileFormat &cp) const;
  bool operator!=(const FileFormat &cp) const {
    return !(*this == cp);
  }
  String stripColumns(const String &s) const;
};

class FileFormatEditData {
public:
  const String m_originalName;
  String       m_name;
  FileFormatEditData(const String &name);
  bool operator==(const FileFormatEditData &fd) const;
  bool operator!=(const FileFormatEditData &fd) const {
    return !(*this == fd);
  }
};

class FileFormatList : public Array<FileFormatEditData> {
public:
  FileFormatList();
  bool validate(int &errorLine) const;
  bool save();
  bool remove(UINT index);
};
