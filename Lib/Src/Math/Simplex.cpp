#include "pch.h"
#include <BitSet.h>
#include <Tokenizer.h>
#include <Math/Simplex.h>

#define EPS 1e-8

#define FSZ(n) format1000(n).cstr()

Tableau::Tableau(size_t xCount, size_t constraintCount, SimplexTracer *tracer, int traceFlags) : m_xCount(xCount) {
  allocate(constraintCount);
  m_tracer     = tracer;
  m_traceFlags = traceFlags;
}

Tableau::Tableau(const Tableau &src) : m_xCount(src.m_xCount) {
  m_slackCount      = src.m_slackCount;
  m_artificialCount = src.m_artificialCount;
  m_table           = src.m_table;
  m_costFactor      = src.m_costFactor;
  m_tracer          = src.m_tracer;
  m_traceFlags      = src.m_traceFlags;
}

void Tableau::allocate(size_t constraintCount) {
  m_slackCount      = 0;
  m_artificialCount = 0;

  m_table.clear();
  for(UINT row = 0; row <= constraintCount; row++) {
    m_table.add(TableauRow(getMaxColumnCount(constraintCount)));
  }

  m_costFactor = createRealArray(getXCount()+1);
}

SimplexResult Tableau::twoPhaseSimplex() {
  SimplexResult rc;

  trace(TRACE_MAINSTEP,_T("Phase 1:Finding basic feasible solution."));
  preparePhase1();
  if((rc = primalSimplex(1))  != SIMPLEX_SOLUTION_OK)
    return rc;

  trace(TRACE_MAINSTEP,_T("End phase 1."));
  if((rc = endPhase1()) != SIMPLEX_SOLUTION_OK)
    return rc;
  traceTableau();

  trace(TRACE_MAINSTEP,_T("Found basic feasible solution."));
  trace(TRACE_MAINSTEP ,_T("Phase 2:Finding optimal basic feasible solution."));
  rc = primalSimplex(2);
  trace(TRACE_MAINSTEP ,_T("End phase 2."));
  traceTableau();

  if(rc == SIMPLEX_SOLUTION_OK) {
    traceBasis(_T("Solution:"));
  }

  return SIMPLEX_SOLUTION_OK;
}

// Parameter phase only for tracing
SimplexResult Tableau::primalSimplex(int phase) {
  const int width           = getWidth();
  const int constraintCount = getConstraintCount();

  for(int r = 1; r <= constraintCount; r++) {
    const TableauRow &row = m_table[r];
    const int bv = row.m_basisVariable;
    const Real &a = row.m_a[bv];
    if(a == 0) {
      continue;
    }
    Real factor = getObjectFactor(bv) / a;
    if(factor == 0) {
      trace(TRACE_WARNINGS,_T("Warning:Cost factor[%s] = 0."), getVariableName(bv).cstr());
      continue;
    }
    for(int col = 0; col <= width; col++) {
      objectFactor(col) -= row.m_a[col] * factor;
    }
    objectFactor(bv) = 0;
  }

  bool looping = false;

  for(int iteration = 1;; iteration++) {
    if(isTracing(TRACE_ITERATIONS)) {
      trace(_T("Phase %d. Iteration %d"), phase, iteration);
      traceTableau();
//      traceBasis(_T("Current basis:"));
    }

    int pivotColumn;
    Real minCost = 1;

    if(looping) {
      for(int col = 1; col <= width; col++) {    // Apply Bland's anti-cycling rule step 1.
        const Real &cc = objectFactor(col);
        if(cc < -EPS) {
          minCost     = cc;
          pivotColumn = col;
          break;
        }
      }
    } else {                                     // else find pivot column the old way
      for(int col = 1; col <= width; col++) {
        const Real &cc = objectFactor(col);
        if(cc < minCost) {
          minCost     = cc;
          pivotColumn = col;
        }
      }
    }

    if(minCost >= -EPS) {
      return SIMPLEX_SOLUTION_OK;                // Optimal value found
    }

    int pivotRow  = -1;
    Real minRatio = 0;
    if(looping) {                                // apply Bland's anti-cycling rule step 2.
      for(int r = 1; r <= constraintCount; r++) {
        const Real &ar = m_table[r].m_a[pivotColumn];
        if(ar > 10*EPS) {
          const Real &br = getRightSide(r);
          const Real ratio = br / ar;
          if((pivotRow == -1) || (ratio < minRatio)) {
            minRatio = ratio;
            pivotRow = r;
          }
        }
      }
      if(pivotRow >= 0) {
        int minIndex = -1;
        for(int r = 1; r <= constraintCount; r++) {
          const Real &ar = m_table[r].m_a[pivotColumn];
          if(ar > 10*EPS) {
            const Real &br = getRightSide(r);
            const Real ratio = br / ar;
            if(ratio == minRatio) {
              const int index = m_table[r].m_basisVariable;
              if(minIndex == -1 || index < minIndex) {
                minIndex = index;
                pivotRow = r;
              }
            }
          }
        }
      }
    } else {                                     // else find pivot row the old way
      for(int r = 1; r <= constraintCount; r++) {
        const Real &ar = m_table[r].m_a[pivotColumn];
        if(ar > EPS) {
          const Real &br = getRightSide(r);
          const Real ratio = br / ar;
          if(pivotRow == -1 || ratio < minRatio) {
            minRatio = ratio;
            pivotRow = r;
          }
        }
      }
    }

    if(pivotRow == -1) {
      return SIMPLEX_SOLUTION_UNLIMITED;
    }

                                                 // Check for degeneracy
    traceDegeneracy(pivotRow, pivotColumn, minRatio);

    DictionaryKey dictKey(this);
    if(m_dictionarySet.contains(dictKey)) {
      looping = true;
      trace(TRACE_WARNINGS, _T("Looping. Applying Blands anti cycling rule."));
    } else {
      m_dictionarySet.add(dictKey);
    }

    pivot(pivotRow,pivotColumn,true);
  }
}

