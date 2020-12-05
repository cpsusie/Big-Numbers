#pragma once

#include "GrammarScanner.h"

class Grammar;
class Production;

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
  // Check, that startsymbol exists on only one leftside, and no rightsides
  void checkStartSymbol();
  // Check, that nonterminal occurs on both a left- and a rightside
  void checkNonTerminal(UINT nonterminal);
  // Check, that terminal-symbol is used in at least one rightside
  void checkTerminal(   UINT terminal);
  // Check, that all nonterminal-symbols are reachable from startsymbol
  void checkReachability();
  // Check, that all nonterminal-symbols terminate
  // a non-terminal A terminate iff A ->* alfa, alfa is sring of terminals with finite length
  void checkTermination();
  // Check that all productions in the grammar are unique
  void checkDuplicateProd();
public:
  GrammarParser(const String &fileName, Grammar &g);
  void readGrammar();
  inline bool ok() const {
    return m_lex.ok();
  }
};
