#include "stdafx.h"
#include "CStringExtractor.h"

CStringExtractor::CStringExtractor(UINT minLength, bool showNames, bool verbose)
: StringExtractor(minLength, showNames, verbose)
{
}

void CStringExtractor::extractStrings(const TCHAR *fname) {
  FILE *f = FOPEN(fname, _T("rb"));
  setvbuf(f, NULL, _IOFBF, 0x10000);

  while(nextString(f)) {
    outputString();
  }
  fclose(f);
}

/* scan input, and return true when a C-like string/char-literal is found.
   takes care of escape-sequences and old/new style C-comments
   Collects the found string in m_buffer
*/
bool CStringExtractor::nextString(FILE *input) {
  m_buffer.clear(-1);
  int state = 0;
  int ch;
  while((ch = fgetc(input)) != EOF) { 
    switch(state) {
    case 0:                           // initial state
      switch(ch) {
      case '"' : state = 1; continue;
      case '\'': state = 3; continue;
      case '/' : state = 5; continue;
      default  :            continue;
      }
      break;

    case 1:                           // inside double-quoted string literal
      switch(ch) {
      case '\\': appendChar(ch);      // escape-character inside double-quoted string literal
                 state = 2;
                 continue;
      case '"' :
      case '\n': appendChar(0);       // exit double-quoted string literal
                 return true;
                 
      default  : appendChar(ch);
                 continue;            // stay inside double-quoted string literal
      }
      break;

    case 2:                           // after \ inside double-quoted string literal
      switch(ch) {
      case '\n': appendChar(ch);
                 return true;
      default  : appendChar(ch);
                 state = 1;
                 continue;
      }
      break;

    case 3:                           // inside single-quoted string literal
      switch(ch) {
      case '\\': appendChar(ch);
                 state = 4;
                 continue;            // escape-character inside single-quoted string listeral
      case '\'': 
      case '\n': appendChar(0);
                 return true;         // exit single-quoted string literal

      default  : appendChar(ch);      // stay inside single-quoted string literal
                 continue;
      }
      break;

    case 4:                           // after \ inside a single-quoted string literal
      switch(ch) {
      case '\n': appendChar(0);
                 return true;
      default  : appendChar(ch);
                 state = 3;
                 continue;
      }
      break;

    case 5:                           // just read /
      switch(ch) {
      case '/':  state = 6; break;    // enter //... comment
      case '*':  state = 7; break;    // enter /* ... */ comment
      default :  state = 0; break;
      }
      break;

    case 6:                           // inside //.... comment
      if(ch == '\n') {                // end of comment
        state = 0;
      }
      break;

    case 7:                           // inside /* .... */ comment
      if(ch == '*') {
        state = 8;
      }
      break;

    case 8:                           // after * inside /* ... */ comment, 
      if(ch == '/') {                 // end of comment
        state = 0; 
      } else {
        state = 7;
      }
      break;

    default:
      throwException(_T("Invalid state:%d: Current input:%c\n"), state, ch);
    }
  }
  switch(state) {
  case 1:
  case 2:
  case 3:
  case 4:
    appendChar(0);
    return true;
  default:
    return false;
  }
}

void CStringExtractor::outputString() {
  if(m_buffer.size() > m_minLength) { // dont include the 0-char 
    printString((const char*)m_buffer.getData());
  }
}

