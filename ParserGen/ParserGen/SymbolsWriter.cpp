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
  const UINT     maxNameLength = grammar.getMaxSymbolNameLength();
  String text;
  if(m_writeTerminals) {
    TCHAR delimiter = ' ';
    for(UINT term = 0; term < grammar.getTermCount(); term++, delimiter = ',') {
      text += format(_T("%c%-*s = %3u\n"), delimiter, maxNameLength, grammar.getSymbolName(term).cstr(), term);
    }
  } else {
    if(options.m_generateNonTerminals) {
      TCHAR delimiter = ' ';
      UINT  ntIndex   = 0;
      for(UINT nterm = grammar.getTermCount(); nterm < grammar.getSymbolCount(); nterm++, delimiter = ',', ntIndex++) {
        text += format(_T("%c%-*s = %3u   /* ntIndex = %3u */\n"), delimiter, maxNameLength, grammar.getSymbolName(nterm).cstr(), nterm, ntIndex);
      }
    }
  }
  writer.printf(_T("%s"),text.cstr());
}

void SymbolsWriter::writeJavaSymbols(TemplateWriter &writer) const {
  const Options &options       = Options::getInstance();
  const Grammar &grammar       = m_coder.getGrammar();
  const UINT     maxNameLength = grammar.getMaxSymbolNameLength();
  String         text;
  if(m_writeTerminals) {
    for(UINT term = 0; term < grammar.getTermCount(); term++) {
      text += format(_T("public static final int %-*s = %3u;\n"), maxNameLength, grammar.getSymbolName(term).cstr(), term);
    }
  } else {
    if(options.m_generateNonTerminals) {
      for(UINT nterm = grammar.getTermCount(); nterm < grammar.getSymbolCount(); nterm++) {
        text += format(_T("public static final int %-*s = %3u;\n"), maxNameLength, grammar.getSymbolName(nterm).cstr(), nterm);
      }
    }
  }
  writer.printf(_T("%s"), text.cstr());
}
