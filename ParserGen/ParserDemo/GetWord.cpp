#include "StdAfx.h"
#include "TextBox.h"

static bool isIdentifierChar(BYTE ch) {
  return _istalpha(ch) || ch == '_';
}

class CharacterClass {
private:
  BYTE m_charClass[256];
public:
  CharacterClass();
  friend String getWord(const TCHAR *s, int pos);
};

CharacterClass::CharacterClass() {
  for(int i = 0; i < ARRAYSIZE(m_charClass); i++) {
    m_charClass[i] = 0;
  }

  for(int i = 0; i < ARRAYSIZE(m_charClass); i++) {
    if(isIdentifierChar(i)) {
      m_charClass[i] = 1;
    }
  }
  for(int i = 0; i < ARRAYSIZE(m_charClass); i++) {
    if(_istdigit(i)) {
      m_charClass[i] = 2;
    }
  }
  m_charClass['#'] = 3;
}


String getWord(const TCHAR *s, int pos) {
  static CharacterClass charClass;
  String tmp(s);
  int start = pos, end = pos;
  _TUCHAR ch = s[pos];
  _TUCHAR chClass = charClass.m_charClass[ch];

  if(_istspace(ch)) {
    return EMPTYSTRING;
  } else {
    while((start > 0) && (charClass.m_charClass[(BYTE)(s[start-1])] == chClass)) {
      start--;
    }
    while(charClass.m_charClass[(BYTE)(s[end+1])] == chClass) {
      end++;
    }
  }
  return substr(tmp, start, end-start+1);
}
