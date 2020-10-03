#include "stdafx.h"
#include <Console.h>
#include <assert.h>

static String getModifierString(SymbolModifier modifier) {
  switch(modifier) {
  case NO_MODIFIER       : return EMPTYSTRING;
  case ZEROORONE         : return _T("?");
  case ZEROORMANY        : return _T("*");
  case ONEORMANY         : return _T("+");
  default                : throwException(_T("Invalid modifier:%d"), modifier);
                           return EMPTYSTRING;
  }
}

GrammarSymbol::GrammarSymbol(int index, const String &name, SymbolType type, int precedence, const SourcePosition &pos, long terminalCount) : m_first1(terminalCount) {
  if(type == NONTERMINAL && terminalCount == 0) {
    throwException(_T("Cannot add nonterminal <%s> when terminalCount == 0"), name.cstr());
  }

  m_index         = index;
  m_name          = name;
  m_type          = type;
  m_precedence    = precedence;
  m_reachable     = false;
  m_terminate     = false;
  m_deriveEpsilon = false;
  m_pos           = pos;

}

const TCHAR *GrammarSymbol::getTypeString() const {
  switch(m_type) {
  case LEFTASSOC_TERMINAL  : return _T("left assoc");
  case RIGHTASSOC_TERMINAL : return _T("right assoc");
  case NONASSOC_TERMINAL   : return _T("non assoc");
  case NONTERMINAL         : return _T("nonterminal");
  case TERMINAL            : return _T("terminal");
  }
  return _T("?");
}

LR1Item::LR1Item(bool kernelItem, short prod, short dot, const BitSet &la) : m_la(la) {
  m_kernelItem = kernelItem;
  m_prod       = prod;
  m_dot        = dot;
  m_succ       = -1;
}

unsigned long LR1Item::coreHashCode() const {
  return ((unsigned long)m_prod << 16) ^ m_dot;
//    m_la.hashval(); DONT USE THIS HERE !!!!
}

static bool equalCore(const LR1Item &i1, const LR1Item &i2) {
  return i1.m_prod == i2.m_prod && i1.m_dot == i2.m_dot;
}

static bool equalCore(const LR1State &e1, const LR1State &e2) { // compare CORE(e1) with CORE(e2)
  const int n = e1.m_noOfKernelItems;
  if(n != e2.m_noOfKernelItems) {
    return false;
  }
  for(int i = 0; i < n; i++) {
    if(!equalCore(e1.m_items[i], e2.m_items[i])) {
      return false;
    }
  }
  return true;
}

static int stateCoreCompareFunction(const LR1State * const &key, const LR1State * const &elem) { // used by hashmap
  return equalCore(*key, *elem) ? 0 : 1;
}

static unsigned long stateCoreHashFunction(const LR1State * const &s) { // used by hashmap
  const LR1State &state = *s;
  const int n = state.m_noOfKernelItems;
  unsigned long v = n;
  for(int i = 0; i < n; i++) {
    v ^= state.m_items[i].coreHashCode();
    v *= 13;
  }
  return v;
}

LR1State::LR1State(int index) {
  m_index           = index;
  m_noOfKernelItems = 0;
}

void LR1State::addItem(const LR1Item &item) {
  m_items.add(item);
  if(item.m_kernelItem) {
    m_noOfKernelItems++;
  }
}

LR1Item *LR1State::findItem(const LR1Item &item) {
  const int n = (int)m_items.size();
  for(int i = 0; i < n; i++) {
    if(equalCore(m_items[i], item)) {
      return &m_items[i];
    }
  }
  return NULL;
}

static int itemCmp(const LR1Item &e1, const LR1Item &e2) {
  int c = e2.m_kernelItem - e1.m_kernelItem; // used by sortItems. put kernelitems first
  if(c) {
    return c;
  }
  c = e1.m_prod - e2.m_prod;                 // then sort by production
  return c ? c : (e1.m_dot - e2.m_dot);      // and by dot
}

void LR1State::sortItems() {
  m_items.sort(itemCmp);
}

Grammar::Grammar(Language language, int verboseLevel)
: m_stateMap(stateCoreHashFunction, stateCoreCompareFunction, 4001)
, m_unfinishedSet(1001) {

  m_language      = language;
  m_verboseLevel  = verboseLevel;
  m_terminalCount = 0;
}

