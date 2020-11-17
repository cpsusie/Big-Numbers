#include "stdafx.h"
#include <FileNameSplitter.h>
#include "TemplateWriter.h"
#include "GrammarCode.h"

GrammarCode::GrammarCode(const Grammar &grammar)
: m_grammar(        grammar                                               )
, m_sourceName(     FileNameSplitter(grammar.getName()).getAbsolutePath() )
, m_grammarName(    FileNameSplitter(m_grammar.getName()).getFileName()   )
, m_parserClassName(m_grammarName + _T("Parser")                          )
, m_tablesClassName(m_grammarName + _T("Tables")                          )
, m_docFileName(    FileNameSplitter(m_sourceName).setExtension(_T("txt")).getFullPath())
{
}

ByteArray symbolSetToByteArray(const SymbolSet &set) {
  const size_t byteCount = (set.getCapacity() - 1) / 8 + 1;
  ByteArray    result(byteCount);
  result.addZeroes(byteCount);
  BYTE *b = (BYTE*)result.getData();
  for(auto it = set.getIterator(); it.hasNext();) {
    const UINT v = (UINT)it.next();
    b[v >> 3] |= (1 << (v & 7));
  }
  return result;
}

String getStateSetComment(const StateSet &set) {
  const size_t n = set.size();
  return format(_T("Used by %s %s")
               ,(n == 1) ? _T("state ") : _T("states")
               ,set.toRangeString(SizeTStringifier(),_T(","), BT_BRACES).cstr());
}

void      outputBeginArrayDefinition(MarginFile &output, const TCHAR *tableName, IntegerType elementType, UINT size) {
  output.setLeftMargin(0);
  output.printf(_T("static const %s %s[%u] = {\n"), getTypeName(elementType), tableName, size);
  output.setLeftMargin(2);
}

ByteCount outputEndArrayDefinition(  MarginFile &output,                         IntegerType elementType, UINT size, bool addNewLine) {
  const ByteCount byteCount = ByteCount::wordAlignedSize(size * getTypeSize(elementType));
  output.setLeftMargin(0);
  output.printf(_T("%s}; // Size of table:%s.\n\n"), addNewLine?_T("\n"):_T(""), byteCount.toString().cstr());
  return byteCount;
}

void newLine(MarginFile &output, String &comment, int minColumn) { // static
  if(comment.length() > 0) {
    if(minColumn > 0) {
      const int fillerSize = minColumn - output.getCurrentLineLength();
      if(fillerSize > 0) {
        output.printf(_T("%*s"), fillerSize, EMPTYSTRING);
      }
    }
    output.printf(_T(" /* %-*s*/\n"), commentWidth, comment.cstr());
    comment = EMPTYSTRING;
  } else {
    output.printf(_T("\n"));
  }
}

void GrammarCode::generateDocFile() const {
  generateDocFile(MarginFile(m_docFileName));
}

void GrammarCode::generateDocFile(MarginFile &output) const {
  const Options &options = Options::getInstance();
  int dumpformat = DUMP_DOCFORMAT;
  if(options.m_generateLookahead) {
    dumpformat |= DUMP_LOOKAHEAD;
  }
  m_grammar.dumpStates(dumpformat, &output);
  output.printf(_T("\n"));
  output.printf(_T("%4u\tterminals\n"              ), m_grammar.getTerminalCount()   );
  output.printf(_T("%4u\tnonterminals\n"           ), m_grammar.getNTCount()         );
  output.printf(_T("%4u\tproductions\n"            ), m_grammar.getProductionCount() );
  output.printf(_T("%4u\tLALR(1) states\n"         ), m_grammar.getStateCount()      );
  output.printf(_T("%4u\titems\n"                  ), m_grammar.getItemCount()       );
  const ByteCount &byteCount = getByteCount();
  if(!byteCount.isEmpty()) {
    output.printf(_T("%s\t required for parsertables\n"), byteCount.toString().cstr());
  }
  output.printf(_T("\n"));
  output.printf(_T("%4u\tshift/reduce  conflicts\n"), m_grammar.m_SRconflicts        );
  output.printf(_T("%4u\treduce/reduce conflicts\n"), m_grammar.m_RRconflicts        );
  output.printf(_T("%4u\twarnings\n"               ), m_grammar.m_warningCount       );

  if(!m_grammar.allStatesConsistent()) {
    _tprintf(_T("See %s for details\n"), output.getName().cstr());
  }
}

void GrammarCode::generateParser() {
  const Options       &options = Options::getInstance();
  SourceTextWriter     headerWriter(     m_grammar.getHeader()    );
  SourceTextWriter     driverHeadWriter( m_grammar.getDriverHead());
  SourceTextWriter     driverTailWriter( m_grammar.getDriverTail());
  TablesWriter         tablesWriter(      *this       );
  ActionsWriter        actionsWriter(     *this       );
  SymbolsWriter        terminalsWriter(   *this, true );
  SymbolsWriter        nonTerminalsWriter(*this, false);
  DocFileWriter        docFileWriter(     *this       );
  TemplateWriter       writer;

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

  writer.generateOutput();
}
