#pragma once

#include "ByteCount.h"

class Grammar;

class AllTemplateTypes {
private:
  const IntegerType m_symbolType, m_termType, m_ntIndexType, m_stateType, m_productionType;
public:
  AllTemplateTypes(const Grammar              &grammar);
  AllTemplateTypes(const AbstractParserTables &tables );

  inline IntegerType getSymbolType()     const { return m_symbolType;     }
  inline IntegerType getTermType()       const { return m_termType;       }
  inline IntegerType getNTIndexType()    const { return m_ntIndexType;    }
  inline IntegerType getProductionType() const { return m_productionType; }
  inline IntegerType getStateType()      const { return m_stateType;      }
};
