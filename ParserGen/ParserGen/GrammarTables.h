#pragma once

typedef Array<ParserActionArray>   ActionMatrix;
typedef Array<SuccessorStateArray> SuccessorMatrix;

class GrammarTables : public ParserTables, public SymbolNameContainer {
private:
  const UINT              m_symbolCount;
  const UINT              m_terminalCount;
  const UINT              m_productionCount;
  const UINT              m_stateCount;
  const String            m_parserClassName;
  const String            m_tablesClassName;
  CompactUShortArray      m_productionLength, m_left;
  StringArray             m_symbolNameArray;
  Array<CompactUIntArray> m_rightSide;
  ActionMatrix            m_actionMatrix;
  SuccessorMatrix         m_successorMatrix;
  mutable StateSet        m_compressibleStateSet;
  mutable ByteCount       m_countTableBytes;
  mutable IntegerType     m_terminalType, m_NTindexType, m_symbolType, m_actionType, m_stateType;

  void                findTemplateTypes()                 const;
  void                initCompressibleStateSet();
  bool                calcIsCompressibleState(UINT state) const;
  inline bool         isCompressibleState(    UINT state) const {
    return m_compressibleStateSet.contains(state);
  }
  inline bool         isOneItemState(         UINT state) const {
    return m_actionMatrix[state].size() == 1;
  }

  void      printCpp(                         MarginFile &output) const;
  void      printJava(                        MarginFile &output) const;
  ByteCount printCompressedActionMatrixCpp(   MarginFile &output) const; // return size in bytes
  ByteCount printCompressedSuccessorMatrixCpp(MarginFile &output) const; // return size in bytes
  ByteCount printProductionLengthTableCpp(    MarginFile &output) const; // return size in bytes
  ByteCount printLeftSideTableCpp(            MarginFile &output) const; // return size in bytes
  ByteCount printRightSideTableCpp(           MarginFile &output) const; // return size in bytes
  ByteCount printSymbolNameTableCpp(          MarginFile &output) const; // return size in bytes
public:
  GrammarTables(const Grammar &g, const String &tableClassName, const String &parserClassName);
  int  getAction(            UINT state, UINT input     ) const override;
  UINT getSuccessor(         UINT state, UINT nt        ) const override;
  UINT getProductionLength(  UINT prod                  ) const override { return m_productionLength[prod];                 }
  UINT getLeftSymbol(        UINT prod                  ) const override { return m_left[prod];                             }
  const TCHAR *getSymbolName(UINT symbol                ) const override { return m_symbolNameArray[symbol].cstr();         }
  void getRightSide(         UINT prod, UINT *dst       ) const override;
  UINT getTerminalCount()                                 const override { return m_terminalCount;                          }
  UINT getSymbolCount()                                   const override { return m_symbolCount;                            }
  UINT getProductionCount()                               const override { return m_productionCount;                        }
  UINT getStateCount()                                    const override { return m_stateCount;                             }
  UINT getLegalInputCount(   UINT state                 ) const override { return (UINT)m_actionMatrix[state].size();       }
  void getLegalInputs(       UINT state, UINT *symbols  ) const override;
  UINT getLegalNTCount(      UINT state                 ) const override { return (UINT)m_successorMatrix[state].size();    }
  void getLegalNTerms(       UINT state, UINT *symbols  ) const override;
  UINT getTableByteCount(    Platform platform          ) const override { return m_countTableBytes.getByteCount(platform); }

  inline IntegerType            getTerminalType()         const          { return m_terminalType;                           }
  inline IntegerType            getActionType()           const          { return m_actionType;                             }
  inline IntegerType            getNTindexType()          const          { return m_NTindexType;                            }
  inline IntegerType            getStateType()            const          { return m_stateType;                              }
  const  ActionMatrix          &getActionMatrix()         const          { return m_actionMatrix;                           }
  const  SuccessorMatrix       &getSuccessorMatrix()      const          { return m_successorMatrix;                        }
  ByteCount                     getTotalSizeInBytes()     const;
  void print(MarginFile &output) const;
};
