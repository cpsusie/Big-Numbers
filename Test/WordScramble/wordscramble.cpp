#include "stdafx.h"
#include <Tokenizer.h>

String scrambleAll(const String &s) {
  String result = s;
  for(size_t i = 0; i < result.length(); i++) {
    int index = rand() % result.length();
    TCHAR tmp = result[i];
    result[i] = result[index];
    result[index] = tmp;
  }
  return result;
}

String scramble(const String &s) {
  if(s.length() <= 5) {
    return s;
  }
  return left(s,2) + scrambleAll(substr(s,2,s.length()-4)) + right(s,2);
}

int _tmain(int argc, TCHAR **argv) {
  try {
    argv++;
    FILE *input = *argv ? FOPEN(*argv,"r") : stdin;
    String text;
    char line[256];
    while(fgets(line,sizeof(line),input)) {
      text += line;
    }
    Array<StringIndex> words;
    const String separators = _T("”\n\r\t !\"#¤%&/()=?`12@3£4$567{8[9]0}+´|¨^~'*-_.:,;\\<>½§");
    for(Tokenizer tok(text,separators); tok.hasNext();) {
      words.add(tok.nextIndex());
    }

    if(words.size() > 0 && words[0].getStart() > 0) {
      String b = left(text,words[0].getStart());
      _tprintf(_T("%s"), b.cstr());
    }
    for(size_t i = 0; i < words.size(); i++) {
      const StringIndex &index = words[i];
      String w = substr(text,index.getStart(),index.getLength());
      _tprintf(_T("%s"), scramble(w).cstr());
      if(i < words.size() - 1) {
        const size_t bstart = index.getStart()+index.getLength();
        const String b = substr(text,bstart,words[i+1].getStart()-bstart);
        _tprintf(_T("%s"), b.cstr());
      }
    }
    if(words.size() > 0) {
      const StringIndex &w = words.last();
      const size_t bstart = w.getStart()+w.getLength();
      if(bstart < text.length()) {
        const String b = right(text,text.length()-bstart);
        _tprintf(_T("%s"), b.cstr());
      }
    } else {
      _tprintf(_T("%s"), text.cstr());
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("%s\n"), e.what());
    exit(-1);
  }
  return 0;
}