SimplexResult Tableau::dualSimplex() {
  const int constraintCount = getConstraintCount();
  const int width           = getWidth();

  for(int iteration = 1;; iteration++) {

    if(isTracing(TRACE_ITERATIONS)) {
      trace(_T("Dual simplex. Iteration %d."), iteration);
      traceTableau();
//      traceBasis(_T("Current basis:"));
    }

    int pivotRow;
    Real minR = 1;
    for(int row = 1; row <= constraintCount; row++) { // find pivot row
      if(getRightSide(row) < minR) {
        pivotRow = row;
        minR     = getRightSide(row);
      }
    }

    if(minR >= 0) { // Every rightside is nonNegative
      return SIMPLEX_SOLUTION_OK;
    }

    int pivotColumn = -1;
    Real minRatio = 0;
    const CompactArray<Real> &tableRow = m_table[pivotRow].m_a;
    for(int col = 1; col <= width; col++) { // find pivot col
      const Real &a = tableRow[col];
      if(a < 0) {
        Real ratio = -getObjectFactor(col)/a;
        if(pivotColumn == -1 || ratio < minRatio) {
          minRatio = ratio;
          pivotColumn = col;
        }
      }
    }

//    printf(_T("pivotColumn:%d\n"),pivotColumn);
    if(pivotColumn == -1) {
      return SIMPLEX_NO_SOLUTION;
    }

    pivot(pivotRow,pivotColumn,false);
  }
}

void Tableau::preparePhase1() {
  m_slackCount      = getInequalityCount();
  m_artificialCount = getConstraintCount();
  const int constraintCount = getConstraintCount();

  int slackIndex = 1;
  for(int row = 1; row <= constraintCount; row++) {
    for(UINT index = 1; index <= m_slackCount; index++) { // Clear Slack matrix
      slackVar(row,index) = 0;
    }
    for(UINT index = 1; index <= m_artificialCount; index++) {         // Clear Artificial matrix
      artificalVar(row,index) = 0;
    }

    artificalVar(row,row) = 1;

    Real slackFactor = getSlackFactor(m_table[row].m_relation);
    if(slackFactor != 0) {                                        // Set Slack[row,slackIndex]
      slackVar(row,slackIndex) = slackFactor;
      slackIndex++;
    }

    if(getRightSide(row) < 0) {
      multiplyRow(row,-1);
    }
  }

  // Set temporary object function, with cost factors = 1 for all artificial variables and 0 for the other variables
  for(int row = 1; row <= constraintCount; row++) {
    int c = m_table[row].m_basisVariable = getArtificialColumn(row);
    objectFactor(c) = 1;
  }
  for(int col = 1; col <= getXCount(); col++) {
    objectFactor(col) = 0;
  }
  for(UINT index = 1; index <= m_slackCount; index++) {
    objectFactor(getSlackColumn(index)) = 0;
  }
}

