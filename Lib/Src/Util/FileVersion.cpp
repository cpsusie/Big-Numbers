#include "pch.h"
#include <FileVersion.h>

String VarFileInfoTranslation::getLanguage() const {
  TCHAR langstr[100];
  if(VerLanguageName(languageID,langstr,sizeof(langstr)) == 0) {
    return _T("?");
  } else {
    return langstr;
  }
}

String StringFileInfo::getProperty(const LPVOID data, const TCHAR *propertyname) {
  TCHAR *p;
  UINT pulen;
  if(VerQueryValue(data
                  ,format(_T("\\StringFileInfo\\%04x%04x\\%s")
                         ,m_translation.languageID
                         ,m_translation.codepage
                         ,propertyname).cstr()
                  ,(void**)&p
                  ,&pulen
                  ) == 0) {
    return EMPTYSTRING;
  }
  return p;
}

StringFileInfo::StringFileInfo(const LPVOID data, VarFileInfoTranslation *t) {
  m_translation = *t;
  comments         = getProperty(data, _T("Comments")        );
  companyName      = getProperty(data, _T("CompanyName")     );
  fileDescription  = getProperty(data, _T("FileDescription") );
  fileVersion      = getProperty(data, _T("FileVersion")     );
  internalName     = getProperty(data, _T("InternalName")    );
  legalCopyright   = getProperty(data, _T("LegalCopyright")  );
  legalTradeMarks  = getProperty(data, _T("LegalTrademarks") );
  originalFileName = getProperty(data, _T("OriginalFilename"));
  privateBuild     = getProperty(data, _T("PrivateBuild")    );
  productName      = getProperty(data, _T("ProductName")     );
  productVersion   = getProperty(data, _T("ProductVersion")  );
  specialBuild     = getProperty(data, _T("SpecialBuild")    );
}

String FileVersion::getProductVersion() const {
  return format(_T("%d.%d.%d.%d")
   ,m_fixedFileInfo.dwProductVersionMS>>16, m_fixedFileInfo.dwProductVersionMS&0xffff
   ,m_fixedFileInfo.dwProductVersionLS>>16, m_fixedFileInfo.dwProductVersionLS&0xffff);
}

String FileVersion::getFileVersion() const {
  return format(_T("%d.%d.%d.%d")
   ,m_fixedFileInfo.dwFileVersionMS>>16, m_fixedFileInfo.dwFileVersionMS&0xffff
   ,m_fixedFileInfo.dwFileVersionLS>>16, m_fixedFileInfo.dwFileVersionLS&0xffff);
}

FileVersion::FileVersion(const String &filename) {
  DWORD dummyhandle;
  TCHAR filename1[256];
  _tcscpy(filename1, filename.cstr()); // take a copy. Used for windows-functions which are not const.
  int fvsize = GetFileVersionInfoSize(filename1, &dummyhandle);
  if(fvsize == 0) {
    throwLastErrorOnSysCallException(_T("GetFileVersionInfoSize"));
  }
  BYTE *data = new BYTE[fvsize];
  try {
    if(GetFileVersionInfo(filename1,dummyhandle,fvsize,data) == 0) {
      throwLastErrorOnSysCallException(_T("GetFileVersionInfo"));
    }
    void *p;
    UINT pulen;
    if(VerQueryValue(data,_T("\\"),&p,&pulen) == 0) {
      throwException(_T("No rootelement in ressource"));
    }
    m_fixedFileInfo = *(VS_FIXEDFILEINFO*)p;

    if(VerQueryValue(data,_T("\\VarFileInfo\\Translation"),&p,&pulen) == 0) {
      delete[] data;
      return;
    }
    int transcount = pulen / 4;
    VarFileInfoTranslation *trans = (VarFileInfoTranslation*)p;
    for(int i = 0; i < transcount; i++) {
      m_fileInfo.add(StringFileInfo(data,trans+i));
    }
    delete[] data;
  } catch(...) {
    delete[] data;
    throw;
  }
}

#pragma comment(lib, "version.lib")
