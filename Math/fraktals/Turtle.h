#pragma once

#include <Math/Transformation.h>

class Turtle {
  Point2D                   m_currentp;
  double                    m_currentdir;
  Rectangle2DTransformation m_tr;
  CDialog                  *m_dlg;
public:
  void init(CDialog *dlg, double minx, double maxx, double miny, double maxy);
  void move(  double l);
  void moveTo(double x, double y);
  void moveTo(const Point2D &p);
  void jump(  double l);
  void jumpTo(double x, double y);
  void jumpTo(const Point2D &p);
  Point2D pos();
  void turn(  double dir);
  void turnTo(double dir);
  double dir(void);
  void OnSize(CDialog *dlg);
};

