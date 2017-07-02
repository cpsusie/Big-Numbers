#include "stdafx.h"
#include <MyUtil.h>
#include <BitSet.h>
#include <Tokenizer.h>
#include <Stack.h>
#include <HashSet.h>
#include <Math/Simplex.h>

class Tracer : public SimplexTracer {
public:
  void handleData(const SimplexTraceElement &data);
};

void Tracer::handleData(const SimplexTraceElement &data) {
  _tprintf(_T("%s"), data.m_msg.cstr());
  _tprintf(_T("\n"));
//  data.m_Tableau.print();
//  pause();
}

class IntegerConstraint {
public:
  short           m_index;
  SimplexRelation m_relation;
  int             m_value;
  IntegerConstraint(short index, SimplexRelation relation, int value);
  String toString() const;
};

IntegerConstraint::IntegerConstraint(short index, SimplexRelation relation, int value) {
  m_index    = index;
  m_relation = relation;
  m_value    = value;
}

String IntegerConstraint::toString() const {
  return format(_T("X[%2d] %s %-5d"), m_index, Tableau::getRelationString(m_relation), m_value);
}

static ULONG integerConstraintHashFunction(const IntegerConstraint &c) {
  return c.m_index * 701 + c.m_relation * 103 + c.m_value * 307;
}

static int integerConstraintCompare(const IntegerConstraint &c1, const IntegerConstraint &c2) {
  int c = c1.m_index - c2.m_index;
  if(c) return c;
  c = c1.m_relation - c2.m_relation;
  if(c) return c;
  return c1.m_value - c2.m_value;
}

static bool operator==(const IntegerConstraint &c1, const IntegerConstraint &c2) {
  return integerConstraintCompare(c1,c2) == 0;
}

class IntegerConstraintArray : public Array<IntegerConstraint> {
public:
  void sort() {
    Array<IntegerConstraint>::sort(integerConstraintCompare);
  }
  String toString() const;
};

static ULONG integerConstraintArrayHashFunction(const IntegerConstraintArray &a) {
  int v = 0;
  for(size_t i = 0; i < a.size(); i++) {
    v = v * 283 + integerConstraintHashFunction(a[i]);
  }
  return v;
}

static int integerConstraintArrayCompare(const IntegerConstraintArray &a1, const IntegerConstraintArray &a2) {
  return (a1 == a2) ? 0 : 1;
}

String IntegerConstraintArray::toString() const {
  String result;
  for(size_t i = 0; i < size(); i++) {
    if(i > 0) result += _T(' ');
    result += (*this)[i].toString();
  }
  return result;
}

class Simplex {
private:
  Tableau                        *m_tableau;
  Tracer                          m_tracer;
  int                             m_lineCount;
  int                             m_xCount;
  BitSet                         *m_integerVariableSet;
  BitSet                         *m_staticIntegerVariableSet; // only used in toString
  CompactIntArray                 m_unboundedVariableSet;
  bool                            m_hasUnboundedVariables;
  SimplexSolution                 m_bestSolution;
  bool                            m_gotSolution;
  int                             m_backtrackCounter;
  HashSet<IntegerConstraintArray> m_integerConstraintsDone;
  Stack<IntegerConstraint>        m_integerConstraintStack;

  void backtrack(const Tableau &tableau0);
  void setSolution(const SimplexSolution &solution);
  IntegerConstraintArray getCurrentActiveIntegerConstraints() const;
  bool readLine(FILE *f, String &str);
  CompactIntArray getIntArray(Tokenizer &t);
  CompactIntArray &removeDuplicates(CompactIntArray &a);
  const BasisVariable *findBasisVariable(const CompactArray<BasisVariable> &bvArray, int index) const;
public:
  Simplex(FILE *f, int traceFlags);
  ~Simplex();

  SimplexResult solve();

  Tableau &getTableau() {
    return *m_tableau;
  }

  SimplexSolution getSolution() const;