Grammar::Grammar(Language language, const ParserTables &src)
: m_stateMap(stateCoreHashFunction, stateCoreCompareFunction, 4001)
, m_unfinishedSet(1001) {

  m_language      = language;
  m_verboseLevel  = 0;
  m_terminalCount = 0;

  SourcePosition dummyPos;
  for(unsigned int t = 0; t < src.getTerminalCount(); t++) {
    addTerminal(src.getSymbolName(t), TERMINAL, 0, dummyPos);
  }
  for(unsigned int nt = src.getTerminalCount(); nt < src.getSymbolCount(); nt++) {
    addNonTerminal(src.getSymbolName(nt), dummyPos);
  }
  for(unsigned int p = 0; p < src.getProductionCount(); p++) {
    Production production(src.getLeftSymbol(p), dummyPos);
    unsigned int rightSide[256]; // guess there is no more than 256 symbols on the rightside of any Production
    src.getRightSide(p, rightSide);
    for(unsigned int s = 0; s < src.getProductionLength(p); s++) {
      production.m_rightSide.add(RightSideSymbol(rightSide[s], NO_MODIFIER));
    }
    addProduction(production);
  }
}

Grammar::~Grammar() {
}

int Grammar::findSymbol(const String &name) const {
  const int *id = m_symbolMap.get(name);
  return id != NULL ? *id : -1;
}

int Grammar::addSymbol(const GrammarSymbol &symbol) {
  const int index = getSymbolCount();
  m_symbolMap.put(symbol.m_name, index);
  m_symbols.add(symbol);
  return index;
}

int Grammar::addTerminal(const String &name, SymbolType type, int precedence, const SourcePosition &pos) {
  m_terminalCount++;
  return addSymbol(GrammarSymbol(getSymbolCount(), name, type, precedence, pos, 1));
}

int Grammar::addNonTerminal(const String &name, const SourcePosition &pos) {
  return addSymbol(GrammarSymbol(getSymbolCount(), name, NONTERMINAL, 0, pos, getTerminalCount()));
}

void Grammar::addProduction(const Production &production) {
  if(getProductionCount() == 0) { // is this the first production
    m_startSymbol = production.m_leftSide;
  }

  const int prodIndex = getProductionCount();
  m_productions.add(production);
  getSymbol(production.m_leftSide).m_leftSideOf.add(prodIndex);
}

void Grammar::addClosureProductions() {
  bool stable;
  do {
    stable = true;
    for(int p = 0; p < getProductionCount(); p++) {
      Production &prod = m_productions[p];
      const int length = prod.getLength();
      for(int s = 0; s < length; s++) {
        RightSideSymbol &symbol = prod.m_rightSide[s];
        switch(symbol.m_modifier) {
        case NO_MODIFIER:
          continue;
        case ZEROORONE  : // ?
          { Production copy(prod);
            symbol.m_modifier = NO_MODIFIER;
            copy.m_rightSide.removeIndex(s);
            addProduction(copy);
            stable = false;
          }
          break;
        case ZEROORMANY : // *
        case ONEORMANY  : // +
          { GrammarSymbol &m = getSymbol(symbol);
            String ntName = m.m_name + _T("_plus");
            int nt = findSymbol(ntName);
            if(nt < 0) {
              nt = addNonTerminal(ntName, prod.m_pos);
              Production newProd1(nt, prod.m_pos);
              newProd1.m_rightSide.add(RightSideSymbol(nt, NO_MODIFIER));
              newProd1.m_rightSide.add(RightSideSymbol(symbol, NO_MODIFIER));
              addProduction(newProd1);
              Production newProd2(nt, prod.m_pos);
              newProd2.m_rightSide.add(RightSideSymbol(symbol, NO_MODIFIER));
              addProduction(newProd2);
            }
            symbol.m_index    = nt;
            symbol.m_modifier = (symbol.m_modifier == ZEROORMANY) ? ZEROORONE : NO_MODIFIER;
            stable = false;
          }
          break;
        default:
          throwException(_T("Invalid modifier in production %s. (=%d)"), getProductionString(p).cstr(), symbol.m_modifier);
          break;
        } // end switch
      } // end for(s...
    } //end for(p...
  } while(!stable);
}

