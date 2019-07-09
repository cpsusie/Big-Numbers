#pragma once

typedef enum {
  EXTREMA_TYPE_MAX
 ,EXTREMA_TYPE_MIN
} ExtremaType;

inline String toString(ExtremaType type) {
  return (type == EXTREMA_TYPE_MAX) ? _T("Maxima") : _T("Minima");
}

class GraphExtremaResult {
private:
  Graph             &m_graph;
  const ExtremaType  m_extremaType;
  const Point2DArray m_extrema;
public:
  GraphExtremaResult(Graph &graph, ExtremaType extremaType, const Point2DArray &extrema)
    : m_graph(graph)
    , m_extremaType(extremaType)
    , m_extrema(extrema)
  {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  inline ExtremaType getExtremaType() const {
    return m_extremaType;
  }
  inline const Point2DArray &getExtrema() const {
    return m_extrema;
  }
  MoveablePointArray getMoveablePointArray() const;
  inline String getExtremaTypeName() const {
    return ::toString(getExtremaType());
  }
  String toString(const TCHAR *name = NULL) const;
};

class GraphExtremaResultArray : public Array<GraphExtremaResult> {
private:
  Graph            &m_graph;
  const ExtremaType m_extremaType;
public:
  inline GraphExtremaResultArray(Graph &graph, ExtremaType extremaType)
    : m_graph(graph)
    , m_extremaType(extremaType)
  {
  }
  inline Graph &getGraph() const {
    return m_graph;
  }
  inline ExtremaType getExtremaType() const {
    return m_extremaType;
  }
  inline String getExtremaTypeName() const {
    return ::toString(getExtremaType());
  }
  MoveablePointArray getMoveablePointArray() const;
  String toString() const;
};
