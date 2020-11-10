#include "stdafx.h"
#include "GrammarParser.h"

GrammarParser::GrammarParser(const String &fileName, Grammar &g)
: m_lex(fileName)
, m_grammar(g)
{
  m_currentPrecedence = 0;
  m_grammar.addTerminal(_T("EOI"), TERMINAL, m_currentPrecedence, SourcePosition(0, 0)); /* always make EOI symbol no. 0, see skeletonparser parser.cpp */
  m_grammar.setName(fileName);
}

void GrammarParser::readGrammar() {
  m_token = next();
  int section = 0;
  while(m_token != PERCENTPERCENT && m_token != EOI) {
    switch(m_token) {
    case PERCENTLCURL:
      if(section > 1) {
        m_lex.error(_T("Only 2 sections allowed."));
      }
      parseHeadBody((section == 0) ? m_grammar.m_header : m_grammar.m_driverHead);
      section++;
      break;

    case TERM        :
    case LEFT        :
    case RIGHT       :
    case NONASSOC    :
      parseTermDef();
      break;

    case EOI         :
      break;

    default          :
      m_lex.error(_T("Unexpected symbol:'%s'."), m_lex.getText().cstr());
      next();
      break;
    }
  }

  if(m_token == PERCENTPERCENT) {
    next();
    while(m_token == NAME) {
      parseProduction();
    }
    if(m_token != PERCENTPERCENT) {
      m_lex.error(_T("Expected %%%%."));
    } else {
      next();
    }
    m_lex.collectBegin();
    while(m_token != EOI) {
      next();
    }
    m_lex.collectEnd();
    m_lex.getCollected(m_grammar.m_driverTail);
/*
    printf("ending text:<%s> at line %d\n",
      m_grammar.m_endingProgramText.m_sourceText.cstr(),
      m_grammar.m_endingProgramText.m_lineno);
*/
  }


  if(ok()) {
    m_grammar.addClosureProductions();
  }

  if(ok()) {
    checkGrammar();
  }
}

void GrammarParser::parseHeadBody(SourceText &source) {
  next();
  m_lex.collectBegin();
  while(m_token != PERCENTRCURL) {
    if(m_token == EOI) {
      m_lex.error(_T("Unexpected EOF."));
      return;
    }
    next();
  }
  m_lex.collectEnd();
  m_lex.getCollected(source);
  next(); // skip %}
}

void GrammarParser::parseTermDef() {
  SymbolType type;
  switch(m_token) {
  case TERM    : type = TERMINAL           ; break;
  case LEFT    : type = LEFTASSOC_TERMINAL ; m_currentPrecedence++; break;
  case RIGHT   : type = RIGHTASSOC_TERMINAL; m_currentPrecedence++; break;
  case NONASSOC: type = NONASSOC_TERMINAL  ; m_currentPrecedence++; break;
  }

  while(next() == NAME) {
    String name = m_lex.getText();
    bool ok = true;
    if(m_grammar.findSymbol(name) >= 0) {
      m_lex.error(_T("Terminal %s already defined."), name.cstr());
      ok = false;
    }
    if(ok) {
      m_grammar.addTerminal(name, type, m_currentPrecedence, m_lex.getSourcePos());
    }
  }
}

void GrammarParser::parseProduction() { // m_token == NAME (=leftside of production)
  const String leftName = m_lex.getText();
  int leftIndex = m_grammar.findSymbol(leftName);
  if(leftIndex >= 0) {
    const GrammarSymbol &sym = m_grammar.getSymbol(leftIndex);
    if(sym.m_type != NONTERMINAL) {
      m_lex.error(_T("Symbol %s is a terminal and cannot be on the leftside of a production."), leftName.cstr());
    }
  } else {
    leftIndex = m_grammar.addNonTerminal(leftName, m_lex.getSourcePos());
  }
  next();
  if(m_token != COLON) {
    m_lex.error(_T("Expected ':'."));
  } else {
    next();
  }
  for(bool rightSideDone = false; !rightSideDone;) {
    parseRightSide(leftIndex);
    switch(m_token) {
    case BAR:
      next();
      break;

    case SEMI:
      rightSideDone = true;
      next();
      break;

    case EOI:
      m_lex.error(_T("Unexpected EOI."));
      rightSideDone = true;

    default:
      m_lex.error(_T("Unexpected symbol:'%s'."), m_lex.getText().cstr());
      next();
      break;

    }
  }
}

