#pragma once

#include "StateActionInfo.h"

class StateActionInfoArray : public CompactArray<const StateActionInfo*>, public SymbolNameContainer {
private:
  const StringArray m_symbolNameArray;
public:
  StateActionInfoArray(const GrammarTables &tables);
  ~StateActionInfoArray() override;
  void clear() override;
  String toString() const;
  const TCHAR *getSymbolName(UINT symbolIndex) const override {
    return m_symbolNameArray[symbolIndex].cstr();
  }
};