SimplexResult Tableau::endPhase1() {
  const int constraintCount = getConstraintCount();
  for(int row = 1; row <= constraintCount; row++) {
    const int bv = m_table[row].m_basisVariable;
    if(bv >= getArtificialColumn(1)) {
      trace(TRACE_WARNINGS,_T("endPhase1:Artificial variabel %s not eliminated. Value=%lg"), getVariableName(bv).cstr(),(double)getRightSide(row));
      traceTableau();

      if(getRightSide(row) == 0) {
        return SIMPLEX_NO_SOLUTION_FOUND;
      } else {
        return SIMPLEX_NO_SOLUTION;
      }
    }
  }

  // Delete all artificial variables
  m_artificialCount = 0;

  // Restore original costFactors
  int col;
  for(col = 1; col <= getXCount(); col++) {
    objectFactor(col) = m_costFactor[col];
  }
  for(int index = 1; col <= (int)m_slackCount; index++) {
    objectFactor(getSlackColumn(index)) = 0;
  }

  return SIMPLEX_SOLUTION_OK;
}

void Tableau::pivot(size_t pivotRow, size_t pivotColumn, bool primalSimplex) {
  TableauRow &row = m_table[pivotRow];
  const int leaveBasis = row.m_basisVariable;
  const int enterBasis = (int)pivotColumn;

  row.m_basisVariable = (int)pivotColumn;

  const Real &factor = row.m_a[pivotColumn];
  const String enteringVarName = getVariableName(enterBasis);
  const String leavingVarName  = getVariableName(leaveBasis);

  if(isTracing(TRACE_PIVOTING)) {
    if(primalSimplex) {
      trace(_T("pivot(%2s,%2s). %s -> %s. Cost[%s]:%-15.10lg   Tab[%2s,%2s]=%-15.10lg   Minimum:%-15.10lg")
            ,FSZ(pivotRow),FSZ(pivotColumn)
            ,enteringVarName.cstr(),leavingVarName.cstr()
            ,enteringVarName.cstr(),(double)getObjectFactor(pivotColumn)
            ,FSZ(pivotRow),FSZ(pivotColumn),(double)factor
            ,(double)getObjectValue());
    } else {
      trace(_T("pivot(%2s,%2s). %s -> %s. %s=B[%2s]:%-15.10lg  Tab[%2s,%2s]=%-15.10lg   Minimum:%-15.10lg")
            ,FSZ(pivotRow),FSZ(pivotColumn)
            ,enteringVarName.cstr(),leavingVarName.cstr()
            ,leavingVarName.cstr(),FSZ(pivotRow),(double)getRightSide(pivotRow)
            ,FSZ(pivotRow),FSZ(pivotColumn),(double)factor
            ,(double)getObjectValue());
    }
  }

  multiplyRow(pivotRow,1.0/factor);

  for(int dstRow = 0; dstRow <= getConstraintCount(); dstRow++) {
    if(dstRow != (int)pivotRow) {
      addRowsToGetZero(dstRow,pivotRow,pivotColumn);
    }
  }
}

void Tableau::multiplyRow(size_t r, const Real &factor) {
  const int w = getWidth();
  TableauRow &row = m_table[r];
  CompactArray<Real> &a = row.m_a;
  for(int col = 0; col <= w; col++) {
    a[col] *= factor;
  }
  if(factor < 0) {
    row.m_relation = reverseRelation(row.m_relation);
  }
}

// Add a multiplum of srcRow to dstRow to get a zero in table[dstRow][column]. Elementary matrix operation
void Tableau::addRowsToGetZero(size_t dstRow, size_t srcRow, size_t column) {
  const int width = getWidth();
  CompactArray<Real> &src = m_table[srcRow].m_a;
  CompactArray<Real> &dst = m_table[dstRow].m_a;
  const Real factor = dst[column];

  for(int col = 0; col <= width; col++) {
    Real d = dst[col] - src[col] * factor;
    dst[col] = (fabs(d) < fabs(EPS * dst[col])) ? 0 : d;
  }
  dst[column] = 0;
}

