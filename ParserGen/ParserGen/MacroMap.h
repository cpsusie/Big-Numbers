#pragma once

#pragma once

#include "Grammar.h"
#include "Macro.h"

class MacroMap : public SymbolNameContainer {
private:
  const SymbolNameContainer &m_nameContainer;
  Array<Macro>               m_macroArray;
  StringHashMap<UINT>        m_valueMap; // map macro-value -> index into macroArray

public:
  MacroMap(const SymbolNameContainer &nameContainer) : m_nameContainer(nameContainer) {
  }
  const TCHAR *getSymbolName(UINT symbolIndex) const override {
    return m_nameContainer.getSymbolName(symbolIndex);
  }
  UINT         getSymbolCount()                const override {
    return m_nameContainer.getSymbolCount();
  }
  UINT         getTerminalCount()              const override {
    return m_nameContainer.getTerminalCount();
  }

  // Return pointer to macro with same value if it exist, or nullptr if not
  const Macro *findMacroByValue( const String &macroValue) const;
  void         addMacro(         const Macro  &m);
  inline UINT  getMacroCount() const {
    return (UINT)m_macroArray.size();
  }
  inline const Array<Macro> getMacroArray() const {
    return m_macroArray;
  }
};
