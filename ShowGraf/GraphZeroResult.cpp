#include "stdafx.h"
#include "GraphZeroResult.h"

MoveablePointArray GraphZeroesResult::getMoveablePointArray() const {
  const size_t n = m_zeroes.size();
  MoveablePointArray result(n);
  for (size_t i = 0; i < n; i++) {
    result.add(new MoveablePoint(&getGraph(), MPT_ROOT, Point2D(m_zeroes[i], 0), SHOWXCOORDINATE));
  }
  return result;
}

String GraphZeroesResult::toString(const TCHAR *name) const {
  return format(_T("Zeroes of %s:%s")
    , name ? name : m_graph.getParam().getDisplayName().cstr()
    , m_zeroes.toStringBasicType().cstr()
  );
}

MoveablePointArray GraphZeroesResultArray::getMoveablePointArray() const {
  MoveablePointArray result;
  for (size_t i = 0; i < size(); i++) {
    result.addAll((*this)[i].getMoveablePointArray(), false);
  }
  return result;
}

String GraphZeroesResultArray::toString() const {
  if (isEmpty()) {
    return _T("No zeroes found");
  }
  if (size() == 1) {
    return __super::toString(_T("\n"));
  }
  StringArray result;
  for (size_t i = 0; i < size(); i++) {
    const GraphZeroesResult &zr = (*this)[i];
    const String             name = format(_T("%s.%s")
      , m_graph.getParam().getDisplayName().cstr()
      , zr.getGraph().getParam().getDisplayName().cstr());
    result.add(zr.toString(name.cstr()));
  }
  return result.toString(_T("\n"));
}
