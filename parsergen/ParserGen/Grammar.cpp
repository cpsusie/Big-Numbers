#include "stdafx.h"

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

GrammarSymbol::GrammarSymbol(UINT index, const String &name, SymbolType type, int precedence, const SourcePosition &pos, UINT termCount)
: m_index( index    )
, m_name(  name     )
, m_type(  type     )
, m_first1(termCount)
, m_pos(   pos      )
{
  if((type == NONTERMINAL) && (termCount == 0)) {
    throwException(_T("Cannot add nonterminal <%s> when terminalCount == 0"), name.cstr());
  }

  m_precedence    = precedence;
  m_reachable     = false;
  m_terminate     = false;
  m_deriveEpsilon = false;
}

const TCHAR *GrammarSymbol::getTypeString() const {
  switch(m_type) {
  case LEFTASSOC_TERMINAL  : return _T("left assoc" );
  case RIGHTASSOC_TERMINAL : return _T("right assoc");
  case NONASSOC_TERMINAL   : return _T("non assoc"  );
  case NONTERMINAL         : return _T("nonterminal");
  case TERMINAL            : return _T("terminal"   );
  }
  return _T("?");
}

static inline bool equalCore(const LR1Item &i1, const LR1Item &i2) {
  return (i1.m_prod == i2.m_prod) && (i1.m_dot == i2.m_dot);
}

static int coreCmp(const LR1State &s1, const LR1State &s2) { // compare CORE(s1) with CORE(s2)
  const BYTE n = s1.m_kernelItemCount;
  if(n != s2.m_kernelItemCount) {
    return false;
  }
  for(BYTE i = 0; i < n; i++) {
    if(!equalCore(s1.m_items[i], s2.m_items[i])) {
      return false;
    }
  }
  return true;
}

// used by hashmap
class StateCoreComparator : public Comparator<const LR1State *> {
public:
  int compare(const LR1State * const &s1, const LR1State * const &s2) override {
    return coreCmp(*s1, *s2) ? 0 : 1;
  }
  AbstractComparator *clone() const override {
    return new StateCoreComparator();
  }
};


static ULONG stateCoreHashFunction(const LR1State * const &s) { // used by hashmap
  const LR1State &state = *s;
  const BYTE      n     = state.m_kernelItemCount;
  ULONG           v     = n;
  for(BYTE i = 0; i < n; i++) {
    v ^= state.m_items[i].coreHashCode();
    v *= 13;
  }
  return v;
}

StateHashMap::StateHashMap(size_t capacity)
  : HashMap(stateCoreHashFunction, StateCoreComparator(), capacity)
{
}

LR1Item *LR1State::findItemWithSameCore(const LR1Item &item) {
  const UINT n = (UINT)m_items.size();
  for(UINT i = 0; i < n; i++) {
    if(equalCore(m_items[i], item)) {
      return &m_items[i];
    }
  }
  return nullptr;
}

static int itemCmp(const LR1Item &e1, const LR1Item &e2) {
  int c = e2.m_kernelItem - e1.m_kernelItem; // used by sortItems. put kernelitems first
  if(c) {
    return c;
  }
  c = (int)e1.m_prod - (int)e2.m_prod;       // then sort by production
  return c ? c : (e1.m_dot - e2.m_dot);      // and by dot
}

void LR1State::sortItems() {
  m_items.sort(itemCmp);
}

Grammar::Grammar()
: m_stateMap(              4001)
, m_unfinishedSet(         1001)
, m_symbolMap(             1001)
, m_termCount(                0)
, m_startSymbol(              0)
, m_startState(               0)
, m_termPermutationDone(  false)
, m_termBitSetCapacity(       0)
, m_statePermutationDone( false)
{
}

