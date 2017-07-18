#pragma once

#include <SynchronizedQueue.h>
#include <CompactStack.h>
#include <TinyBitSet.h>
#include <PropertyContainer.h>
#include "NumberTransformation.h"
#include <Math/Double80.h>

//#define SAVE_CALCULATORINFO

class OrbitPoint : public CPoint {
public:
  inline OrbitPoint() {
  }
  inline OrbitPoint(const CPoint &p, D3DCOLOR oldColor) : CPoint(p) {
    m_oldColor = oldColor;
  }
  D3DCOLOR m_oldColor;
};

class PointSet : public BitSet {
private:
  CRect m_rect;
  int   m_width;
  static inline UINT getPixelCount(const CRect &r) {
    return r.Width() * r.Height();
  }
  inline UINT getIndex(const CPoint &p) const {
    return (p.y-m_rect.top) * m_width + (p.x-m_rect.left);
  }
  inline CPoint getPoint(size_t index) const {
    return CPoint((int)(index % m_width + m_rect.left), (int)(index / m_width + m_rect.top));
  }
public:
  PointSet(const CRect &r)
    : BitSet(getPixelCount(r))
    , m_rect(r)
    , m_width(r.Width())
  {
  }
  inline void add(const CPoint &p) {            // assume p is inside CRect
    BitSet::add(getIndex(p));
  }
  inline bool contains(const CPoint &p) const { // assume p is inside CRect
    return BitSet::contains(getIndex(p));
  }
  inline const CRect &getRect() const {
    return m_rect;
  }
  inline CPoint next(Iterator<size_t> &it) const {
    return getPoint(it.next());
  }
};

#ifdef SAVE_CALCULATORINFO

class CalculatorInfo {
private:
  int                  m_calculatorId;
  PointSet             m_blackSet, m_edgeSet, m_innerSet;
public:
  CalculatorInfo(int calculatorId, const CRect &r)
    : m_calculatorId(calculatorId)
    , m_blackSet(r)
    , m_edgeSet( r)
    , m_innerSet(r)
  {
  }
  inline void addBlack(const CPoint &p) {
    m_blackSet.add(p);
  }
  inline bool contains(const CPoint &p) const {
    return m_blackSet.getRect().PtInRect(p) ? m_blackSet.contains(p) : false;
  }
  void setEdgeAndInnerSet(const PointSet &edgeSet, const PointSet &innerSet) {
    m_edgeSet = edgeSet; m_innerSet = innerSet;
  }
  const PointSet &getEdgeSet() const {
    return m_edgeSet;
  }
  const PointSet &getInnerSet() const {
    return m_innerSet;
  }
  void addEdgeSetToBlackSet() {
    m_blackSet += m_edgeSet;
  }
  String toString() const;
};

#endif

// None of these colors should be used in the generated colormap
#define EMPTY_COLOR        RGB(255,255,255)
#define FILL_COLOR         RGB(  0,  0,  0)

// if an element of the colormap equals FILL_COLOR use this one instead
#define NEXTTO_EMPTY_COLOR RGB(255,255,254)
#define NEXTTO_BLACK       RGB(  0,  0,  1)
#define NEXTTO_FILL_COLOR  RGB(255,  0,  1)

typedef NumberPoint2D<Real>                 RealPoint2D;
typedef NumberInterval<Real>                RealInterval;
typedef NumberRectangle<Real>               RealRectangle;
typedef NumberIntervalTransformation<Real>  RealIntervalTransformation;
typedef LinearNumberTransformation<Real>    RealLinearTransformation;
typedef NumberRectangleTransformation<Real> RealRectangleTransformation;

class MBContainer : public DWordPixelAccessor, public PropertyChangeListener {
public:
  MBContainer(PixRect *pr) : DWordPixelAccessor(pr,0) {
  }
  virtual const RealRectangleTransformation &getTransformation()       const  = 0;
  virtual UINT                               getMaxIteration()         const  = 0;
  virtual const D3DCOLOR                    *getColorMap()             const  = 0;
  virtual FPUPrecisionMode                   getPrecisionMode()        const  = 0;
  virtual PixelAccessor                     *getPixelAccessor()               = 0;
  virtual bool                               calculateWithOrbit()      const  = 0;
  virtual bool                               useEdgeDetection()        const  = 0;
  virtual bool                               getJobToDo(CRect &rect)          = 0;
  virtual void                               paintMark(const CPoint &p)       = 0;
};