  String toString() const;
};

Simplex::Simplex(FILE *f, int traceFlags) : m_integerConstraintsDone(integerConstraintArrayHashFunction,integerConstraintArrayCompare) {
  m_tableau                  = NULL;
  m_integerVariableSet       = NULL;
  m_staticIntegerVariableSet = NULL;
  m_gotSolution              = false;
  m_hasUnboundedVariables    = false;
  m_lineCount                = 0;

  int constraintCount;

  String line;
  if(!readLine(f,line) || _stscanf(line.cstr(), _T("%d %d"), &m_xCount, &constraintCount) != 2) {
    throwException(_T("Invalid input. Expected number of variables and number of conditions"));
  }

  Array<TableauConstraint> tableauConstraints;
  for(int i = 1; i <= constraintCount; i++) {
    if(!readLine(f,line)) {
      throwException(_T("Unexpected end of input. Specified %d constraint. Can only read %d"), constraintCount, tableauConstraints.size());
    }
    TableauConstraint constraint(line);
    if(constraint.getXCount() != m_xCount) {
      throwException(_T("Wrong number of coefficients (=%d) specified in line %d. Number of variables=%d"), constraint.getXCount(), m_lineCount, m_xCount);
    }
    tableauConstraints.add(constraint);
  }

  if(!readLine(f,line)) {
    throwException(_T("Unexpected end of input. Expected cost factors"));
  }
  TableauCostFactors costFactors(line);
  if(costFactors.size() != m_xCount) {
    throwException(_T("Wrong number of cost factors (=%d) specified in line %d. Number of variables=%d"), costFactors.size(), m_lineCount, m_xCount);
  }

  m_integerVariableSet = new BitSet(m_xCount+1);
  for(int i = 0; i <= m_xCount; i++) {
    m_unboundedVariableSet.add(-1);
  }

  while(readLine(f,line)) {
    Tokenizer tok(line, _T(" :,"));
    if(!tok.hasNext()) {
      throwException(_T("Unexpected input in line %d:<%s>"), m_lineCount, line.cstr());
    }
    String label = tok.next();
    if(label.equalsIgnoreCase(_T("integers"))) {
      CompactIntArray a = getIntArray(tok);
      for(size_t i = 0; i < a.size(); i++) {
        const int index = a[i];
        if(index < 1 || index > m_xCount) {
          throwException(_T("Index of integer-variable=%d out of range in line %d. Legal interval=[1..%d]"), index, m_lineCount, m_xCount);
        }
        m_integerVariableSet->add(index);
      }
    } else if(label.equalsIgnoreCase(_T("unbounded"))) {
      CompactIntArray a = removeDuplicates(getIntArray(tok));
      for(size_t i = 0; i < a.size(); i++) {
        const int index0 = a[i];
        if(index0 < 1 || index0 > m_xCount) {
          throwException(_T("Index of unbounded-variable=%d out of range in line %d. Legal interval=[1..%d]"), index0, m_lineCount, m_xCount);
        }
        m_unboundedVariableSet[index0] = (int)m_unboundedVariableSet.size();
        m_unboundedVariableSet.add(index0);
        for(size_t c = 0; c < tableauConstraints.size(); c++) {
          TableauConstraint &con = tableauConstraints[c];
          con.addLeftSideCoefficient(-con.getLeftSideCoefficient(index0));
        }
        costFactors.add(-costFactors[index0]);
        m_hasUnboundedVariables = true;
      }
    } else {
      throwException(_T("Unexpected input in line %d:<%s>"), m_lineCount, line.cstr());
    }
  }

  int xCount = costFactors.size();

  m_tableau                  = new Tableau(xCount, constraintCount, traceFlags ? &m_tracer : NULL, traceFlags);
  m_staticIntegerVariableSet = new BitSet(*m_integerVariableSet);

  for(size_t i = 0; i < tableauConstraints.size(); i++) {
    m_tableau->setConstraint(i+1,tableauConstraints[i]);
  }
  m_tableau->setCostFactors(costFactors);
  m_tableau->checkTableau();
}

