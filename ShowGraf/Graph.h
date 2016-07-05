#pragma once

#include <float.h>
#include <MFCUtil/Viewport2D.h>
#include <MFCUtil/Coordinatesystem/DataRange.h>
#include "GraphParameters.h"

typedef enum {
  EXPRESSIONGRAPH
 ,DATAGRAPH
 ,ISOCURVEGRAPH
} GraphType;

class Graph {
private:
  bool m_visible;
protected:
  GraphParameters *m_param;
  Graph(GraphParameters *param) {
    m_param = param;
  }
  static inline double getMinPositive(double x, double r) {
    return (x > 0 && (r == 0 || x < r)) ? x : r;
  }
  static bool pointDefined(const Point2D &p) {
    return _finite(p.x) && !_isnan(p.x) && _finite(p.y) && !_isnan(p.y);
  }

public:
  virtual ~Graph() {
    delete m_param;
  }
  GraphParameters &getParam() const {
    return *m_param;
  }
  virtual void calculate() {}
  virtual void paint(Viewport2D &vp) = 0;
  virtual bool isEmpty() const  = 0;
  virtual const DataRange &getDataRange() const = 0;
  virtual GraphType getType() const = 0;
  virtual double  distance(const CPoint &p, const RectangleTransformation &tr) const = 0;
  double  distance(const Point2DP &p) const {;
    return distance(p,RectangleTransformation::id);
  }
  virtual double  getSmallestPositiveX() const = 0;
  virtual double  getSmallestPositiveY() const = 0;
  void setStyle(GraphStyle style) {
    m_param->m_style = style;
  }
  virtual void setRollSize(int size) {
  }
  virtual bool isPointGraph() const = 0;
  void setVisible(bool visible) {
    m_visible = visible;
  }
  bool isVisible() const {
    return m_visible;
  }
};

class PointGraph : public Graph {
private:
  Point2DArray         m_pointArray;
  mutable Point2DArray m_processedData;
  mutable bool         m_dataProcessed;
  DataRange            m_range;

protected:
  void findDataRange();
  PointGraph(GraphParameters *param);
public:
  void paint(Viewport2D &vp);

  bool isEmpty() const {
    return m_pointArray.size() == 0;
  }
  
  void addPoint(const Point2D &p);
  void clear();

  const DataRange &getDataRange() const {
    return m_range;
  }
  
  const Point2DArray &getProcessedData() const;
  
  const Point2DArray &getDataPoints() const {
    return m_pointArray;
  }
  
  void    setRollSize(int size);
  double  distance(const CPoint &p, const RectangleTransformation &tr) const;
  double  getSmallestPositiveX() const;
  double  getSmallestPositiveY() const;
  bool    isPointGraph() const {
    return true;
  }
};

