#include "pch.h"
#include <MyUtil.h>
#include <FileVersion.h>
#include <Semaphore.h>
#include <Language.h>

String Language::getLanguageName() const { // strip country-name enclosed by (..)
  const int index = (int)m_langName.find('(');
  return (index >= 0) ? left(m_langName, index) : m_langName;
}

static Array<Language> *resultArray;

static BOOL CALLBACK enumLocalsCallback(LPTSTR localsString) {
  LCID lcid;

  _stscanf(localsString,_T("%x"), &lcid);

//  DWORD lcid = MAKELCID(MAKELANGID(langId,SUBLANG_DEFAULT), SORT_DEFAULT);

/*
  ERROR_INSUFFICIENT_BUFFER
  ERROR_INVALID_FLAGS
  ERROR_INVALID_PARAMETER
*/

  TCHAR languageName[1000];
  LANGID langID = MAKELANGID(lcid,SUBLANG_DEFAULT);
  if(VerLanguageName(langID, languageName, ARRAYSIZE(languageName)) != 0) {
//  if(GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, languageName, sizeof(languageName)) == 0) {
    Language language;
    language.m_langID   = langID;
    language.m_langName = languageName;
    resultArray->add(language);
  }
  return TRUE;
}


const Array<Language> &Language::getSystemLanguages() { // static
  static Array<Language> result;
  static Semaphore       gate;

  gate.wait();
  try {
    if(result.size() == 0) {
      resultArray = &result;

      EnumSystemLocales(enumLocalsCallback,LCID_INSTALLED);
    }
    gate.notify();
  } catch(...) {
    gate.notify();
    throw;
  }

  return result;
}

const Array<Language> &Language::getSupportedLanguages() { // static
  static Array<Language> result;
  static Semaphore       gate;

  gate.wait();
  try {
    if(result.size() == 0) {
      const String fileName = getModuleFileName(nullptr);
      FileVersion version(fileName);
      for(int i = 0; i < (int)version.m_fileInfo.size(); i++) {
        const VarFileInfoTranslation &vfit = version.m_fileInfo[i].getFileInfoTranslation();
        Language language;
        language.m_langID   = vfit.languageID;
        language.m_langName = vfit.getLanguage();
        result.add(language);
      }
    }
    gate.notify();
  } catch(...) {
    gate.notify();
    throw;
  }
  return result;
}

const Language &Language::getBestSupportedLanguage(LANGID langID) { // static
  int englishIndex = 0;
  const Array<Language> &supportedLanguages = getSupportedLanguages();
  for(int i = 0; i < (int)supportedLanguages.size(); i++) {
    const Language &language = supportedLanguages[i];
    if(PRIMARYLANGID(language.m_langID) == PRIMARYLANGID(langID)) {
      return language;
    }
    if(PRIMARYLANGID(language.m_langID) == LANG_ENGLISH) {
      englishIndex = i;
    }

  }
  return supportedLanguages[englishIndex];
}

void Language::setLanguageForThread(LANGID langID) {
  const Language &language = getBestSupportedLanguage(langID);
  if(!SetThreadUILanguage(language.m_langID)) {
    throwLastErrorOnSysCallException(_T("SetThreadUILanguage"));
  }
}

static void setLanguageForProcess(LANGID langID) {
  const Language &language = Language::getBestSupportedLanguage(langID);
  if(!SetThreadUILanguage(MAKELCID(language.m_langID, SORT_DEFAULT))) {
    throwException(getLastErrorText());
  }
}

LANGID Language::getLanguageForThread() { // static
  return GetThreadUILanguage();
}