Simplex::~Simplex() {
  delete m_tableau;
  delete m_integerVariableSet;
  delete m_staticIntegerVariableSet;
}

bool Simplex::readLine(FILE *f, String &str) { // read a line skipping empty lines.
  for(;;) {
    if(!::readLine(f,str)) {
      return false;
    }
    m_lineCount++;
    if(str.trim().length() != 0) {
      break;
    }
  }
  return true;
}

CompactIntArray Simplex::getIntArray(Tokenizer &tok) {
  CompactIntArray result;
  while(tok.hasNext()) {
    result.add(tok.getInt());
  }
  return result;
}

// removes duplicate elements.
CompactIntArray &Simplex::removeDuplicates(CompactIntArray &a) {
  a.sort(intHashCmp);
  if(a.size() > 1) {
    int last = a[0];
    for(size_t i = 1; i < a.size();) {
      if(a[i] != last) {
        last = a[i];
        i++;
      } else {
        a.remove(i);
      }
    }
  }
  return a;
}


void Simplex::setSolution(const SimplexSolution &solution) {
  m_bestSolution = solution;
  m_gotSolution  = true;
}

SimplexSolution Simplex::getSolution() const {
  if(!m_gotSolution) {
    throwException(_T("Cannot get solution, because no solution is found"));
  }
  if(!m_hasUnboundedVariables) {
    return m_bestSolution;
  }
/*
  printf("Tableau-solution:\n%s",m_bestSolution.toString().cstr());
  printf("Unbounded variables:[");
  for(int k = 0; k < m_unboundedVariableSet.size(); k++) {
    int index = m_unboundedVariableSet[k];
    if(index >= 0) {
      printf("%d ", index);
    }
  }
  printf("]\n");
*/
  const CompactArray<BasisVariable> &bvArray = m_bestSolution.getVariables();
  CompactArray<BasisVariable> result;
  for(size_t i = 0; i < bvArray.size(); i++) {
    const BasisVariable &bv = bvArray[i];
    if(bv.m_index >= (int)m_unboundedVariableSet.size() || m_unboundedVariableSet[bv.m_index] == -1) {
      result.add(bv);
    } else {
      const int partnerIndex = m_unboundedVariableSet[bv.m_index]; // >= 0
      const BasisVariable *partner = findBasisVariable(bvArray,partnerIndex);
      if(partner == NULL) {
        if(bv.m_index <= m_xCount) {
          result.add(bv);
        } else {
          result.add(BasisVariable(bv.m_prefix,partnerIndex,-bv.m_value,-bv.m_costFactor));
        }
      } else { // partner != NULL
        if(bv.m_index <= m_xCount) {
          result.add(BasisVariable(bv.m_prefix,bv.m_index,bv.m_value - partner->m_value,bv.m_costFactor));
        } else {
          // Do nothing. It has already been added
        }
      }
    }
  }
  return SimplexSolution(result,m_bestSolution.getTotalCost());
}

const BasisVariable *Simplex::findBasisVariable(const CompactArray<BasisVariable> &bvArray, int index) const {
  for(size_t j = 0; j < bvArray.size(); j++) {
    const BasisVariable &bv = bvArray[j];
    if(bv.m_index == index) {
      return &bv;
    }
  }
  return NULL;
}