void Tableau::checkTableau() {
  for(int row = 0; row <= getConstraintCount(); row++) {
    if(rowIsZero(row)) {
      throwException(_T("Row %d is zero"), row);
    }
  }

  for(int col = 1; col <= getXCount(); col++) {
    if(getObjectFactor(col) == 0) {
      throwException(_T("CostFactor[%d] is zero"), col);
    }
  }

  trace(TRACE_MAINSTEP,_T("Checked ok. Unknown:%d, Constraints:%d"), getXCount(), getConstraintCount());
  traceTableau();
}

bool Tableau::rowIsZero(size_t row) {
  for(int col = 1; col <= getXCount() ;col++) {
    if(m_table[row].m_a[col] != 0) {
      return false;
    }
  }
  return true;
}

int Tableau::getSlackColumn(size_t index) const {
  if(index < 1 || index > m_slackCount) {
    throwException(_T("getSlackColumn:Illegal argument:index=%s. Valid interval=[1..%zu]"), FSZ(index), m_slackCount);
  }
  return (int)(m_xCount + index);
}

int Tableau::getArtificialColumn(size_t index) const {
  if(index < 1 || index > m_artificialCount) {
    throwException(_T("getArtificialColumn:Illegal argument:index=%s. Valid interval=[1..%s]"), FSZ(index), FSZ(m_artificialCount));
  }
  return (int)(m_xCount + m_slackCount + index);
}

Real &Tableau::slackVar(size_t row, size_t index) {
  if(row < 1 || (int)row > getConstraintCount() || index < 1 || index > m_slackCount) {
    throwException(_T("slackVar:Illegal arguments. (row,index)=(%s,%s). Valid row interval=[1..%s]. Valid index interval=[1..%s]")
                   ,FSZ(row), FSZ(index), FSZ(getConstraintCount()), FSZ(m_slackCount));
  }
  return m_table[row].m_a[getSlackColumn(index)];
}

Real &Tableau::artificalVar(size_t row, size_t index) {
  if(row < 1 || (int)row > getConstraintCount() || index < 1 || index > m_artificialCount) {
    throwException(_T("artificalVar:Illegal arguments. (row,index)=(%s,%s). Valid row interval=[1..%s]. Valid index interval=[1..%s]")
                   ,FSZ(row), FSZ(index), FSZ(getConstraintCount()), FSZ(m_artificialCount));
  }
  return m_table[row].m_a[getArtificialColumn(index)];
}

int Tableau::getInequalityCount() const {
  int count = 0;
  for(int row = 1; row <= getConstraintCount(); row++) {
    if(m_table[row].m_relation != EQUALS) {
      count++;
    }
  }
  return count;
}

void Tableau::setRelation(size_t row, SimplexRelation relation) { // private
  if(strchr( "=<>", relation ) == nullptr) {
    throwException(_T("Tableau::setRelation::Invalid relational operator (=%c). Must be <, > or ="),relation);
  }

  if(row < 1 || (int)row > getConstraintCount()) {
    throwException(_T("setRelation:Illegal argument:Row=%s. Valid interval=[1..%s]"),FSZ(row),FSZ(getConstraintCount()));
  }
  m_table[row].m_relation = relation;
}

void Tableau::setCostFactors(TableauCostFactors &factors) {
  if(factors.size() != getXCount()) {
    throwException(_T("Tableau::setCostFactors:Invalid number of factors specified. Argument.size=%d. xCount=%d."),factors.size(),getXCount());
  }
  for(int c = 1; c <= factors.size(); c++) {
    setCostFactor(c,factors[c]);
  }
}

void Tableau::setCostFactor(size_t xIndex, const Real &value) { // private
  if(xIndex < 1 || (int)xIndex > getXCount()) {
    throwException(_T("Tableau::setCostFactor:Illegal argument. xIndex=%s. Valid interval=[1..%s]"),FSZ(xIndex),FSZ(getXCount()));
  }

  objectFactor(xIndex) = value;
  m_costFactor[xIndex] = value;
}

