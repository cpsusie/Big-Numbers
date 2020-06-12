#pragma once

#if defined(_DEBUG)
#define USE_DEBUGSTRING
#endif // _DEBUG

#if defined(USE_DEBUGSTRING)
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

#if defined(_DEBUG)
#define TRACE_REDUCTION_CALLSTACK
#endif

#if defined(TRACE_REDUCTION_CALLSTACK)

#include <Stack.h>
#include <PropertyContainer.h>

namespace Expr {

class ExpressionNode;

class ReductionStackElement {
private:
  const UINT            m_index;
  const TCHAR          *m_method;
  String                m_str;
  const ExpressionNode *m_node;
public:
  ReductionStackElement(UINT index, const TCHAR *method, const String &str, const ExpressionNode *node = NULL)
    : m_index( index )
    , m_method(method)
    , m_node(  node  )
  {
    m_str = format(_T("%-20s:%s"), m_method, str.cstr());
  }
  inline UINT getIndex() const {
    return m_index;
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
  REDUCTION_STACKHIGHT // int*
 ,REDUCTION_STACKTOP   // ReductionStackElement*
} ReductionStackProperties;

class ReductionStack : public Stack<ReductionStackElement>, public PropertyContainer {
public:
  void clear();
  void push(const TCHAR *method, const String &s, const ExpressionNode *n = NULL);
  void pop( const TCHAR *method);
  const ReductionStackElement *topPointer(UINT index=0) const;
  static const TCHAR *getRawName(const TCHAR *str);
};

#define DEFINEREDUCTIONSTACK   ReductionStack m_reductionStack

#define STARTREDUCTION()       m_reductionStack.clear()
#define DEFFUNC                const TCHAR *_func = ReductionStack::getRawName(__TFUNCTION__)
#define _GETSTACK()             getTree().getReductionStack()
#define _PUSH(...)             _GETSTACK().push(_func,__VA_ARGS__)
#define _POP()                 _GETSTACK().pop(_func)
#define _REDSTR(n)             ((n).isReduced()?_T("(R)"):EMPTYSTRING)

#define ENTERMETHOD()          DEFFUNC; _PUSH(format(_T("n:<%s>%s"), toString().cstr(),_REDSTR(*this)),node())
#define ENTERMETHOD1(v)        DEFFUNC; _PUSH(format(_T("%s:<%s>"), _T(#v), (v).toString().cstr()))
#define ENTERMETHOD2(v1,v2)    DEFFUNC; _PUSH(format(_T("%s:<%s>, %s:<%s>"), _T(#v1), (v1).toString().cstr(), _T(#v2), (v2).toString().cstr()))
#define ENTERMETHOD2NUM(v1,v2) DEFFUNC; _PUSH(format(_T("%s:<%s>, %s:<%s>"), _T(#v1), ::toString(v2).cstr(), _T(#v2), ::toString(v2).cstr()))
#define _LEAVEMETHOD()          _POP()
#define _RETURN(x)              { _LEAVEMETHOD(); return x; }

#define RETURNNULL                                                                    \
{ _POP();                                                                             \
  _PUSH(_T("Return NULL"));                                                           \
  _RETURN(NULL);                                                                      \
}

#define RETURNBOOL(b)                                                                 \
{ const bool _b = b;                                                                  \
  _POP();                                                                             \
  _PUSH(format(_T("Return %s"), boolToStr(_b)));                                      \
  _RETURN(_b);                                                                        \
}

#define RETURNNODE(n)                                                                 \
{ const SNode _n = n;                                                                 \
  CHECKISCONSISTENT(_n);                                                              \
  _POP();                                                                             \
  _PUSH(format(_T("Reduced:<%s>%s"), _n.toString().cstr(),_REDSTR(_n)),_n.node());    \
  _RETURN(_n);                                                                        \
}

#define RETURNTHIS                                                                    \
{ const SNode _n = *this;                                                             \
  _POP();                                                                             \
  _PUSH(format(_T("Unchanged:<%s>%s"), _n.toString().cstr(),_REDSTR(_n)),_n.node());  \
  _RETURN(_n);                                                                        \
}

#define RETURNSHOWSTR(v)                                                              \
{ const String _s = (v).toString();                                                   \
  _POP();                                                                             \
  _PUSH(format(_T("%s:<%s>"), _T(#v), _s.cstr()));                                    \
  _RETURN(v);                                                                         \
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

#define RETURNNULL        return NULL
#define RETURNBOOL(b)     return b
#define RETURNNODE(n)     return n
#define RETURNTHIS        return *this
#define RETURNSHOWSTR(v)  return v

}; // namespace Expr

#endif // TRACE_REDUCTION_CALLSTACK