bool Grammar::canTerminate(const Production &prod) const {
  const int length = prod.getLength();
  for(int s = 0; s < length; s++) {
    if(!getSymbol(prod.m_rightSide[s]).m_terminate) {
      return false;
    }
  }
  return true;
}

bool Grammar::deriveEpsilon(const Production &prod) const {
  const int length = prod.getLength();
  for(int s = 0; s < length; s++) {
    if(!getSymbol(prod.m_rightSide[s]).m_deriveEpsilon) {
      return false;
    }
  }
  return true;
}

void Grammar::findReachable() {
  getSymbol(getStartSymbol()).m_reachable = true;
  const int n = getProductionCount();
  bool stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(int p = 0; p < n; p++) {
      const Production &prod = m_productions[p];
      if(getSymbol(prod.m_leftSide).m_reachable) {
        const int length = prod.getLength();
        for(int s = 0; s < length; s++) {
          GrammarSymbol &rightSymbol = getSymbol(prod.m_rightSide[s]);
          if(!rightSymbol.m_reachable) {
//            printf(_T(")%s is reachable\n", rightSymbol.m_name.cstr());
            rightSymbol.m_reachable = true;
            stable = false;
          }
        }
      }
    }
  } while(!stable);
}

void Grammar::findTerminate() {
  for(int s = 0; s < getTerminalCount(); s++) { // all terminals terminate
    getSymbol(s).m_terminate = true;
  }
  const int n = getProductionCount();
  bool stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(int p = 0; p < n; p++) {
      const Production &prod = m_productions[p];
      if(!getSymbol(prod.m_leftSide).m_terminate && canTerminate(prod)) {
//          _tprintf(_T("%s can terminate\n"), getSymbol(prod.m_leftSide).m_name.cstr());
        getSymbol(prod.m_leftSide).m_terminate = true;
        stable = false;
      }
    }
  } while(!stable);
}

void Grammar::findEpsilonSymbols() {
  const int n = getProductionCount();
  bool stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(int p = 0; p < n; p++) {
      const Production &prod = m_productions[p];
      if(!getSymbol(prod.m_leftSide).m_deriveEpsilon && deriveEpsilon(prod)) {
        getSymbol(prod.m_leftSide).m_deriveEpsilon = true;
//          _tprintf(_T("%s derive eps\n"), getSymbol(prod.m_leftSide).m_name.cstr());
        stable = false;
      }
    }
  } while(!stable);
/*
  for(int s = 0; s < getSymbolCount(); s++)
    if(getSymbol(s).m_deriveEpsilon)
      _tprintf(_T("%s -> eps\n"), getSymbol(s).m_name.cstr());
*/
}

void Grammar::findFirst1Sets() {
  findEpsilonSymbols();
  const int n = getProductionCount();
  bool stable;
  do {
    stable = true;
    for(int p = 0; p < n; p++) {
      const Production &prod = m_productions[p];
      const int length = prod.getLength();
      GrammarSymbol &ls = getSymbol(prod.m_leftSide);
      for(int k = 0; k < length; k++) {
        const int sk = prod.m_rightSide[k];
        if(isTerminal(sk)) {
          if(!ls.m_first1.contains(sk)) {
            ls.m_first1.add(sk);
//            _tprintf(_T("first1(%s) : "), ls.m_name.cstr()); dump(ls.m_first1); printf(_T(")\n");
            stable = false;
          }
          break;
        }
        else { // nonterminal
          const GrammarSymbol &nt = getSymbol(sk);
          const BitSet diff(nt.m_first1 - ls.m_first1);
          if(!diff.isEmpty()) {
            ls.m_first1 += nt.m_first1;
//            printf(_T(")first1(%s) : ", ls.m_name.cstr()); dump(ls.m_first1); printf(_T(")\n");
            stable = false;
          }
          if(!nt.m_deriveEpsilon) {
            break;
          }
        }
      }
    }

/*
  gotoxy(0, 0);
  for(long s = getTerminalCount(); s < getSymbolCount(); s++) {
    GrammarSymbol &nonTerm = getSymbol(s);
    _tprintf(_T("first(%-10s):"), nonTerm.m_name.cstr());
    dump(nonTerm.m_first1);
    _tprintf(_T("\n"));
  }
  pause();
*/
  } while(!stable);
}

