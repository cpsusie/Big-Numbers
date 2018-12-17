#pragma once

#include <Math/Matrix.h>
#include <Math/Expression/Expression.h>

using namespace Expr;

class DiffEquationDescription {
private:
  // name of the function; exquation is assumed to look like the following: m_name' = m_expr
  String m_name;
  String m_expr;
public:
  inline DiffEquationDescription() {
  }
  inline DiffEquationDescription(const String &name, const String &expr) : m_name(name), m_expr(expr) {
  }
  inline String setName(const String &name) {
    const String oldName = m_name; m_name = name; return oldName;
  }
  inline const String &getName() const {
    return m_name;
  }
  // return old exprText
  inline String setExprText(const String &expr) {
    const String oldExpr = m_expr; m_expr = expr; return oldExpr;
  }
  inline const String &getExprText() const {
    return m_expr;
  }
  static bool isValidName(const String &name);
};

class DiffEquationDescriptionArray : public Array<DiffEquationDescription> {
private:
  // Prefixed to all expressions in EquationSystem before compile
  String m_commonText;
public:
  // Prefixed to all expressions in EquationSystem
  inline const String &getCommonText() const {
    return m_commonText;
  }
  String setCommonText(const String &text) {
    const String oldText = m_commonText; m_commonText = text; return oldText;
  }
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
private:
  CompactArray<ExpressionInputIndex> m_input;
public:
  inline void addInputIndex(const ExpressionInputIndex &e) {
    m_input.add(e);
  }
  Real evaluate(const Vector &x);
};

typedef enum {
  ERROR_INNAME   = 'N'
 ,ERROR_INCOMMON = 'C'
 ,ERROR_INEXPR   = 'E'
} ErrorLocation;

class ErrorPosition {
private:
  ErrorLocation  m_location;
  int            m_eqIndex; // == -1 if m_location == ERROR_INCOMMON
  SourcePosition m_pos;
public:
  ErrorPosition(const String &error);
  inline ErrorPosition() {
  }
  inline ErrorLocation getLocation() const {
    return m_location;
  }
  inline int getEquationIndex() const {
    return m_eqIndex;
  }
  inline void setSourcePosition(const SourcePosition &pos) {
    m_pos = pos;
  }
  inline const SourcePosition &getSourcePosition() const {
    return m_pos;
  }
};

class CompilerErrorList : public StringArray {
public:
  void vaddError(UINT eqIndex, ErrorLocation expr, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void addError( UINT eqIndex, ErrorLocation expr, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  // add errors from Expression.compile(). return true if any errors from common text
  bool addErrors(UINT eqIndex, const StringArray &errors, const String &expr, UINT prefixLen);
  bool isOk() const {
    return isEmpty();
  }
  void throwFirstError() const { // assume !isOk()
    throwException((*this)[0]);
  }
};

class DiffEquationSystem : public VectorFunction {
private:
  DiffEquationDescriptionArray             m_equationDescriptionArray;
  CompactArray<ExpressionWithInputVector*> m_exprArray;
  void cleanup();
public:
  DiffEquationSystem() {
  }
  ~DiffEquationSystem();
  DiffEquationSystem(           const DiffEquationSystem &src);
  DiffEquationSystem &operator=(const DiffEquationSystem &src);
  bool        compile(CompilerErrorList &errorList);
  bool        compile(        const DiffEquationDescriptionArray &desc, CompilerErrorList &errorList);
  // throw Exception if desc does not compile
  void        setDescription( const DiffEquationDescriptionArray &desc);
  static bool validate(       const DiffEquationDescriptionArray &desc, CompilerErrorList &errorList);
  inline bool isCompiled() const {
    return m_exprArray.size() > 0;
  }
  inline UINT getEquationCount() const {
    return (UINT)m_equationDescriptionArray.size();
  }
  inline const DiffEquationDescriptionArray &getEquationDescriptionArray() const {
    return m_equationDescriptionArray;
  }
  Vector operator()(const Vector &x);
};