void Tableau::addConstraint(size_t xIndex, SimplexRelation relation, const Real &rightSide) {
  if(xIndex < 1 || (int)xIndex >= getXCount()) {
    throwException(_T("addConstraint:Invalid xIndex=%s. Valid interval=[1..%s]"), FSZ(xIndex), FSZ(getXCount()));
  }

  Real currentValue = 0;
  int  bvRow        = -1;

  for(size_t i = 1; i < m_table.size(); i++) {
    if(m_table[i].m_basisVariable == (int)xIndex) {
      currentValue = getRightSide(i);
      bvRow = (int)i;
      break;
    }
  }

  String varName = getVariableName(xIndex);
  switch(relation) {
  case EQUALS     :
    if(currentValue == rightSide) {
      trace(_T("addConstraint:No need to add constraint %s = %-16lg. %s already has the specified value."), varName.cstr(), (double)rightSide, varName.cstr());
      return;
    } else if(currentValue < rightSide) {
      relation = GREATERTHAN;
    } else {
      relation = LESSTHAN;
    }
    break;

  case LESSTHAN   :
    if(currentValue <= rightSide) {
      trace(_T("addConstraint:No need to add constraint %s <= %-16lg. %s=%-16lg."), varName.cstr(), (double)rightSide, varName.cstr(), (double)currentValue);
      return;
    }
    break;

  case GREATERTHAN:
    if(currentValue >= rightSide) {
      trace(_T("addConstraint:No need to add constraint %s >= %-16lg. %s=%-16lg."), varName.cstr(), (double)rightSide, varName.cstr(), (double)currentValue);
      return;
    }
    break;
  }

  if(isTracing(TRACE_ITERATIONS)) {
    trace(_T("Add constraint %s %s %lg"), varName.cstr(), getRelationString(relation), (double)rightSide);
  }

  m_table.add(TableauRow(getMaxColumnCount()));
  const int newRowIndex = getConstraintCount();
  addSlackColumn();
  TableauRow &newRow = m_table[newRowIndex];
  newRow.m_a[xIndex] = 1;
  slackVar(newRowIndex,m_slackCount) = getSlackFactor(relation);
  setRightSide(newRowIndex,rightSide);
  newRow.m_basisVariable = getSlackColumn(m_slackCount);
  objectFactor(newRow.m_basisVariable) = 1;

//  if(isTracing()) trace(_T("addConstraint before normalizing:\n %s"),toString().cstr());

  if(bvRow >= 0) {
    addRowsToGetZero(newRowIndex,bvRow,xIndex);
  }

  if(getRightSide(newRowIndex) > 0)
    multiplyRow(newRowIndex,-1);

//  objectValue() += getRightSide(newRowIndex);
}

// Returns the index of the new column
void Tableau::addSlackColumn() {
  m_slackCount++;
  const int c = getSlackColumn(m_slackCount);
  if(getWidth() == m_table[0].m_a.size()) {
    for(int row = 0; row < getRowCount(); row++) {
      m_table[row].m_a.insert(c,0.0);
    }
  } else {
    for(int row = 0; row < getRowCount(); row++) {
      m_table[row].m_a[c] = 0;
    }
  }
}

void Tableau::setConstraint(size_t row, const TableauConstraint &constraint) {
  setConstraint(row,constraint.m_leftSide,constraint.m_relation,constraint.m_rightSide);
}

void Tableau::setConstraint(size_t row, const CompactArray<Real> &leftSide, SimplexRelation relation, const Real &rightSide) {
  if(row < 1 || (int)row > getConstraintCount()) {
    throwException(_T("Tableau::setConstraint:row=%s out of range. Legal interval=[1..%s]")
                  ,FSZ(row),FSZ(getConstraintCount()));
  }
  if(leftSide.size() != getXCount()) {
    throwException(_T("Tableau::setConstraint:Invalid number of values specified for leftside. leftSide.size=%s. Tableau.xCount=%s.")
                  ,FSZ(leftSide.size()),FSZ(getXCount()));
  }

  for(size_t col = 0; col < leftSide.size(); col++) {
    setLeftSide(row,col+1,leftSide[col]);
  }
  setRelation(row,relation);
  setRightSide(row,rightSide);
}

void Tableau::setLeftSide(size_t row, size_t column, const Real &value) {
  if((row < 1) || (column < 1) || ((int)row > getConstraintCount()) || ((int)column > getWidth())) {
    throwException(_T("Tableu::setLeftSide:Illegal index (row,column)=(%s,%s). valid row interval=[1..%s], valid columnIndex=[1..%s]")
                  ,FSZ(row),FSZ(column),FSZ(getConstraintCount()),FSZ(getWidth()));
  }
  m_table[row].m_a[column] = value;
}

void Tableau::setRightSide(size_t row, const Real &b) {
  if(row < 1 || (int)row > getConstraintCount()) {
    throwException(_T("Tableu::setRightSide:Illegal argument: row=%s. Valid row interval=[1..%s]")
                  ,FSZ(row),FSZ(getConstraintCount()));
  }
  m_table[row].setRightSide(b);
}