int Grammar::getItemCount() const {
  const int n = getStateCount();
  int count = 0;
  for(int i = 0; i < n; i++) {
    count += (int)m_states[i].m_items.size();
  }
  return count;
}

// assume item = A -> alfa . B beta [la]
// computes first1(beta la)
BitSet Grammar::first1(const LR1Item &item) const {
  const Production &prod = getProduction(item);
  const int length = prod.getLength();
  BitSet result(getTerminalCount());
  for(int k = item.m_dot+1; k < length; k++) {
    const int symbol = prod.m_rightSide[k];
    if(isTerminal(symbol)) {
      result += symbol;
      return result;
    } else { // nonterminal
      const GrammarSymbol &nt = getSymbol(symbol);
      result += nt.m_first1;
      if(!nt.m_deriveEpsilon) {
        return result;
      }
    }
  }
  return result += item.m_la;
}

int Grammar::findState(const LR1State &state) const {
  const int *i = m_stateMap.get(&state);
  return i ? *i : -1;
}

bool Grammar::isShiftItem(const LR1Item &item) const { // is item = "A -> alfa . a beta [la]"
  const Production &prod = getProduction(item);
  return item.m_dot < prod.getLength() && isTerminal(prod.m_rightSide[item.m_dot]);
}

bool Grammar::isReduceItem(const LR1Item &item) const { // is item = "A -> alfa . [la]"
  return item.m_dot == getProduction(item).getLength();
}

bool Grammar::isAcceptItem(const LR1Item &item) const {
  return (item.m_prod == 0) && isReduceItem(item);
}

int Grammar::getShiftSymbol(const LR1Item &item) const { // Assume item = "A -> alfa . x beta [la]". Return x
  return getProduction(item).m_rightSide[item.m_dot];
}

int Grammar::addState(const LR1State &state) { // return index of state
  const int stateIndex = getStateCount();
  m_states.add(state);
  m_stateMap.put(&m_states[stateIndex], stateIndex);
  return stateIndex;
}

void Grammar::computeClosure(LR1State &state, bool allowNewItems) {
  bool changed;
  do {
    changed = false;
    for(size_t i = 0; i < state.m_items.size(); i++) {
      const LR1Item &item = state.m_items[i];
      const Production &prod = getProduction(item);
      if(item.m_dot < prod.getLength() && isNonTerminal(prod.m_rightSide[item.m_dot])) { // item is A -> alfa . B beta [la]
        const GrammarSymbol &B = getSymbol(prod.m_rightSide[item.m_dot]);
        const BitSet la(first1(item));
        for(size_t p = 0; p < B.m_leftSideOf.size(); p++) {
          const LR1Item newItem(false, B.m_leftSideOf[p], 0, la); // newItem is B -> . gamma [first1(beta la)] (nonkernelitem)
          LR1Item *oldItem = state.findItem(newItem);
          if(oldItem == NULL) {
            if(!allowNewItems) {
              throwException(_T("Grammar::computeClosure:No new items allowed"));
            }
            state.addItem(newItem);
            changed = true;
          } else {
            if(!(newItem.m_la - oldItem->m_la).isEmpty()) {
              oldItem->m_la += newItem.m_la;
              changed = true;
            }
          }
        }
      }
    }
  } while(changed);
  state.sortItems();
}

bool Grammar::mergeLookahead(LR1State &dst, const LR1State &src) {
  bool changed = false;
  for(int i = 0; i < src.m_noOfKernelItems; i++) {
    LR1Item &dstItem = dst.m_items[i];
    const LR1Item &srcItem = src.m_items[i];

    assert(equalCore(dstItem, srcItem));

    if(!(srcItem.m_la - dstItem.m_la).isEmpty()) {
      dstItem.m_la += srcItem.m_la;
      changed  = true;
    }
  }
  return changed;
}

