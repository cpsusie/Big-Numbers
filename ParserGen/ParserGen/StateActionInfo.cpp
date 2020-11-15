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

StateActionInfo::StateActionInfo(UINT state, const ParserActionArray &actionArray, const SymbolNameContainer &nameContainer)
: m_state(          state                          )
, m_legalTermCount( actionArray.getLegalTermCount())
, m_nameContainer(  nameContainer                  )
{
  initPointers(nullptr);

  const Options &options = Options::getInstance();
  if(!options.m_useTableCompression) {
    setTermListCompression(actionArray);
    m_compressMethod = ParserTables::CompCodeTermList;
  } else {
    ParserActionArray     shiftActionArray;
    TermSetReductionArray termSetReductionArray;
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
          termSetReductionArray.add(TermSetReduction(prod, pa.m_term, m_nameContainer));
          termSetReductionMap.put(prod, index);
        }
      }
    }
    termSetReductionArray.sortBySetSize();
    m_compressMethod = findCompressionMethod(shiftActionArray, termSetReductionArray);
  }
}

StateActionInfo::StateActionInfo(const StateActionInfo *parent, const TermSetReduction &termSetReduction)
: m_state(         parent->m_state                  )
, m_legalTermCount(termSetReduction.getTermSetSize())
, m_nameContainer( parent->m_nameContainer          )
{
  initPointers(parent);
  m_compressMethod = setTermSetReduceCompression(termSetReduction);
}

StateActionInfo::StateActionInfo(const StateActionInfo *parent, const ParserActionArray &actionArray)
: m_state(         parent->m_state                )
, m_legalTermCount(actionArray.getLegalTermCount())
, m_nameContainer( parent->m_nameContainer        )
{
  initPointers(parent);
  m_compressMethod = setTermListCompression(actionArray);
}

StateActionInfo::StateActionInfo(const StateActionInfo *parent, const ParserActionArray &shiftActionArray, const TermSetReductionArray &termSetReductionArray)
: m_state(         parent->m_state                  )
, m_legalTermCount(shiftActionArray.getLegalTermCount() + termSetReductionArray.getLegalTermCount())
, m_nameContainer( parent->m_nameContainer          )
{
  initPointers(parent);
  m_compressMethod = findCompressionMethod(shiftActionArray, termSetReductionArray);
}

StateActionInfo::~StateActionInfo() {
  SAFEDELETE(m_termListActionArray);
  SAFEDELETE(m_termSetReduction   );
  SAFEDELETE(m_child[0]           );
  SAFEDELETE(m_child[1]           );
}

void StateActionInfo::initPointers(const StateActionInfo *parent) {
  m_parent              = parent;
  m_recurseLevel        = parent ? (parent->m_recurseLevel + 1) : 0;
  m_termListActionArray = nullptr;
  m_termSetReduction    = nullptr;
  m_child[0]            = nullptr;
  m_child[1]            = nullptr;
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

CompressionMethod StateActionInfo::setTermListCompression(const ParserActionArray &actionArray) {
  assert(getLegalTermCount() == actionArray.getLegalTermCount());
  m_termListActionArray = new ParserActionArray(actionArray); TRACE_NEW(m_termListActionArray);
  return (getLegalTermCount() == 1) ? ParserTables::CompCodeOneItem : ParserTables::CompCodeTermList;
}

CompressionMethod StateActionInfo::setTermSetReduceCompression(const TermSetReduction &termSetReduction) {
  assert(getLegalTermCount() == termSetReduction.getTermSetSize());
  if(getLegalTermCount() == 1) {
    return setTermListCompression(termSetReduction);
  } else {
    m_termSetReduction = new TermSetReduction(termSetReduction); TRACE_NEW(m_termSetReduction);
    return ParserTables::CompCodeTermSet;
  }
}

CompressionMethod StateActionInfo::findCompressionMethod(const ParserActionArray &shiftActionArray, const TermSetReductionArray &termSetReductionArray) {
  const Options &options       = Options::getInstance();
  const UINT     shiftActions  = shiftActionArray.getLegalTermCount();
  const UINT     reduceActions = (UINT)termSetReductionArray.size();

  assert(shiftActions + reduceActions >= 1);

  if(shiftActions + reduceActions == 1) {
    if(reduceActions == 0) {
      return setTermListCompression(shiftActionArray);
    } else { // reduceActions == 1
      return setTermSetReduceCompression(termSetReductionArray[0]);
    }
  }

  if((reduceActions == 0) || (termSetReductionArray[0].getTermSetSize() < 2) || (m_recurseLevel >= options.m_maxRecursiveCalls)) {
    return setTermListCompression(mergeActionArrays(shiftActionArray, termSetReductionArray));
  }

  // (shiftActions + reduceActions >= 2) && (reduceActions >= 1) && (m_sameReductionArray[0].getSetSize() >= 2)
  m_child[0] = new StateActionInfo(this, termSetReductionArray[0]);

  const size_t n = termSetReductionArray.size();
  TermSetReductionArray termSetReductions(n-1); // make it = termSetReductionArray without 1st element
  for(size_t i = 1; i < n; i++) {
    termSetReductions.add(termSetReductionArray[i]);
  }
  m_child[1] = new StateActionInfo(this, shiftActionArray, termSetReductions);
  return ParserTables::CompCodeSplitNode;
}

String StateActionInfo::toString() const {
  String result;
  result = format(_T("State %u %-20s (Legal tokens:%u)\n"), m_state, compressMethodToString(getCompressionMethod()), m_legalTermCount);
  switch(getCompressionMethod()) {
  case ParserTables::CompCodeTermList:
  case ParserTables::CompCodeOneItem :
    result += indentString(m_termListActionArray->toString(m_nameContainer),3);
    break;
  case ParserTables::CompCodeSplitNode:
    { const String cstr0 = indentString(getChild(0).toString(),2);
      const String cstr1 = indentString(getChild(1).toString(),2);
      result += indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
    }
    break;
  case ParserTables::CompCodeTermSet:
    result += indentString(m_termSetReduction->toString(),3);
    break;
  default:
    result += format(_T("Unknown compressMethod:%d"), getCompressionMethod());
  }
  return result;
}
