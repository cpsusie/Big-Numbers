#include "stdafx.h"
#include <Tokenizer.h>
#include <Random.h>

class WordSpace : public String {
public:
  int m_spaceCount;
  WordSpace(const String &s);
  int length() const;
};

WordSpace::WordSpace(const String &s) : String(s) {
  m_spaceCount = 1;
}

int WordSpace::length() const {
  return (int)String::length() + m_spaceCount;
}

class TextPacker {
private:
  String m_text;
  UINT   m_lmargin,m_rmargin;
  bool   m_skipNewLine;
  int    m_pos;
  void addLine(const String &s);
  void fillSpace(Array<WordSpace> &words, int firstWord, int lastWord, int l);
public:
  TextPacker();
  void addText(const String &s);
  bool setMargin(UINT l, UINT r);
  bool skipNewLine(bool skip);
  const String &getText() const {
    return m_text;
  }
};

TextPacker::TextPacker() {
  m_lmargin = 0;
  m_rmargin = 80;
  m_pos = 0;
}

bool TextPacker::setMargin(UINT l, UINT r) {
  if(l > r - 5) {
    return false;
  }
  if((l > 200) || (r > 500)) {
    return false;
  }
  m_lmargin = l;
  m_rmargin = r;
  m_pos = l;
  return true;
}

bool TextPacker::skipNewLine(bool skip) {
  bool old = m_skipNewLine;
  m_skipNewLine = skip;
  return old;
}

void TextPacker::fillSpace(Array<WordSpace> &words, int firstWord, int lastWord, int l) {
  int k = (m_rmargin - l) / (lastWord - firstWord);
  if(k >= 1) {
    for(int i = firstWord; i < lastWord; i++) {
      words[i].m_spaceCount += k;
    }
    l += k * (lastWord - firstWord);
  }

  while((UINT)l < m_rmargin) {
    CompactIntArray c;
    for(int i = firstWord; i < lastWord; i++) {
      c.add(i);
    }
    while((UINT)l < m_rmargin && c.size() > 0) { // insert spaces to fill out the whole line
      int r = randInt((UINT)c.size());
      words[c[r]].m_spaceCount++;
      l++;
      c.remove(r);
    }
  }
}

void TextPacker::addLine(const String &s) {
  Array<WordSpace> words;
  for(Tokenizer tok(s.cstr(),_T(" \t")); tok.hasNext(); ) {
    words.add(WordSpace(tok.next()));
  }
  for(UINT i = 0; i < words.size();) {
    int firstWord = i;
    // find the last word which can be placed at the line
    UINT l;
    for(l = m_lmargin; i < words.size() && l < m_rmargin;) {
      if(l + words[i].length() > m_rmargin) {
        break;
      }
      l += words[i].length();
      i++;
    }
    m_text += spaceString(m_lmargin);
    int lastWord = (i > 0) ? (i-1) : 0;
    if(lastWord <= firstWord) { // only 1 word on line
      m_text += words[firstWord];
      i++;
    } else {
      words[lastWord].m_spaceCount = 0;
      l--;
      if(i < words.size()) { // is this the last line in the section ?
        fillSpace(words,firstWord,lastWord,l);
      }
      for(int j = firstWord; j <= lastWord; j++) {
        WordSpace &w = words[j];
        m_text += w;
        m_text += spaceString(w.m_spaceCount);
      }
    }
    m_text += _T("\n");
  }
}

void TextPacker::addText(const String &s) {
  switch(m_skipNewLine) {
  case false:
    { for(Tokenizer tok(s.cstr(), _T("\n\r")); tok.hasNext();) {
        addLine(tok.next());
    }
      break;
    }
  case true:
    { String tmp = s;
      addLine(tmp.replace('\n',' ').replace('\r',EMPTYSTRING));
    }
    break;
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:TextPacker [-mleft,right] [-n] file\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool skipNewLine = false;
  int left,right;

  randomize();
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'm':
        if(sscanf(cp+1,"%d,%d",&left,&right) != 2) {
          usage();
        }
        if((left < 0) || (right < left)) {
          usage();
        }
        break;

      case 'n':
        skipNewLine = true;
        continue;

      default:
        usage();
      }
      break;
    }
  }
  try {
    if(!*argv) {
      usage();
    }
    String buffer = readTextFile(*argv);
    TextPacker pack;
    pack.setMargin(left,right);
    pack.skipNewLine(skipNewLine);
    pack.addText(buffer);
    _tprintf(_T("%s\n"),pack.getText().cstr());
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"),e.what());
    return -1;
  }
  return 0;
}
