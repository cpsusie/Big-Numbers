#include "stdafx.h"
#include "ParserTree.h"
#include "SqlSymbol.h"

String SqlParser::nl(int indent) const {
  int i = m_indent + indent;
  if(i < 0) i = 0;
  return String("\n") + spaceString(i,' ');
}

String SqlParser::sprintTerm(SyntaxNodep n) {
  if(m_noSpaceSet.contains(n->getToken()) || memberPairNoSpace(m_lastToken,n->getToken())) {
    return n->getSymbol();
  } else {
    return String(_T(" ")) + n->getSymbol();
  }
}

String SqlParser::sprintTree(SyntaxNodep n) {
  InsideMark *inside = m_inside.get(n->getToken());
  IndentSize *indent = m_indentSet.get(n->getToken());
  if(inside) inside->setInside(true);
  String res;
  if(n->isTerminal()) {
    res = sprintTerm(n);
  } else {
    if(indent) m_indent += indent->m_lmarg;
    for(unsigned int i = 0; i < n->childCount(); i++)
      res += sprintTree(n->getChild(i));
    if(indent) m_indent -= indent->m_lmarg;
  }


  if(indent) {
    switch(indent->m_type) {
    case INDENT_ALWAYS  :
      res = nl(indent->m_size) + res;
      break;
    case INDENT_WHENINSIDE  :
      { InsideMark *m = m_inside.get(indent->m_insideToken);
        if(m->m_isinside)
          res = nl(indent->m_size) + res;
        break;
      }
    case INDENT_WHENNOTINSIDE:
      { InsideMark *m = m_inside.get(indent->m_insideToken);
        if(!m->m_isinside)
          res = nl(indent->m_size) + res;
        break;
      }
    case INDENT_COUNT   :
      { InsideMark *m = m_inside.get(indent->m_insideToken);
        if(m->m_isinside && (m->m_counter % m->m_countmax) == 0) {
          res = nl(indent->m_size) + res;
        }
        m->m_counter++;
        break;
      }
    }
  }

  if(n->isTerminal())
    m_lastToken = n->getToken();

  if(inside) inside->setInside(false);
  return res;
}

String SqlParser::beautify() {
  m_indent    = -2;
  m_lastToken = -1;
  return trim(sprintTree(m_root)) + _T("\n");
}