SimplexResult Simplex::solve() {
  if(m_tableau->isTracing(TRACE_TABLEAU)) {
    m_tableau->trace(_T("%s"),toString().cstr());
  }
  SimplexResult r = m_tableau->twoPhaseSimplex();
  if(r != SIMPLEX_SOLUTION_OK) {
    return r;
  }
  if(m_integerVariableSet->isEmpty()) {
    setSolution(m_tableau->getSolution());
  } else {
    if(m_tableau->isTracing(TRACE_MAINSTEP)) {
      m_tableau->trace(_T("Found optimal feasible noninteger solution"));
    }
    if(m_tableau->isTracing(TRACE_TABLEAU)) {
      m_tableau->trace(_T("%s"), m_tableau->toString().cstr());
    }
    if(m_tableau->isTracing(TRACE_SOLUTIONS)) {
      m_tableau->trace(_T("%s"), m_tableau->getSolution().toString().cstr());
    }
    if(m_tableau->isTracing(TRACE_MAINSTEP)) {
      m_tableau->trace(_T("Now backtracking for integer solutions"));
    }

   m_backtrackCounter = 0;
   backtrack(*m_tableau);
  }
  return m_gotSolution ? SIMPLEX_SOLUTION_OK : SIMPLEX_NO_SOLUTION;
}

static bool isInteger(const Real &x) {
  return fabs(x - round(x)) < 1e-13;
}

void Simplex::backtrack(const Tableau &tableau0) {
  const int XCount = tableau0.getXCount();
  const CompactArray<BasisVariable> basisVariables = tableau0.getSolution().getVariables();
  for(size_t i = 0; i < basisVariables.size(); i++) {
    const BasisVariable &v = basisVariables[i];
    if(m_integerVariableSet->contains(v.m_index) && !isInteger(v.m_value)) {

      m_integerVariableSet->remove(v.m_index);

      for(int t = 0; t < 1; t++) {
        Tableau tableau(tableau0);
        SimplexRelation relation  = t ? LESSTHAN : GREATERTHAN;
        int rightSide = t ? (int)floor(v.m_value) : (int)ceil(v.m_value);

        m_integerConstraintStack.push(IntegerConstraint(v.m_index, relation, rightSide));

        tableau.addConstraint(v.m_index, relation, rightSide);

        if(!m_integerVariableSet->isEmpty()) {
          SimplexResult r = tableau.dualSimplex();
          if(r == SIMPLEX_SOLUTION_OK) {
            backtrack(tableau);
          }
        } else {
          IntegerConstraintArray currentActiveConstraints = getCurrentActiveIntegerConstraints();
          if(m_integerConstraintsDone.contains(currentActiveConstraints)) {
            if(tableau.isTracing(TRACE_ITERATIONS)) {
              tableau.trace(_T("Integer constraints %s already done"), currentActiveConstraints.toString().cstr());
            }
          } else {
            m_integerConstraintsDone.add(currentActiveConstraints);
            SimplexResult r = tableau.dualSimplex();

            if(r == SIMPLEX_SOLUTION_OK) {
              TCHAR *prefix = _T("integer solution");
              if(!m_gotSolution || tableau.getObjectValue() < m_bestSolution.getTotalCost()) {
                setSolution(tableau.getSolution());
                prefix = _T("best integer solution");
              }
              if(tableau.isTracing(TRACE_ITERATIONS)) {
                tableau.trace(_T("Found %s %d."), prefix, ++m_backtrackCounter);
              }
              if(tableau.isTracing(TRACE_TABLEAU)) {
                tableau.trace(_T("%s"), tableau.toString().cstr());
              }
              if(tableau.isTracing(TRACE_SOLUTIONS)) {
                tableau.trace(_T("Integer constraints:%s\n%s"), currentActiveConstraints.toString().cstr(),tableau.getSolution().toString().cstr());
              }
            }
          }
        }
        m_integerConstraintStack.pop();
      }

      m_integerVariableSet->add(v.m_index);
    }
  }
}

IntegerConstraintArray Simplex::getCurrentActiveIntegerConstraints() const {
  IntegerConstraintArray result;
  const int h = m_integerConstraintStack.getHeight();
  for(int i = 0; i < h; i++) {
    result.add(m_integerConstraintStack.top(i));
  }
  result.sort();
  return result;
}

