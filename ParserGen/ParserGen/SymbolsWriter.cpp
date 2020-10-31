#include "stdafx.h"
#include "GrammarCode.h"

void SymbolsWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const Grammar &grammar = m_coder.getGrammar();
  switch(grammar.getLanguage()) {
  case CPP : writeCppSymbols(writer);
             break;
  case JAVA: writeJavaSymbols(writer);
             break;
  }
}

void SymbolsWriter::writeCppSymbols(TemplateWriter &writer) const {
  const Grammar &grammar       = m_coder.getGrammar();
  const int      maxNameLength = grammar.getMaxSymbolNameLength();
  String text;
  if(m_terminals) {
    char delimiter = ' ';
    for(int s = 0; s < grammar.getTerminalCount(); s++, delimiter=',') {
      text += format(_T("%c%-*s = %3d\n"), delimiter, maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
    }
  } else {
    if(m_coder.getFlags().m_generateNonTerminals) {
      char delimiter = ' ';
      for(int s = grammar.getTerminalCount(); s < grammar.getSymbolCount(); s++, delimiter=',') {
        text += format(_T("%c%-*s = %3d\n"), delimiter, maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
      }
    }
  }
  writer.printf(_T("%s"),text.cstr());
}

void SymbolsWriter::writeJavaSymbols(TemplateWriter &writer) const {
  const Grammar &grammar       = m_coder.getGrammar();
  int            maxNameLength = grammar.getMaxSymbolNameLength();
  String         text;
  if(m_terminals) {
    for(int s = 0; s < grammar.getTerminalCount(); s++) {
      text += format(_T("public static final int %-*s = %3d;\n"), maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
    }
  } else {
    if(m_coder.getFlags().m_generateNonTerminals) {
      for(int s = grammar.getTerminalCount(); s < grammar.getSymbolCount(); s++) {
        text += format(_T("public static final int %-*s = %3d;\n"), maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
      }
    }
  }
  writer.printf(_T("%s"), text.cstr());
}
