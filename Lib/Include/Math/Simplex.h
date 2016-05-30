#pragma once

#include <HashSet.h>
#include <BitSet.h>
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
    return m_leftSide.size();
  }

  void addLeftSideCoefficient(const Real &a) {
    m_leftSide.add(a);
  }

  const Real &getLeftSideCoefficient(unsigned int index) const { // index=[1..size]
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
    return CompactArray<Real>::size();
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
  TableauRow(unsigned int size);

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

  const unsigned int m_xCount;                  // Number of original variables
  unsigned int m_slackCount, m_artificialCount; // Number of slack- and artificial variables

  Array<TableauRow>       m_table;              // size = getRowCount()
  CompactArray<Real>      m_costFactor;         // size = m_xCount
  SimplexTracer          *m_tracer;
  int                     m_traceFlags;

  int getWidth() const {
    return m_xCount + m_slackCount + m_artificialCount;
  }

  int getRowCount() const {
    return m_table.size();
  }

  int getMaxColumnCount() const {
    return getMaxColumnCount(getConstraintCount());
  }

  int getMaxColumnCount(unsigned int constraintCount) const {
    return m_xCount + 2 * constraintCount + 1;
  }

  int getSlackColumn(     unsigned int index) const;        // index=[1..slackCount]
  int getArtificialColumn(unsigned int index) const;        // index=[1..artificialCount]

  Real &slackVar(    unsigned int row, unsigned int index); // row=[1..constraintCount], index=[1..slackCount]
  Real &artificalVar(unsigned int row, unsigned int index); // row=[1..constraintCount], index=[1..artificialCount]

  Real &objectFactor(unsigned int column) {
    return m_table[0].m_a[column];
  }

  void allocate(           unsigned int constraintCount);
  void preparePhase1();
  SimplexResult endPhase1();
  void pivot(              unsigned int pivotRow, unsigned int pivotColumn, bool primalSimplex);
  void multiplyRow(        unsigned int row, const Real &factor);
  void addRowsToGetZero(   unsigned int dstRow, unsigned int srcRow, unsigned int column);
  void addSlackColumn();

  void setLeftSide(        unsigned int row, unsigned int column, const Real &value);
  void setRightSide(       unsigned int row, const Real &b);
  const Real &getRightSide(unsigned int row) const;
  void setRelation(        unsigned int row, SimplexRelation relation);
  void setCostFactor(      unsigned int xIndex, const Real &value);

  bool rowIsZero(          unsigned int row);
  int  getInequalityCount() const;
  TCHAR getVariablePrefix(int index) const; // 'X' for original variables, 'S' for Slack- and  'A' for artificial variables

  void checkInvariant() const;
  void traceTableau() const;
  void traceBasis(const TCHAR *label) const;
  void traceDegeneracy(int pivotRow, int pivotColumn, const Real &minRatio) const;
  void trace(int flag, const TCHAR *format, ...) const;
  void vtrace(const TCHAR *format, va_list argptr) const;
public:
  Tableau(unsigned int xCount, unsigned int constraintCount, SimplexTracer *tracer = NULL, int traceFlags = TRACE_MAINSTEP);
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

  void trace(const TCHAR *format, ...) const;

  bool isTracing(int flag) const {
    return m_tracer != NULL && ((m_traceFlags & flag) != 0);
  }

  int getXCount() const {
    return m_xCount;
  }

  int getSlackCount() const {
    return m_slackCount;
  }

  int getArtCount() const {
    return m_artificialCount;
  }

  int getConstraintCount() const {
    return m_table.size()-1;
  }

  const Real &getCostFactor(unsigned int column) const {
    return m_costFactor[column];
  }

  const Real &getObjectFactor(unsigned int column) const {
    return m_table[0].m_a[column];
  }

  const Real getObjectValue() const {
    return -m_table[0].m_a[0];
  }

  bool isPrimalFeasible() const;
  bool isDualFeasible() const;

  void addConstraint(unsigned int xIndex, SimplexRelation relation, const Real &rightSide);
  void setCostFactors(TableauCostFactors &factors);
  void setConstraint(unsigned int row, const TableauConstraint &constraint);
  void setConstraint(unsigned int row, const CompactArray<Real> &leftSide, SimplexRelation relation, const Real &rightSide);
  void checkTableau();
  CompactArray<BasisVariable> getBasisVariables() const;
  BitSet getNonBasisVariables() const;
  SimplexSolution getSolution() const;
  String getVariableName(int index) const;
  String toString(int fieldSize = 12, int decimals = 5) const;

  static Real               getSlackFactor(   SimplexRelation relation);
  static const TCHAR       *getRelationString(SimplexRelation relation);
  static SimplexRelation    reverseRelation(  SimplexRelation relation);
  static CompactArray<Real> createRealArray(unsigned int size);

  friend class DictionaryKey;
};
