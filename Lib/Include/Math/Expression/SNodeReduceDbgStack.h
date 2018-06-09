#pragma once

#ifdef _DEBUG
#define USE_DEBUGSTRING
#endif // _DEBUG

#ifdef USE_DEBUGSTRING
class UpdateableDebugStr {
private:
  BYTE m_count;
public:
  inline UpdateableDebugStr() : m_count(0) {
  }
  inline void disable() { m_count++; }
  inline bool enable()  { return (--m_count == 0); }
  inline bool enabled() const { return m_count == 0; }
};

class AlwaysEnabledDebugStr {
public:
  inline bool enabled() { return true; }
};

#define DECLAREDEBUGSTRING      protected: String m_debugStr; AlwaysEnabledDebugStr _setDebugStr
#define DECLAREARRAYDEBUGSTRING protected: String m_debugStr; UpdateableDebugStr    _setDebugStr
#define UPDATEDEBUGSTRING( a)   { if(getDebuggerPresent()) { (a).m_debugStr = (a).toString(); } }
#define SETDEBUGSTRING()        if(_setDebugStr.enabled()) UPDATEDEBUGSTRING(*this)
#define DISABLEDEBUGSTRING(a)   (a)._setDebugStr.disable()
#define ENABLEDEBUGSTRING( a)   { if((a)._setDebugStr.enable()) UPDATEDEBUGSTRING(a); }
#else // USE_DEBUGSTRING
#define DECLAREDEBUGSTRING
#define DECLAREARRAYDEBUGSTRING
#define SETDEBUGSTRING()
#define UPDATEDEBUGSTRING( a)
#define DISABLEDEBUGSTRING(a)
#define ENABLEDEBUGSTRING( a)
#endif  // USE_DEBUGSTRING

#define TRACE_REDUCTION_CALLSTACK

#ifdef TRACE_REDUCTION_CALLSTACK

#include <Stack.h>
#include <PropertyContainer.h>

namespace Expr {

class ExpressionNode;

class ReductionStackElement {
private:
  const TCHAR          *m_method;
  String                m_str;
  const ExpressionNode *m_node;
public:
  ReductionStackElement(const TCHAR *method, const String &str, const ExpressionNode *node = NULL) : m_method(method), m_node(node) {
    m_str = format(_T("%-20s:%s"), m_method, str.cstr());
  }
  inline const TCHAR *getMethod() const {
    return m_method;
  }
  inline const String &toString() const {
    return m_str;
  }
  inline const ExpressionNode *getNode() const {
    return m_node;
  }
  inline bool hasNode() const {
    return m_node != NULL;
  }
};

typedef enum {
  REDUCTION_STACKHIGHT
} ReductionStackProperties;

class ReductionStack : public Stack<ReductionStackElement>, public PropertyContainer {
public:
  void clear();
  void push(const TCHAR *method, const String &s, const ExpressionNode *n = NULL);
  void pop( const TCHAR *method);
  static const TCHAR *getRawName(const TCHAR *str);
};

#define DEFINEREDUCTIONSTACK   ReductionStack m_reductionStack

#define GETSTACK()             getTree().getReductionStack()
#define STARTREDUCTION()       m_reductionStack.clear()
#define DEFFUNC                const TCHAR *_func = ReductionStack::getRawName(__TFUNCTION__)
#define _PUSH(...)             GETSTACK().push(_func,__VA_ARGS__)
#define _POP()                 GETSTACK().pop(_func)
#define ENTERMETHOD()          DEFFUNC; _PUSH(format(_T("n:<%s>"), toString().cstr()),node())
#define ENTERMETHOD1(v)        DEFFUNC; _PUSH(format(_T("%s:<%s>"), _T(#v), (v).toString().cstr()))
#define ENTERMETHOD2(v1,v2)    DEFFUNC; _PUSH(format(_T("%s:<%s>, %s:<%s>"), _T(#v1), (v1).toString().cstr(), _T(#v2), (v2).toString().cstr()))
#define ENTERMETHOD2NUM(v1,v2) DEFFUNC; _PUSH(format(_T("%s:<%s>, %s:<%s>"), _T(#v1), ::toString(v2).cstr(), _T(#v2), ::toString(v2).cstr()))
#define LEAVEMETHOD()          _POP()
#define RETURN(x)              { LEAVEMETHOD(); return x; }

#define RETURNNULL                                                       \
{ _PUSH(_T("Return NULL"));                                              \
  _POP();                                                                \
  RETURN(NULL);                                                          \
}

#define RETURNBOOL(b)                                                    \
{ const bool _b = b;                                                     \
  _PUSH(format(_T("Return %s"), boolToStr(_b)));                         \
  _POP();                                                                \
  RETURN(_b);                                                            \
}

#define RETURNNODE(n)                                                    \
{ const SNode &_n = n;                                                   \
  CHECKISCONSISTENT(_n);                                                 \
  _PUSH(format(_T("Reduced:<%s>"), _n.toString().cstr()),_n.node());     \
  _POP();                                                                \
  RETURN(_n);                                                            \
}

#define RETURNNODEP(n)                                                   \
{ const SNode &_n = n;                                                   \
  CHECKISCONSISTENT(_n);                                                 \
  _PUSH(format(_T("Reduced:<%s>"), _n.toString().cstr()),_n.node());     \
  _POP();                                                                \
  RETURN(_n.node());                                                     \
}

#define RETURNSHOWSTR(v)                                                 \
{ const String _s = (v).toString();                                      \
  _PUSH(format(_T("%s:<%s>"), _T(#v), _s.cstr()));                       \
  _POP();                                                                \
  RETURN(v);                                                             \
}

#define RETURNPSHOWSTR(p)                                                \
{ const String _s = (p)->toString();                                     \
  _PUSH(format(_T("%s:<%s>"), _T(#p), _s.cstr()));                       \
  _POP();                                                                \
  RETURN(p);                                                             \
}

}; // namespace Expr

#else // TRACE_REDUCTION_CALLSTACK

namespace Expr {

#define DEFINEREDUCTIONSTACK
#define STARTREDUCTION()
#define ENTERMETHOD()
#define ENTERMETHOD1(v)
#define ENTERMETHOD2(v1,v2)
#define ENTERMETHOD2NUM(v1,v2)
#define LEAVEMETHOD()

#define RETURN(x)         return x
#define RETURNNULL        return NULL
#define RETURNBOOL(b)     return b
#define RETURNNODE(n)     return n
#define RETURNSHOWSTR(v)  return v
#define RETURNPSHOWSTR(p) return p

}; // namespace Expr

#endif // TRACE_REDUCTION_CALLSTACK

