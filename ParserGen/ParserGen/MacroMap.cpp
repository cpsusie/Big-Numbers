#include "stdafx.h"
#include "MacroMap.h"

const Macro *MacroMap::findMacroByValue(const String &macroValue) const {
  const UINT *indexp = m_valueMap.get(macroValue);
  return indexp ? &m_macroArray[*indexp] : nullptr;
}

void MacroMap::addMacro(const Macro &m) {
  assert(m.getIndex() >= 0);
  const UINT index = (UINT)m_macroArray.size();
  m_macroArray.add(m);
  m_valueMap.put(m.getValue(), index);
}