void GrammarParser::parseRightSide(int leftSide) {
  Production prod(leftSide, m_lex.getSourcePos());
  while(m_token == NAME) {
    const String         name = m_lex.getText();
    const SourcePosition pos  = m_lex.getSourcePos();

    next();
    const SymbolModifier modifier   = parseModifier();
    int rightIndex = m_grammar.findSymbol(name);
    if(rightIndex < 0) {
      rightIndex = m_grammar.addNonTerminal(name, pos);
    } else if(m_grammar.isTerminal(rightIndex)) {
      prod.m_precedence = m_grammar.getSymbol(rightIndex).m_precedence;
    }
    prod.m_rightSide.add(RightSideSymbol(rightIndex, modifier));
  }
  if(m_token == PREC) { // %prec specifier
    next();
    switch(m_token) {
    case NUMBER:
      { prod.m_precedence = (short)m_lex.getNumber();
        next();
      }
      break;

    case NAME:
      { const String name = m_lex.getText();
        next();
        int tokenIndex = m_grammar.findSymbol(name);
        bool ok = true;
        if(tokenIndex < 0) {
          m_lex.error(_T("Unknown symbol in %%prec-clause:%s."), name.cstr());
          ok = false;
        } else if(!m_grammar.isTerminal(tokenIndex)) {
          m_lex.error(_T("Symbol %s must be terminal in %%prec-clause."), name.cstr());
          ok = false;
        }
        if(ok) {
          prod.m_precedence = m_grammar.getSymbol(tokenIndex).m_precedence;
        }
      }
      break;

    default:
      m_lex.error(_T("Expected NAME of NUMBER."));
    }
  }

  if(m_token == LCURL) {
    const SourcePosition sourcePos = m_lex.getSourcePos();
    CompactShortArray usedDollar;
    m_actionBody = EMPTYSTRING;
    m_lex.collectBegin();
    next();
    parseActionBody(sourcePos, usedDollar, prod);
    if(m_token != RCURL) {
      m_lex.error(_T("Expected '}'."));
    } else {
      next();
    }
    m_lex.collectEnd();
    SourceText tmp;
    m_lex.getCollected(tmp);
    prod.m_actionBody.m_sourceText = StringCollector::trimIndent(sourcePos, m_actionBody + tmp.m_sourceText);
    prod.m_actionBody.m_pos        = SourcePositionWithName(m_lex.getAbsoluteFileName(), sourcePos);
/*
    printf("body:<%s> at line %d\n",
      prod.m_actionBody.m_sourceText.cstr(),
      prod.m_actionBody.m_lineno);
*/
  }

  m_grammar.addProduction(prod);
}

SymbolModifier GrammarParser::parseModifier() {
  switch(m_token) {
  case QUEST: next(); return ZEROORONE;
  case STAR : next(); return ZEROORMANY;
  case PLUS : next(); return ONEORMANY;
  default   : return NO_MODIFIER;
  }
}

void GrammarParser::parseActionBody(const SourcePosition &sourcePos, CompactShortArray &usedDollar, const Production &prod) {
  for(;;) {
    switch(m_token) {
    case LCURL:
      next();
      parseActionBody(sourcePos, usedDollar, prod);
      if(m_token != RCURL) {
        m_lex.error(_T("Expected '}'."));
      } else {
        next();
      }
      break;

    case RCURL:
      return;

    case EOI:
      m_lex.error(sourcePos, _T("This codeblock has no end."));
      return;

    case DOLLARDOLLAR:
      { SourceText tmp;
        m_lex.getCollected(tmp);
        m_lex.collectEnd();
        next();
        m_actionBody += tmp.m_sourceText + _T("m_leftSide ");
        m_lex.collectBegin();
      }
      break;

    case DOLLAR:
      { SourceText tmp;
        m_lex.getCollected(tmp);
        m_lex.collectEnd();
        next();
        if(m_token != NUMBER) {
          m_lex.error(_T("Expected number."));
          m_actionBody += _T("$ ");
        } else {
          const int prodLength  = prod.getLength();
          const int symbolIndex = (int)m_lex.getNumber();
          const int fromTop = prodLength - symbolIndex;
          if(symbolIndex < 1 || symbolIndex > prodLength) {
            m_lex.warning(m_lex.getSourcePos(), _T("$%d is not in range [1..%d]."), symbolIndex, prodLength);
            m_grammar.m_warningCount++;
          } else {
            if(m_grammar.isTerminal(prod.m_rightSide[symbolIndex-1])) {
              m_lex.warning(m_lex.getSourcePos(), _T("$%d is a terminal."), symbolIndex);
              m_grammar.m_warningCount++;
            }
          }
          next();
          m_actionBody += tmp.m_sourceText + format(_T("getStackTop(%d)"), fromTop);
        }

        m_lex.collectBegin();
      }
      break;

    default:
      next();
      break;

    }
  }
}

