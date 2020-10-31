#pragma once

#include "ByteCount.h"

typedef CompactShortArray  RawActionArray;
typedef CompactUshortArray SuccesorArray;

class GrammarTables : public ParserTables {
private:
  UINT                         m_terminalCount;
  CompactUshortArray           m_productionLength, m_left;
  StringArray                  m_symbolName;
  Array<CompactIntArray>       m_rightSide;
  Array<ActionArray>           m_stateActions;
  Array<ActionArray>           m_stateSucc;
  const String                 m_parserClassName;
  const String                 m_tablesClassName;
  mutable BitSet               m_compressibleStateSet;
  mutable ByteCount            m_countTableBytes;
  mutable IntegerType          m_terminalType, m_NTIndexType, m_symbolType, m_actionType, m_stateType;

  void                findTemplateTypes() const;

  UINT                getMaxInputCount() const;
  void                initCompressibleStateSet();
  bool                calcIsCompressibleState(UINT state) const;
  inline bool         isCompressibleState(    UINT state) const {
    return m_compressibleStateSet.contains(state);
  }
  inline bool         isOneItemState(UINT state) const {
    return m_stateActions[state].size() == 1;
  }

  void      printCpp( MarginFile &output, bool useTableCompression) const;
  void      printJava(MarginFile &output, bool useTableCompression) const;
  ByteCount printCompressedActionMatrixCpp(  MarginFile &output) const;                                              // return size in bytes
  ByteCount printSuccessorMatrixCpp(         MarginFile &output) const;                                              // return size in bytes
  ByteCount printProductionLengthTableCpp(   MarginFile &output) const;                                              // return size in bytes
  ByteCount printLeftSideTableCpp(           MarginFile &output) const;                                              // return size in bytes
  ByteCount printRightSideTableCpp(          MarginFile &output) const;                                              // return size in bytes
  ByteCount printSymbolNameTableCpp(         MarginFile &output) const;                                              // return size in bytes
public:
  GrammarTables(const Grammar &g, const String &tableClassName, const String &parserClassName);
  int  getAction(   UINT state, UINT input)      const override;
  UINT getSuccessor(UINT state, UINT nt   )      const override;

  UINT getProductionLength(UINT prod)            const override {
    return m_productionLength[prod];
  }

  UINT getLeftSymbol(UINT prod)                  const override {
    return m_left[prod];
  }

  const TCHAR *getSymbolName(UINT symbol)        const override {
    return m_symbolName[symbol].cstr();
  }

  void getRightSide(UINT prod, UINT *dst)        const override;

  UINT getTerminalCount()                        const override {
    return m_terminalCount;
  }

  UINT getSymbolCount()                          const override {
    return (UINT)m_symbolName.size();
  }

  UINT getProductionCount()                      const override {
    return (UINT)m_productionLength.size();
  }

  UINT getStateCount()                           const override {
    return (UINT)m_stateActions.size();
  }

  UINT getLegalInputCount(UINT state)            const override {
    return (UINT)m_stateActions[state].size();
  }

  void getLegalInputs(UINT state, UINT *symbols) const override;
  UINT getTableByteCount(   Platform platform  ) const override;

  inline IntegerType  getTerminalType() const { return m_terminalType; }
  inline IntegerType  getActionType()   const { return m_actionType;   }

  BitSet              getLookaheadSet(  UINT state) const;
  RawActionArray      getRawActionArray(UINT state) const;
  BitSet              getNTOffsetSet(   UINT state) const;
  SuccesorArray       getSuccessorArray(UINT state) const;

  const Array<ActionArray> &getStateActions() const {
    return m_stateActions;
  }
  ByteCount getTotalSizeInBytes() const;
  void print(MarginFile &output, Language language, bool useTableCompression) const;
};