Grammar::Grammar(const AbstractParserTables &src)
: m_stateMap(              4001)
, m_unfinishedSet(         1001)
, m_symbolMap(             1001)
, m_termCount(                0)
, m_startSymbol(              0)
, m_startState(               0)
, m_termPermutationDone(   true)
, m_termBitSetCapacity(       0)
, m_statePermutationDone(  true)
{
  SourcePosition dummyPos;
  for(UINT t = 0; t < src.getTermCount(); t++) {
    addTerm(src.getSymbolName(t), TERMINAL, 0, dummyPos);
  }
  for(UINT nt = src.getTermCount(); nt < src.getSymbolCount(); nt++) {
    addNTerm(src.getSymbolName(nt), dummyPos);
  }
  for(UINT p = 0; p < src.getProductionCount(); p++) {
    Production production(src.getLeftSymbol(p), dummyPos);
    UINT rightSide[256]; // guess there is no more than 256 symbols on the rightside of any Production
    src.getRightSide(p, rightSide);
    for(UINT s = 0; s < src.getProductionLength(p); s++) {
      production.m_rightSide.add(RightSideSymbol(rightSide[s], NO_MODIFIER));
    }
    addProduction(production);
  }
}

void Grammar::setName(const String &name) {
  m_name = name;
}

int Grammar::getSymbolIndex(const String &name) const {
  const UINT *id = m_symbolMap.get(name.cstr());
  return id != nullptr ? *id : -1;
}

UINT Grammar::addSymbol(const GrammarSymbol &symbol) {
  const UINT index = getSymbolCount();
  m_symbolArray.add(symbol);
  m_symbolMap.put(m_symbolArray.last().m_name.cstr(), index);
  return index;
}

UINT Grammar::addTerm(const String &name, SymbolType type, int precedence, const SourcePosition &pos) {
  m_termCount++;
  m_termBitSetCapacity++;
  return addSymbol(GrammarSymbol(getSymbolCount(), name, type, precedence, pos, 1));
}

UINT Grammar::addNTerm(const String &name, const SourcePosition &pos) {
  return addSymbol(GrammarSymbol(getSymbolCount(), name, NONTERMINAL, 0, pos, getTermCount()));
}

void Grammar::addProduction(const Production &production) {
  if(getProductionCount() == 0) { // is this the first production
    m_startSymbol = production.m_leftSide;
  }

  const UINT prodIndex = getProductionCount();
  m_productions.add(production);
  getSymbol(production.m_leftSide).m_leftSideOf.add(prodIndex);
}

