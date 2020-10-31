#include "stdafx.h"
#include <FileNameSplitter.h>
#include "GrammarCode.h"

const ByteCount ByteCount::s_pointerSize(4,8); // sizeof(void*) in x86 and x64

GrammarCode::GrammarCode(const String &templateName
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
}

ByteArray bitSetToByteArray(const BitSet &set) {
  const size_t byteCount = (set.getCapacity() - 1) / 8 + 1;
  ByteArray    result(byteCount);
  result.addZeroes(byteCount);
  BYTE *b = (BYTE*)result.getData();
  for(ConstIterator<size_t> it = set.getIterator(); it.hasNext();) {
    const UINT v = (UINT)it.next();
    b[v >> 3] |= (1 << (v & 7));
  }
  return result;
}

void newLine(MarginFile &output, String &comment, int minColumn) { // static
  if(comment.length() > 0) {
    if(minColumn > 0) {
      const int fillerSize = minColumn - output.getCurrentLineLength();
      if(fillerSize > 0) {
        output.printf(_T("%*s"), fillerSize, EMPTYSTRING);
      }
    }
    output.printf(_T(" /* %s */\n"), comment.cstr());
    comment = EMPTYSTRING;
  } else {
    output.printf(_T("\n"));
  }
}

GrammarCode::~GrammarCode() {
}

void GrammarCode::generateDocFile() {
  generateDocFile(MarginFile(m_docFileName));
}

void GrammarCode::generateDocFile(MarginFile &output) {
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
  const ByteCount &byteCount = getByteCount();
  if(!byteCount.isEmpty()) {
    output.printf(_T("%s\t required for parsertables\n"), byteCount.toString().cstr());
  }
  output.printf(_T("\n"));
  output.printf(_T("%4d\tshift/reduce  conflicts\n"), m_grammar.m_SRconflicts);
  output.printf(_T("%4d\treduce/reduce conflicts\n"), m_grammar.m_RRconflicts);
  output.printf(_T("%4d\twarnings\n"), m_grammar.m_warningCount);

  if(!m_grammar.allStatesConsistent()) {
    _tprintf(_T("See %s for details\n"), output.getName().cstr());
  }
}

void GrammarCode::generateParser() {
  SourceTextWriter     headerWriter(     m_grammar.getHeader()    );
  SourceTextWriter     driverHeadWriter( m_grammar.getDriverHead());
  SourceTextWriter     driverTailWriter( m_grammar.getDriverTail());
  TablesWriter         tablesWriter(      *this       );
  ActionsWriter        actionsWriter(     *this       );
  SymbolsWriter        terminalsWriter(   *this, true );
  SymbolsWriter        nonTerminalsWriter(*this, false);
  DocFileWriter        docFileWriter(     *this       );
  TemplateWriter writer(m_templateName, m_implOutputDir, m_headerOutputDir, m_flags);

  writer.addKeywordHandler(_T("FILEHEAD"           ), headerWriter         );
  writer.addKeywordHandler(_T("CLASSHEAD"          ), driverHeadWriter     );
  writer.addKeywordHandler(_T("CLASSTAIL"          ), driverTailWriter     );
  writer.addKeywordHandler(_T("TABLES"             ), tablesWriter         );
  writer.addKeywordHandler(_T("ACTIONS"            ), actionsWriter        );
  writer.addKeywordHandler(_T("TERMINALSYMBOLS"    ), terminalsWriter      );
  writer.addKeywordHandler(_T("NONTERMINALSYMBOLS" ), nonTerminalsWriter   );
  writer.addKeywordHandler(_T("DOCFILE"            ), docFileWriter        );
  writer.addMacro(         _T("GRAMMARNAME"        ), m_grammarName        );
  writer.addMacro(         _T("PARSERCLASSNAME"    ), m_parserClassName    );
  writer.addMacro(         _T("TABLESCLASSNAME"    ), m_tablesClassName    );
  writer.addMacro(         _T("DOCFILENAME"        ), m_docFileName        );
  writer.addMacro(         _T("TERMINALCOUNT"      ), toString(m_grammar.getTerminalCount(  )));
  writer.addMacro(         _T("SYMBOLCOUNT"        ), toString(m_grammar.getSymbolCount(    )));
  writer.addMacro(         _T("PRODUCTIONCOUNT"    ), toString(m_grammar.getProductionCount()));
  writer.addMacro(         _T("STATECOUNT"         ), toString(m_grammar.getStateCount(     )));
  writer.addMacro(         _T("OUTPUTDIR"          ), m_implOutputDir      );
  writer.addMacro(         _T("HEADERDIR"          ), m_headerOutputDir    );
  writer.addMacro(         _T("NAMESPACE"          ), m_nameSpace          );
  if(m_nameSpace.length() > 0) {
    writer.addMacro(       _T("PUSHNAMESPACE"      ), format(_T("\nnamespace %s {\n" ), m_nameSpace.cstr()));
    writer.addMacro(       _T("POPNAMESPACE"       ), format(_T("}; // namespace %s" ), m_nameSpace.cstr()));
  } else {
    writer.addMacro(       _T("PUSHNAMESPACE"      ), _T("$NOLINE$"));
    writer.addMacro(       _T("POPNAMESPACE"       ), _T("$NOLINE$"));
  }

  writer.generateOutput();
}
