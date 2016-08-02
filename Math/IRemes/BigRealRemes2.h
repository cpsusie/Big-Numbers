#pragma once

#include <HashMap.h>
#include <NumberInterval.h>
#include <Math/Rectangle2D.h>
#include <Math/MathLib.h>
#include <Math/BigRealMatrix.h>

class ExtremaKey {
private:
  int m_M,m_K;
public:
  ExtremaKey(const int M, const int K) : m_M(M), m_K(K) {
  }

  ExtremaKey() : m_M(0), m_K(0) {
  }

  bool operator==(const ExtremaKey &k) const {
    return m_M == k.m_M && m_K == k.m_K;
  }

  bool operator!=(const ExtremaKey &k) const {
    return !operator==(k);
  }

  unsigned long hashCode() const {
    return m_M * 31 + m_K;
  }

  friend tostream &operator<<(tostream &out, const ExtremaKey &key);
  friend tistream &operator>>(tistream &in ,       ExtremaKey &key);
};

class ExtremaVector : public BigRealVector {
private:
  BigReal m_E;
  BigReal m_MMQuot;
public:
  ExtremaVector() {
  }
  ExtremaVector(const BigRealVector &extrema, const BigReal &E, const BigReal &MMQuot) 
    : BigRealVector(extrema), m_E(E), m_MMQuot(MMQuot) {
  }
  const BigReal &getMMQuot() const {
    return m_MMQuot;
  }
  const BigReal &getE() const {
    return m_E;
  }

  friend tostream &operator<<(tostream &out, const ExtremaVector &v);
  friend tistream &operator>>(tistream &in ,       ExtremaVector &v);
};

class ExtremaHashMap : public HashMap<ExtremaKey, Array<ExtremaVector> > {
public:
  ExtremaHashMap();
  void save(const String &name);
  void load(const String &name);
  void save(tostream &out);
  void load(tistream &in);
};

class RemesTargetFunction : public FunctionTemplate<BigReal,BigReal> {
public:
  virtual String getName() const = 0;
  virtual const NumberInterval<BigReal> &getInterval() const = 0;
  virtual int getDigits() const = 0;
};

class Remes;

class RemesHandler : public AlgorithmHandler<Remes> {
public:
  void handleData(const Remes &data) = 0;
};

typedef enum {
  REMES_INITIALIZE
 ,REMES_SEARCH_E
 ,REMES_SEARCH_EXTREMA
 ,REMES_FINALIZE_ITERATION
 ,REMES_NOCONVERGENCE
} RemesState;

class Remes {
private:
  const BigReal                m_left, m_right;
  RemesTargetFunction         &m_targetFunction;   // Function to be approximated
  const bool                   m_useRelativeError;
  const unsigned int           m_digits;
  int                          m_M, m_K, m_N;      // m_N = m_M + m_K
  BigRealVector                m_coefficient;      // Coefficient[0..N+1] = { a[0]..a[M], b[1]..b[K], E }. b[0] = 1. Dim=N+2
  bool                         m_hasCoefficients;  // set to true the first time m_coefficient vector is calculated
  int                          m_coefVectorIndex;
  BigReal                      m_E, m_nextE;
  BigReal                      m_Q, m_QEpsilon;
  int                          m_mainIteration   , m_searchEIteration;
  int                          m_minExtremumIndex, m_maxExtremumIndex, m_extremaCount;
  BigRealVector                m_extrema;          // Extremum[0..N+1].                                              Dim=N+2
  BigRealVector                m_functionValue;    // Values targetFunction(x) for x = m_extr[0..N+1].               Dim=N+2
  BigRealVector                m_errorValue;       // Values of errorFunction(x) for x = m_extr[0..N+1].             Dim=N+2
  BigReal                      m_maxError;
  BigReal                      m_MMQuot, m_lastMMQuot, m_MMQuotEps;     // Default value is 1e-22. Stop criterium. Iteration stops when 1-|minExtr/maxExtr| < m_mmQuotEps
  bool                         m_solveStateInterpolationDone;
  bool                         m_solveStateHighPrecision;
  bool                         m_solveStateDecrM;
  RemesState                   m_state;
  String                       m_warning;
  RemesHandler                *m_handler;

  static ExtremaHashMap extremaMap;

  String               getMapFileName() const;
  void                 initSolveState(const int M, const int K);
  void                 nextSolveState();
  bool                 hasNextSolveState() const;
  void                 saveExtremaToMap(const BigReal &E, const BigReal &mmQuot);
  BigRealVector        getDefaultInitialExtrema(            const int M, const int K);
  BigRealVector        findInitialExtremaByInterpolation(   const int M, const int K);
  bool                 hasFastInterpolationOfExtrema(       const int M, const int K);
  BigRealVector        getFastInitialExtremaByInterpolation(const int M, const int K);
  bool                 hasSavedExtrema(                     const int M, const int K);
  const ExtremaVector &getBestSavedExtrema(                 const int M, const int K);
  BigRealVector        getInterpolatedExtrema(const BigRealVector &defaultExtrema, const BigRealVector &defaultSubExtrema, const BigRealVector &finalSubExtrema) const;

  void                 initCoefficients();
  void                 setExtrema(      const BigRealVector &extrema);
  void                 setExtrema(      const ExtremaVector &extrema);
  void                 findCoefficients();
  void                 findExtrema();
  BigRealVector        findFinalExtrema(const int M, const int K, const bool highPrecision);
  BigReal              findExtremum(    const BigReal &l, const BigReal &m, const BigReal &r, int depth);
  void                 setExtremum(     const int index, const BigReal &x);
  void                 setMMQuotEpsilon(const BigReal &MMQuotEps);  // set stop criterium. 
  BigReal              approximation(   const BigReal &x) const; // Pm(x) / Pk(x)
  BigReal              errorFunction(   const BigReal &x) const; // m_useRelativeError ? (1 - sFunction(x) * approximation(x)) : (m_targetFunction(x)-approximation(x))
  BigReal              sFunction(       const BigReal &x) const; // m_useRelativeError ? (1/m_targetFunction(x)) : 1
  BigReal              targetFunction(  const BigReal &x) const; // m_useRelativeError ?  1 : m_targetFunction(x)
  String               getHeaderString() const;
public:
  Remes(RemesTargetFunction &targetFunction, const bool useRelativeError);
  Remes(const Remes &src);

  void solve(const int M, const int K);

  inline void setHandler(RemesHandler *handler) {
    m_handler = handler;
  }
  void loadExtremaFromFile();
  void saveExtremaToFile();
  inline RemesTargetFunction &getTargetFunction() {
    return m_targetFunction;
  }
  inline int  getMainIteration() const {
    return m_mainIteration;
  }
  inline int  getSearchEIteration() const {
    return m_searchEIteration;
  }
  inline int  getExtremaCount() const {
    return m_extremaCount;
  }
  inline const BigReal &getMaxError() const {
    return m_maxError;
  }
  inline int   getCoefVectorIndex() const {
    return m_coefVectorIndex;
  }
  inline bool  hasErrorPlot() const {
    return m_hasCoefficients;
  }
  void        getErrorPlot(int n, Point2DArray &pa) const;

  String      getCFunctionString(bool useDouble80) const;
  String      getJavaFunctionString() const;
  String      getExtremumString(int index) const;
  StringArray getCoefficientStringArray() const;
  String      getSearchEString() const;

  inline const String &getWarning() const {
    return m_warning;
  }

  inline RemesState getState() const {
    return m_state;
  }
};
