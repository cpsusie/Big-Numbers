#pragma once

#ifdef _DEBUG
#define TRACE_REDUCTION_CALLSTACK
#endif

#ifdef TRACE_REDUCTION_CALLSTACK

#include <Stack.h>
#include <PropertyContainer.h>

namespace Expr {

class ParserTree;
class ExpressionNode;

class ReductionStackElement {
public:
  const TCHAR          *m_method;
  String                m_str;
  const ExpressionNode *m_node;
  ReductionStackElement(const TCHAR *method, const String &str, const ExpressionNode *node = NULL) : m_method(method), m_node(node) {
    m_str = format(_T("%-20s:%s"), m_method, str.cstr());
  }
  inline const String &toString() const {
    return m_str;
  }
};

typedef enum {
  REDUCTION_STACKHIGHT
} ReductionStackProperties;

class ReductionStack : public Stack<ReductionStackElement>, public PropertyContainer {
private:
  ParserTree *m_tree;
public:
  ReductionStack() : m_tree(NULL) {
  }
  void push(const TCHAR *method, const String &s, const ExpressionNode *n = NULL);
  void pop( const TCHAR *method);
  void reset(ParserTree *tree);
  static const TCHAR *getRawName(const TCHAR *str);
};

#define DEFINEREDUCTIONSTACK   ReductionStack m_reductionStack

#define GETSTACK()             getTree().getReductionStack()
#define STARTREDUCTION(tree)   m_reductionStack.reset(tree)
#define DEFFUNC                const TCHAR *_func = ReductionStack::getRawName(__TFUNCTION__)
#define _PUSH(...)             GETSTACK().push(_func,__VA_ARGS__)
#define _POP()                 GETSTACK().pop(_func)
#define ENTERMETHOD()          DEFFUNC; _PUSH(format(_T("n:<%s>"), toString().cstr()),m_node)
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
  _n.isConsistent();                                                     \
  _PUSH(format(_T("Reduced:<%s>"), _n.toString().cstr()),_n.m_node);     \
  _POP();                                                                \
  RETURN(_n);                                                            \
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
#define STARTREDUCTION(tree)
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

