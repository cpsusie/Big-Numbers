#pragma once

#include <Array.h>

class Language {
public:
  LANGID m_langID;
  String m_langName;

  String getLanguageName() const; // strip country-name enclosed by (..)
  static const Array<Language> &getSystemLanguages();
  static const Array<Language> &getSupportedLanguages();
  static const Language        &getBestSupportedLanguage(LANGID langID);

  static void   setLanguageForThread(LANGID langID);
  static LANGID getLanguageForThread();
};


