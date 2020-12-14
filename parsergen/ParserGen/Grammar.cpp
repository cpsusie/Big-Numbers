#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"

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

const TCHAR *RightSideSymbol::getModifierString(SymbolModifier modifier) { // static
  switch(modifier) {
  case NO_MODIFIER         : return EMPTYSTRING;
  case ZEROORONE           : return _T("?");
  case ZEROORMANY          : return _T("*");
  case ONEORMANY           : return _T("+");
  default                  : throwInvalidArgumentException(__TFUNCTION__,_T("modifier=%u"), modifier);
  }
  return EMPTYSTRING;
}

String RightSideSymbol::toString(const AbstractSymbolNameContainer &nameContainer) const {
  return nameContainer.getSymbolName(m_index) + getModifierString();
}

Grammar::Grammar()
: m_termPermutationDone(  false)
, m_termBitSetCapacity(       0)
, m_statePermutationDone( false)
{
  init();
}

Grammar::Grammar(const Grammar &src)
: m_termPermutationDone(   true)
, m_termBitSetCapacity(       0)
, m_statePermutationDone( false)
, m_name(src.getName()         )
{
  init();
  for(UINT symbolIndex = 0; symbolIndex < src.getSymbolCount(); symbolIndex++) {
    addSymbol(src.getSymbol(symbolIndex));
  }
  for(UINT p = 0; p < src.getProductionCount(); p++) {
    addProduction(Production(this, src.getProduction(p)));
  }
  for(auto statep : src.m_stateArray) {
    addState(fetchState(statep));
  }
  *m_result     = *src.m_result;
  m_driverHead  = src.m_driverHead;
  m_driverTail  = src.m_driverTail;
  m_header      = src.m_header;
  m_startState  = src.m_startState;
  m_startSymbol = src.m_startSymbol;
}

Grammar::Grammar(const AbstractParserTables &src)
: m_termPermutationDone(   true)
, m_termBitSetCapacity(       0)
, m_statePermutationDone( false)
{
  init();
  SourcePosition dummyPos;
  for(UINT term = 0; term < src.getTermCount(); term++) {
    addTerm(src.getSymbolName(term), TERMINAL, 0, dummyPos);
  }
  for(UINT nterm = src.getTermCount(); nterm < src.getSymbolCount(); nterm++) {
    addNTerm(src.getSymbolName(nterm), dummyPos);
  }
  CompactUIntArray rightSide(256);
  rightSide.insert(0, (UINT)0, 256);
  for(UINT p = 0; p < src.getProductionCount(); p++) {
    Production production(this, src.getLeftSymbol(p), dummyPos);
    const size_t length = src.getProductionLength(p);
    if(length > rightSide.size()) {
      rightSide.insert(rightSide.size(), (UINT)0, length - rightSide.size());
    }
    src.getRightSide(p, rightSide.begin());
    for(size_t i = 0; i < length; i++) {
      production.m_rightSide.add(RightSideSymbol(rightSide[i], NO_MODIFIER));
    }
    addProduction(production);
  }
}

void Grammar::init() {
  m_termCount     = 0;
  m_startSymbol   = 0;
  m_startState    = 0;

  initFreeLists();
  m_result = new GrammarResult(); TRACE_NEW(m_result);
}

Grammar::~Grammar() {
  SAFEDELETE(m_result);
  for(auto statep : m_stateArray) {
    releaseState(statep);
  }
  m_stateArray.clear();
  clearStateList();
  clearItemList();
}

UINT Grammar::addSymbol(const GrammarSymbol &symbol) {
  const UINT index = getSymbolCount();
  m_symbolArray.add(symbol);
  m_symbolMap.put(m_symbolArray.last().m_name.cstr(), index);
  if(symbol.getType() != NONTERMINAL) {
    m_termCount++;
    m_termBitSetCapacity++;
  }
  return index;
}

UINT Grammar::addTerm(const String &name, SymbolType type, int precedence, const SourcePosition &pos) {
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
  m_productionArray.add(production);
  getSymbol(production.m_leftSide).m_leftSideOf.add(prodIndex);
}

