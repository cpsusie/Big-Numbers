#include "stdafx.h"
#include "MoveablePointArray.h"

bool MoveablePointArray::removePointSet(const BitSet &set) { // set of indices
  bool changed = false;
  for(Iterator<size_t> it = ((BitSet&)set).getReverseIterator(); it.hasNext();) {
    removePoint(it.next());
    changed = true;
  }
  return changed;
}

void MoveablePointArray::removePoint(size_t index) {
  MoveablePoint *p = (*this)[index];
  remove(index);
  delete p;
}

void MoveablePointArray::clear(intptr_t capacity) {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    delete (*this)[i];
  }
  __super::clear(capacity);
}

BitSet MoveablePointArray::findPointsBelongingToGraph(const Graph *g) const {
  BitSet result(size() + 1);
  if(size()) {
    for(const MoveablePoint *const *start = &first(), *const *end = &last(), *const *p = start; p <= end; p++) {
      if((*p)->getGraph() == g) {
        result.add(p - start);
      }
    }
  }
  return result;
}

BitSet MoveablePointArray::findPointsBelongingToGraphWithType(const Graph *g, MoveablePointType type) const {
  BitSet result(size() + 1);
  if(size()) {
    for(const MoveablePoint *const *start = &first(), *const *end = &last(), *const *p = start; p <= end; p++) {
      if(((*p)->getGraph() == g) && ((*p)->getType() == type)) {
        result.add(p - start);
      }
    }
  }
  return result;
}

class GraphPointType {
private:
  MoveablePointType m_type;
  const Graph      *m_graph;
public:
  inline GraphPointType()
    : m_type(MPT_UNDEFINED)
    , m_graph(NULL)
  {
  }
  inline GraphPointType(MoveablePointType type, const Graph *graph)
    : m_type(type)
    , m_graph(graph)
  {
  }
  inline MoveablePointType getType() const {
    return m_type;
  }
  inline const Graph *getGraph() const {
    return m_graph;
  }
  inline ULONG hashCode() const {
    return pointerHash(m_graph) ^ (UINT)m_type;
  }
  inline bool operator==(const GraphPointType &e) const {
    return (e.m_graph == m_graph) && (e.m_type == m_type);
  }
};

bool MoveablePointArray::addAll(const MoveablePointArray &a, bool removeOldOfSameType) {
  if(a.isEmpty()) {
    return false;
  }
  bool changed = false;
  if(removeOldOfSameType) {
    CompactHashSet<GraphPointType> combinationSet(a.size());
    for(Iterator<MoveablePoint*> it = ((MoveablePointArray&)a).getIterator(); it.hasNext();) {
      const MoveablePoint &p = *(it.next());
      combinationSet.add(GraphPointType(p.getType(), p.getGraph()));
    }
    BitSet oldPointSet(size() + 1);
    for(Iterator<GraphPointType> it = combinationSet.getIterator(); it.hasNext();) {
      const GraphPointType &e = it.next();
      oldPointSet += findPointsBelongingToGraphWithType(e.getGraph(), e.getType());
    }
    changed = removePointSet(oldPointSet);
  }
  changed |= __super::addAll(a);
  return changed;
}
