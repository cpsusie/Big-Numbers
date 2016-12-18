#pragma once

#include <Math/MathLib.h>
#include <Math/Expression/Expression.h>

class DiffEquationDescription {
public:
  String m_name;     // name of the function;
  String m_expr;
  DiffEquationDescription() {
  }
  DiffEquationDescription(const String &name, const String &expr) : m_name(name), m_expr(expr) {
  }
  static bool isValidName(const String &name);
};

typedef Array<DiffEquationDescription> DiffEquationSystemDescription;

class ExpressionInputIndex {
public:
  UINT  m_index;
  Real *m_var;
  inline ExpressionInputIndex() : m_index(-1), m_var(NULL) {
  }
  inline ExpressionInputIndex(UINT index, Real *var) : m_index(index), m_var(var) {
  }
};

class ExpressionWithInputVector : public Expression {
public:
  CompactArray<ExpressionInputIndex> m_input;
  Real evaluate(const Vector &x);
};

class ErrorPosition {
public:
  int            m_eqIndex;
  bool           m_inExpr;
  SourcePosition m_pos;
  ErrorPosition(const String &error);
  inline ErrorPosition() {}
};

class CompilerErrorList : public StringArray {
public:
  void vaddError(UINT eqIndex, bool expr, const TCHAR *format, va_list argptr);
  void addError( UINT eqIndex, bool expr, const TCHAR *format, ...);
  void addErrors(UINT eqIndex, const StringArray &errors); // errors from Expression.compile()
  bool isOk() const {
    return isEmpty();
  }
  void throwFirstError() const { // assume !isOk()
    throwException((*this)[0]);
  }
};

class DiffEquationSystem : public VectorFunction {
private:
  DiffEquationSystemDescription            m_equationSystemDescription;
  CompactArray<ExpressionWithInputVector*> m_exprArray;
  void cleanup();
public:
  DiffEquationSystem() {
  }
  ~DiffEquationSystem();
  DiffEquationSystem(           const DiffEquationSystem &src);
  DiffEquationSystem &operator=(const DiffEquationSystem &src);
  bool        compile(CompilerErrorList &errorList);
  bool        compile(        const DiffEquationSystemDescription &desc, CompilerErrorList &errorList);
  void        setDescription( const DiffEquationSystemDescription &desc); // throw Exception if desc does not compile
  static bool validate(       const DiffEquationSystemDescription &desc, CompilerErrorList &errorList);
  inline bool isCompiled() const {
    return m_exprArray.size() > 0;
  }
  inline int getEquationCount() const {
    return (int)m_equationSystemDescription.size();
  }
  inline const DiffEquationSystemDescription &getEquationsystemDescription() const {
    return m_equationSystemDescription;
  }
  Vector operator()(const Vector &x);
};
