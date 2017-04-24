#include "stdafx.h"

#ifndef TABLEBASE_BUILDER

#include <HashMap.h>
#include "ExternEngine.h"

typedef enum {
  ENGINE_DEPTH
 ,ENGINE_SELDEPTH
 ,ENGINE_SCORE
 ,ENGINE_LOWERBOUND
 ,ENGINE_UPPERBOUND
 ,ENGINE_TIME
 ,ENGINE_NODES
 ,ENGINE_NODESPS
 ,ENGINE_PV
 ,ENGINE_STRING
 ,ENGINE_HASHFULL
 ,ENGINE_MULTIPV
 ,ENGINE_CPULOAD
} EngineInfoField;

class InfoHashMap : public StrHashMap<EngineInfoField> {
public:
  InfoHashMap();
};

InfoHashMap::InfoHashMap() {
  put(_T("depth"     ), ENGINE_DEPTH      );
  put(_T("seldepth"  ), ENGINE_SELDEPTH   );
  put(_T("score"     ), ENGINE_SCORE      );
  put(_T("lowerbound"), ENGINE_LOWERBOUND );
  put(_T("upperbound"), ENGINE_UPPERBOUND );
  put(_T("time"      ), ENGINE_TIME       );
  put(_T("nodes"     ), ENGINE_NODES      );
  put(_T("nps"       ), ENGINE_NODESPS    );
  put(_T("pv"        ), ENGINE_PV         );
  put(_T("string"    ), ENGINE_STRING     );
  put(_T("hashfull"  ), ENGINE_HASHFULL   );
  put(_T("multipv"   ), ENGINE_MULTIPV    );
  put(_T("cpuload"   ), ENGINE_CPULOAD    );
}

void EngineInfoLine::reset() {
  m_pv = m_score = m_string = EMPTYSTRING;
  m_depth    = 0;
  m_seldepth = 0;
  m_time     = 0;
  m_nodes    = 0;
  m_nodesps  = 0;
  m_hashFull = 0;
  m_cpuLoad  = 0;
}

EngineInfoLine &EngineInfoLine::operator+=(const String &line) {
  static InfoHashMap keywords;

  for(Tokenizer tok(line, _T(" ")); tok.hasNext();) {
    EngineInfoField *field = keywords.get(tok.next().cstr());
    if(field == NULL) {
      continue;
    }
    switch(*field) {
    case ENGINE_DEPTH   :
      m_depth = tok.getInt();
      break;
    case ENGINE_SELDEPTH   :
      m_seldepth = tok.getInt();
      break;
    case ENGINE_SCORE   :
      { const String s = tok.next();
        if(s == _T("cp")) {
          m_score = _T("=") + tok.next();
        } else if(s == _T("mate")) {
          const int moves = tok.getInt();
          if(moves > 0) {
            m_score = format(_T("Mate in %d"), moves);
          } else {
            m_score = format(_T("Loose in %d"), -moves);
          }
        }
      }
      break;
    case ENGINE_LOWERBOUND:
      if(m_score.length()) m_score[0] = _T('>');
      break;
    case ENGINE_UPPERBOUND:
      if(m_score.length()) m_score[0] = _T('<');
      break;
    case ENGINE_TIME    :
      m_time = tok.getInt();
      break;
    case ENGINE_NODES   :
      m_nodes = tok.getInt64();
      break;
    case ENGINE_NODESPS :
      m_nodesps = tok.getUint();
      break;
    case ENGINE_PV      :
      m_pv = tok.getRemaining();
      return *this;
    case ENGINE_STRING:
      m_string = tok.getRemaining();
      return *this;
    case ENGINE_HASHFULL:
      m_hashFull = tok.getInt();
      break;
    case ENGINE_MULTIPV :
      m_multiPV = tok.getInt();
      break;
    case ENGINE_CPULOAD :
      m_cpuLoad = tok.getInt();
      break;
    }
  }
  return *this;
}

String EngineInfoLine::toString(const EngineVerboseFields &evf) const {
  String result;
  if(m_score.length() > 0) {
    if(evf.m_depth   ) result += format(_T("depth:%2d "     ), m_depth                     );
    if(evf.m_seldepth) result += format(_T("seldepth:%2d "  ), m_seldepth                  );
    if(evf.m_time    ) result += format(_T("time:%6.2lf "   ), (double)m_time/1000         );
    if(evf.m_nodes   ) result += format(_T("nodes:%13s "    ), format1000(m_nodes  ).cstr());
    if(evf.m_nodesps ) result += format(_T("nodes/sec:%10s "), format1000(m_nodesps).cstr());
    if(evf.m_score   ) result += format(_T("score%-6s "     ), m_score.cstr()              );
    if(evf.m_cpuLoad ) result += m_cpuLoad  ? format(_T("CPU:%4.1lf%% "     ), (double)m_cpuLoad/10.0) : _T("          ");
    if(evf.m_hashfull) result += m_hashFull ? format(_T("HashLoad:%4.1lf%% "), (double)m_hashFull/10.0) : _T("               ");
    if(result.length()) result += _T("\n");
  }
  if(evf.m_string && (m_string.length() > 0)) {
    result += m_string;
    result += _T("\n");
  }
  return result;
}

#endif // TABLEBASE_BUILDER