const Real &Tableau::getRightSide(size_t row) const {
  if(row < 1 || (int)row > getConstraintCount()) {
    throwException(_T("Tableu::getRightSide:Illegal argument: row=%s. Valid row interval=[1..%s]")
                  ,FSZ(row),FSZ(getConstraintCount()));
  }
  return m_table[row].getRightSide();
}

bool Tableau::isPrimalFeasible() const {
  const int constraintCount = getConstraintCount();
  for(int row = 1; row <= constraintCount; row++) {
    if(getRightSide(row) < 0) {
      return false;
    }
  }
  return true;
}

bool Tableau::isDualFeasible() const {
  const int columnCount = getWidth();
  const CompactArray<Real> &objectRow = m_table[0].m_a;
  for(int col = 1; col <= columnCount; col++) {
    if(objectRow[col] < 0) {
      return false;
    }
  }
  return true;
}

void Tableau::traceTableau() const {
  if(isTracing(TRACE_TABLEAU)) trace(_T("%s"),toString().cstr());
}

void Tableau::traceBasis(const TCHAR *label) const {
  if(isTracing(TRACE_SOLUTIONS)) trace(_T("%s\n%s"), label, getSolution().toString().cstr());
}

void Tableau::traceDegeneracy(int pivotRow, int pivotColumn, const Real &minRatio) const {
  if(!isTracing(TRACE_WARNINGS)) {
    return;
  }
  const int constraintCount = getConstraintCount();
  String traceString;
  TCHAR *delimiter = EMPTYSTRING;
  for(int r = 1; r <= constraintCount; r++) {
    if(r == pivotRow) {
      continue;
    }
    const TableauRow &row = m_table[r];
    const Real       &ar  = row.m_a[pivotColumn];
    if(ar > EPS) {
      const Real ratio = row.getRightSide() / ar;
      if(ratio == minRatio) {
	    traceString += format(_T("%s%d"),delimiter,r);
        delimiter = _T(",");
      }
    }
  }
  if(traceString.length() > 0) {
    trace(_T("Degenerated rows:[%s]"),traceString.cstr());
  }
}

void Tableau::trace(int flag, _In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
  if(isTracing(flag)) {
    va_list argptr;
    va_start(argptr,format);
    vtrace(format,argptr);
    va_end(argptr);
  }
}

void Tableau::trace(_In_z_ _Printf_format_string_ TCHAR const * const format,...) const {
  va_list argptr;
  va_start(argptr,format);
  vtrace(format,argptr);
  va_end(argptr);
}

void Tableau::vtrace(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) const {
  if(m_tracer != nullptr) {
    m_tracer->handleData(SimplexTraceElement(*this,vformat(format, argptr)));
  }
}

SimplexTracer *Tableau::setTracer(SimplexTracer *tracer, int traceFlags) {
  SimplexTracer *old = m_tracer;
  m_tracer     = tracer;
  m_traceFlags = traceFlags;
  return old;
}

String Tableau::toString(int fieldSize, int decimals) const {
  const String  matrixFormatString = format(_T("%%%d.%dlg "), fieldSize, decimals);
  const TCHAR  *matrixFormat       = matrixFormatString.cstr();
  const int     width              = getWidth();
  const int     constraintCount    = getConstraintCount();

#define NEWLINE _T('\n')

  const String separatorLine = format(_T("%s\n"), spaceString(12 + (width+1) * (fieldSize+1) + 3,_T('_')).cstr());

  String result;
  result += format(_T("%-23sB %-*s"), format(_T("size:%dx%d"),getConstraintCount(),getWidth()).cstr(),fieldSize-8,EMPTYSTRING);

  for(int col = 1; col <= width; col++) {
    result += format(_T("%*s "), fieldSize, getVariableName(col).cstr());
  }
  result += NEWLINE;
  result += separatorLine;

  result += format(_T("%-*s"), 17+fieldSize, _T("Orig. cost: "));
  for(int col = 1; col <= getXCount(); col++) {
    result += format(matrixFormat, (double)m_costFactor[col]);
  }
  result += NEWLINE;
  result += _T("ObjectValue:");

  result += format(matrixFormat, (double)getObjectValue());
  result += _T("    "); // filler instead of relation
  for(int col = 1; col <= width; col++) {
    result += format(matrixFormat, (double)getObjectFactor(col));
  }
  result += NEWLINE;
  result += separatorLine;

  const bool printOriginalRelation = (m_slackCount == 0);
  for(int r = 1; r <= constraintCount; r++ ) {
    const TableauRow &row = m_table[r];
    result += format(_T("%3d %-6s ="),r, getVariableName(row.m_basisVariable).cstr());
    result += format(matrixFormat, (double)getRightSide(r));
    result += format(_T(" %-2s "), printOriginalRelation ? getRelationString(reverseRelation(row.m_relation)) : _T("="));
    for(int col = 1; col <= width; col++) {
      result += format(matrixFormat, (double)row.m_a[col]);
    }
    result += NEWLINE;
  }

  return result;
}