void Grammar::addClosureProductions() {
  bool stable;
  do {
    stable = true;
    for(UINT p = 0; p < getProductionCount(); p++) {
      Production &prod = m_productions[p];
      const UINT length = prod.getLength();
      for(UINT s = 0; s < length; s++) {
        RightSideSymbol &symbol = prod.m_rightSide[s];
        switch(symbol.m_modifier) {
        case NO_MODIFIER:
          continue;
        case ZEROORONE  : // ?
          { Production copy(prod);
            symbol.m_modifier = NO_MODIFIER;
            copy.m_rightSide.remove(s);
            addProduction(copy);
            stable = false;
          }
          break;
        case ZEROORMANY : // *
        case ONEORMANY  : // +
          { GrammarSymbol &m = getSymbol(symbol);
            String ntName = m.m_name + _T("_plus");
            int nt = getSymbolIndex(ntName);
            if(nt < 0) {
              nt = addNTerm(ntName, prod.m_pos);
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
  const UINT length = prod.getLength();
  for(UINT s = 0; s < length; s++) {
    if(!getSymbol(prod.m_rightSide[s]).m_terminate) {
      return false;
    }
  }
  return true;
}

bool Grammar::deriveEpsilon(const Production &prod) const {
  const UINT length = prod.getLength();
  for(UINT s = 0; s < length; s++) {
    if(!getSymbol(prod.m_rightSide[s]).m_deriveEpsilon) {
      return false;
    }
  }
  return true;
}

void Grammar::findReachable() {
  getSymbol(getStartSymbol()).m_reachable = true;
  const UINT n = getProductionCount();
  bool stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(UINT p = 0; p < n; p++) {
      const Production &prod = getProduction(p);
      if(getSymbol(prod.m_leftSide).m_reachable) {
        const UINT length = prod.getLength();
        for(UINT s = 0; s < length; s++) {
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
  for(UINT s = 0; s < getTermCount(); s++) { // all terminals terminate
    getSymbol(s).m_terminate = true;
  }
  const UINT n = getProductionCount();
  bool stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(UINT p = 0; p < n; p++) {
      const Production &prod = getProduction(p);
      if(!getSymbol(prod.m_leftSide).m_terminate && canTerminate(prod)) {
//          _tprintf(_T("%s can terminate\n"), getSymbol(prod.m_leftSide).m_name.cstr());
        getSymbol(prod.m_leftSide).m_terminate = true;
        stable = false;
      }
    }
  } while(!stable);
}

void Grammar::findEpsilonSymbols() {
  const UINT n = getProductionCount();
  bool stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(UINT p = 0; p < n; p++) {
      const Production &prod = getProduction(p);
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
  const UINT n = getProductionCount();
  bool stable;
  do {
    stable = true;
    for(UINT p = 0; p < n; p++) {
      const Production &prod   = getProduction(p);
      const UINT        length = prod.getLength();
      GrammarSymbol    &ls     = getSymbol(prod.m_leftSide);
      for(UINT k = 0; k < length; k++) {
        const UINT sk = prod.m_rightSide[k];
        if(isTerminal(sk)) {
          if(!ls.m_first1.contains(sk)) {
            ls.m_first1.add(sk);
//            _tprintf(_T("first1(%s) : "), ls.m_name.cstr()); dump(ls.m_first1); printf(_T(")\n");
            stable = false;
          }
          break;
        } else { // nonterminal
          const GrammarSymbol &nt = getSymbol(sk);
          const TermSet        diff(nt.m_first1 - ls.m_first1);
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
  for(UINT nterm = getTermCount(); nterm < getSymbolCount(); nterm++) {
    const GrammarSymbol &NTSymbol = getSymbol(nterm);
    _tprintf(_T("first(%-10s):"), NTSymbol.m_name.cstr());
    dump(NTSymbol.m_first1);
    _tprintf(_T("\n"));
  }
  pause();
*/
  } while(!stable);
}

// assume item = A -> alfa . B beta [la]
// computes first1(beta la)
TermSet Grammar::first1(const LR1Item &item) const {
  const Production &prod   = getProduction(item.m_prod);
  const UINT        length = prod.getLength();
  TermSet           result(getTermCount());
  for(UINT k = item.m_dot+1; k < length; k++) {
    const UINT symbol = prod.m_rightSide[k];
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

int Grammar::findStateWithSameCore(const LR1State &state) const {
  const UINT *i = m_stateMap.get(&state);
  return i ? *i : -1;
}

bool Grammar::isShiftItem(const LR1Item &item) const { // is item = "A -> alfa . a beta [la]"
  const Production &prod = getProduction(item.m_prod);
  return (item.m_dot < prod.getLength()) && isTerminal(prod.m_rightSide[item.m_dot]);
}

UINT Grammar::addState(const LR1State &state) {
  const UINT stateIndex = getStateCount();
  m_states.add(state);
  m_stateMap.put(&m_states.last(), stateIndex);
  m_succStateSetInterval.setTo(stateIndex + 1);
  m_shiftStateSetInterval.setTo(stateIndex + 1);
  return stateIndex;
}

void Grammar::computeClosure(LR1State &state, bool allowNewItems) {
  bool changed;
  do {
    changed = false;
    for(size_t i = 0; i < state.m_items.size(); i++) {
      const LR1Item    &item = state.m_items[i];
      const Production &prod = getProduction(item.m_prod);
      if(item.m_dot < prod.getLength() && isNonTerminal(prod.m_rightSide[item.m_dot])) { // item is A -> alfa . B beta [la]
        const GrammarSymbol &B = getSymbol(prod.m_rightSide[item.m_dot]);
        const TermSet la(first1(item));
        for(size_t p = 0; p < B.m_leftSideOf.size(); p++) {
          const LR1Item newItem(false, B.m_leftSideOf[p], 0, la); // newItem is B -> . gamma [first1(beta la)] (nonkernelitem)
          LR1Item *oldItem = state.findItemWithSameCore(newItem);
          if(oldItem == nullptr) {
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
  for(BYTE i = 0; i < src.m_kernelItemCount; i++) {
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
  const UINT itemCount = (UINT)state.m_items.size();
  BitSet itemsDone(itemCount); // this is correct!
  for(UINT i = 0; i < itemCount; i++) {
    if(itemsDone.contains(i)) {
      continue;
    }
    const LR1Item &origItem = state.m_items[i];
    if(isAcceptItem(origItem)) {
      continue; // No successor, but accept
    }
    const Production &origProduction = getProduction(origItem.m_prod);
    if(origItem.m_dot < origProduction.getLength()) { // origItem is A -> alfa . X beta [la]
      BitSet successorItems(itemCount);
      successorItems += i;
      UINT symbolNr = origProduction.m_rightSide[origItem.m_dot];
      LR1State newState(getStateCount());

      // newItem is A -> alfa X . beta [la] (kernelItem)
      newState.addItem(LR1Item(true, origItem.m_prod, origItem.m_dot+1, origItem.m_la));
      itemsDone += i;
      for(UINT k = i+1; k < itemCount; k++) {
        if(itemsDone.contains(k)) {
          continue;
        }
        const LR1Item    &sameSymbolItem = state.m_items[k];
        const Production &sameSymbolProd = getProduction(sameSymbolItem.m_prod);
        if(sameSymbolItem.m_dot < sameSymbolProd.getLength()
          && sameSymbolProd.m_rightSide[sameSymbolItem.m_dot] == symbolNr) { // sameSymbolItem is C -> gamma . X zeta [la1]
          // newItem1 is C -> gamma X . zeta [la1] (kernelItem)
          newState.addItem(LR1Item(true, sameSymbolItem.m_prod, sameSymbolItem.m_dot+1, sameSymbolItem.m_la));
          itemsDone      += k;
          successorItems += k;
        }
      }
      newState.sortItems();
      int succStateIndex = findStateWithSameCore(newState);
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

      for(auto it = successorItems.getIterator(); it.hasNext();) {
        state.m_items[it.next()].m_succ = succStateIndex;
      }
    }
  }
}

void Grammar::generateStates() {
  verbose(2, _T("Computing FIRST1 sets\n"));
  findFirst1Sets();
  m_stateMap.clear();
  m_unfinishedSet.clear();

  TermSet eoiset(getTermCount());
  eoiset.add(0); // EOI
  LR1Item initialItem(true, 0, 0, eoiset);
  LR1State initialState(0);
  initialState.addItem(initialItem);

  computeClosure(initialState, true);
  addState(initialState);
//  dump(initialState);
  verbose(2, _T("Computing LALR(1) states\n"));

  for(UINT i = 0; i < getStateCount(); i++) {
    verbose(2, _T("state %u\r"), i);
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

  m_result.clear(getStateCount());
  for(UINT s = 0; s < getStateCount(); s++) {
    checkStateIsConsistent(s);
  }
}

ConflictSolution Grammar::resolveShiftReduceConflict(const GrammarSymbol &terminal, const LR1Item &item) const {
  short productionPrecedence = getProduction(item.m_prod).m_precedence;
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

void Grammar::checkStateIsConsistent(UINT stateIndex) {
  const LR1State &state = m_states[stateIndex];
  m_result.m_stateResult.add(StateResult(stateIndex));
  StateResult &sr = m_result.m_stateResult[stateIndex];

  TermSet symbolsDone(getSymbolCount());
  const UINT itemCount = (UINT)state.m_items.size();

  for(UINT i = 0; i < itemCount; i++) {
    const LR1Item &item = state.m_items[i];
    if(isShiftItem(item)) {
      const UINT t = getShiftSymbol(item);
      if(symbolsDone.contains(t)) {
        continue;
      }
      for(UINT j = 0; j < itemCount; j++) {
        if(j == i) {
          continue;
        }
        const LR1Item &item1 = state.m_items[j];
        if(isReduceItem(item1) && item1.m_la.contains(t)) {
          const GrammarSymbol &terminal = getSymbol(t);
          switch(resolveShiftReduceConflict(terminal, item1)) {
          case CONFLICT_NOT_RESOLVED:
            m_result.addSRError(_T("Shift/reduce conflict. Shift or reduce by prod %-3u (prec=%d) on '%s' (prec=%d, %s).")
                                  ,item1.m_prod
                                  ,getProduction(item1.m_prod).m_precedence
                                  ,terminal.m_name.cstr()
                                  ,terminal.m_precedence
                                  ,terminal.getTypeString());
            break;
          case CHOOSE_SHIFT:
            sr.m_actions.add(ParserAction(t, item.getSuccessor()));
            symbolsDone += t;
            m_result.addWarning(_T("Shift/reduce conflict on %s (prec=%d, %s). Choose shift instead of reduce by prod %u (prec=%d).")
                               ,terminal.m_name.cstr()
                               ,terminal.m_precedence
                               ,terminal.getTypeString()
                               ,item1.m_prod
                               ,getProduction(item1.m_prod).m_precedence);
            break;
          case CHOOSE_REDUCE:
            sr.m_actions.add(ParserAction(t, -((int)(item1.m_prod))));
            symbolsDone += t;
            m_result.addWarning(_T("Shift/reduce conflict on %s (prec=%d, %s). Choose reduce by prod %u (prec=%d).")
                                ,terminal.m_name.cstr()
                                ,terminal.m_precedence
                                ,terminal.getTypeString()
                                ,item1.m_prod
                                ,getProduction(item1.m_prod).m_precedence);
            break;
          }
        }
      }
      if(!symbolsDone.contains(t)) {
        if(item.m_succ >= 0) {
          sr.m_actions.add(ParserAction(t, item.getSuccessor()));
        }
        symbolsDone += t;
        continue;
      }
    }
  }

  for(UINT i = 0; i < itemCount; i++) {
    const LR1Item &itemi = state.m_items[i];
    if(isReduceItem(itemi)) {
      TermSet tokensReducedByOtherItems(getTermCount());
      if(isAcceptItem(itemi)) {  // check if this is start -> S . [EOI]
        sr.m_actions.add(ParserAction(0, 0));
        if(symbolsDone.contains(0)) {
          throwException(_T("Token EOI already done in state %u while generating Acceptitem"), state.m_index);
        }
        symbolsDone += 0;
        continue;
      }
      for(UINT j = 0; j < itemCount; j++) {
        if(j == i) {
          continue;
        }
        const LR1Item &itemj = state.m_items[j];
        if(isReduceItem(itemj)) {
          const TermSet intersection(itemi.m_la & itemj.m_la);
          if(!intersection.isEmpty()) {
            if(itemj.m_prod < itemi.m_prod) {
              tokensReducedByOtherItems += intersection;
              m_result.addWarning(_T("Reduce/reduce conflict on %s between prod %u and prod %u. Choose prod %u.")
                                 ,symbolSetToString(intersection).cstr()
                                 ,itemj.m_prod
                                 ,itemi.m_prod
                                 ,itemj.m_prod);
            }
          }
        }
      }
      TermSet itemTokens(itemi.m_la - tokensReducedByOtherItems);
      for(auto it = itemTokens.getIterator(); it.hasNext(); ) {
        const UINT t = (UINT)it.next();
        if(!symbolsDone.contains(t)) {
          sr.m_actions.add(ParserAction(t, -(int)(itemi.m_prod)));
          symbolsDone += t;
        }
      }
    }
  }

  for(UINT i = 0; i < itemCount; i++) {
    const LR1Item &itemi = state.m_items[i];
    if(!isShiftItem(itemi) && !isReduceItem(itemi)) {
      const UINT nt = getShiftSymbol(itemi);
      if(!symbolsDone.contains(nt)) {
        if(itemi.getSuccessor() >= 0) {
          sr.m_succs.add(SuccessorState((USHORT)nt, (USHORT)itemi.getSuccessor()));
        }
        symbolsDone += nt;
      }
    }
  } // for
  sr.sortArrays();
}

void GrammarResult::addSRError(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  m_stateResult.last().m_errors.add(vformat(format, argptr));
  va_end(argptr);
  m_SRconflicts++;
}

void GrammarResult::addWarning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  m_stateResult.last().m_warnings.add(vformat(format, argptr));
  va_end(argptr);
  m_warningCount++;
}

void StateResult::sortArrays() {
  m_actions.sortByTerm(); // sort actions by symbolnumber (terminal   )
  m_succs.sortByNTerm();  // sort result  by symbolnumber (nonTerminal)
}

UINT Grammar::getItemCount() const {
  size_t count = 0;
  for(auto it = m_states.getIterator(); it.hasNext();) {
    count += it.next().m_items.size();
  }
  return (UINT)count;
}
