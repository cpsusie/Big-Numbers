#pragma once

inline double dist(const CPoint &p1, const CPoint &p2) {
  return sqrt(sqr(p1.x-p2.x) + sqr(p1.y-p2.y));
}

class vector {
public:
  CPoint m_p;
  inline vector(const CPoint &p) : m_p(p){ 
  }
  inline vector(int x, int y) : m_p(x,y) {
  }
  inline vector(const CPoint &p1, const CPoint &p2) 
    : m_p(p2.x-p1.x, p2.y-p1.y)
  {
  }
  inline vector operator+(const vector &v) const {
    return vector(m_p.x + v.m_p.x, m_p.y + v.m_p.y);
  }
  inline vector operator-(const vector &v) const {
    return vector(m_p.x - v.m_p.x, m_p.y - v.m_p.y);
  }
  inline double operator*(const vector &v) const {
    return m_p.x * v.m_p.x + m_p.y*v.m_p.y;
  }

  inline double length() const {
    return sqrt(sqr(m_p.x) + sqr(m_p.y));
  }
};

inline vector operator-(const CPoint &p1, const CPoint &p2) {
  return CPoint(p1.x - p2.x, p1.y - p2.y);
}

inline double det(const vector &v1, const vector &v2) {
  return v1.m_p.x * v2.m_p.y - v1.m_p.y * v2.m_p.x;
}

class line {
public:
  CPoint m_p1, m_p2;
  line(const CPoint &p1, const CPoint &p2) {
    m_p1 = p1;
    m_p2 = p2;
  }
  line(const CPoint &p , const vector &v) : m_p1(p) {
    m_p2.x = p.x + v.m_p.x;
    m_p2.x = p.y + v.m_p.y;
  }
  void paint(CDC &dc, const CRect &rect) const;
};

typedef CompactArray<CPoint> PointArray;

class polygon {
private:
  PointArray m_points;
public:
  void paint(CDC &dc) const;
  void convexHull();
  void addPoint(const CPoint &p);
  void removeNearest(const CPoint &p);
  int findTopPoint() const;
  int findBottomPoint() const;
  int findNearest(const CPoint &p) const;
  double pointInside(const CPoint &p) const; // 1=inside, -1=outside, 0=edge
  inline int pointCount() const { return (int)m_points.size(); }
  inline const CPoint &point(int i) const { return m_points[i]; }
};

class CPolygondistDlg : public CDialog {
private:
  HICON   m_hIcon;
  polygon m_poly;
public:
  CPolygondistDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_POLYGONDIST_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand( UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnLButtonUp(  UINT nFlags, CPoint point);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnButtonfindmaxdist();
  afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()
};
