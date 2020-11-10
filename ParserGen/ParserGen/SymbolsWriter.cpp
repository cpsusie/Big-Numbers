#include "stdafx.h"
#include "TemplateWriter.h"
#include "GrammarCode.h"

void SymbolsWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  const Options &options = Options::getInstance();
  const Grammar &grammar = m_coder.getGrammar();
  switch(options.m_language) {
  case CPP : writeCppSymbols(writer);
             break;
  case JAVA: writeJavaSymbols(writer);
             break;
  }
}

void SymbolsWriter::writeCppSymbols(TemplateWriter &writer) const {
  const Options &options       = Options::getInstance();
  const Grammar &grammar       = m_coder.getGrammar();
  const int      maxNameLength = grammar.getMaxSymbolNameLength();
  String text;
  if(m_terminals) {
    TCHAR delimiter = ' ';
    for(UINT s = 0; s < grammar.getTerminalCount(); s++, delimiter = ',') {
      text += format(_T("%c%-*s = %3u\n"), delimiter, maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
    }
  } else {
    if(options.m_generateNonTerminals) {
      TCHAR delimiter = ' ';
      for(UINT s = grammar.getTerminalCount(); s < grammar.getSymbolCount(); s++, delimiter = ',') {
        text += format(_T("%c%-*s = %3u\n"), delimiter, maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
      }
    }
  }
  writer.printf(_T("%s"),text.cstr());
}

void SymbolsWriter::writeJavaSymbols(TemplateWriter &writer) const {
  const Options &options       = Options::getInstance();
  const Grammar &grammar       = m_coder.getGrammar();
  int            maxNameLength = grammar.getMaxSymbolNameLength();
  String         text;
  if(m_terminals) {
    for(UINT s = 0; s < grammar.getTerminalCount(); s++) {
      text += format(_T("public static final int %-*s = %3u;\n"), maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
    }
  } else {
    if(options.m_generateNonTerminals) {
      for(UINT s = grammar.getTerminalCount(); s < grammar.getSymbolCount(); s++) {
        text += format(_T("public static final int %-*s = %3u;\n"), maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
      }
    }
  }
  writer.printf(_T("%s"), text.cstr());
}
