#pragma once

class D3CoordinateSystem {
private:
  D3LineArray *m_box;
  D3LineArrow *m_xaxes, *m_yaxes, *m_zaxes;
public:
  D3CoordinateSystem(D3Scene &scene);
  ~D3CoordinateSystem();
  void setVisible(bool visible);
};

