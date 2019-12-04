#pragma once

#ifdef MULTITHREADEDEXTREMAFINDER

class ExtremumSearchParam;
class ExtremumResult;
typedef SynchronizedQueue<const ExtremumSearchParam*> ExtremumSearchParamQueue;
typedef SynchronizedQueue<const ExtremumResult*     > ResultQueue;

class MultiExtremaFinder {
  friend class ExtremumFinder;
  friend class ExtremumNotifier;
private:
  FastSemaphore                            m_lock;
  Remes                                   &m_remes;
  RunnableArray                            m_jobArray;
  CompactArray<const ExtremumSearchParam*> m_paramArray;
  CompactArray<const ExtremumResult*>      m_resultArray;
  ExtremumSearchParamQueue                 m_paramQueue;
  ResultQueue                              m_resultQueue;
  void putExtremum(int index, const BigReal &extremum);
public:
  MultiExtremaFinder(Remes *remes);
  ~MultiExtremaFinder();
  void insertJob(int index, const BigReal &l, const BigReal &r, const BigReal &m);
  void findAllExtrema();
};

#endif // MULTITHREADEDEXTREMAFINDER
