#include "stdafx.h"
#include <CompactHashMap.h>
#include "StateActionInfo.h"

TermSetReduction::operator ParserActionArray() const {
  ParserActionArray result(getTermSetSize());
  const short action = -(int)m_prod;
  for(auto it = m_termSet.getIterator(); it.hasNext();) {
    result.add(ParserAction((USHORT)it.next(), action));
  }
  return result;
}

ParserActionArray StateActionInfo::mergeActionArrays(const ParserActionArray &shiftActionArray, const TermSetReductionArray &termSetReductionArray) { // static
  ParserActionArray result(shiftActionArray.getLegalTermCount() + termSetReductionArray.getLegalTermCount());
  result.addAll(shiftActionArray);
  if(termSetReductionArray.size() > 0) {
    for(auto it = termSetReductionArray.getIterator(); it.hasNext();) {
      result.addAll((ParserActionArray)it.next());
    }
    if(result.size() > 1) {
      result.sortByTerm();
    }
  }
  return result;
}

void StateActionInfo::splitActionArray(const SymbolNameContainer &nameContainer, const ParserActionArray &actionArray, ParserActionArray &shiftActionArray, TermSetReductionArray &termSetReductionArray) { // static
  CompactUIntHashMap<UINT, 256>  termSetReductionMap(241); // map from reduce-production -> index into termSetReductionArray
  for(const ParserAction pa : actionArray) {
    if(pa.m_action > 0) {
      shiftActionArray.add(pa);
    } else { // pa.action <= 0.....reduce (or accept)
      const UINT  prod = -pa.m_action;
      const UINT *indexp = termSetReductionMap.get(prod);
      if(indexp) {
        termSetReductionArray[*indexp].addTerminal(pa.m_term);
      } else {
        const UINT index = (UINT)termSetReductionArray.size();
        termSetReductionArray.add(TermSetReduction(prod, pa.m_term, nameContainer));
        termSetReductionMap.put(prod, index);
      }
    }
  }
  termSetReductionArray.sortBySetSize();
}

StateActionInfo *StateActionInfo::allocateStateActionInfo(UINT state, const SymbolNameContainer &nameContainer, const ParserActionArray &actionArray) {
  const Options &options = Options::getInstance();
  if(!options.m_useTableCompression) {
    return new StateActionInfoTermList(nullptr, state, nameContainer, actionArray);
  } else {
    ParserActionArray     shiftActionArray;
    TermSetReductionArray termSetReductionArray;
    splitActionArray(                 nameContainer, actionArray, shiftActionArray, termSetReductionArray);
    return allocateStateActionInfo(nullptr, state, nameContainer, shiftActionArray, termSetReductionArray);
  }
}

StateActionInfo *StateActionInfo::allocateStateActionInfo(const StateActionInfo *parent, UINT state, const SymbolNameContainer &nameContainer, const ParserActionArray &shiftActionArray, const TermSetReductionArray &termSetReductionArray) {
  const Options &options       = Options::getInstance();
  const UINT     shiftActions  = shiftActionArray.getLegalTermCount();
  const UINT     reduceActions = (UINT)termSetReductionArray.size();

  assert(shiftActions + reduceActions >= 1);

  if(shiftActions + reduceActions == 1) {
    if(reduceActions == 0) {
      return allocateTermListCompression(parent, state, nameContainer, shiftActionArray);
    } else { // reduceActions == 1
      return allocateTermSetCompression(parent, state, nameContainer, termSetReductionArray[0]);
    }
  }

  const BYTE recurseLevel = parent ? parent->getRecurseLevel() + 1 : 0;
  if((reduceActions == 0) || (termSetReductionArray[0].getTermSetSize() < 2) || (recurseLevel >= options.m_maxRecursiveCalls)) {
    return allocateTermListCompression(parent, state, nameContainer, mergeActionArrays(shiftActionArray, termSetReductionArray));
  }

  const UINT totalReduceItems = termSetReductionArray.getLegalTermCount();
  StateActionInfoSplitNode *p = new StateActionInfoSplitNode(parent, state, totalReduceItems + shiftActions, nameContainer); TRACE_NEW(p);
  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_termSetReductionArray[0].getTermSetSize() >= 2)
  StateActionInfo *child0 = allocateTermSetCompression(p, state, nameContainer, termSetReductionArray[0]);

  const size_t n = termSetReductionArray.size();
  TermSetReductionArray termSetReductions(n-1); // make it = termSetReductionArray without 1st element
  for(size_t i = 1; i < n; i++) {
    termSetReductions.add(termSetReductionArray[i]);
  }
  StateActionInfo *child1 = allocateStateActionInfo(p, state, nameContainer, shiftActionArray, termSetReductions);
  p->setChild(0, child0).setChild(1, child1);
  return p;
}

StateActionInfo *StateActionInfo::allocateTermListCompression(const StateActionInfo *parent, UINT state, const SymbolNameContainer &nameContainer, const ParserActionArray &shiftActionArray) {
  StateActionInfo *p;
  if(shiftActionArray.size() == 1) {
    p = new StateActionInfoOneItem( parent, state, nameContainer, shiftActionArray[0]); TRACE_NEW(p);
  } else {
    p = new StateActionInfoTermList(parent, state, nameContainer, shiftActionArray); TRACE_NEW(p);
  }
  return p;
}

StateActionInfo *StateActionInfo::allocateTermSetCompression(const StateActionInfo *parent, UINT state, const SymbolNameContainer &nameContainer, const TermSetReduction &termSetReduction) {
  StateActionInfo *p;
  if(termSetReduction.getTermSetSize() == 1) {
    const ParserActionArray paa(termSetReduction);
    p = new StateActionInfoOneItem(parent, state, nameContainer, paa[0]); TRACE_NEW(p);
  } else {
    p = new StateActionInfoTermSet(parent, state, nameContainer, termSetReduction); TRACE_NEW(p);
  }
  return p;
}

String StateActionInfo::toString() const {
  return format(_T("State %u %-20s (Legal tokens:%u)\n"), m_state, compressMethodToString(getCompressionMethod()), m_legalTermCount);
}

String StateActionInfoTermList::toString() const {
  return __super::toString()
       + indentString(m_termListActionArray.toString(m_nameContainer),3);
}

StateActionInfoSplitNode::~StateActionInfoSplitNode() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

StateActionInfoSplitNode &StateActionInfoSplitNode::setChild(BYTE index, StateActionInfo *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

String StateActionInfoSplitNode::toString() const {
  const String cstr0 = indentString(getChild(0).toString(),2);
  const String cstr1 = indentString(getChild(1).toString(),2);
  return __super::toString()
       + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
}

String StateActionInfoOneItem::toString() const {
  return __super::toString()
       + indentString(m_action.toString(m_nameContainer),3);
}

String StateActionInfoTermSet::toString() const {
  return __super::toString()
       + indentString(m_termSetReduction.toString(),3);
}
