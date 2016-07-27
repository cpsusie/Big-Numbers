#include "stdafx.h"
#include "GrammarCode.h"
#include "TemplateWriter.h"

CodeFlags::CodeFlags() {
  m_lineDirectives       = true;
  m_generateBreaks       = true;
  m_generateActions      = true;
  m_generateLookahead    = false;
  m_generateNonTerminals = false;
  m_useTableCompression  = true;
}

GrammarCoder::GrammarCoder(const String &templateName
                          ,Grammar      &grammar
                          ,const String &implOutputDir
                          ,const String &headerOutputDir
                          ,CodeFlags     flags) 
: m_grammar(        grammar                                                     )
, m_templateName(   templateName                                                )
, m_implOutputDir(  implOutputDir                                               )
, m_headerOutputDir(headerOutputDir                                             )
, m_flags(          flags                                                       )
, m_sourceName(     FileNameSplitter(grammar.getName()).getAbsolutePath()       )
, m_grammarName(    FileNameSplitter(m_grammar.getName()).getFileName()         )
, m_parserClassName(m_grammarName + "Parser"                                    )
, m_tablesClassName(m_grammarName + "Tables"                                    )
, m_docFileName(    FileNameSplitter(m_sourceName).setExtension("txt").getFullPath())
{
  m_tablesByteCount  = 0;
}

GrammarCoder::~GrammarCoder() {
}

class ActionsWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
public:
  ActionsWriter(GrammarCoder &coder) : m_coder(coder) {}
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

void ActionsWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  MarginFile    &output  = writer.getOutput();
  const Grammar &grammar = m_coder.getGrammar();
  const CodeFlags &flags = m_coder.getFlags();

  const int productionCount = grammar.getProductionCount();
  int actionCount     = 0;

  for(int p = 0; p < productionCount; p++) { // first count the number of real actions
    if(grammar.getProduction(p).m_actionBody.isDefined()) {
      actionCount++;
    }
  }

  if(actionCount == 0) {
    return; // dont generate any switch
  }

  for(int p = 0; p < productionCount; p++) {
    const Production &prod = grammar.getProduction(p);
    if(!prod.m_actionBody.isDefined()) {
      continue;
    }
    output.printf(_T("case %d: /* %s */\n"), p, grammar.getProductionString(p).cstr());
    int m = output.getLeftMargin();
    output.setLeftMargin(m+2);
    writeSourceText(output, prod.m_actionBody, writer.getPos(), flags.m_lineDirectives);    
    if(flags.m_generateBreaks) {
      output.printf(_T("break;\n"));
    }
    output.setLeftMargin(m);
  }
}

class TablesWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
public:
  TablesWriter(GrammarCoder &coder) : m_coder(coder) {};
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

void TablesWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  GrammarTables tables(m_coder.getGrammar(), m_coder.getTablesClassName());

  tables.print(writer.getOutput(), m_coder.getGrammar().getLanguage(), m_coder.getFlags().m_useTableCompression);

  m_coder.setByteCount(tables.getTotalSizeInBytes(m_coder.getFlags().m_useTableCompression));
}

class SymbolsWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
  bool m_terminals;
  void writeCppSymbols(TemplateWriter &writer) const;
  void writeJavaSymbols(TemplateWriter &writer) const;
