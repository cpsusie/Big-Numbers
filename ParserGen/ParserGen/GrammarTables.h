#pragma once

#include "CompressEncoding.h"

class GrammarCode;

class GrammarTables : public AbstractParserTables {
private:
  const GrammarCode      &m_grammarCode;
  const Grammar          &m_grammar;
  const GrammarResult    &m_grammarResult;
  const AllTemplateTypes m_types;

  mutable StateSet       m_compressibleStateSet;
  mutable ByteCount      m_countTableBytes;

  void                initCompressibleStateSet();
  bool                calcIsCompressibleState(UINT state) const;
  inline bool         isCompressibleState(    UINT state) const {
    return m_compressibleStateSet.contains(state);
  }

  void      printCpp(                              MarginFile &output) const;
  void      printJava(                             MarginFile &output) const;
  ByteCount printCppParserTablesTemplateTransShift(MarginFile &output) const; // return size in bytes
  ByteCount printCompressedTransShiftMatrixCpp(    MarginFile &output) const; // return size in bytes
  ByteCount printProdLengthArrayCpp(               MarginFile &output) const; // return size in bytes
  ByteCount printLeftSideArrayCpp(                 MarginFile &output) const; // return size in bytes
  ByteCount printRightSideTableCpp(                MarginFile &output) const; // return size in bytes
  ByteCount printSymbolNamesCpp(                   MarginFile &output) const; // return size in bytes
public:
  GrammarTables(const GrammarCode &grammarCode);
  inline const GrammarCode   &getGrammarCode()                      const {
    return m_grammarCode;
  }
  inline const Grammar       &getGrammar()                          const {
    return m_grammar;
  }
  inline const GrammarResult &getGrammarResult()                    const {
    return m_grammarResult;
  }
  UINT           getSymbolCount()                                   const final;
  UINT           getTermCount()                                     const final;
  UINT           getProductionCount()                               const final;
  UINT           getStateCount()                                    const final;
  UINT           getStartState()                                    const final;
  const String  &getSymbolName(         UINT symbolIndex          ) const final;
  int            getAction(             UINT state, UINT term     ) const final;
  UINT           getSuccessor(          UINT state, UINT nterm    ) const final;
  UINT           getProductionLength(   UINT prod                 ) const final;
  UINT           getLeftSymbol(         UINT prod                 ) const final;
  void           getRightSide(          UINT prod, UINT *dst      ) const final;
  UINT           getTableByteCount(     Platform platform         ) const final { return m_countTableBytes.getByteCount(platform); }

  ByteCount                     getTotalSizeInBytes()     const;
  void print(MarginFile &output) const;
};
