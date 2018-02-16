#pragma once

#include <HashSet.h>
#include <BitSet.h>
#include "Real.h"
#include "AlgorithmHandler.h"

typedef enum {
  SIMPLEX_SOLUTION_OK        =  0,
  SIMPLEX_NO_SOLUTION        = -1,
  SIMPLEX_NO_SOLUTION_FOUND  = -2,
  SIMPLEX_SOLUTION_UNLIMITED = -3
} SimplexResult;

typedef enum {
  EQUALS      = '=' //
 ,LESSTHAN    = '<' // less than or equal
 ,GREATERTHAN = '>' // greater than or equal
} SimplexRelation;

class Tableau;

class DictionaryKey : public BitSet {
public:
  DictionaryKey(const Tableau *tab);
};

class DictionaryKeySet : public HashSet<DictionaryKey> {
public:
  DictionaryKeySet();
};

class BasisVariable {
public:
  TCHAR m_prefix;
  int   m_index;
  Real  m_value;
  Real  m_costFactor;
  BasisVariable()
  : m_prefix(_T('?'))
  , m_index(-1)
  , m_value(0)
  , m_costFactor(0)
  {
  }
  BasisVariable(TCHAR prefix, int index, const Real &value, const Real &costFactor)
  : m_prefix(prefix)
  , m_index(index)
  , m_value(value)
  , m_costFactor(costFactor)
  {
  }
  String getName() const;
  String toString() const;
};

class SimplexSolution {
private:
  CompactArray<BasisVariable> m_variables;
  Real                        m_totalCost;
public:
  SimplexSolution(const CompactArray<BasisVariable> variables, const Real &totalCost);
  SimplexSolution();
  String toString() const;

  const CompactArray<BasisVariable> &getVariables() const {
    return m_variables;
  }
  const Real &getTotalCost() const {
    return m_totalCost;
  }
};

class SimplexTraceElement {
public:
  const Tableau &m_tableau;
  String         m_msg;
  SimplexTraceElement(const Tableau &tableau, const String &msg) : m_tableau(tableau) {
    m_msg = msg;
  }
};

typedef AlgorithmHandler<SimplexTraceElement> SimplexTracer;

class TableauConstraint {
private:
  CompactArray<Real> m_leftSide;
  SimplexRelation    m_relation;
  Real               m_rightSide;

public:
  TableauConstraint(const String &str);
  TableauConstraint(const CompactArray<Real> &leftSide, SimplexRelation relation, const Real &rightSide);

  int getXCount() const {
    return (int)m_leftSide.size();
  }

  void addLeftSideCoefficient(const Real &a) {
    m_leftSide.add(a);
  }

  const Real &getLeftSideCoefficient(UINT index) const { // index=[1..size]
    return m_leftSide[index-1];
  }

  friend class Tableau;
};

class TableauCostFactors : private CompactArray<Real> {
public:
  TableauCostFactors() {};
  TableauCostFactors(const String &str);

  const Real &operator[](int index) const {                      // index=[1..size]
    return CompactArray<Real>::operator[](index-1);
  }
  int size() const {
    return (int)CompactArray<Real>::size();
  }
  void add(const Real &x) {
    CompactArray<Real>::add(x);
  }
};

#define TRACE_MAINSTEP     0x01
#define TRACE_ITERATIONS   0x02
#define TRACE_PIVOTING     0x04
#define TRACE_TABLEAU      0x08
#define TRACE_SOLUTIONS    0x10
#define TRACE_WARNINGS     0x20
#define TRACE_ALL          0x3f

class TableauRow {
private:
  int                m_basisVariable;
  SimplexRelation    m_relation;
  CompactArray<Real> m_a;
public:
  TableauRow(UINT size);

  const Real &getRightSide() const {
    return m_a[0];
  }

  void setRightSide(const Real &b) {
    m_a[0] = b;
  }

  friend class Tableau;
  friend class DictionaryKey;
};

class Tableau {
private:
  DictionaryKeySet m_dictionarySet;

  const size_t m_xCount;                  // Number of original variables
  size_t m_slackCount, m_artificialCount; // Number of slack- and artificial variables

  Array<TableauRow>       m_table;              // size = getRowCount()
  CompactArray<Real>      m_costFactor;         // size = m_xCount
  SimplexTracer          *m_tracer;
  int                     m_traceFlags;

