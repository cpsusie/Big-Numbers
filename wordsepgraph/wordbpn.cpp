#include "stdafx.h"
#include "WordBpn.h"

#define NLETTERS       30 /*  'A' .. 'Z','[','|',']',' ' */
#define SEP_LIMIT      0.5
#define PERCEPFILE     _T("ORDDELER")

#define INPUTCOUNT (NLETTERS*SEPARATE_WINSIZE)
#define HIDDENCOUNT 12
#define OUTPUTCOUNT 1

#define MAXWORDLEN     512

CompactIntArray WordBpn::getLayerUnits() { // static
  CompactIntArray units;
  units.add(INPUTCOUNT);
  units.add(HIDDENCOUNT);
  units.add(OUTPUTCOUNT);
  return units;
}

WordBpn::WordBpn() : Bpn(getLayerUnits(),true) {
  m_updateAlways = true;
}

void WordBpn::load() {
  Bpn::load(PERCEPFILE);
}

void WordBpn::save() {
  Bpn::save(PERCEPFILE);
}

LetterMap::LetterMap() {
  int i;
  for(i =  0 ; i <  256; i++) map[i] = NLETTERS;
  for(i = 'A'; i <= ']'; i++) map[i] = i  - 'A';
  for(i = 'a'; i <= '}'; i++) map[i] = i  - 'a';

  map[0x91] = map[0x92] = '['  - 'A';
  map[0x9d] = map[0x9b] = '\\' - 'A';
  map[0x8f] = map[0x86] = ']'  - 'A';

  map[(BYTE)'æ'] = map[(BYTE)'Æ'] = '['  - 'A';
  map[(BYTE)'ø'] = map[(BYTE)'Ø'] = '\\' - 'A';
  map[(BYTE)'å'] = map[(BYTE)'Å'] = ']'  - 'A';
  map[' '] = NLETTERS - 1;
}

static const double    targetPattern[] = { 0.1, 0.9 };

const LetterMap WordBpnInputPattern::s_charMap;

WordBpnInputPattern::WordBpnInputPattern(const WordWindow &ww) : CompactDoubleArray(INPUTCOUNT) {
  for(int j = 0; j < SEPARATE_WINSIZE; j++) {
    setInputToLetter(j,ww.m_window[j]);
  }
}

void WordBpnInputPattern::setInputToLetter(int n, BYTE ch) {
  int p;

  for(int i = 0; i < NLETTERS; i++) {
    add(0);
  }
  if((p = s_charMap[ch] ) < NLETTERS ) {
    (*this)[n * NLETTERS + p] = 1;
  } else {
    throwException(isprint(ch) ? _T("Illegal character '%c'"):_T("Illegal character #%-2d"), ch);
  }
}

bool WordBpn::learnWordWindow(const WordWindow &ww) {
  CompactDoubleArray  result;
  recognize(WordBpnInputPattern(ww), result);
  bool netoutput = result[0] > SEP_LIMIT;
  if(m_updateAlways || netoutput != ww.m_allowSeparation) {
    CompactDoubleArray t;
    t.add(targetPattern[ww.m_allowSeparation?1:0]);
    learnLast(t);
  }
  return netoutput != ww.m_allowSeparation;
}

bool WordBpn::separateWordWindow(const WordWindow &ww) {
  CompactDoubleArray  result;
  recognize(WordBpnInputPattern(ww), result);
  m_result = (float)result[0];
  return m_result > SEP_LIMIT;
}

WordWindow::WordWindow(const String &word, bool allowseparation) {
  memset(m_window, 0, sizeof(m_window));
  USES_CONVERSION;
  strncpy(m_window,T2A(word.cstr()),SEPARATE_WINSIZE);
  m_allowSeparation = allowseparation;
}

String WordWindow::toString() const {
  return format(_T("[%s-%s]")
               ,left( m_window,SEPARATE_POINT).cstr()
               ,right(m_window,SEPARATE_POINT).cstr()
               );
}

WordWindowList::WordWindowList(const String &word) {
  int          len  = 0;
  bool         allowseparation[MAXWORDLEN]; // can we separate after word[i]
  String       wordStripped;
  const TCHAR *srcp = word.cstr();

  for(;*srcp; srcp++) { // find/remove dashes
    if(*srcp == '-') {
      allowseparation[len] = true;
    } else {
      allowseparation[++len] = false;
      wordStripped += *srcp;
    }
  }
  for(int i = 1; i < len; i++) {
    String wordwin;
    for(int j = 0, l = i - SEPARATE_POINT; j < SEPARATE_WINSIZE; j++, l++) {
      wordwin += ((l < 0) || (l >= len)) ? ' ' : wordStripped[l];
    }
    add(WordWindow(wordwin,allowseparation[i]));
  }
}

void WordBpn::learnWord(const String &word) {
  WordWindowList wwl(word);
  m_errorCount = 0;
  bool learnforward = (randInt() % 2) == 1;
  if(learnforward) {
    for(size_t i = 0; i < wwl.size(); i++) {
      if(learnWordWindow(wwl[i])) {
        m_errorCount++;
      }
    }
  } else {
    for(intptr_t i = wwl.size(); i-- >= 0;) {
      if(learnWordWindow(wwl[i])) {
        m_errorCount++;
      }
    }
  }
}

String WordBpn::separateWord(const String &word) {
  bool allowseparation[MAXWORDLEN]; // can we separate after word[i]
  size_t i;
  WordWindowList wwl(word);
  allowseparation[0] = false;
  for(i = 0; i < wwl.size(); i++) {
    allowseparation[i+1] = separateWordWindow(wwl[i]);
  }
  String result;
  assert(wwl.size() == word.length()-1);
  for(i = 0; i <= wwl.size();) {
    if(allowseparation[i]) {
      result += _T("-");
    }
    result += word[i++];
  }
  return result;
}

void WordBpn::setParameters(const TrainingParameters &params) {
  setLearningRate(params.m_learningRate);
  setMomentum(params.m_momentum);
  m_updateAlways = params.m_updateAlways;
}

TrainingParameters WordBpn::getParameters() {
  TrainingParameters params;
  params.m_learningRate = getLearningRate();
  params.m_momentum     = getMomentum();
  params.m_updateAlways = m_updateAlways;
  return params;
}