void Grammar::computeSuccessors(LR1State &state) {
//dump(state); pause();
  BitSet itemsDone(state.m_items.size()); // this is correct!
  for(UINT i = 0; i < state.m_items.size(); i++) {
    if(itemsDone.contains(i)) {
      continue;
    }
    const LR1Item &origItem = state.m_items[i];
    if(isAcceptItem(origItem)) {
      continue; // No successor, but accept
    }
    const Production &origProduction = getProduction(origItem);
    if(origItem.m_dot < origProduction.getLength()) { // origItem is A -> alfa . X beta [la]
      CompactIntArray successorItems;
      successorItems.add(i);
      int symbolNr = origProduction.m_rightSide[origItem.m_dot];
      LR1State newState(getStateCount());
      const LR1Item newItem(true, origItem.m_prod, origItem.m_dot+1, origItem.m_la); // newItem is A -> alfa X . beta [la] (kernelItem)
      newState.addItem(newItem);
      itemsDone += i;
      for(UINT k = i+1; k < state.m_items.size(); k++) {
        if(itemsDone.contains(k)) {
          continue;
        }
        const LR1Item    &sameSymbolItem = state.m_items[k];
        const Production &sameSymbolProd = getProduction(sameSymbolItem);
        if(sameSymbolItem.m_dot < sameSymbolProd.getLength()
          && sameSymbolProd.m_rightSide[sameSymbolItem.m_dot] == symbolNr) { // sameSymbolItem is C -> gamma . X zeta [la1]
          const LR1Item newItem1(true, sameSymbolItem.m_prod, sameSymbolItem.m_dot+1, sameSymbolItem.m_la); // newItem1 is C -> gamma X . zeta [la1] (kernelItem)
          newState.addItem(newItem1);
          itemsDone += k;
          successorItems.add(k);
        }
      }
      newState.sortItems();
      int succStateIndex = findState(newState);
      if(succStateIndex < 0) {
        computeClosure(newState, true);
        succStateIndex = addState(newState);
      } else {
        if(mergeLookahead(m_states[succStateIndex], newState)) {
          m_unfinishedSet.add(succStateIndex);
//          printf(_T(")%d ", succStateIndex);
        }
      }

      assert(succStateIndex >= 0);

      for(size_t j = 0; j < successorItems.size(); j++) {
        state.m_items[successorItems[j]].m_succ = succStateIndex;
      }
    }
  }
}

void Grammar::generateStates() {
  verbose(2, _T("Computing FIRST1 sets\n"));
  findFirst1Sets();
  m_stateMap.clear();
  m_unfinishedSet.clear();

  BitSet eoiset(getTerminalCount());
  eoiset.add(0); // EOI
  LR1Item initialItem(true, 0, 0, eoiset);
  LR1State initialState(0);
  initialState.addItem(initialItem);

  computeClosure(initialState, true);
  addState(initialState);
//  dump(initialState);
  verbose(2, _T("Computing LALR(1) states\n"));

  for(int i = 0; i < getStateCount(); i++) {
    verbose(2, _T("state %d\r"), i);
    computeSuccessors(m_states[i]);
  }

/*
  Array<int> length = m_stateHash.length();
  FILE *ff = fopen("c:\\temp\\hlength.dat", "w");
  for(i = 0; i < length.size(); i++) {
    fprintf(ff, "%d %d\n", i, length[i]);
  }
  fclose(ff);
*/
  while(!m_unfinishedSet.isEmpty()) {
    const int unfinishedState = m_unfinishedSet.select();
    m_unfinishedSet.remove(unfinishedState);
    verbose(2, _T("Closing state %d.\r"), unfinishedState);
    computeClosure(m_states[unfinishedState], false);
    computeSuccessors(m_states[unfinishedState]);
  }

  verbose(2, _T("Checking consistensy\n"));

  m_warningCount = m_SRconflicts = m_RRconflicts = 0;
  for(int i = 0; i < getStateCount(); i++) {
    LR1State &state = m_states[i];
    m_result.add(StateResult());
    checkStateIsConsistent(state, m_result.last());
  }
}

ConflictSolution Grammar::resolveShiftReduceConflict(const GrammarSymbol &terminal, const LR1Item &item) const {
  short productionPrecedence = getProduction(item).m_precedence;
  short terminalPrecedence   = terminal.m_precedence;

  if((productionPrecedence == 0) || (terminalPrecedence == 0)) {
    return CONFLICT_NOT_RESOLVED;
  }

  if( (terminalPrecedence < productionPrecedence)
    || (productionPrecedence == terminalPrecedence && terminal.m_type != RIGHTASSOC_TERMINAL) ) {
    return CHOOSE_REDUCE;
  } else {
    return CHOOSE_SHIFT;
  }
}

