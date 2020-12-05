#include "stdafx.h"
#include "StateResult.h"

void StateResult::sortArrays() {
  m_termActionArray.sortByTerm();      // sort actions by symbolnumber (terminal   )
  m_ntermNewStateArray.sortByNTerm();  // sort result  by symbolnumber (nonTerminal)
}