TCHAR Tableau::getVariablePrefix(size_t index) const {
  if(index < 1) {
    return _T('?');
  } else if(index <= (int)m_xCount) {
    return _T('X');
  } else if((m_slackCount      > 0) && ((int)index <= getSlackColumn(m_slackCount))) {
    return _T('S');
  } else if((m_artificialCount > 0) && ((int)index <= getArtificialColumn(m_artificialCount))) {
    return _T('A');
  } else {
    return _T('#');
  }
}

static String getVariableName(TCHAR prefix, size_t index) {
  return format(_T("%c%-3s"), prefix, FSZ(index));
}

String Tableau::getVariableName(size_t index) const {
  return ::getVariableName(getVariablePrefix(index),index);
}

SimplexSolution Tableau::getSolution() const {
  return SimplexSolution(getBasisVariables(),getObjectValue());
}

CompactArray<BasisVariable> Tableau::getBasisVariables() const {
  CompactArray<BasisVariable> result;
  for(int r = 1; r <= getConstraintCount(); r++) {
    const int index = m_table[r].m_basisVariable;
    result.add(BasisVariable(getVariablePrefix(index), index, getRightSide(r), (index <= getXCount()) ? getCostFactor(index) : 0));
  }
  return result;
}

void Tableau::checkInvariant() const {
  const CompactArray<BasisVariable> bv = getBasisVariables();
  const int constraintCount = getConstraintCount();
  for(size_t row = 0; row < bv.size(); row++) {
    const BasisVariable &v       = bv[row];
    const int            col     = v.m_index;
    const String         varName = v.getName();
    if(getObjectFactor(col) != 0) {
      throwException(_T("%s is basic but objectFactor != 0 (=%le)"),varName.cstr(), (double)getObjectFactor(col));
    }
    int count = 0;
    for(int i = 1; i <= constraintCount; i++) {
      const Real &a = m_table[i].m_a[col];
      if(a != 0) {
        count++;
        if(a != 1) {
          throwException(_T("Coefficient [%d][%d] != 1 (=%le) for basisVar %s"),i,col,a,varName.cstr());
        }
      }
    }
    if(count != 1) {
      throwException(_T("BasisVar %s has more than 1 coefficient != 0 in column %d"), varName.cstr(), col);
    }
  }
}

CompactArray<Real> Tableau::createRealArray(size_t size) { // static
  CompactArray<Real> result(size);
  for(size_t i = 0; i < size; i++) {
    result.add(0);
  }
  return result;
}

Real Tableau::getSlackFactor(SimplexRelation relation) { // static
  switch(relation) {
  case GREATERTHAN: return -1;
  case LESSTHAN   : return  1;
  case EQUALS     : return  0;
  default         : throwException(_T("Simplex:Invalid relation:'%c'. Must be '<','>' or '='"),relation);
                    return 0;
  }
}

const TCHAR *Tableau::getRelationString(SimplexRelation relation) { // static
  switch(relation) {
  case GREATERTHAN: return _T(">=");
  case LESSTHAN   : return _T("<=");
  case EQUALS     : return _T("=");
  default         : throwException(_T("getRelationString:Illegal argument:%c. Must be '<','>' or '='"),relation);
  }
  return _T("?");
}

SimplexRelation Tableau::reverseRelation(SimplexRelation relation) { // static
  switch(relation) {
  case GREATERTHAN: return LESSTHAN;
  case LESSTHAN   : return GREATERTHAN;
  case EQUALS     : return EQUALS;
  default         : throwException(_T("reverseRelation:Illegal argument:%c. Must be '<','>' or '='"),relation);
  }
  return EQUALS;
}

