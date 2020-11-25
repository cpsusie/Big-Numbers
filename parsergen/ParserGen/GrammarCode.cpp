#include "stdafx.h"
#include <FileNameSplitter.h>
#include "TemplateWriter.h"
#include "CompressedActionMatrixCpp.h"
#include "CompressedTransSuccMatrixCpp.h"
#include "GrammarCode.h"

GrammarCode::GrammarCode(Grammar &grammar)
: m_grammar(        grammar                                               )
, m_sourceName(     FileNameSplitter(grammar.getName()).getAbsolutePath() )
, m_grammarName(    FileNameSplitter(m_grammar.getName()).getFileName()   )
, m_parserClassName(m_grammarName + _T("Parser")                          )
, m_tablesClassName(m_grammarName + _T("Tables")                          )
, m_docFileName(    FileNameSplitter(m_sourceName).setExtension(_T("txt")).getFullPath())
{
  if(!grammar.getResult().allStatesConsistent()) {
    return;
  }
  const Options &options = Options::getInstance();
  if(options.m_useTableCompression) {
    ActionMatrixCompression::CompressedActionMatrix am(m_grammar);
    const ByteCount savedBytes = am.getSavedBytesByOptimizedTermBitSets();
    if(savedBytes.getByteCount(PLATFORM_X64) > 20) {
      const OptimizedBitSetPermutation &termBitSetPermutation = am.getTermBitSetPermutation();
      m_grammar.reorderTerminals(termBitSetPermutation, termBitSetPermutation.getNewCapacity());
    }

    if(options.m_compressSuccTransposed) {
      TransposedSuccessorMatrixCompression::CompressedTransSuccMatrix sm(m_grammar);
      const ByteCount savedBytes = sm.getSavedBytesByOptimizedStateBitSets();
      if(savedBytes.getByteCount(PLATFORM_X64) > 20) {
        const OptimizedBitSetPermutation &stateBitSetPermutation = sm.getStateBitSetPermutation();
        m_grammar.reorderStates(stateBitSetPermutation, stateBitSetPermutation.getNewCapacity());
      }
    }
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
  output.printf(_T("%4u\tterminals\n"              ), m_grammar.getTermCount()       );
  output.printf(_T("%4u\tnonterminals\n"           ), m_grammar.getNTermCount()      );
  output.printf(_T("%4u\tproductions\n"            ), m_grammar.getProductionCount() );
  output.printf(_T("%4u\tLALR(1) states\n"         ), m_grammar.getStateCount()      );
  output.printf(_T("%4u\titems\n"                  ), m_grammar.getItemCount()       );
  const ByteCount &byteCount = getByteCount();
  if(!byteCount.isEmpty()) {
    output.printf(_T("%s\t required for parsertables\n"), byteCount.toString().cstr());
  }
  const GrammarResult &result = m_grammar.getResult();
  output.printf(_T("\n"));
  output.printf(_T("%4u\tshift/reduce  conflicts\n"), result.m_SRconflicts        );
  output.printf(_T("%4u\treduce/reduce conflicts\n"), result.m_RRconflicts        );
  output.printf(_T("%4u\twarnings\n"               ), result.m_warningCount       );

  if(!result.allStatesConsistent()) {
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
  writer.addMacro(         _T("TERMINALCOUNT"      ), toString(m_grammar.getTermCount(      )));
  writer.addMacro(         _T("SYMBOLCOUNT"        ), toString(m_grammar.getSymbolCount(    )));
  writer.addMacro(         _T("PRODUCTIONCOUNT"    ), toString(m_grammar.getProductionCount()));
  writer.addMacro(         _T("STATECOUNT"         ), toString(m_grammar.getStateCount(     )));

  writer.generateOutput();
}

ByteArray bitSetToByteArray(const BitSet &bitSet, UINT capacity) {
  const size_t byteCount = getSizeofBitSet(capacity ? capacity : (UINT)bitSet.getCapacity());
  if(capacity && (capacity < bitSet.getCapacity())) {
    BitSet tmp(bitSet.getCapacity());
    tmp.add(capacity, bitSet.getCapacity() - 1);
    if(!(bitSet & tmp).isEmpty()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("BitSet:%s, capacity=%u"), bitSet.toRangeString().cstr(), capacity);
    }
  }
  ByteArray    result(byteCount);
  result.addZeroes(byteCount);
  BYTE *b = (BYTE*)result.getData();
  for(auto it = bitSet.getIterator(); it.hasNext();) {
    const UINT v = (UINT)it.next();
    b[v >> 3] |= (1 << (v & 7));
  }
  return result;
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

UINT outputBeginBitSetTableDefinition(MarginFile &output, const TCHAR *tableName, UINT capacity, UINT count) {
  output.setLeftMargin(0);
  const UINT bytesPerBitSet = getSizeofBitSet(capacity);
  const UINT totalSize   = count * bytesPerBitSet;
  output.printf(_T("static const %s %s[%u] = { /* capacity(bitset)=%u, bytes in bitset=%u */\n")
               ,getTypeName(TYPE_UCHAR), tableName, totalSize, capacity, bytesPerBitSet);
  output.setLeftMargin(2);
  return totalSize;
}

ByteCount outputEndBitSetTableDefinition(MarginFile &output, UINT size, bool addNewLine) {
  return outputEndArrayDefinition(output, TYPE_UCHAR, size, addNewLine);
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

