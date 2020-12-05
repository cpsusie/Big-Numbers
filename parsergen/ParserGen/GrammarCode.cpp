#include "stdafx.h"
#include <FileNameSplitter.h>
#include "TemplateWriter.h"
#include "CompressedTransSuccMatrixCpp.h"
#include "CompressedTransShiftMatrixCpp.h"
#include "Grammar.h"
#include "GrammarResult.h"
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
  if(options.m_findOptimalTableCompression) {
    listCompressionCombination();
    exit(0);
  }
  if(options.getOptParam(OPTPARAM_SHIFT).m_enabled) {
    CompressedTransShiftMatrix sm(m_grammar);
    if(sm.getSavedBytesByOptimizedStateBitSets().getByteCount(PLATFORM_X64) > 20) {
      m_grammar.reorderStates(sm.getStateBitSetPermutation());
    }
    if(sm.getSavedBytesByOptimizedTermBitSets().getByteCount(PLATFORM_X64) > 20) {
      m_grammar.reorderTerminals(sm.getTermBitSetPermutation());
    }
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

static constexpr UINT recurseLevelWidth   = 10;
static constexpr UINT splitNodeCountWidth = 14;
static constexpr UINT bitSetCapacityWidth = 14;
static constexpr UINT minBitSetSizeWidth  = 14;
static constexpr UINT minBitSetRangeWidth = 14;

void GrammarCode::listCompressionCombination() {
  MatrixOptimizeParameters bestShiftParam, bestSuccParam;
  ByteCount                bestByteCount;

  Options &options = Options::getInstance();

  { // Successor Matrix Transposed
    _tprintf(_T("%*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s\n")
            ,recurseLevelWidth        , _T("SHFT.recurseLvl"   )
            ,minBitSetSizeWidth       , _T("SHFT.bitsetsize"   )
            ,recurseLevelWidth        , _T("SUCC.recurseLvl"   )
            ,minBitSetSizeWidth       , _T("SUCC.bitsetsize"   )
            ,ByteCount::tableformWidth, _T("SuccCodeArray"     )
            ,ByteCount::tableformWidth, _T("StateArrayTable"   )
            ,ByteCount::tableformWidth, _T("NewStateArrayTable")
            ,ByteCount::tableformWidth, _T("StateBitSetTable"  )
            ,ByteCount::tableformWidth, _T("Total"             )
            ,splitNodeCountWidth      , _T("Splitnodes"        )
            ,minBitSetRangeWidth      , _T("shiftBitset.ran"   )
            ,minBitSetRangeWidth      , _T("succBitset.ran"    )
            );

    MatrixOptimizeParameters &optShift  = options.getOptimizeParameters(OPTPARAM_SHIFT );
    MatrixOptimizeParameters &optReduce = options.getOptimizeParameters(OPTPARAM_REDUCE);
    MatrixOptimizeParameters &optSucc   = options.getOptimizeParameters(OPTPARAM_SUCC  );

    for(optShift.m_maxRecursion = 0; optShift.m_maxRecursion < Options::maxRecursiveCalls; optShift.m_maxRecursion++) {
      for(optShift.m_minBitSetSize = 2; optShift.m_minBitSetSize < 50; optShift.m_minBitSetSize++) {
        for(optSucc.m_maxRecursion = 0; optSucc.m_maxRecursion < Options::maxRecursiveCalls; optSucc.m_maxRecursion++) {
          for(optSucc.m_minBitSetSize = 2; optSucc.m_minBitSetSize < 50; optSucc.m_minBitSetSize++) {
            Grammar tempGrammar(m_grammar);
            const TableTypeByteCountMap map         = CompressedTransShiftMatrix::findTablesByteCount(tempGrammar);
            const ByteCount             matrixTotal = map.getSum();
            _tprintf(_T("%*u %*u %*u %*u %*s %*s %*s %*s %*s %*u %*s %*s\n")
                    ,recurseLevelWidth        , optShift.m_maxRecursion
                    ,minBitSetSizeWidth       , optShift.m_minBitSetSize
                    ,recurseLevelWidth        , optSucc.m_maxRecursion
                    ,minBitSetSizeWidth       , optSucc.m_minBitSetSize
                    ,ByteCount::tableformWidth, map.getTableString(BC_SUCCESSORCODEARRAY ).cstr()
                    ,ByteCount::tableformWidth, map.getTableString(BC_STATEARRAYTABLE    ).cstr()
                    ,ByteCount::tableformWidth, map.getTableString(BC_NEWSTATEARRAYTABLE ).cstr()
                    ,ByteCount::tableformWidth, map.getTableString(BC_STATEBITSETTABLE   ).cstr()
                    ,ByteCount::tableformWidth, matrixTotal.toStringTableForm().cstr()
                    ,splitNodeCountWidth      , map.getSplitNodeCount()
                    ,minBitSetRangeWidth      , map.getShiftStateBitSetInterval().toString().cstr()
                    ,minBitSetRangeWidth      , map.getSuccStateBitSetInterval().toString().cstr()
                    );
            if(bestByteCount.isEmpty() || (matrixTotal < bestByteCount)) {
              bestShiftParam = optShift;
              bestSuccParam  = optSucc;
            }
          }
        }
      }
    }
  }
}

ByteArray bitSetToByteArray(const BitSet &bitSet, UINT capacity) {
  if(capacity == 0) {
    capacity = (UINT)bitSet.getCapacity();
  }
  return bitSetToByteArray(bitSet, BitSetInterval(0, capacity));
}

ByteArray bitSetToByteArray(const BitSet &bitSet, const BitSetInterval &interval) {
  if(!interval.checkBitSetValues(bitSet)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("BitSet:%s, interval=%s"), bitSet.toRangeString().cstr(), interval.toString().cstr());
  }
  const size_t byteCount = interval.getSizeofBitSet();
  ByteArray    result(byteCount);
  result.addZeroes(byteCount);
  BYTE *b = (BYTE*)result.getData();
  const UINT base = interval.getFrom();
  for(auto it = bitSet.getIterator(); it.hasNext();) {
    const UINT v = (UINT)it.next() - base;
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

UINT outputBeginBitSetTableDefinition(MarginFile &output, const TCHAR *tableName, const BitSetInterval &interval, UINT count) {
  output.setLeftMargin(0);
  const UINT bytesPerBitSet = interval.getSizeofBitSet();
  const UINT totalSize      = count * bytesPerBitSet;
  output.printf(_T("static const %s %s[%u] = { /* range:%s, bytes in bitset=%u */\n")
               ,getTypeName(TYPE_UCHAR), tableName, totalSize, interval.toString().cstr(), bytesPerBitSet);
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