TableauRow::TableauRow(UINT size) {
  m_a = Tableau::createRealArray(size);
  m_basisVariable = 0;
  m_relation = EQUALS;
}

TableauConstraint::TableauConstraint(const CompactArray<Real> &leftSide, SimplexRelation relation, const Real &rightSide) {
  m_leftSide  = leftSide;
  m_relation  = relation;
  m_rightSide = rightSide;
}

TableauConstraint::TableauConstraint(const String &str) {
  String copy(str);
  bool gotRelation  = false;
  bool gotRightSide = false;
  for(Tokenizer tok(copy.cstr(),_T(" ,\t\n\r")); tok.hasNext();) {
    String s = tok.next();
    double x;
    if(_stscanf(s.cstr(), _T("%le"),&x) != 1) {
      if(gotRelation) {
        throwException(_T("TableauConstraint:Illegal symbol <%s> in argument <%s>"),s.cstr(),str.cstr());
      } else {
        if(_tcschr(_T("=<>"), s[0]) == nullptr) {
          throwException(_T("TableauConstraint:Relation for constraint <%s> must be '<','>' or '=' (read value:'%c')"),str.cstr(), s[0]);
        }
        m_relation = (SimplexRelation)s[0];
        gotRelation = true;
      }
    } else if(!gotRelation) {
      m_leftSide.add(x);
    } else if(gotRightSide) {
      throwException(_T("TableauConstraint:Only one value accepted after relation '%c'"),m_relation);
    } else {
      m_rightSide  = x;
      gotRightSide = true;
    }
  }
  if(m_leftSide.size() == 0) {
    throwException(_T("TableauConstraint:At least one coefficient expected"));
  }
  if(!gotRelation) {
    throwException(_T("TableauConstraint:Relation '<','>' or '=' must be specified"));
  }
  if(!gotRightSide) {
    throwException(_T("TableauConstraint:Value after relation '%c' must be specified"),m_relation);
  }
}

TableauCostFactors::TableauCostFactors(const String &str) {
  String copy(str);
  for(Tokenizer tok(copy.cstr(),_T(" ,\t\n\r")); tok.hasNext();) {
    String s = tok.next();
    double x;
    if(_stscanf(s.cstr(),_T("%le"),&x) != 1) {
      throwException(_T("TableauCostFactors:Illegal symbol <%s>. Expected number."),s.cstr());
    }
    add(x);
  }
  if(size() == 0) {
    throwException(_T("TableauConstraint:At least one coefficient expected"));
  }
}

static int dictionaryCmp(const DictionaryKey &key1, const DictionaryKey &key2) {
  return key1 == key2 ? 0 : 1;
}

static unsigned long dictionaryHash(const DictionaryKey &key) {
  return key.hashCode();
}

DictionaryKeySet::DictionaryKeySet() : HashSet<DictionaryKey>(dictionaryHash,dictionaryCmp,100) {
}

DictionaryKey::DictionaryKey(const Tableau *tableau) : BitSet(tableau->getWidth()+1) {
  const Array<TableauRow> &t = tableau->m_table;
  const int n = (int)t.size();
  for(int i = 0; i < n; i++) {
    add(t[i].m_basisVariable);
  }
}

String BasisVariable::getName() const {
  return ::getVariableName(m_prefix,m_index);
}

String BasisVariable::toString() const {
  return format(_T("%s = %-16.10lg Cost = %-.10lg"), getName().cstr(), (double)m_value, (double)m_costFactor);
}

static int basisVarCmp(const BasisVariable &v1, const BasisVariable &v2) {
  return v1.m_index - v2.m_index;
}

SimplexSolution::SimplexSolution(const CompactArray<BasisVariable> variables, const Real &totalCost)
: m_variables(variables)
, m_totalCost(totalCost)
{
  m_variables.sort(basisVarCmp); // sort by index
}

SimplexSolution::SimplexSolution() : m_totalCost(0) {
}

String SimplexSolution::toString() const {
  String result;
  Real sum = 0;
  for(size_t i = 0; i < m_variables.size(); i++) {
    const BasisVariable &v = m_variables[i];
    result += format(_T("%s\n"),v.toString().cstr());
    sum += v.m_value * v.m_costFactor;
  }
  result += format(_T("Minimum:%-15.10lg TestSum:%-.10lg Difference:%.10lg\n"), (double)m_totalCost, (double)sum, (double)(m_totalCost - sum));
  return result;
}
