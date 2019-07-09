#pragma once

#include "MoveablePoint.h"

class MoveablePointArray : public CompactArray<MoveablePoint*> {
public:
  MoveablePointArray() {
  }
  explicit MoveablePointArray(size_t capacity) 
    : CompactArray<MoveablePoint*>(capacity)
  {
  }
  MoveablePointArray(const MoveablePointArray &src)
    : CompactArray<MoveablePoint*>(src)
  {
  }
  void clear(intptr_t capacity=0);
  bool addAll(const MoveablePointArray &a, bool removeOldOfSameType); // return true, if changed
  void removePoint(size_t index);
  bool removePointSet(const BitSet &set); // set of indices. return true, if changed

  BitSet findPointsBelongingToGraph(const Graph *g) const;
  BitSet findPointsBelongingToGraphWithType(const Graph *g, MoveablePointType type) const;
  void paint(  CDC &dc, CFont &font) const;
  void unpaint(CDC &dc) const;
};
