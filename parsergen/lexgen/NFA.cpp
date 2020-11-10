#include "stdafx.h"
#include "NFA.h"

NFAstate::NFAstate(int edge) {
  m_id        = -1;
  m_edge      = edge;
  m_charClass = (edge == EDGE_CHCLASS) ? new BitSet(MAX_CHARS) : nullptr;
  m_next1     = m_next2 = nullptr;
  m_accept    = nullptr;
}

NFAstate *NFAstate::successor(int c) const {
  return (m_edge == c || (m_edge == EDGE_CHCLASS && m_charClass->contains(c))) ? m_next1 : nullptr;
}

void NFAstate::print(MarginFile &f) {
  const int oldm = f.getLeftMargin();
  f.printf(_T("NFA state %3d"), m_id);

  if(m_next1 != nullptr) {
    f.printf(_T(" -> %3d"), m_next1->m_id);
    if(m_next2) {
      f.printf(_T(" (%3d)"), m_next2->m_id);
    } else {
      f.printf(_T(" (---)"));
    }
    f.printf(_T(" on "));
    switch(m_edge) {
    case EDGE_CHCLASS :
      f.setLeftMargin(30);
      printCharClass(f, *m_charClass);
      break;
    case EDGE_EPSILON :
      f.printf(_T("EPSILON "));
      break;
    default           :
      printChar(f, m_edge);
      break;
    }
  }

//  if( nfa == start )
//      printf(_T(" (START STATE)"));

  if(m_accept != nullptr) {
    f.printf(_T(" %s"), m_accept->dumpFormat().cstr());
  }
  f.printf(_T("\n"));
  f.setLeftMargin(oldm);
}

String AcceptAction::dumpFormat() const {
  String res = format(_T("[accepting, line %3d <%s>]")
                      ,m_pos.getLineNumber()
                      ,left(trim(m_sourceText), 50).replace('\n', ' ').replace('\t', ' ').cstr());
  if(m_anchor) {
    res += format(_T(" Anchor: %s%s")
                  ,m_anchor & ANCHOR_START ? _T("start ") : EMPTYSTRING
                  ,m_anchor & ANCHOR_END   ? _T("end")    : EMPTYSTRING);
  }
  return res;
}
