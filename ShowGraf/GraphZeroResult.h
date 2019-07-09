#pragma once

#include "MoveablePointArray.h"

class GraphZeroesResult {
private:
  Graph                   &m_graph;
  const CompactDoubleArray m_zeroes;
public:
  GraphZeroesResult(Graph &graph, const CompactDoubleArray &zeroes)
    : m_graph(graph)
    , m_zeroes(zeroes)
  {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  inline const CompactDoubleArray &getZeroes() const {
    return m_zeroes;
  }
  MoveablePointArray getMoveablePointArray() const;
  String toString(const TCHAR *name = NULL) const;
};

class GraphZeroesResultArray : public Array<GraphZeroesResult> {
private:
  Graph &m_graph;
public:
  inline GraphZeroesResultArray(Graph &graph) : m_graph(graph) {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  MoveablePointArray getMoveablePointArray() const;
  String toString() const;
};