static int parserActionCompareToken(const ParserAction &p1, const ParserAction &p2) {
  return p1.m_token - p2.m_token;
}

void Grammar::checkStateIsConsistent(const LR1State &state, StateResult &result) {
  BitSet symbolsDone(getSymbolCount());
  const int itemCount = (int)state.m_items.size();

  for(int i = 0; i < itemCount; i++) {
    const LR1Item &item = state.m_items[i];
    if(isShiftItem(item)) {
      const int t = getShiftSymbol(item);
      if(symbolsDone.contains(t)) {
        continue;
      }
      for(int j = 0; j < itemCount; j++) {
        if(j == i) {
          continue;
        }
        const LR1Item &item1 = state.m_items[j];
        if(isReduceItem(item1) && item1.m_la.contains(t)) {
          const GrammarSymbol &terminal = getSymbol(t);
          switch(resolveShiftReduceConflict(terminal, item1)) {
          case CONFLICT_NOT_RESOLVED:
            m_SRconflicts++;
            result.m_errors.add(format(_T("Shift/reduce conflict. Shift or reduce by prod %-3d (prec=%d) on '%s' (prec=%d, %s).")
                                      ,item1.m_prod
                                      ,getProduction(item1).m_precedence
                                      ,terminal.m_name.cstr()
                                      ,terminal.m_precedence
                                      ,terminal.getTypeString()));
            break;
          case CHOOSE_SHIFT:
            result.m_actions.add(ParserAction(t, item.getSuccessor()));
            symbolsDone += t;
            result.m_warnings.add(format(_T("Shift/reduce conflict on %s (prec=%d, %s). Choose shift instead of reduce by prod %d (prec=%d).")
                                        ,terminal.m_name.cstr()
                                        ,terminal.m_precedence
                                        ,terminal.getTypeString()
                                        ,item1.m_prod
                                        ,getProduction(item1).m_precedence));
            m_warningCount++;
            break;
          case CHOOSE_REDUCE:
            result.m_actions.add(ParserAction(t, -item1.m_prod));
            symbolsDone += t;
            result.m_warnings.add(format(_T("Shift/reduce conflict on %s (prec=%d, %s). Choose reduce by prod %d (prec=%d).")
                                        ,terminal.m_name.cstr()
                                        ,terminal.m_precedence
                                        ,terminal.getTypeString()
                                        ,item1.m_prod
                                        ,getProduction(item1).m_precedence));
            m_warningCount++;
            break;
          }
        }
      }
      if(!symbolsDone.contains(t)) {
        if(item.m_succ >= 0) {
          result.m_actions.add(ParserAction(t, item.getSuccessor()));
        }
        symbolsDone += t;
        continue;
      }
    }
  }

  for(int i = 0; i < itemCount; i++) {
    const LR1Item &itemi = state.m_items[i];
    if(isReduceItem(itemi)) {
      BitSet tokensReducedByOtherItems(getTerminalCount());
      if(isAcceptItem(itemi)) {  // check if this is start -> S . [EOI]
        result.m_actions.add(ParserAction(0, 0));
        if(symbolsDone.contains(0)) {
          throwException(_T("Token EOI already done in state %d while generating Acceptitem"), state.m_index);
        }
        symbolsDone += 0;
        continue;
      }
      for(int j = 0; j < itemCount; j++) {
        if(j == i) {
          continue;
        }
        const LR1Item &itemj = state.m_items[j];
        if(isReduceItem(itemj)) {
          const BitSet intersection(itemi.m_la & itemj.m_la);
          if(!intersection.isEmpty()) {
            if(itemj.m_prod < itemi.m_prod) {
              tokensReducedByOtherItems += intersection;
              result.m_warnings.add(format(_T("Reduce/reduce conflict on %s between prod %d and prod %d. Choose prod %d.")
                                          ,symbolSetToString(intersection).cstr()
                                          ,itemj.m_prod
                                          ,itemi.m_prod
                                          ,itemj.m_prod));
              m_warningCount++;
            }
          }
        }
      }
      BitSet itemTokens(itemi.m_la - tokensReducedByOtherItems);
      for(Iterator<size_t> it = itemTokens.getIterator(); it.hasNext(); ) {
        const unsigned short t = (unsigned short)it.next();
        if(!symbolsDone.contains(t)) {
          result.m_actions.add(ParserAction(t, -itemi.m_prod));
          symbolsDone += t;
        }
      }
    }
  }

  for(int i = 0; i < itemCount; i++) {
    const LR1Item &itemi = state.m_items[i];
    if(!isShiftItem(itemi) && !isReduceItem(itemi)) {
      const int nt = getShiftSymbol(itemi);
      if(!symbolsDone.contains(nt)) {
        if(itemi.getSuccessor() >= 0) {
          result.m_succs.add(ParserAction(nt, itemi.getSuccessor()));
        }
        symbolsDone += nt;
      }
    }
  } // for

  result.m_actions.sort(parserActionCompareToken); // sort actions by symbolnumber (lookahead symbol)
  result.m_succs.sort(  parserActionCompareToken); // sort result by symbolnumber  (nonTerminal)
}

