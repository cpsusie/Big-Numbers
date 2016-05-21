#pragma once

#include "MyString.h"

class FileNameSplitter {
private:
  TCHAR m_drive[_MAX_DRIVE];
  TCHAR m_dir[ _MAX_DIR  ];
  TCHAR m_fname[_MAX_FNAME];
  TCHAR m_ext[  _MAX_EXT  ];

public:
  FileNameSplitter(const String &fullPath);

  String getDrive() const { 
    return m_drive;
  }

  String getDir() const {
    return m_dir;
  }

  String getFileName() const {
    return m_fname;
  }

  String getExtension() const {
    return m_ext;
  }

  FileNameSplitter &setDrive(    const TCHAR *drive    );
  FileNameSplitter &setDrive(    const String    &drive    );
  FileNameSplitter &setDir(      const TCHAR *dir      );
  FileNameSplitter &setDir(      const String    &dir      );
  FileNameSplitter &setFileName( const TCHAR *fileName );
  FileNameSplitter &setFileName( const String    &fileName );
  FileNameSplitter &setExtension(const TCHAR *extension);
  FileNameSplitter &setExtension(const String    &extension);

  String getFullPath() const;
  String getAbsolutePath() const;

  static TCHAR *getChildName(  TCHAR *dst, const TCHAR *dir, const TCHAR *fileName);
  static String     getChildName(  const String &dir, const String &fileName);

};
