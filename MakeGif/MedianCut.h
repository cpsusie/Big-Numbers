#pragma once

#define POINTDIMENSIONCOUNT 3

typedef float PointDimensionType;

class DimPoint {
public:
  PointDimensionType m_x[POINTDIMENSIONCOUNT];
};

class DimPointWithIndex : public DimPoint {
public:
  UINT m_index;
  DimPointWithIndex() : m_index(0) {
  }
  DimPointWithIndex(const DimPoint &p) : DimPoint(p), m_index(0) {
  }
};

typedef CompactArray<DimPoint>          DimPointArray;
typedef CompactArray<DimPointWithIndex> DimPointWithIndexArray;
 
DimPointArray medianCut(DimPointWithIndexArray &points, UINT desiredSize);