  int getWidth() const {
    return (int)(m_xCount + m_slackCount + m_artificialCount);
  }

  int getRowCount() const {
    return (int)m_table.size();
  }

  int getMaxColumnCount() const {
    return getMaxColumnCount(getConstraintCount());
  }

  int getMaxColumnCount(size_t constraintCount) const {
    return (int)(m_xCount + 2 * constraintCount + 1);
  }

  int getSlackColumn(     size_t index) const;  // index=[1..slackCount]
  int getArtificialColumn(size_t index) const;  // index=[1..artificialCount]

  Real &slackVar(    size_t row, size_t index); // row=[1..constraintCount], index=[1..slackCount]
  Real &artificalVar(size_t row, size_t index); // row=[1..constraintCount], index=[1..artificialCount]

  Real &objectFactor(size_t column) {
    return m_table[0].m_a[column];
  }

  void allocate(size_t constraintCount);
  void preparePhase1();
  SimplexResult endPhase1();
  void pivot(              size_t pivotRow, size_t pivotColumn, bool primalSimplex);
  void multiplyRow(        size_t row, const Real &factor);
  void addRowsToGetZero(   size_t dstRow, size_t srcRow, size_t column);
  void addSlackColumn();

  void setLeftSide(        size_t row, size_t column, const Real &value);
  void setRightSide(       size_t row, const Real &b);
  const Real &getRightSide(size_t row) const;
  void setRelation(        size_t row, SimplexRelation relation);
  void setCostFactor(      size_t xIndex, const Real &value);

  bool rowIsZero(          size_t row);
  int  getInequalityCount() const;
  TCHAR getVariablePrefix(size_t index) const; // 'X' for original variables, 'S' for Slack- and  'A' for artificial variables

  void checkInvariant() const;
  void traceTableau() const;
  void traceBasis(const TCHAR *label) const;
  void traceDegeneracy(int pivotRow, int pivotColumn, const Real &minRatio) const;
  void trace(int flag, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) const;
  void vtrace(         _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) const;
public:
  Tableau(size_t xCount, size_t constraintCount, SimplexTracer *tracer = NULL, int traceFlags = TRACE_MAINSTEP);
  Tableau(const Tableau &src);

  SimplexResult primalSimplex(int phase); // Parameter phase only for tracing
  SimplexResult dualSimplex();
  SimplexResult twoPhaseSimplex();
  SimplexResult maksimize();
  SimplexResult minimize();

  SimplexTracer *setTracer(SimplexTracer *tracer, int traceFlags = TRACE_MAINSTEP);

  SimplexTracer *getTracer() {
    return m_tracer;
  }

  void trace(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const;

  bool isTracing(int flag) const {
    return m_tracer != NULL && ((m_traceFlags & flag) != 0);
  }

  int getXCount() const {
    return (int)m_xCount;
  }

  int getSlackCount() const {
    return (int)m_slackCount;
  }

  int getArtCount() const {
    return (int)m_artificialCount;
  }

  int getConstraintCount() const {
    return (int)m_table.size()-1;
  }

  const Real &getCostFactor(size_t column) const {
    return m_costFactor[column];
  }

  const Real &getObjectFactor(size_t column) const {
    return m_table[0].m_a[column];
  }

  const Real getObjectValue() const {
    return -m_table[0].m_a[0];
  }

  bool isPrimalFeasible() const;
  bool isDualFeasible() const;

  void addConstraint(size_t xIndex, SimplexRelation relation, const Real &rightSide);
  void setCostFactors(TableauCostFactors &factors);
  void setConstraint(size_t row, const TableauConstraint &constraint);
  void setConstraint(size_t row, const CompactArray<Real> &leftSide, SimplexRelation relation, const Real &rightSide);
  void checkTableau();
  CompactArray<BasisVariable> getBasisVariables() const;
  BitSet getNonBasisVariables() const;
  SimplexSolution getSolution() const;
  String getVariableName(size_t index) const;
  String toString(int fieldSize = 12, int decimals = 5) const;

  static Real               getSlackFactor(   SimplexRelation relation);
  static const TCHAR       *getRelationString(SimplexRelation relation);
  static SimplexRelation    reverseRelation(  SimplexRelation relation);
  static CompactArray<Real> createRealArray(size_t size);

  friend class DictionaryKey;
};
