#pragma once

#include "GrammarScanner.h"

class Grammar;

class GrammarParser {
private:
  GrammarScanner  m_lex;
  Grammar        &m_grammar;
  Token           m_token;
  String          m_actionBody;
  short           m_currentPrecedence;
  Token next() { 
    return m_token = m_lex.next();
  }
  void parseTermDef();
  void parseHeadBody(SourceText &source);
  void parseActionBody(const SourcePosition &sourcePos, CompactShortArray &usedDollar, const Production &prod);
  void parseProduction();
  void parseRightSide(int leftside);
  SymbolModifier parseModifier();

  void checkGrammar();
  void checkStartSymbol();
  void checkNonTerminal(int nonterminal);
  void checkTerminal(int terminal);
  void checkReachability();
  void checkTermination();
  void checkDuplicateProd();
public:
  GrammarParser(const String &fileName, int tabSize, Grammar &g);
  void readGrammar();
  bool ok() { return m_lex.ok(); }
};
