#pragma once

#include "StringArray.h"

class DirComponents : public StringArray {
private:
  const bool m_absolutePath;
public:
  DirComponents(const string &dir);
  inline bool isAbsolutePath() const {
    return m_absolutePath;
  }
  string toString() const;
};

class FileNameSplitter {
private:
  char m_drive[_MAX_DRIVE];
  char m_dir[ _MAX_DIR  ];
  char m_fname[_MAX_FNAME];
  char m_ext[  _MAX_EXT  ];

public:
  FileNameSplitter(const string &fullPath);

  inline string getDrive() const {
    return m_drive;
  }

  inline string getDir() const {
    return m_dir;
  }

  inline DirComponents getDirComponents() const {
    return DirComponents(getDir());
  }
  inline string getFileName() const {
    return m_fname;
  }

  string getExtension() const {
    return m_ext;
  }

  FileNameSplitter &setDrive(    const char         *drive     );
  FileNameSplitter &setDrive(    const string        &drive    );
  FileNameSplitter &setDir(      const char         *dir       );
  FileNameSplitter &setDir(      const string        &dir      );
  FileNameSplitter &setDir(      const DirComponents &dc       ) {
    return setDir(dc.toString());
  }
  FileNameSplitter &setFileName( const char          *fileName  );
  FileNameSplitter &setFileName( const string        &fileName  );
  FileNameSplitter &setExtension(const char          *extension );
  FileNameSplitter &setExtension(const string        &extension );

  string getFullPath()     const;
  string getAbsolutePath() const;

  static char  *getChildName(char        *dst, const char  *dir, const char *fileName);
  static string getChildName(const string &dir, const string &fileName);
};