String Simplex::toString() const {
  String result = m_tableau->toString();
  if(!m_staticIntegerVariableSet->isEmpty()) {
    result += _T("Integer variables:");
    TCHAR *delimiter = NULL;
    for(Iterator<size_t> it = m_staticIntegerVariableSet->getIterator(); it.hasNext();) {
      if(delimiter == NULL) {
        delimiter = _T(",");
      } else {
        result += delimiter;
      }
      result += m_tableau->getVariableName(it.next());
    }
    result += _T('\n');
  }
  if(m_hasUnboundedVariables) {
    result += _T("Unbounded variables:");
    TCHAR *delimiter = NULL;
    for(int i = 1; i <= m_xCount; i++) {
      if(m_unboundedVariableSet[i] == -1) {
        continue;
      }
      if(delimiter == NULL) {
        delimiter = _T(",");
      } else {
        result += delimiter;
      }
      result += m_tableau->getVariableName(i);
    }
    result += _T('\n');
  }
  return result;
}

static void usage( void ) {
  _ftprintf(stderr, _T("Usage:simplex [-v[level1,level2,...]] file\n"
                       "    level = 0 : trace warnings (Default if -v is specified)\n"
                       "    level = 1 : trace mainsteps\n"
                       "    level = 2 : trace iterations\n"
                       "    level = 3 : trace solutions\n"
                       "    level = 4 : trace pivoting\n"
                       "    level = 5 : trace tabelaus\n"
                       "    level = 6 : trace all\n")
           );
  exit(-1);
}

static const int traceLevels[] = {
  TRACE_WARNINGS
 ,TRACE_MAINSTEP
 ,TRACE_ITERATIONS
 ,TRACE_SOLUTIONS
 ,TRACE_PIVOTING
 ,TRACE_TABLEAU
 ,TRACE_WARNINGS | TRACE_MAINSTEP | TRACE_ITERATIONS | TRACE_SOLUTIONS | TRACE_PIVOTING | TRACE_TABLEAU
};

int main( int argc, char **argv ) {
  char *cp;
  int traceFlags = 0;
  for(argv++; *argv && *(cp = *argv ) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'v':
        { Tokenizer tok(cp+1, _T(","));
          while(tok.hasNext()) {
            String s = tok.next();
            int level;
            if(_stscanf(s.cstr(),_T("%u"),&level) != 1 || level >= ARRAYSIZE(traceLevels)) {
              continue;
            }
            traceFlags |= traceLevels[level];
          }
          if(traceFlags == 0) {
            traceFlags = traceLevels[0];
          }
        }
        break;
      default :
        usage();
        break;
      }
      break;
    }
  }

  try {
    if(!*argv) {
      usage();
    }
    FILE *f = FOPEN(*argv,_T("r"));

    Simplex simplex(f,traceFlags);
    fclose(f);

    switch(simplex.solve()) {
    case SIMPLEX_SOLUTION_OK:
      _tprintf(_T("\nSolution:\n"));
      if(traceFlags & TRACE_TABLEAU) {
        _tprintf(_T("%s"),simplex.toString().cstr());
      }
      _tprintf(_T("%s"),simplex.getSolution().toString().cstr());
      break;

    case SIMPLEX_NO_SOLUTION:
      if(traceFlags & TRACE_TABLEAU) {
        _tprintf(_T("%s"),simplex.toString().cstr());
      }
      _tprintf(_T("No solution exists\n"));
      break;

    case SIMPLEX_NO_SOLUTION_FOUND:
      if(traceFlags & TRACE_TABLEAU) {
        _tprintf(_T("%s"), simplex.toString().cstr());
      }
      _tprintf(_T("No solution found\n"));
      break;

    case SIMPLEX_SOLUTION_UNLIMITED:
      if(traceFlags & TRACE_TABLEAU) {
        _tprintf(_T("%s"), simplex.toString().cstr());
      }
      _tprintf(_T("Unlimited solution\n"));
      break;
    }
  } catch(Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;
}