void Grammar::addClosureProductions() {
  bool stable;
  do {
    stable = true;
    for(UINT p = 0; p < getProductionCount(); p++) {
      Production &prod   = m_productionArray[p];
      const UINT  length = prod.getLength();
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
          { GrammarSymbol &m      = getSymbol(symbol);
            const String   ntName = m.m_name + _T("_plus");
            int            nt     = getSymbolIndex(ntName);
            if(nt < 0) {
              nt = addNTerm(ntName, prod.m_pos);
              Production newProd1(this, nt, prod.m_pos);
              newProd1.m_rightSide.add(RightSideSymbol(nt, NO_MODIFIER));
              newProd1.m_rightSide.add(RightSideSymbol(symbol, NO_MODIFIER));
              addProduction(newProd1);
              Production newProd2(this, nt, prod.m_pos);
              newProd2.m_rightSide.add(RightSideSymbol(symbol, NO_MODIFIER));
              addProduction(newProd2);
            }
            symbol.m_index    = nt;
            symbol.m_modifier = (symbol.m_modifier == ZEROORMANY) ? ZEROORONE : NO_MODIFIER;
            stable = false;
          }
          break;
        default:
          throwException(_T("Invalid modifier in production %s. (=%d)"), prod.toString().cstr(), symbol.m_modifier);
          break;
        } // end switch
      } // end for(s...
    } //end for(p...
  } while(!stable);
}

void Grammar::findReachable() {
  getSymbol(getStartSymbol()).m_reachable = true;
  const UINT n = getProductionCount();
  bool       stable;
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
  bool       stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(UINT p = 0; p < n; p++) {
      const Production &prod = getProduction(p);
      if(!getSymbol(prod.m_leftSide).m_terminate && prod.canTerminate()) {
//          _tprintf(_T("%s can terminate\n"), getSymbol(prod.m_leftSide).m_name.cstr());
        getSymbol(prod.m_leftSide).m_terminate = true;
        stable = false;
      }
    }
  } while(!stable);
}

