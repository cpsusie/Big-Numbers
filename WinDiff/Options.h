#pragma once

#include "FileFormat.h"
#include "RegexFilter.h"

class Options {
private:
  static const TCHAR *rootPath;
  static String getKeyName(int index);
  String getKeyName() const;
  static int getNameToIndex(const String &name);
  static int getNextIndex();
  static RegistryKey getOptionsRootKey();
  static RegistryKey getOptionsSubKey(const String &name);
  static LOGFONT     getDefaultLogFont();
  static LOGFONT     logFontFromString(const String &str);
  static String      logFontToString(const LOGFONT &lf);
  void setDefault();

public:
  int          m_index;
  String       m_name;
  bool         m_ignoreWhiteSpace;
  bool         m_ignoreCase;
  bool         m_ignoreStrings;
  bool         m_ignoreComments;
  bool         m_ignoreColumns;
  bool         m_ignoreRegex;
  bool         m_stripComments;
  bool         m_viewWhiteSpace;
  int          m_tabSize;
  int          m_defaultExtensionIndex;
  LOGFONT      m_logFont;
  int          m_nameFontSizePct;
  bool         m_show1000Separator;
  CSize        m_windowSize;
  FileFormat   m_fileFormat;
  RegexFilter  m_regexFilter;

  static RegistryKey getRootKey();
  static RegistryKey getSubKey(const String &name) {
    return getRootKey().createOrOpenKey(name);
  }
  static const TCHAR *getRootPath() {
    return rootPath;
  }

  Options();
  bool                  save();
  bool                  load(const String &name);
  bool                  load(int index);
  static bool           remove(const String &name);
  static bool           remove(int index);
  static void           saveData(RegistryKey &key, const Options &options);
  static void           loadData(RegistryKey &key,       Options &options);
  static StringArray    getExistingNames();
  void                  setAsDefault();
  static String         getDefaultOptionName();
  static void           setDefaultOptionName(const String &name);
  static Array<Options> getExisting();
  bool                  validate() const;
  static Options        getDefaultOptions();
};

class OptionsAccessor {
public:
  const Options &getOptions() const;
};

class OptionsUpdater : public OptionsAccessor {
public:
  Options &getOptions();
};

class OptionsEditData {
public:
  const int    m_originalIndex;
  String       m_name;
  OptionsEditData(const String &name, int index);
};

class OptionList : public Array<OptionsEditData> {
private:
  void sortByNewIndex();
public:
  OptionList();
  bool validate(int &errorLine) const;
  bool save();
  bool move(  UINT index, bool up);
  bool remove(UINT index);
};
