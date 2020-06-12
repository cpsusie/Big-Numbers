//
// A Fast Algorithm for computing longest common subsequences
// by James W. Hunt
// and Thomas G. Szymanski
// from Communications of the ACM May 1977 Volume 20 no. 5

#include "stdafx.h"
#include <Timer.h>
#include <Date.h>
#include <Math.h>
#include "LcsSimple.h"

// #define DUMPMATCHLIST
// #define DUMPTRESHLINKS
// #define DUMPELEMENTPAIRS

LcsSimple::LcsSimple(const LineArray &a, const LineArray &b, UINT *docSize, LcsComparator &cmp, CompareJob *job)
: Lcs(cmp, job)
{
  m_matchList = NULL;
  init(a, b);
  if(m_seqReversed) {
    m_docSize[1] = docSize[0];
    m_docSize[0] = docSize[1];
  } else {
    m_docSize[0] = docSize[0];
    m_docSize[1] = docSize[1];
  }
}

LcsSimple::~LcsSimple() {
  clear();
}

void LcsSimple::allocateMatchList(size_t size) {
  m_matchList = new MatchArray*[size]; TRACE_NEW(m_matchList);
  memset(m_matchList,0,size*sizeof(m_matchList[0]));
}

void LcsSimple::clear() {
  SAFEDELETEARRAY(m_matchList);
}

void LcsSimple::init(const LineArray &a, const LineArray &b) {
  const size_t as = a.size();
  const size_t bs = b.size();
  m_A.clear();
  m_B.clear();
  m_A.add(LcsElement(-1,EMPTYSTRING)); // add a dummy-Element to make the real Elements from [1..n]
  m_B.add(LcsElement(-1,EMPTYSTRING)); // do
  if(as >= bs) {
    m_seqReversed = false;
    for(UINT i = 0; i < as; i++) {
      m_A.add(LcsElement(i+1,a[i]));
    }
    for(UINT i = 0; i < bs; i++) {
      m_B.add(LcsElement(i+1,b[i]));
    }
    m_n = as;
  } else { // bs > as
    m_seqReversed = true;
    for(UINT i = 0; i < as; i++) {
      m_B.add(LcsElement(i+1,a[i]));
    }
    for(UINT i = 0; i < bs; i++) {
      m_A.add(LcsElement(i+1,b[i]));
    }
    m_n = bs;
  }
  allocateMatchList(m_n+1);
  allocateTreshAndLinkArrays(m_n+1);
}

void LcsSimple::findLcs(ElementPairArray &result) {
  // step 1: build linked list
  const size_t matchListTotal = findMatchList();

  // step 2 initialize the m_tresh Array
  const UINT mnp1 = (UINT)m_n+1;
  for(size_t i = 1; i <= m_n; i++) {
    m_tresh[i] = mnp1;
  }

  if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(5, _T("Comparing"));

  // step 3 compute successive m_tresh values
  size_t progress = 0;
  for(UINT i = 1; i <= m_n; i++) {
    MatchArray &m = *m_matchList[i];
    const size_t ms = m.size();
    for(size_t mi = 0; mi < ms; mi++) {
      if(((progress++ & 0x3ff) == 0) && m_job) {
        m_job->setSubProgressPercent(SPERCENT(progress,matchListTotal));
      }
      const UINT j = m[mi];
      const UINT k = findK(j);
      if(j < m_tresh[k]) {
        m_tresh[k] = j;
        m_link[k] = newLink(i,j,m_link[k-1]);
      }
    }
  }

#if defined(DUMPTRESHLINKS)
//  dumpThreshLinks();
#endif // DUMPTRESHLINKS
  intptr_t k;
  for(k = m_n; k >= 0; k--) {
    if(m_tresh[k] < mnp1) {
      break;
    }
  }

END_TIMEMEASURE(  5, m_docSize[0] + m_docSize[1]);

  if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(6, _T("Building pairs"));

  CompactArray<Link*> pairs;
  for(Link *ptr = m_link[k]; ptr != NULL; ptr = ptr->m_next) {
    pairs.add(ptr);
  }

  size_t pairCount = 0;
  const size_t totalPairCount = pairs.size();

  result.setCapacity(totalPairCount);
  if(m_seqReversed) {
    for(size_t k = pairs.size(); k--;) {
      if(((pairCount++ & CHECK_INTERVAL) == 0) && m_job) { m_job->setSubProgressPercent(SPERCENT(pairCount,totalPairCount)); }
      result.add(ElementPair(pairs[k]->m_j-1,pairs[k]->m_i-1));
    }
  } else {
    for(size_t k = pairs.size(); k--;) {
      if(((pairCount++ & CHECK_INTERVAL) == 0) && m_job) { m_job->setSubProgressPercent(SPERCENT(pairCount,totalPairCount)); }
      result.add(ElementPair(pairs[k]->m_i-1,pairs[k]->m_j-1));
    }
  }
#if defined(DUMPELEMENTPAIRS)
  redirectDebugLog();
  debugLog(_T("ElementPairArray:\n%s"), result.toString(_T("\n")).cstr());
#endif // DUMPELEMENTPAIRS

END_TIMEMEASURE(  6, m_docSize[0]);
}