void Grammar::dump(const Production &prod, MarginFile *f) const {
  fprintf(f, _T("%s -> "), getSymbol(prod.m_leftSide).m_name.cstr());
  for(int i = 0; i < prod.getLength(); i++) {
    fprintf(f, _T("%s "), getSymbol(prod.m_rightSide[i]).m_name.cstr());
  }
  fprintf(f, _T("prec %d"), prod.m_precedence);
}

String Grammar::getRightSide(int p) const {
  const Production &prod = m_productions[p];
  const int n = prod.getLength();

  if(n == 0) {
    return _T("epsilon");
  }

  String result;
  for(int i = 0; i < n; i++) {
    const int s = prod.m_rightSide[i];
    if(i > 0) {
      result += _T(" ");
    }
    result += getSymbol(s).m_name;
  }
  return result;
}

String Grammar::getProductionString(int prod) const {
  return getSymbol(m_productions[prod].m_leftSide).m_name
       + _T(" -> ")
       + getRightSide(prod);
}

int Grammar::getMaxSymbolNameLength() const {
  const int n = getSymbolCount();
  int m = 0;
  for(int i = 0; i < n; i++) {
    const int l = (int)getSymbol(i).m_name.length();
    if(l > m) {
      m = l;
    }
  }
  return m;
}

int Grammar::getMaxNonTerminalNameLength() const {
  const int n = getSymbolCount();
  int m = 0;
  for(int i = getTerminalCount(); i < n; i++) {
    const int l = (int)getSymbol(i).m_name.length();
    if(l > m) {
      m = l;
    }
  }
  return m;
}

String Grammar::symbolSetToString(const BitSet &set) const {
  String result;
  Iterator<size_t> it = set.getIterator();
  int i = 0;
  while(it.hasNext()) {
    const int s = (int)it.next();
    if(i++ != 0) {
      result += _T(" ");
    }
    result += getSymbol(s).m_name;
  }
  return String(_T("[")) + result + _T("]");
}

String Grammar::itemToString(const LR1Item &item, int flags) const {
  String result;
  const Production &prod = getProduction(item);
  result = format(_T(" (%3d)%c %-15s -> "), item.m_prod, item.m_kernelItem?'K':' ', getSymbol(prod.m_leftSide).m_name.cstr());
  for(int i = 0; i < item.m_dot; i++) {
    result += getSymbol(prod.m_rightSide[i]).m_name;
    result += _T(" ");
  }
  result += _T(".");
  const int n = prod.getLength();
  const TCHAR *delimiter = EMPTYSTRING;
  for(int i = item.m_dot; i < n; i++, delimiter = _T(" ")) {
    result += delimiter;
    result += getSymbol(prod.m_rightSide[i]).m_name;
  }
  if(flags & DUMP_LOOKAHEAD) {
    result += symbolSetToString(item.m_la);
  }

  if((flags & DUMP_SUCC) && (item.m_succ >= 0)) {
    result += format(_T(" -> %d"), item.getSuccessor()); // ie not reduce-item
  }
  return result;
}