void GrammarParser::checkGrammar() {
  for(UINT i = 1; i < m_grammar.getSymbolCount(); i++) { // dont check EOI
    const GrammarSymbol &sym = m_grammar.getSymbol(i);
    if(sym.m_type == NONTERMINAL) {
      checkNonTerminal(i);
    } else {
      checkTerminal(i);
    }
  }
  if(m_grammar.getProductionCount() == 0) {
    m_lex.error(_T("No productions in grammar."));
  } else {
    checkDuplicateProd();
    checkReachability();
    checkTermination();
  }
}

void GrammarParser::checkNonTerminal(UINT nt) { // check, that nonterminal occurs on both a left- and a rightside
  if(nt == m_grammar.getStartSymbol()) {
    checkStartSymbol();
  } else {
    bool leftsideFound  = false;
    bool rightsideFound = false;

    for(UINT i = 0; i < m_grammar.getProductionCount(); i++) {
      const Production &prod = m_grammar.getProduction(i);
      if(prod.m_leftSide == nt) {
        leftsideFound = true;
      }
      if(!rightsideFound) {
        for(UINT j = 0; j < prod.getLength(); j++) {
          if(prod.m_rightSide[j] == nt) {
            rightsideFound = true;
            break;
          }
        }
      }
      if(leftsideFound && rightsideFound) {
        return;
      }
    }
    const GrammarSymbol &nonterminal = m_grammar.getSymbol(nt);
    if(!leftsideFound) {
      m_lex.error(nonterminal.m_pos, _T("Nonterminal %s is not found on the leftside of any production."), nonterminal.m_name.cstr());
    }
    if(!rightsideFound) {
      m_lex.error(nonterminal.m_pos, _T("Nonterminal %s is not found on the rightside of any production."), nonterminal.m_name.cstr());
    }
  }
}

void GrammarParser::checkStartSymbol() { // check, that startsymbol exists on only one leftside, and no rightsides
  UINT countStartProd = 0;
  for(UINT i = 0; i < m_grammar.getProductionCount(); i++) {
    const Production &prod = m_grammar.getProduction(i);
    if(prod.m_leftSide == m_grammar.getStartSymbol()) {
      countStartProd++;
      if(countStartProd > 1) {
        m_lex.error(prod.m_pos, _T("Only 1 startproduction allowed."));
      }
    }
    for(UINT j = 0; j < prod.getLength(); j++) {
      if(prod.m_rightSide[j] == m_grammar.getStartSymbol()) {
        m_lex.error(prod.m_pos, _T("Startsymbol not allowed on rightside of production."));
      }
    }
  }
  if(countStartProd < 1) {
    m_lex.error(_T("No startproduction specified."));
  }
}

void GrammarParser::checkTerminal(UINT t) { // check, that terminal-symbol is used in at least one rightside
  for(UINT i = 0; i < m_grammar.getProductionCount(); i++) {
    const Production &prod = m_grammar.getProduction(i);
    for(UINT j = 0; j < prod.getLength(); j++) {
      if(prod.m_rightSide[j] == t) {
        return; // found
      }
    }
  }
  if(Options::getInstance().m_verboseLevel >= 2) {
    const GrammarSymbol &terminal = m_grammar.getSymbol(t);
    m_lex.warning(terminal.m_pos, _T("Terminal %s is not used."), terminal.m_name.cstr());
  }
  m_grammar.m_warningCount++;
}

void GrammarParser::checkDuplicateProd() {
  for(UINT nt = m_grammar.getTerminalCount(); nt < m_grammar.getSymbolCount(); nt++) { // for all nonterminals
    const CompactUIntArray &prodlist  = m_grammar.getSymbol(nt).m_leftSideOf;
    const UINT              prodCount = (UINT)prodlist.size();
    for(UINT i = 0; i < prodCount; i++) {
      const Production &prodi = m_grammar.getProduction(prodlist[i]);
      for(UINT j = 0; j < i; j++) {
        const Production &prodj = m_grammar.getProduction(prodlist[j]);
        if(prodi.m_rightSide == prodj.m_rightSide) {
          m_lex.error(prodi.m_pos, _T("This production equals another."));
        }
      }
    }
  }
}

void GrammarParser::checkReachability() {
  m_grammar.findReachable();
  for(UINT i = m_grammar.getTerminalCount(); i < m_grammar.getSymbolCount(); i++) {
    const GrammarSymbol &sym = m_grammar.getSymbol(i);
    if(!sym.m_reachable) {
      m_lex.error(sym.m_pos, _T("%s is not reachable from startsymbol."), sym.m_name.cstr());
    }
  }
}

void GrammarParser::checkTermination() {
  m_grammar.findTerminate();
  for(UINT i = 0; i < m_grammar.getSymbolCount(); i++) {
    const GrammarSymbol &sym = m_grammar.getSymbol(i);
    if(!sym.m_terminate) {
      m_lex.error(sym.m_pos, _T("Nonterminal %s does not terminate."), sym.m_name.cstr());
    }
  }
}