void Grammar::findEpsilonSymbols() {
  const UINT n = getProductionCount();
  bool       stable;
  do {
//    gotoxy(0, 0); dump(); pause();
    stable = true;
    for(UINT p = 0; p < n; p++) {
      const Production &prod = getProduction(p);
      if(!getSymbol(prod.m_leftSide).m_deriveEpsilon && prod.deriveEpsilon()) {
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

void Grammar::computeClosure(LR1State *state, bool allowNewItems) {
  bool changed;
  do {
    changed = false;
    for(size_t i = 0; i < state->m_items.size(); i++) {
      const LR1Item    *item = state->m_items[i];
      const Production &prod = getProduction(item->m_prod);
      if(item->m_dot < prod.getLength()) {
        const UINT symbol = prod.m_rightSide[item->m_dot];
        if(isNonTerminal(symbol)) { // item is A -> alfa . B beta [la]
          const GrammarSymbol &B = getSymbol(symbol);
          const TermSet        la(item->first1());
          for(UINT p : B.m_leftSideOf) {
            LR1Item *newItem = fetchItem(false, p, 0, la); // newItem is B -> . gamma [first1(beta la)] (nonkernelitem)
            LR1Item *oldItem = state->findItemWithSameCore(newItem);
            if(oldItem == nullptr) {
              if(!allowNewItems) {
                throwException(_T("%s:No new items allowed"), __TFUNCTION__);
              }
              state->addItem(newItem);
              changed = true;
            } else {
              if(!(newItem->m_la - oldItem->m_la).isEmpty()) {
                oldItem->m_la += newItem->m_la;
                changed = true;
              }
              releaseItem(newItem);
            }
          } // for
        } // if
      } // if
    } // for
  } while(changed);
  if(allowNewItems) {
    state->sortItems();
  }
}

void Grammar::computeSuccessors(LR1State *state) {
  //dump(state); pause();
  const UINT itemCount = (UINT)state->m_items.size();
  BitSet     itemsDone(itemCount); // this is correct!
  for(UINT i = 0; i < itemCount; i++) {
    if(itemsDone.contains(i)) {
      continue;
    }
    const LR1Item &origItem = *state->m_items[i];
    if(origItem.isAcceptItem()) {
      continue; // No successor, but accept
    }
    const Production &origProduction = getProduction(origItem.m_prod);
    if(origItem.m_dot < origProduction.getLength()) { // origItem is A -> alfa . X beta [la]
      BitSet    successorItems(itemCount);
      successorItems += i;
      UINT      symbolNr = origProduction.m_rightSide[origItem.m_dot];
      LR1State *newState = fetchState();

      // newItem is A -> alfa X . beta [la] (kernelItem)
      newState->addItem(fetchItem(true, origItem.m_prod, origItem.m_dot+1, origItem.m_la));
      itemsDone += i;
      for(UINT k = i+1; k < itemCount; k++) {
        if(itemsDone.contains(k)) {
          continue;
        }
        const LR1Item    &sameSymbolItem = *state->m_items[k];
        const Production &sameSymbolProd = getProduction(sameSymbolItem.m_prod);
        if(sameSymbolItem.m_dot < sameSymbolProd.getLength()
          && sameSymbolProd.m_rightSide[sameSymbolItem.m_dot] == symbolNr) { // sameSymbolItem is C -> gamma . X zeta [la1]
          // newItem1 is C -> gamma X . zeta [la1] (kernelItem)
          newState->addItem(fetchItem(true, sameSymbolItem.m_prod, sameSymbolItem.m_dot+1, sameSymbolItem.m_la));
          itemsDone      += k;
          successorItems += k;
        }
      }
      newState->sortItems();
      int succStateIndex = m_stateArray.getIndex(newState);
      if(succStateIndex < 0) {
        computeClosure(newState, true);
        succStateIndex = addState(newState);
      } else {
        if(m_stateArray[succStateIndex]->mergeLookahead(newState)) {
          m_unfinishedSet.add(succStateIndex);
//          printf(_T(")%d ", succStateIndex);
        }
        releaseState(newState);
      }
      assert(succStateIndex >= 0);
      for(auto it = successorItems.getIterator(); it.hasNext();) {
        state->m_items[it.next()]->m_newState = succStateIndex;
      }
    }
  }
}

void Grammar::generateStates() {
  verbose(2, _T("Computing FIRST1 sets\n"));
  findFirst1Sets();
  m_stateArray.clear();
  m_unfinishedSet.clear();

  TermSet eoiset(getTermCount());
  eoiset.add(0); // EOI
  LR1State *initialState = fetchState();
  initialState->addItem(fetchItem(true, 0, 0, eoiset));

  computeClosure(initialState, true);
  addState(initialState);
//  dump(initialState);
  verbose(2, _T("Computing LALR(1) states\n"));

  for(UINT i = 0; i < getStateCount(); i++) {
    verbose(2, _T("state %u\r"), i);
    computeSuccessors(m_stateArray[i]);
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
    const UINT unfinishedState = m_unfinishedSet.select();
    m_unfinishedSet.remove(unfinishedState);
    verbose(2, _T("Closing state %u.\r"), unfinishedState);
    computeClosure(   m_stateArray[unfinishedState], false);
    computeSuccessors(m_stateArray[unfinishedState]);
  }
  checkAllStates();
}

void Grammar::checkAllStates() {
  verbose(2, _T("Checking consistensy\n"));
  m_result->clear(getStateCount());
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
   || ((productionPrecedence == terminalPrecedence) && (terminal.m_type != RIGHTASSOC_TERMINAL)) ) {
    return CHOOSE_REDUCE;
  } else {
    return CHOOSE_SHIFT;
  }
}

void Grammar::checkStateIsConsistent(UINT stateIndex) {
  const LR1State &state     = *m_stateArray[stateIndex];
  m_result->m_stateResult.add(StateResult(stateIndex));
  StateResult    &sr        = m_result->m_stateResult.last();

  SymbolSet       symbolsDone(getSymbolCount());
  const UINT      itemCount = (UINT)state.m_items.size();

  for(UINT i = 0; i < itemCount; i++) {
    const LR1Item &item = *state.m_items[i];
    UINT           term;
    if(item.isShiftItem(term)) {
      if(symbolsDone.contains(term)) {
        continue;
      }
      for(UINT j = 0; j < itemCount; j++) {
        if(j == i) {
          continue;
        }
        const LR1Item &item1 = *state.m_items[j];
        if(item1.isReduceItem() && item1.m_la.contains(term)) {
          const GrammarSymbol &terminal = getSymbol(term);
          switch(resolveShiftReduceConflict(terminal, item1)) {
          case CONFLICT_NOT_RESOLVED:
            m_result->addSRError(_T("Shift/reduce conflict. Shift or reduce by prod %-3u (prec=%d) on '%s' (prec=%d, %s).")
                                ,item1.m_prod
                                ,getProduction(item1.m_prod).m_precedence
                                ,terminal.m_name.cstr()
                                ,terminal.m_precedence
                                ,terminal.getTypeString());
            break;
          case CHOOSE_SHIFT:
            sr.m_termActionArray.add(TermActionPair(term, PA_SHIFT, item.getNewState()));
            symbolsDone += term;
            m_result->addWarning(_T("Shift/reduce conflict on %s (prec=%d, %s). Choose shift instead of reduce by prod %u (prec=%d).")
                                ,terminal.m_name.cstr()
                                ,terminal.m_precedence
                                ,terminal.getTypeString()
                                ,item1.m_prod
                                ,getProduction(item1.m_prod).m_precedence);
            break;
          case CHOOSE_REDUCE:
            sr.m_termActionArray.add(TermActionPair(term, PA_REDUCE, item1.m_prod));
            symbolsDone += term;
            m_result->addWarning(_T("Shift/reduce conflict on %s (prec=%d, %s). Choose reduce by prod %u (prec=%d).")
                                ,terminal.m_name.cstr()
                                ,terminal.m_precedence
                                ,terminal.getTypeString()
                                ,item1.m_prod
                                ,getProduction(item1.m_prod).m_precedence);
            break;
          }
        }
      }
      if(!symbolsDone.contains(term)) {
        if(item.m_newState >= 0) {
          sr.m_termActionArray.add(TermActionPair(term, PA_SHIFT, item.getNewState()));
        }
        symbolsDone += term;
        continue;
      }
    }
  }

  for(UINT i = 0; i < itemCount; i++) {
    const LR1Item &itemi = *state.m_items[i];
    if(itemi.isReduceItem()) {
      TermSet tokensReducedByOtherItems(getTermCount());
      if(itemi.isAcceptItem()) {  // check if this is start -> S . [EOI]
        sr.m_termActionArray.add(TermActionPair(0, PA_REDUCE, 0));
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
        const LR1Item &itemj = *state.m_items[j];
        if(itemj.isReduceItem()) {
          const TermSet intersection(itemi.m_la & itemj.m_la);
          if(!intersection.isEmpty()) {
            if(itemj.m_prod < itemi.m_prod) {
              tokensReducedByOtherItems += intersection;
              m_result->addWarning(_T("Reduce/reduce conflict on %s between prod %u and prod %u. Choose prod %u")
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
        const UINT term = (UINT)it.next();
        if(!symbolsDone.contains(term)) {
          sr.m_termActionArray.add(TermActionPair(term, PA_REDUCE, itemi.m_prod));
          symbolsDone += term;
        }
      }
    }
  }
  for(auto itemp : state.m_items) {
    if(!itemp->isShiftItem() && !itemp->isReduceItem()) {
      const UINT nterm = itemp->getShiftSymbol();
      if(!symbolsDone.contains(nterm)) {
        if(itemp->getNewState() >= 0) {
          sr.m_ntermNewStateArray.add(NTermNewStatePair((USHORT)nterm, (USHORT)itemp->getNewState()));
        }
        symbolsDone += nterm;
      }
    }
  } // for
  sr.sortArrays();
}

UINT Grammar::getItemCount() const {
  UINT count = 0;
  for(auto statep : m_stateArray) {
    count += statep->getItemCount();
  }
  return count;
}
