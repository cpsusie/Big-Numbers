#include "stdafx.h"
#include "Grammar.h"
#include "AllTemplateTypes.h"

AllTemplateTypes::AllTemplateTypes(const Grammar &grammar)
  : m_symbolType(    findIntType(0, grammar.getSymbolCount()     - 1))
  , m_termType(      findIntType(0, grammar.getTermCount()       - 1))
  , m_ntIndexType(   findIntType(0, grammar.getNTermCount()      - 1))
  , m_stateType(     findIntType(0, grammar.getStateCount()      - 1))
  , m_productionType(findIntType(0, grammar.getProductionCount() - 1))
{
}

AllTemplateTypes::AllTemplateTypes(const AbstractParserTables &tables)
  : m_symbolType(    findIntType(0, tables.getSymbolCount()      - 1))
  , m_termType(      findIntType(0, tables.getTermCount()        - 1))
  , m_ntIndexType(   findIntType(0, tables.getNTermCount()       - 1))
  , m_stateType(     findIntType(0, tables.getStateCount()       - 1))
  , m_productionType(findIntType(0, tables.getProductionCount()  - 1))
{
}