String Grammar::stateToString(const LR1State &state, int flags) const {
  String result;
  result = format(_T("State %d:\n"), state.m_index);
  const int itemstodump = (flags & DUMP_KERNELONLY) ? state.m_noOfKernelItems : (int)state.m_items.size();
  for(int i = 0; i < itemstodump; i++) {
    const LR1Item &item = state.m_items[i];
    if(isShiftItem(item) && !(flags & DUMP_SHIFTITEMS)) {
      continue;
    }
    result += itemToString(state.m_items[i], flags) + _T("\n");
  }
  if(flags & DUMP_ACTIONS) {
    result += _T("\n");
    const ActionArray &actions    = m_result[state.m_index].m_actions;
    const ActionArray &successors = m_result[state.m_index].m_succs;
    for(size_t i = 0; i < actions.size(); i++) {
      const ParserAction &act = actions[i];
      if(act.m_action > 0) { // shiftItem
        result += format(_T("   shift to %d on %s\n"), act.m_action, getSymbol(act.m_token).m_name.cstr());
      } else if(act.m_action < 0) { // reduceItem
        result += format(_T("   reduce by %d on %s\n"), -act.m_action, getSymbol(act.m_token).m_name.cstr());
      } else { // accept
        result += format(_T("   accept on %s\n"), getSymbol(act.m_token).m_name.cstr());
      }
    }
    for(size_t i = 0; i < successors.size(); i++) {
      const ParserAction &act = successors[i];
      result += format(_T("   goto %d on %s\n"), act.m_action, getSymbol(act.m_token).m_name.cstr());
    }
  }
  return result;
}

void Grammar::dump(const BitSet &set, MarginFile *f) const {
  f->printf(_T("%s"), symbolSetToString(set).cstr());
}

void Grammar::dump(const LR1Item &item, int flags, MarginFile *f) const {
  f->printf(_T("%s\n"), itemToString(item, flags).cstr());
}

void Grammar::dump(const LR1State &state, int flags, MarginFile *f) const {
  f->printf(_T("%s\n"), stateToString(state, flags).cstr());
  if(flags & DUMP_ERRORS) {
    const StringArray &errors = m_result[state.m_index].m_errors;
    for(size_t i = 0; i < errors.size(); i++) {
      _tprintf(_T("%s(%d) : error --- %s\n"), f->getAbsolutName().cstr(), f->getLineNumber(), errors[i].cstr());
      f->printf(_T("error --- %s\n"), errors[i].cstr());
    }
  }
  if(flags & DUMP_WARNINGS) {
    const StringArray &warnings = m_result[state.m_index].m_warnings;
    for(size_t i = 0; i < warnings.size(); i++) {
      verbose(2, _T("%s(%d) : warning --- %s\n"), f->getAbsolutName().cstr(), f->getLineNumber(), warnings[i].cstr());
      f->printf(_T("warning --- %s\n"), warnings[i].cstr());
    }
  }
}

void Grammar::dumpStates(int flags, MarginFile *f) const {
  for(int i = 0; i < getStateCount(); i++) {
    dump(m_states[i], flags, f);
  }
}

void Grammar::dump(MarginFile *f) const {
  for(int i = 0; i < getSymbolCount(); i++) {
    const GrammarSymbol &sym = getSymbol(i);
    f->printf(_T("Symbol:%-20s, %4d %-11s "), sym.m_name.cstr(), sym.m_precedence, sym.getTypeString());
    if(sym.m_reachable    ) f->printf(_T("reachable "));
    if(sym.m_terminate    ) f->printf(_T("terminate "));
    if(sym.m_deriveEpsilon) f->printf(_T("derive e "));
    if(isNonTerminal(i)) {
      dump(sym.m_first1);
    }
    f->printf(_T("\n"));
  }

  for(int i = 0; i < getProductionCount(); i++) {
    dump(m_productions[i], f);
    f->printf(_T("\n"));
  }
}

void Grammar::dumpFirst1Sets(FILE *f) const {
  const int maxLength = getMaxNonTerminalNameLength() + 1;
  for(int i = getTerminalCount(); i < getSymbolCount(); i++) {
    const GrammarSymbol &nt = getSymbol(i);
    _ftprintf(f, _T("%-*.*s:%s\n"), maxLength, maxLength, nt.m_name.cstr(), symbolSetToString(nt.m_first1).cstr());
  }
}

void Grammar::verbose(int level, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
  if(level <= m_verboseLevel) {
    va_list argptr;
    va_start(argptr, format);
    _vtprintf(format, argptr);
    va_end(argptr);
  }
}
