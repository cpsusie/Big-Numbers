#pragma once

#include <Bpn.h>

#define SEPARATE_WINSIZE 8
#define SEPARATE_POINT   4

class WordWindow {
public:
  char m_window[SEPARATE_WINSIZE+1];
  bool m_allowSeparation;

  WordWindow(const String &word, bool allowSeparation);
  String toString() const;
};

class WordWindowList : public Array<WordWindow> {
public:
  WordWindowList(const String &word);
};

class TrainingParameters {
public:
  float m_learningRate;
  float m_momentum;
  bool  m_updateAlways;
};

class LetterMap {
private:
  BYTE map[256];
public:
  LetterMap();
  inline BYTE operator[](BYTE ch) const {
    return map[ch];
  }
};

class WordBpnInputPattern : public CompactDoubleArray {
private:
  static const LetterMap s_charMap;

  void setInputToLetter(int n, BYTE ch);
public:
  WordBpnInputPattern(const WordWindow &ww);
};

class WordBpn : public Bpn {
private:
  static CompactIntArray getLayerUnits();
  void  setInputToLetter(WordBpnInputPattern &inputPattern, int n, BYTE ch);
  float m_result;
  bool  m_errorCount; // no of errors in the last learnword
  bool  m_updateAlways;  
public:
  WordBpn();
  bool   learnWordWindow(   const WordWindow    &ww    );
  bool   separateWordWindow(const WordWindow    &ww    );
  void   learnWord(         const String        &word  );
  String separateWord(      const String        &word  );
  void   setParameters(const TrainingParameters &params);
  TrainingParameters getParameters();
  float getNetOutput() const {
    return m_result;
  }
  void  load();
  void  save();
  int getErrorCount() const {
    return m_errorCount;
  }
};

