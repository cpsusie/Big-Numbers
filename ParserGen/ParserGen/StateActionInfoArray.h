#pragma once

#include "StateActionInfo.h"

class StateActionInfoArray : public CompactArray<const StateActionInfo*> {
private:
  const GrammarTables &m_tables;
public:
  StateActionInfoArray(const GrammarTables &tables);
  ~StateActionInfoArray() override;
  void clear() override;
  String toString() const;
};
