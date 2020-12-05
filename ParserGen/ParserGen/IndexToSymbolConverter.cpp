#include "stdafx.h"
#include <AbstractSymbolNameContainer.h>
#include "IndexToSymbolConverter.h"

IndexToSymbolConverter::IndexToSymbolConverter(const AbstractSymbolNameContainer &nameContainer, bool rowsIndexedByTerminal)
  : m_rowsIndexedByTerminal(rowsIndexedByTerminal)
  , m_startSymbol(rowsIndexedByTerminal ? 0 : nameContainer.getTermCount())
  , m_indexCount(rowsIndexedByTerminal ? nameContainer.getTermCount() : nameContainer.getNTermCount())
{
}
