#include "pch.h"
#include <DebugLog.h>
#include <CallCounter.h>

CallCounter::~CallCounter() {
  debugLog(_T("%-20s Calls:%14s\n")
          ,m_name.cstr()
          ,format1000(m_callCount).cstr()
          );
/*
  debugLog(_T("%-20s Calls:%14s MaxInt:%14s MaxDouble:%21.16le\n")
          ,m_name.cstr()
          ,format1000(m_callCount).cstr()
          ,format1000(m_maxInt).cstr()
          ,m_maxDouble
          );
  if(!m_map.isEmpty()) debugLog(_T("%s"), mapToString().cstr());
*/
}

/*
void CallCounter::incr(int mapKey) {
  m_callCount++;
  UINT *v = m_map.get(mapKey);
  if(v) {
    (*v)++;
  } else {
    m_map.put(mapKey, 1);
  }
}

class CallEntry {
public:
  int          m_mapKey; // key
  UINT         m_calls;  // value. Number of calls for each key
  inline CallEntry(int mapKey, UINT calls) : m_mapKey(mapKey), m_calls(calls) {
  }
};

static int callEntryCmp(const CallEntry &e1, const CallEntry &e2) {
  return e1.m_mapKey - e2.m_mapKey;
}

String CallCounter::mapToString() {
  Array<CallEntry> entryArray(m_map.size());

  for(auto it = m_map.getIterator(); it.hasNext();) {
    const Entry<int, UINT> &e = it.next();
    entryArray.add(CallEntry(e.getKey(), e.getValue()));
  }
  entryArray.sort(callEntryCmp);

  const int n = (int)entryArray.size();
  String result;
  for(int i = 0; i < n; i++) {
    const CallEntry &e = entryArray[i];
    result += format(_T("%8d %8lu\n"), e.m_mapKey, e.m_calls);
  }
  return result;
}
*/
