#pragma once

#include <Math/Matrix.h>
#include <Math/Expression/Expression.h>

class DiffEquationDescription {
public:
  // name of the function;
  String m_name;
  String m_expr;
  DiffEquationDescription() {
  }
  DiffEquationDescription(const String &name, const String &expr) : m_name(name), m_expr(expr) {
  }
  static bool isValidName(const String &name);
};

class DiffEquationSystemDescription : public Array<DiffEquationDescription> {
public:
  // prefixed to all expression
  String m_commonText;
};

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

typedef enum {
  ERROR_INNAME   = 'N'
 ,ERROR_INCOMMON = 'C'
 ,ERROR_INEXPR   = 'E'
} ErrorLocation;

class ErrorPosition {
public:
  ErrorLocation  m_location;
  int            m_eqIndex;
  SourcePosition m_pos;
  ErrorPosition(const String &error);
  inline ErrorPosition() {}
};

class CompilerErrorList : public StringArray {
public:
  void vaddError(UINT eqIndex, ErrorLocation expr, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void addError( UINT eqIndex, ErrorLocation expr, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  // add errors from Expression.compile(). return true if any errors from common text
  bool addErrors(UINT eqIndex, const StringArray &errors, const String &expr, int prefixLen);
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
  // throw Exception if desc does not compile
  void        setDescription( const DiffEquationSystemDescription &desc);
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
