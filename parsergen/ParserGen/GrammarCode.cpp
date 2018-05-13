#include "stdafx.h"
#include "GrammarCode.h"

GrammarCoder::GrammarCoder(const String &templateName
                          ,Grammar      &grammar
                          ,const String &implOutputDir
                          ,const String &headerOutputDir
                          ,const String &nameSpace
                          ,CodeFlags     flags)
: m_grammar(        grammar                                                     )
, m_templateName(   templateName                                                )
, m_implOutputDir(  implOutputDir                                               )
, m_headerOutputDir(headerOutputDir                                             )
, m_flags(          flags                                                       )
, m_sourceName(     FileNameSplitter(grammar.getName()).getAbsolutePath()       )
, m_grammarName(    FileNameSplitter(m_grammar.getName()).getFileName()         )
, m_parserClassName(m_grammarName + _T("Parser")                                )
, m_tablesClassName(m_grammarName + _T("Tables")                                )
, m_nameSpace(      nameSpace                                                   )
, m_docFileName(    FileNameSplitter(m_sourceName).setExtension(_T("txt")).getFullPath())
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
  const Grammar               &grammar  = m_coder.getGrammar();
  const CodeFlags              flags    = m_coder.getFlags();

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

  String text;
  for(int p = 0; p < productionCount; p++) {
    const Production &prod = grammar.getProduction(p);
    if(!prod.m_actionBody.isDefined()) {
      continue;
    }
    writer.printf(_T("case %d: /* %s */\n"), p, grammar.getProductionString(p).cstr());
    writer.incrLeftMargin(2);
    writer.writeSourceText(prod.m_actionBody);
    if(flags.m_generateBreaks) {
      writer.printf(_T("break;\n"));
    }
    writer.decrLeftMargin(2);
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
  String tmpFileName = TemplateWriter::createTempFileName(_T("txt"));
  MarginFile f(tmpFileName);
  tables.print(f, m_coder.getGrammar().getLanguage(), m_coder.getFlags().m_useTableCompression);
  f.close();
  const String text = readTextFile(tmpFileName);
  writer.printf(_T("%s"),text.cstr());
  unlink(tmpFileName);
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

class DocFileWriter : public KeywordHandler {
private:
  GrammarCoder &m_coder;
public:
  DocFileWriter(GrammarCoder &coder) : m_coder(coder) {}
  void handleKeyword(TemplateWriter &writer, String &line) const;
};

void DocFileWriter::handleKeyword(TemplateWriter &writer, String &line) const {
  m_coder.generateDocFile();
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

  SourceTextWriter headerWriter(     m_grammar.getHeader()    );
  SourceTextWriter driverHeadWriter( m_grammar.getDriverHead());
  SourceTextWriter driverTailWriter( m_grammar.getDriverTail());
  TablesWriter     tablesWriter(*this);
  ActionsWriter    actionsWriter(*this);
  SymbolsWriter    terminalsWriter(*this, true);
  SymbolsWriter    nonTerminalsWriter(*this, false);
  DocFileWriter    docFileWriter(*this);
  NewFileHandler   newFileHandler;

  TemplateWriter writer(m_templateName, m_implOutputDir, m_headerOutputDir, m_flags);

  writer.addKeywordHandler(_T("FILEHEAD"           ), headerWriter      );
  writer.addKeywordHandler(_T("CLASSHEAD"          ), driverHeadWriter  );
  writer.addKeywordHandler(_T("CLASSTAIL"          ), driverTailWriter  );
  writer.addKeywordHandler(_T("TABLES"             ), tablesWriter      );
  writer.addKeywordHandler(_T("ACTIONS"            ), actionsWriter     );
  writer.addKeywordHandler(_T("TERMINALSYMBOLS"    ), terminalsWriter   );
  writer.addKeywordHandler(_T("NONTERMINALSYMBOLS" ), nonTerminalsWriter);
  writer.addKeywordHandler(_T("DOCFILE"            ), docFileWriter     );
  writer.addKeywordHandler(_T("NEWFILE"            ), newFileHandler    );
  writer.addKeywordHandler(_T("NEWHEADERFILE"      ), newFileHandler    );
  writer.addMacro(         _T("GRAMMARNAME"        ), m_grammarName     );
  writer.addMacro(         _T("PARSERCLASSNAME"    ), m_parserClassName );
  writer.addMacro(         _T("TABLESCLASSNAME"    ), m_tablesClassName );
  writer.addMacro(         _T("DOCFILENAME"        ), m_docFileName     );
  writer.addMacro(         _T("TERMINALCOUNT"      ), toString(m_grammar.getTerminalCount()));
  writer.addMacro(         _T("SYMBOLCOUNT"        ), toString(m_grammar.getSymbolCount()));
  writer.addMacro(         _T("PRODUCTIONCOUNT"    ), toString(m_grammar.getProductionCount()));
  writer.addMacro(         _T("STATECOUNT"         ), toString(m_grammar.getStateCount()));
  writer.addMacro(         _T("OUTPUTDIR"          ), m_implOutputDir    );
  writer.addMacro(         _T("HEADERDIR"          ), m_headerOutputDir  );
  writer.addMacro(         _T("NAMESPACE"          ), m_nameSpace        );
  if(m_nameSpace.length() > 0) {
    writer.addMacro(       _T("PUSHNAMESPACE"      ), format(_T("namespace %s {\n"    ), m_nameSpace.cstr()));
    writer.addMacro(       _T("POPNAMESPACE"       ), format(_T("}; // namespace %s\n"), m_nameSpace.cstr()));
  } else {
    writer.addMacro(       _T("PUSHNAMESPACE"      ), _T(""));
    writer.addMacro(       _T("POPNAMESPACE"       ), _T(""));
  }

  writer.generateOutput();
}

