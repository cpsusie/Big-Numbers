#pragma once

class VarFileInfoTranslation {
public:
  LANGID         languageID;
  unsigned short codepage;
  String getLanguage() const;
};

class StringFileInfo {
private:
  String getProperty(const LPVOID data, const TCHAR *propertyname);
  VarFileInfoTranslation m_translation;
public:
  const VarFileInfoTranslation &getFileInfoTranslation() const { return m_translation; }
  String comments;
  String companyName;
  String fileDescription;
  String fileVersion;
  String internalName;
  String legalCopyright;
  String legalTradeMarks;
  String originalFileName;
  String privateBuild;
  String productName;
  String productVersion;
  String specialBuild;
  StringFileInfo(const LPVOID data, VarFileInfoTranslation *t);
};

class FileVersion {
public:
  VS_FIXEDFILEINFO      m_fixedFileInfo;
  Array<StringFileInfo> m_fileInfo;
  FileVersion(const String &filename);
  String getProductVersion() const;
  String getFileVersion() const;
};

