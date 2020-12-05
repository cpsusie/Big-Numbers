#pragma once

#include "ByteCount.h"

class AbstractParserTables;
class Grammar;

class AllTemplateTypes {
private:
  const IntegerType m_symbolType, m_termType, m_ntIndexType, m_actionType, m_stateType;
public:
  AllTemplateTypes(const Grammar              &grammar);
  AllTemplateTypes(const AbstractParserTables &tables );

  inline IntegerType getSymbolType()  const { return m_symbolType;  }
  inline IntegerType getTermType()    const { return m_termType;    }
  inline IntegerType getNTIndexType() const { return m_ntIndexType; }
  inline IntegerType getActionType()  const { return m_actionType;  }
  inline IntegerType getStateType()   const { return m_stateType;   }
};
