#include "stdafx.h"

String findShortestKnightRoute(UINT from, UINT to) {
  UINT            dist[64];
  FieldSet        total, front;
  memset(dist,-1,sizeof(dist));
  total.add(from);
  front = total;
  dist[from] = 0;
  while(!total.contains(to)) {
    FieldSet newFront;
    for(Iterator<UINT> it = front.getIterator(); it.hasNext();) {
      const UINT p   = it.next();
      const UINT pd1 = dist[p]+1;
      const DirectionArray &da = MoveTable::knightMoves[p];
      for(int i = 0; i < da.m_count; i++) {
        const UINT d = da.m_directions[i].m_fields[1];
        if(total.contains(d)) continue;
        newFront.add(d);
        total.add(d);
        dist[d] = min(pd1, dist[d]);
      }
    }
    front = newFront;
  }
  CompactIntArray route;
  UINT p = to;
  route.add(p);
  for(UINT d = dist[p]; d-- > 0;) {
    const DirectionArray &da = MoveTable::knightMoves[p];
    for(int i = 0; i < da.m_count; i++) {
      const UINT p1 = da.m_directions[i].m_fields[1];
      if(dist[p1] == d) {
        p = p1;
        break;
      }
    }
    route.add(p);
  }
  String result;
  const TCHAR *delimiter = NULL;
  for(UINT i = (UINT)route.size()-1; i--;) {
    if(delimiter) result += delimiter; else delimiter = _T(" ");
    result += getFieldName(route[i]);
  }
  return result;
}
