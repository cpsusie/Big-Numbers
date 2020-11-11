#include "stdafx.h"
#include "GraphExtremaResult.h"

MoveablePointArray GraphExtremaResult::getMoveablePointArray() const {
  const size_t n = m_extrema.size();
  MoveablePointArray result(n);
  const MoveablePointType pointType = (getExtremaType() == EXTREMA_TYPE_MAX)
                                    ? MPT_MAXIMUM
                                    : MPT_MINIMUM;
  for(size_t i = 0; i < n; i++) {
    result.add(new MoveablePoint(&getGraph(), pointType, m_extrema[i]));
  }
  return result;
}

String GraphExtremaResult::toString(const TCHAR *name) const {
  return format(_T("%s of %s:%s")
               ,getExtremaTypeName().cstr()
               ,name ? name : m_graph.getParam().getDisplayName().cstr()
               ,m_extrema.toString().cstr()
  );
}

MoveablePointArray GraphExtremaResultArray::getMoveablePointArray() const {
  MoveablePointArray result;
  for(auto it = getIterator(); it.hasNext();) {
    result.addAll(it.next().getMoveablePointArray(), false);
  }
  return result;
}

String GraphExtremaResultArray::toString() const {
  if(isEmpty()) {
    return format(_T("No %s found"), getExtremaTypeName().cstr());
  }
  if(size() == 1) {
    return __super::toString(_T("\n"));
  }
  StringArray result(size());
  for(auto it = getIterator(); it.hasNext();) {
    const GraphExtremaResult &er = it.next();
    const String              name = format(_T("%s.%s")
                                           ,m_graph.getParam().getDisplayName().cstr()
                                           ,er.getGraph().getParam().getDisplayName().cstr());
    result.add(er.toString(name.cstr()));
  }
  return result.toString(_T("\n"));
}
