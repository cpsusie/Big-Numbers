#include "pch.h"
#include <PriorityQueue.h>
#include <algorithm>
#include <MathUtil.h>
#include <MedianCut.h>

class Block {
private:
  PointDimensionType getSideLength(int dimension) const;
  static void findMinMaxCorner(DimPointWithIndex **points, UINT pointCount, DimPoint &minCorner, DimPoint &maxCorner);
public:
  DimPointWithIndex           **m_points;
  UINT                          m_pointCount;
  DimPoint                      m_minCorner, m_maxCorner;
  const DimPoint               &m_scale;
  mutable PointDimensionType    m_longestSideLength;
  Block(DimPointWithIndex **points, UINT pointCount, const DimPoint &scale);
  int                getLongestSideIndex()  const;
  PointDimensionType getLongestSideLength() const;
  DimPoint           getAveragePoint()   const;
  void               setIndexOnAllPoints(UINT index) const;
  String             toString() const;
};

Block::Block(DimPointWithIndex **points, UINT pointCount, const DimPoint &scale) : m_scale(scale) {
  m_points            = points;
  m_pointCount        = pointCount;
  m_longestSideLength = -1;
  findMinMaxCorner(points, pointCount, m_minCorner, m_maxCorner);
}

void Block::findMinMaxCorner(DimPointWithIndex **points, UINT pointCount, DimPoint &minCorner, DimPoint &maxCorner) { // static
  if(pointCount == 0) {
    for(int j = POINTDIMENSIONCOUNT; j--;) {
      minCorner.m_x[j] = maxCorner.m_x[j] = 0;
    }
  } else {
    DimPointWithIndex **p = points, **endp = p + pointCount;
    PointDimensionType *minx = minCorner.m_x, *maxx = maxCorner.m_x;

    for(int j = POINTDIMENSIONCOUNT; j--;) {
      minx[j] = maxx[j] = (*p)->m_x[j];
    }
    for(p++; p < endp; p++) {
      const PointDimensionType *x = (*p)->m_x;
      for(int j = 0; j < POINTDIMENSIONCOUNT; j++, x++) {
        if(*x < minx[j]) {
          minx[j] = *x;
        } else if(*x > maxx[j]) {
          maxx[j] = *x;
        }
      }
    }
  }
}

void Block::setIndexOnAllPoints(UINT index) const {
  for(DimPointWithIndex **p = m_points, **endp = p + m_pointCount; p < endp;) {
    (*(p++))->m_index = index;
  }
}

DimPoint Block::getAveragePoint() const {
  DimPoint result;

  if(m_pointCount == 0) {
    for(int j = POINTDIMENSIONCOUNT; j--;) {
      result.m_x[j] = 0;
    }
  } else {
    PointDimensionType sum[POINTDIMENSIONCOUNT];
    for(int j = POINTDIMENSIONCOUNT; j--;) {
      sum[j] = 0;
    }
    for(DimPointWithIndex **p = m_points, **endp = p + m_pointCount; p < endp; p++) {
      for(int j = POINTDIMENSIONCOUNT; j--;) {
        sum[j] += (*p)->m_x[j];
      }
    }
    for(int j = POINTDIMENSIONCOUNT; j--;) {
      result.m_x[j] = sum[j] / m_pointCount;
    }
  }
  return result;
}

PointDimensionType Block::getSideLength(int dimension) const {
  const PointDimensionType scale = m_scale.m_x[dimension];
  PointDimensionType d = m_maxCorner.m_x[dimension] - m_minCorner.m_x[dimension];
  return (scale == 0) ? d : d / scale;
}

int Block::getLongestSideIndex() const {
  PointDimensionType m = getSideLength(0);
  int maxIndex = 0;
  for(int j = 1; j < POINTDIMENSIONCOUNT; j++) {
    const PointDimensionType diff = getSideLength(j);
    if(diff > m) {
      m = diff;
      maxIndex = j;
    }
  }
  return maxIndex;
}

PointDimensionType Block::getLongestSideLength() const {
  if(m_longestSideLength < 0) {
    const int j = getLongestSideIndex();
    m_longestSideLength = getSideLength(j);
  }
  return m_longestSideLength;
}

String Block::toString() const {
  return format(_T("%.0f"), getLongestSideLength());
}

class BlockComparator : public Comparator<Block> {
public:
  int compare(const Block &b1, const Block &b2) {
    return sign(b2.getLongestSideLength() - b1.getLongestSideLength());
  }
  AbstractComparator *clone() const {
    return new BlockComparator();
  }
};

template <int index> class CoordinatePointComparatorFast {
public:
  bool operator()(DimPointWithIndex *left, DimPointWithIndex *right) {
    return left->m_x[index] < right->m_x[index];
  }
};

class CoordinatePointComparatorSlow {
private:
  const int m_index;
public:
  CoordinatePointComparatorSlow(int index) : m_index(index) {
  }
  bool operator()(DimPointWithIndex *left, DimPointWithIndex *right) {
    return left->m_x[m_index] < right->m_x[m_index];
  }
};

static DimPoint findDimensionScale(const Block &block) {
  DimPoint result;
  for(int j = POINTDIMENSIONCOUNT; j--;) {
    result.m_x[j] = block.m_maxCorner.m_x[j] - block.m_minCorner.m_x[j];
  }
  return result;
}

DimPointArray medianCut(DimPointWithIndexArray &points, UINT desiredSize) {

//  redirectDebugLog();

  if(points.size() <= desiredSize) { // just set indices in points and return it
    DimPointArray result;
    for(UINT index = 0; index < points.size(); index++) {
      points[index].m_index = index;
      result.add(points[index]);
    }
    return result;
  }

  CompactArray<DimPointWithIndex*> pp(points.size());
  for(size_t i = 0; i < points.size(); i++) {
    pp.add(&points[i]);
  }
  DimPoint scale;

  Block initialBlock(&pp[0], (UINT)pp.size(), scale);
  scale = findDimensionScale(initialBlock);

  PriorityQueue<Block> blockPtq(BlockComparator(), false);
  blockPtq.add(initialBlock);

  while(blockPtq.size() < desiredSize) {
    Block longestBlock = blockPtq.remove();

//  debugLog(_T("qsize:%3zd, q:%s\n"), blockPtq.size(), blockPtq.toString().cstr());

    DimPointWithIndex **begin  = longestBlock.m_points;
    DimPointWithIndex **median = longestBlock.m_points + (longestBlock.m_pointCount+1)/2;
    DimPointWithIndex **end    = longestBlock.m_points + longestBlock.m_pointCount;
    const int longestSideIndex = longestBlock.getLongestSideIndex();
    switch(longestSideIndex) {
    case 0 :
      std::nth_element(begin, median, end, CoordinatePointComparatorFast<0>());
      break;
    case 1 :
      std::nth_element(begin, median, end, CoordinatePointComparatorFast<1>());
      break;
    case 2 :
      std::nth_element(begin, median, end, CoordinatePointComparatorFast<2>());
      break;
    default:
      std::nth_element(begin, median, end, CoordinatePointComparatorSlow(longestSideIndex));
      break;
    }

    Block block1(begin, (UINT)(median-begin), scale), block2(median, (UINT)(end-median), scale);
    blockPtq.add(block1);
    blockPtq.add(block2);
  }
  DimPointArray result;
  while(!blockPtq.isEmpty()) {
    Block block = blockPtq.remove();
    block.setIndexOnAllPoints((UINT)result.size());
    result.add(block.getAveragePoint());
  }
  return result;
}