typedef enum {
  CALC_SUSPENDED  = 0
 ,CALC_RUNNING    = 1
 ,CALC_TERMINATED = 2
} CalculatorState;

#define CALC_SUSPEND_PENDING  0x01
#define CALC_KILL_PENDING     0x02

typedef BitSet16 CalculatorSet;

class CalculatorPool;

class MBCalculator : public Thread {
private:
  CalculatorPool     &m_pool;
  const int           m_id;
  const ULONG         m_pendingMask;
  bool                m_edgeTracing;
  MBContainer        &m_mbc;
  CRect               m_currentRect;
  OrbitPoint         *m_orbitPoints;
  Semaphore           m_gate;
  mutable Semaphore   m_wakeup;
#ifdef SAVE_CALCULATORINFO
  const char         *m_phase;
  CalculatorInfo     *m_info;
  void addInfoToPool();
#define SETPHASE(str) m_phase = str;
#else
#define SETPHASE(str)
#endif

  UINT (MBCalculator::*m_itCount)(const Real &, const Real &, UINT);

  UINT  findITCountPaintOrbit(const Real &X, const Real &Y, UINT maxIteration);
  UINT  findITCountFast(      const Real &X, const Real &Y, UINT maxIteration);
  void  followBlackEdge(const CPoint &p);
  void  fillInnerArea(PointSet &innerSet);
  void  releaseOrbitPoints();
  PixelAccessor *handlePending();

  inline UINT findItCount(const Real &X, const Real &Y, UINT maxIteration) {
    return (this->*m_itCount)(X, Y, maxIteration);
  }
public:
  MBCalculator(CalculatorPool *pool, int id);
  ~MBCalculator() {
    releaseOrbitPoints();
  }

  UINT run();
  void wakeUp() {
    m_wakeup.signal();
  }
};

typedef enum {
  RUNNINGSET        // CalculatorSet
 ,CALCULATIONACTIVE // bool
} PoolProperty;

class CalculatorPool : private CompactArray<MBCalculator*>, public PropertyContainer {
private:
  static int            s_CPUCount;
  static CalculatorSet  s_maxSet;
  static const TCHAR   *s_stateName[3];
  MBContainer          &m_mbc;
  mutable Semaphore     m_gate;
  ULONG                 m_pendingFlags;
  ULONG                 m_suspendAllPendingFlags;
  ULONG                 m_killAllPendingFlags;
  CalculatorSet         m_calculatorsInState[3]; // one for each possible value of CalculatorState
  CalculatorSet         m_existing;

  void            setStateInternal(int id, CalculatorState state);
  CalculatorState getStateInternal(int id) const;
  void            killAllInternal();
  void            wakeAllInternal();
  void            waitUntilAllTerminated();
  void            waitUntilNoRunningInternal();
  void            waitUntilAllTerminatedInternal();
  void            addToExistingInternal(int id);
  void            clearExistingInternal();
  static inline const TCHAR *getStateName(CalculatorState state) {
    return s_stateName[state];
  }
  void notifyIfChanged(bool          oldIsActive  );
  void notifyIfChanged(CalculatorSet oldRunningSet);
  void createAllPendinglMasks();

#ifdef SAVE_CALCULATORINFO
  Array<CalculatorInfo> m_infoArray;
#endif

public:
  CalculatorPool(MBContainer &mbc);
 ~CalculatorPool();
  MBContainer &getMBContainer() {
    return m_mbc;
  }
  static inline int getCPUCount() {
    return s_CPUCount;
  }
  CalculatorState getState(int id);
  void            setState(int id, CalculatorState state);
  String getStatesString() const;

  int        getPendingMask(int id) {
    return 3 << (2*id);
  }
  inline int getPendingFlags(int id) {
    return (m_pendingFlags >> (2*id)) & 3;
  }
  inline int isPending(ULONG mask) {
    return m_pendingFlags & mask;
  }

  inline CalculatorSet getCalculatorsInState(CalculatorState state) const {
    return m_calculatorsInState[state];
  }
  void startCalculators(int count);
  inline bool isCalculationActive() const {
    return m_calculatorsInState[CALC_TERMINATED] < m_existing;
  }
  void suspendCalculation();
  void resumeCalculation();
  void killAll();
  void waitUntilNoRunning();

#ifdef SAVE_CALCULATORINFO
  void addCalculatorInfo(const CalculatorInfo &info);
  const CalculatorInfo *findInfo(const CPoint &p) const;
#endif

};