public:
  SymbolsWriter(GrammarCoder &coder, bool terminals) : m_coder(coder) { m_terminals = terminals; }
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

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
  const Grammar &grammar = m_coder.getGrammar();  
  int   maxNameLength    = grammar.getMaxSymbolNameLength();
  MarginFile &output     = writer.getOutput();
  if(m_terminals) {
    char delimiter = ' ';
    for(int s = 0; s < grammar.getTerminalCount(); s++, delimiter=',') {
      output.printf(_T("%c%-*s = %3d\n"), delimiter, maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
    }
  } else {
    if(m_coder.getFlags().m_generateNonTerminals) {
      char delimiter = ' ';
      for(int s = grammar.getTerminalCount(); s < grammar.getSymbolCount(); s++, delimiter=',') {
        output.printf(_T("%c%-*s = %3d\n"), delimiter, maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
      }
    }
  }
}

void SymbolsWriter::writeJavaSymbols(TemplateWriter &writer) const {
  const Grammar &grammar = m_coder.getGrammar();  
  int   maxNameLength    = grammar.getMaxSymbolNameLength();
  MarginFile &output     = writer.getOutput();
  if(m_terminals) {
    for(int s = 0; s < grammar.getTerminalCount(); s++) {
      output.printf(_T("public static final int %-*s = %3d;\n"), maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
    }
  } else {
    if(m_coder.getFlags().m_generateNonTerminals) {
      for(int s = grammar.getTerminalCount(); s < grammar.getSymbolCount(); s++) {
        output.printf(_T("public static final int %-*s = %3d;\n"), maxNameLength, grammar.getSymbol(s).m_name.cstr(), s);
      }
    }
  }
}

class DocFileWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
public:
  DocFileWriter(GrammarCoder &coder) : m_coder(coder) {}
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

void DocFileWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  m_coder.generateDocFile(writer.getOutput());
}

void GrammarCoder::generateDocFile() {
  generateDocFile(MarginFile(m_docFileName));
}

void GrammarCoder::generateDocFile(MarginFile &output) {
  int dumpformat = DUMP_DOCFORMAT;
  if(getFlags().m_generateLookahead) {
    dumpformat |= DUMP_LOOKAHEAD;
  }
  m_grammar.dumpStates(dumpformat, &output);
  output.printf(_T("\n"));
  output.printf(_T("%4d\tterminals\n")     , m_grammar.getTerminalCount()   );
  output.printf(_T("%4d\tnonterminals\n")  , m_grammar.getNonTerminalCount());
  output.printf(_T("%4d\tproductions\n")   , m_grammar.getProductionCount() );
  output.printf(_T("%4d\tLALR(1) states\n"), m_grammar.getStateCount()      );
  output.printf(_T("%4d\titems\n")         , m_grammar.getItemCount()       );
  const int byteCount = getByteCount();
  if(byteCount > 0) {
    output.printf(_T("%4d\tbytes required for parsertables\n"), byteCount);
  }
  output.printf(_T("\n"));
  output.printf(_T("%4d\tshift/reduce  conflicts\n"), m_grammar.m_SRconflicts);
  output.printf(_T("%4d\treduce/reduce conflicts\n"), m_grammar.m_RRconflicts);
  output.printf(_T("%4d\twarnings\n"), m_grammar.m_warningCount);

  if(!m_grammar.allStatesConsistent()) {
    _tprintf(_T("See %s for details\n"), output.getName().cstr());
  }
}

void GrammarCoder::generateParser() {

  SourceTextWriter headerWriter(     m_grammar.getHeader()    , m_flags.m_lineDirectives);
  SourceTextWriter driverHeadWriter( m_grammar.getDriverHead(), m_flags.m_lineDirectives);
  SourceTextWriter driverTailWriter( m_grammar.getDriverTail(), m_flags.m_lineDirectives);
  TablesWriter     tablesWriter(*this);
  ActionsWriter    actionsWriter(*this);
  SymbolsWriter    terminalsWriter(*this, true);
  SymbolsWriter    nonTerminalsWriter(*this, false);
  DocFileWriter    docFileWriter(*this);
  NewFileHandler   newFileHandler;

  TemplateWriter writer(m_templateName, m_implOutputDir, m_headerOutputDir, false);

  writer.addKeywordHandler("FILEHEAD"           , headerWriter      );
  writer.addKeywordHandler("CLASSHEAD"          , driverHeadWriter  );
  writer.addKeywordHandler("CLASSTAIL"          , driverTailWriter  );
  writer.addKeywordHandler("TABLES"             , tablesWriter      );
  writer.addKeywordHandler("ACTIONS"            , actionsWriter     );
  writer.addKeywordHandler("TERMINALSYMBOLS"    , terminalsWriter   );
  writer.addKeywordHandler("NONTERMINALSYMBOLS" , nonTerminalsWriter);
  writer.addKeywordHandler("DOCFILE"            , docFileWriter     );
  writer.addKeywordHandler("NEWFILE"            , newFileHandler    );
  writer.addKeywordHandler("NEWHEADERFILE"      , newFileHandler    );
  writer.addMacro(         "GRAMMARNAME"        , m_grammarName     );
  writer.addMacro(         "PARSERCLASSNAME"    , m_parserClassName );
  writer.addMacro(         "TABLESCLASSNAME"    , m_tablesClassName );
  writer.addMacro(         "DOCFILENAME"        , m_docFileName     );
  writer.addMacro(         "TERMINALCOUNT"      , toString(m_grammar.getTerminalCount()));
  writer.addMacro(         "SYMBOLCOUNT"        , toString(m_grammar.getSymbolCount()));
  writer.addMacro(         "PRODUCTIONCOUNT"    , toString(m_grammar.getProductionCount()));
  writer.addMacro(         "STATECOUNT"         , toString(m_grammar.getStateCount()));
  writer.addMacro(         "OUTPUTDIR"          , m_implOutputDir);
  writer.addMacro(         "HEADERDIR"          , m_headerOutputDir);

  writer.generateOutput();
}