UINT LcsSimple::findK(UINT j) const { // find k:m_tresh[k-1] < j <= m_tresh[k]. ie min(k:m_tresh[k] <= j
  UINT l = 1;
  UINT r = (UINT)m_n;
  while(l < r) {
    const UINT mid = (l+r)>>1;
    if(m_tresh[mid] < j) {
      l = mid + 1;
    } else {               // m_tresh[mid] >= j
      r = mid;
    }
  }
  return r;
}

#define ESTIMATED_COMPARECOUNT(n) (1.725*nlogn((double)n))

class LcsSortJob : public CompareSubJob {
private:
  CompactArray<LcsElement> &m_a;
  IndexComparator          &m_cmp;
  const size_t              m_lineCount;
  const size_t              m_estimateCompareCount;
public:
  LcsSortJob(CompactArray<LcsElement> &a, IndexComparator &cmp)
  :m_a(a)
  ,m_cmp(cmp)
  ,m_lineCount(a.size())
  ,m_estimateCompareCount((size_t)ESTIMATED_COMPARECOUNT(a.size()))
  {
    m_cmp.setRunnable(this);
  }
  UINT safeRun() {
#if defined(MEASURE_STEPTIME)
  debugLog(_T("%s(lineCount:%s, compareCountEstimate:%s\n")
          ,__TFUNCTION__
          ,format1000(m_lineCount).cstr()
          ,format1000(m_estimateCompareCount).cstr());
#endif
    m_a.sort(1, m_a.size()-1, m_cmp);
#if defined(MEASURE_STEPTIME)
  debugLog(_T("%s(lineCount:%s, compareCountEstimate:%s actual compareCount:%s\n")
          ,__TFUNCTION__
          ,format1000(m_lineCount).cstr()
          ,format1000(m_estimateCompareCount).cstr()
          ,format1000(m_cmp.getCompareCount()).cstr());
#endif
    return 0;
  }
  USHORT getProgressPercent() const {
    return SPERCENT(m_cmp.getCompareCount(), m_estimateCompareCount);
  }
  size_t getWeight() const {
    return m_lineCount;
  }
};

size_t LcsSimple::findMatchList() {

BEGIN_TIMEMEASURE(3, _T("Sorting files"));

  Execute2(m_job).run(LcsSortJob(m_A, IndexComparator( m_cmp))
                     ,LcsSortJob(m_B, IndexComparatorR(m_cmp)));

END_TIMEMEASURE(  3, nlogn(max((double)m_A.size(), (double)m_B.size())));

  if(m_job) m_job->incrProgress();

BEGIN_TIMEMEASURE(4, _T("Finding matching lines"));

  const size_t bs = m_B.size();
  size_t bi = 1;
  size_t result = 0;
  size_t matchArrayCount   = 0;
  size_t matchArraySizeSum = 0;
  for(size_t ai = 1; ai <= m_n; ai++) {
    const LcsElement &AiElement = m_A[ai];
    if(((ai & CHECK_INTERVAL) == 0) && m_job) { m_job->setSubProgressPercent(SPERCENT(ai,m_n)); }

    while((bi < bs) && (m_cmp.compare(m_B[bi].m_s, AiElement.m_s) < 0)) {
      bi++;
    }
    if((ai > 1) && (m_cmp.compare(AiElement.m_s, m_A[ai-1].m_s) == 0)) {
      result += (m_matchList[AiElement.m_index] = m_matchList[m_A[ai-1].m_index])->size();
    } else {
      MatchArray m;
      matchArrayCount++;
      while((bi < bs) && (m_cmp.compare(m_B[bi].m_s, AiElement.m_s) == 0)) {
        m.add(m_B[bi++].m_index);
      }
      m.setCapacity(m.size());
      m_tmpM.add(m);
      result += m.size();
      matchArraySizeSum += m.size();
      m_matchList[AiElement.m_index] = &m_tmpM.last();
    }
  }

END_TIMEMEASURE(  4, m_docSize[0] + m_docSize[1]);

#if defined(DUMPMATCHLIST)
dumpMatchList();
#endif // DUMPMATCHLIST
  return result;
}

#if defined(DUMPMATCHLIST)
void LcsSimple::dumpMatchList() const {
  redirectDebugLog();
  debugLog(_T("MatchList:\n"));
  for(size_t i = 1; i <= m_n; i++) {
    MatchArray &m = *m_matchList[i];
    const size_t ms = m.size();
    debugLog(_T("%6zu:%s\n"), i, m.toStringBasicType().cstr());
  }
}
#endif // DUMPMATCHLIST
