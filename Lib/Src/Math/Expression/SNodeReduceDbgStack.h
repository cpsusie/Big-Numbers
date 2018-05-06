#pragma once


#ifdef TRACE_REDUCTION_CALLSTACK
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

class ReductionStack : public Stack<ReductionStackElement> {
private:
  ParserTree *m_tree;
public:
  ReductionStack() : m_tree(NULL) {
  }
  void push(const TCHAR *method, const String &s, const ExpressionNode *n = NULL);
  void pop( const TCHAR *method);
  void reset(ParserTree *tree);
};

#define DEFINEREDUCTIONSTACK   ReductionStack _reductionStack;

#define STARTREDUCTION(tree)   _reductionStack.reset(tree)
#define ENTERMETHOD()          _reductionStack.push(method, format(_T("n:<%s>"), toString().cstr()),m_node)
#define ENTERMETHOD1(v)        _reductionStack.push(method, format(_T("%s:<%s>"), _T(#v), (v).toString().cstr()))
#define ENTERMETHOD2(v1,v2)    _reductionStack.push(method, format(_T("%s:<%s>, %s:<%s>"), _T(#v1), (v1).toString().cstr(), _T(#v2), (v2).toString().cstr()))
#define ENTERMETHOD2NUM(v1,v2) _reductionStack.push(method, format(_T("%s:<%s>, %s:<%s>"), _T(#v1), ::toString(v2).cstr(), _T(#v2), ::toString(v2).cstr()))
#define LEAVEMETHOD()          _reductionStack.pop(method)

#define RETURN(x) { LEAVEMETHOD(); return x; }

#define RETURNNULL                                                                         \
{ _reductionStack.push(method, _T("Return NULL"));                                         \
  _reductionStack.pop(method);                                                             \
  RETURN(NULL);                                                                            \
}

#define RETURNBOOL(b)                                                                      \
{ const bool _b = b;                                                                       \
  _reductionStack.push(method, format(method, _T("Return %s"), boolToStr(_b)));            \
  _reductionStack.pop(method);                                                             \
  RETURN(_b);                                                                              \
}

#define RETURNNODE(n)                                                                       \
{ const SNode &_n = n;                                                                      \
  _reductionStack.push(method, format(_T("Reduced:<%s>"), _n.toString().cstr()),_n.m_node); \
  _reductionStack.pop(method);                                                              \
  RETURN(_n);                                                                               \
}

#define RETURNSHOWSTR(v)                                                                   \
{ const String _s = (v).toString();                                                        \
  _reductionStack.push(method, format(_T("%s:<%s>"), _T(#v), _s.cstr()));                  \
  _reductionStack.pop(method);                                                             \
  RETURN(v);                                                                               \
}

#define RETURNPSHOWSTR(p)                                                                  \
{ const String _s = (p)->toString();                                                       \
  _reductionStack.push(method, format(_T("%s:<%s>"), _T(#p), _s.cstr()));                  \
  _reductionStack.pop(method);                                                             \
  RETURN(p);                                                                               \
}

#else // TRACE_REDUCTION_CALLSTACK

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

#endif // TRACE_REDUCTION_CALLSTACK
