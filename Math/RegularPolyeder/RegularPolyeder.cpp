#include "stdafx.h"
#include <Math.h>
#include <Math/MathLib.h>
#include <Math/Matrix.h>

#define ANGLEG(v1,v2)  degrees(angle(v1,v2))

static void print(const D3DXVECTOR3 &v, FILE *f = stdout) {
  _ftprintf(f,_T("%f;%f;%f;"),v.x,v.y,v.z);
}

static double newton(double startx, Function &f) {
  double x = startx;
  const double EPS = 1e-4;
  for(;;) {
    double y = f(x);
    _tprintf(_T("x,y:%20.15le, DEGREE(%20.15le),%20.15le)\n"),x,RAD2GRAD(x),y);
    if(fabs(y) < 1e-10) return x;
    double x1 = (x == 0)?EPS:(x*(1+EPS));
    double y1 = f(x1);
    double dfdx = (y-y1)/(x-x1);
    x -= y / dfdx;
  }
}

class FindTheta : public Function {
public:
  D3DXVECTOR3 d1,d2,d3,d4;
  double operator()(const double &x);
};

double FindTheta::operator()(const double &v) {
  D3DXVECTOR3 p1 = rotate(d2,d1,(float)v);
  D3DXVECTOR3 p2 = rotate(d4,d3,(float)v);
  return dist(p1,p2);
}

static void find_tetra_theta() {
  FindTheta func;
  func.d1.x = 1;
  func.d1.y = 0;
  func.d1.z = 0;
  func.d2.x = (float)cos(GRAD2RAD(120));
  func.d2.y = (float)sin(GRAD2RAD(120));
  func.d2.z = 0;
  func.d3.x = (float)cos(GRAD2RAD(120));
  func.d3.y = (float)sin(GRAD2RAD(120));
  func.d3.z = 0;
  func.d4.x = (float)cos(GRAD2RAD(180));
  func.d4.y = (float)sin(GRAD2RAD(180));
  func.d4.z = 0;
  _tprintf(_T("d1:")); print(func.d1); _tprintf(_T("\n"));
  _tprintf(_T("d2:")); print(func.d2); _tprintf(_T("\n"));
  _tprintf(_T("d3:")); print(func.d3); _tprintf(_T("\n"));
  _tprintf(_T("d4:")); print(func.d4); _tprintf(_T("\n"));

  _tprintf(_T("%le\n"),newton(M_PI/2,func));
}

static void find_dodeka_theta() {
  FindTheta func;
  func.d1.x = 1;
  func.d1.y = 0;
  func.d1.z = 0;
  func.d2.x = (float)cos(GRAD2RAD(108));
  func.d2.y = (float)sin(GRAD2RAD(108));
  func.d2.z = 0;
  func.d3.x = (float)cos(GRAD2RAD(-108));
  func.d3.y = (float)sin(GRAD2RAD(-108));
  func.d3.z = 0;
  func.d4.x = (float)cos(GRAD2RAD(-216));
  func.d4.y = (float)sin(GRAD2RAD(-216));
  func.d4.z = 0;
  _tprintf(_T("d1:")); print(func.d1);_tprintf(_T("\n"));
  _tprintf(_T("d2:")); print(func.d2);_tprintf(_T("\n"));
  _tprintf(_T("d3:")); print(func.d3);_tprintf(_T("\n"));
  _tprintf(_T("d4:")); print(func.d4);_tprintf(_T("\n"));

  _tprintf(_T("%le\n"),newton(M_PI/2,func));
}

class FindIkoTheta : public Function {
public:
  D3DXVECTOR3 d[5];
  D3DXVECTOR3 d6;
  double dd;
  double operator()(const double &x);
};

double FindIkoTheta::operator()(const double &z) {
  D3DXVECTOR3 tmp = d6;
  tmp.z = (float)z;
  return dist(tmp,d[0]) - dd;
}

static void find_ikosa_theta() {
  FindIkoTheta func;
  for(int i = 0; i < 5; i++) {
    func.d[i].x = (float)cos(GRAD2RAD(72*i));
    func.d[i].y = (float)sin(GRAD2RAD(72*i));
    func.d[i].z = 0;
  }
  func.dd = dist(func.d[0],func.d[1]);
  func.d6.x = 0; func.d6.y = 0; func.d6.z = (float)0.6180339887498948482045868343656;

//  _tprintf(_T("%le\n"),newton(0.1,func)); // resultat = func.d6.z;

  D3DXVECTOR3 v1 = func.d6;
  D3DXVECTOR3 v2 = func.d[0];
  D3DXVECTOR3 v3 = func.d[1];
  D3DXVECTOR3 v4 = func.d[4];

  D3DXVECTOR3 v5 = (v1 + v2) / 2;

  D3DXVECTOR3 v6 = (v3 - v5);
  D3DXVECTOR3 v7 = (v4 - v5);
  _tprintf(_T("vinkel:%20.15lf\n"), ANGLEG(v6,v7));
}

class FindOktaTheta : public Function {
public:
  D3DXVECTOR3 d[4];
  D3DXVECTOR3 d6;
  double dd;
  double operator()(const double &x);
};

double FindOktaTheta::operator()(const double &z) {
  D3DXVECTOR3 tmp = d6;
  tmp.z = (float)z;
  return dist(tmp,d[0]) - dd;
}

static void find_okta_theta() {
  FindOktaTheta func;
  for(int i = 0; i < 4; i++) {
    func.d[i].x = (float)cos(GRAD2RAD(90*i));
    func.d[i].y = (float)sin(GRAD2RAD(90*i));
    func.d[i].z = 0;
  }
  func.dd = dist(func.d[0],func.d[1]);
  func.d6.x = 0; func.d6.y = 0; func.d6.z = 1;

//  _tprintf(_T("%le\n"),newton(0.1,func)); // resultat = func.d6.z;

  D3DXVECTOR3 v1 = func.d6;
  D3DXVECTOR3 v2 = func.d[0];
  D3DXVECTOR3 v3 = func.d[1];
  D3DXVECTOR3 v4 = func.d[3];

  D3DXVECTOR3 v5 = (v1 + v2) / 2;

  D3DXVECTOR3 v6 = (v3 - v5);
  D3DXVECTOR3 v7 = (v4 - v5);
  _tprintf(_T("vinkel:%20.15lf\n"), ANGLEG(v6,v7));
}

// ----------------------------------------------------------
typedef enum {
  FORMAT_WAVEFRONT
 ,FORMAT_MSDDRAW
} PrintFormat;

class Face {
private:
  int         m_n;
public:
  int         m_p[5];
  D3DXVECTOR3 m_normal;

  Face();
  int  getPointCount() const { return m_n; }
  void addPoint(int p);
  bool hasEdge(int i1, int i2) const;
  bool hasPoint(int p) const;
  void reverseOrder();
  void print(FILE *f, PrintFormat pf) const;
};

Face::Face() {
  m_n = 0;
}

void Face::addPoint(int p) {
  assert(m_n < 5);
  m_p[m_n++] = p;
}

bool Face::hasEdge(int i1, int i2) const {
  for(int i = 0; i < m_n; i++) {
    if(m_p[i] == i1) {
      if(i > 0 && i < m_n - 1) {
        if(m_p[i-1] == i2 || m_p[i+1] == i2) {
          return true;
        }
      }
      if(i == 0 && (m_p[m_n-1] == i2 || m_p[1] == i2)) {
        return true;
      }
      if(i == m_n-1 && (m_p[0] == i2 || m_p[m_n-2] == i2)) {
        return true;
      }
    }
  }
  return false;
}

bool Face::hasPoint(int p) const {
  for(int i = 0; i < m_n; i++) {
    if(m_p[i] == p) {
      return true;
    }
  }
  return false;
}

bool operator==(const Face &p1, const Face &p2) {
  const int pcount = p1.getPointCount();
  if(p2.getPointCount() != pcount) {
    return false;
  }
  for(int i = 0; i < pcount; i++) {
    if(p1.m_p[i] != p2.m_p[i]) {
      return false;
    }
  }
  return true;
}

void Face::reverseOrder() {
  const int n = m_n / 2;
  for(int i = 0; i < n; i++) {
    std::swap(m_p[i],m_p[m_n-1-i]);
  }
}

void Face::print(FILE *f, PrintFormat pf) const {
  switch(pf) {
  case FORMAT_WAVEFRONT:
  case FORMAT_MSDDRAW  :
    { _ftprintf(f,_T("%d;"),m_n);
      for(int i = 0; i < m_n; i++) {
        _ftprintf(f,_T("%d%c"),m_p[i],(i==m_n-1)?';':',');
      }
    }
    break;
  }
}

class RegPolyEder {
private:
  const PrintFormat         m_printFormat;
  CompactArray<D3DXVECTOR3> m_points;
  CompactArray<Face>        m_faces;

  int         edgeCount(int i1, int i2) const;
  int         findFace(const Face &p) const;
  int         findPoint(const D3DXVECTOR3 &p) const;
  D3DXVECTOR3 getNormal(int p) const;
  void        adjustPoints();
  void        adjustNormals();
  void        adjustOrder();
  D3DXVECTOR3 getMiddlePoint(int f) const;
  void        makeFace(int corners, double theta, const D3DXVECTOR3 &start, const D3DXVECTOR3 &normal, const D3DXVECTOR3 &dir, int s);
  bool        hasRightFaceOrder(int f) const;
  void        printWaveFront(FILE *f) const;
  void        printDDraw(    FILE *f) const;
public:
  RegPolyEder(int facecount, PrintFormat pf);
  void scale(double factor);
  double sideLength()  const;
  double outerRadius() const;
  double innerRadius() const;
  void print(FILE *f = stdout) const;
};

int RegPolyEder::findPoint(const D3DXVECTOR3 &p) const {
  const size_t pn = m_points.size();
  for(size_t i = 0; i < pn; i++) {
    const double d = dist(m_points[i],p);
    if(d < 1e-2) return (int)i;
  }
  return -1;
}

int RegPolyEder::findFace(const Face &p) const {
  for(size_t i = 0; i < m_faces.size(); i++) {
    if(m_faces[i] == p) {
      return (int)i;
    }
  }
  return -1;
}

int RegPolyEder::edgeCount(int i1, int i2) const {
  int count = 0;
  for(size_t i = 0; i < m_faces.size(); i++) {
    const Face &p = m_faces[i];
    if(p.hasEdge(i1,i2)) count++;
  }
  return count;
}

D3DXVECTOR3 RegPolyEder::getNormal(int p) const {
  D3DXVECTOR3 n;
  n.x = n.y = n.z = 0;
  for(size_t i = 0; i < m_faces.size(); i++) {
    const Face &face = m_faces[i];
    if(face.hasPoint(p)) {
      n += face.m_normal;
    }
  }
  return n;
}

D3DXVECTOR3 RegPolyEder::getMiddlePoint(int f) const {
  D3DXVECTOR3 m;
  m.x = m.y = m.z = 0;
  const Face &face = m_faces[f];
  const int pcount = face.getPointCount();
  for(int i = 0; i < pcount; i++) {
    const D3DXVECTOR3 &p = m_points[face.m_p[i]];
    m += p;
  }
  m /= (float)pcount;
  return m;
}

void RegPolyEder::adjustPoints() {
  D3DXVECTOR3 m;
  m.x = m.y = m.z = 0;
  const size_t pn = m_points.size();
  for(size_t i = 0; i < pn; i++) {
    m += m_points[i];
  }
  m.x /= pn; m.y /= pn; m.z /= pn;
  for(size_t i = 0; i < pn; i++) {
    m_points[i] -= m;
  }
}

void RegPolyEder::adjustNormals() {
  for(size_t f = 0; f < m_faces.size(); f++) {
    Face &face = m_faces[f];
    face.m_normal = getMiddlePoint((int)f);
    face.m_normal /= length(face.m_normal);
  }
}

void RegPolyEder::adjustOrder() {
  for(size_t f = 0; f < m_faces.size(); f++) {
    if(!hasRightFaceOrder((int)f)) {
      m_faces[f].reverseOrder();
    }
  }
}

bool RegPolyEder::hasRightFaceOrder(int f) const {
  const Face &face = m_faces[f];
  switch(m_printFormat) {
  case FORMAT_WAVEFRONT:
    return cross(m_points[face.m_p[0]], m_points[face.m_p[1]]) * m_points[face.m_p[2]] > 0;

  case FORMAT_MSDDRAW  :
    { D3DXMATRIX m;
      for(int i = 0; i < 3; i++) {
        const D3DXVECTOR3 &p = m_points[face.m_p[i]];
        m(0,i) = p.x; m(1,i) = p.y; m(2,i) = p.z;
      }
      return det(m) > 0;
    }
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("printformat=%d"), m_printFormat);
    return false;
  }
}

void RegPolyEder::scale(double factor) {
  size_t pn = m_points.size();
  for(size_t i = 0; i < pn; i++) {
    m_points[i] *= (float)factor;
  }
}

double RegPolyEder::sideLength() const {
  const Face &face = m_faces[0];
  return dist(m_points[face.m_p[0]],m_points[face.m_p[1]]);
}

double RegPolyEder::outerRadius() const {
  return length(m_points[0]);
}

double RegPolyEder::innerRadius() const {
  return length(getMiddlePoint(0));
}

void RegPolyEder::print(FILE *f) const {
  switch(m_printFormat) {
  case FORMAT_WAVEFRONT: printWaveFront(f); break;
  case FORMAT_MSDDRAW  : printDDraw(f);     break;
  }
}

void RegPolyEder::printWaveFront(FILE *f) const {
  const size_t pn = m_points.size();
  for (size_t i = 0; i < pn; i++) {
    const D3DXVECTOR3 &v = m_points[i];
    _ftprintf(f, _T("v %f %f %f\n"), v.x, v.y, v.z);
  }
  const size_t fn = m_faces.size();
  for (size_t i = 0; i < fn; i++) {
    const D3DXVECTOR3 normal = m_faces[i].m_normal;
    _ftprintf(f, _T("vn %f %f %f\n"), normal.x, normal.y, normal.z);
  }
  for (size_t i = 0; i < fn; i++) {
    const Face &face = m_faces[i];
    const int pcount = face.getPointCount();
    _ftprintf(f, _T("f"));
    for(int p = 0; p < pcount; p++) {
      _ftprintf(f, _T(" %d//%d"), face.m_p[p]+1, (int)i+1);
    }
    _ftprintf(f, _T("\n"));
  }
}

void RegPolyEder::printDDraw(FILE *f) const {
  _ftprintf(f,_T("%s"),
              _T("xof 0303txt 0032\n\n"
                 "Header {\n"
                 " 1;\n"
                 " 0;\n"
                 " 1;\n"
                 "}\n\n"
                 "Mesh {\n")
           );
  size_t pn = m_points.size();
  _ftprintf(f,_T("%d;\n"),(int)pn);
  for(size_t i = 0; i < pn; i++) {
    ::print(m_points[i],f);
    _ftprintf(f,_T("%c\n"),(i == pn-1)?';':',');
  }
  size_t fn = m_faces.size();
  _ftprintf(f,_T("%d;\n"),(int)fn);
  for(size_t i = 0; i < fn; i++) {
    m_faces[i].print(f, FORMAT_MSDDRAW);
    _ftprintf(f,_T("%c\n"),(i == fn-1)?';':',');
  }

  _ftprintf(f,_T("%s"),
              _T("\n\nMeshMaterialList {\n"
                 " 1;\n"
                 " 1;\n"
                 " 0;\n\n"
                 " Material {\n"
                 "  1.000000;1.000000;1.000000;1.000000;;\n"
                 "  0.000000;\n"
                 "  0.000000;0.000000;0.000000;;\n"
                 "  0.000000;0.000000;0.000000;;\n"
                 " }\n"
                 "}\n\n")
           );

  _ftprintf(f,_T("MeshNormals {\n"));
  _ftprintf(f,_T("%d;\n"), (int)fn);
  for(size_t i = 0; i < fn; i++) {
    const Face &face = m_faces[i];
    ::print(face.m_normal,f);
    _ftprintf(f,_T("%c\n"),(i == fn-1)?';':',');
  }
  _ftprintf(f,_T("%d;\n"), (int)fn);
  for(size_t i = 0; i < fn; i++) {
    int pcount = m_faces[i].getPointCount();
    _ftprintf(f,_T("%d;"),pcount);
    for(int k = 0; k < pcount; k++) {
      _ftprintf(f,_T("%d%c"),(int)i,(k==pcount-1)?';':',');
    }
    _ftprintf(f,_T("%c\n"),(i == fn-1)?';':',');
  }
  _ftprintf(f,_T("}\n"));


  _ftprintf(f,_T("MeshTextureCoords {\n  %d;\n"), (int)pn);
  for(size_t i = 0; i < pn; i++) {
    _ftprintf(f,_T("    %d;%d;%c\n"),0,0,(i==pn-1)?';':',');
  }
  _ftprintf(f,_T("}\n"));
  _ftprintf(f,_T("}\n"));
}

void RegPolyEder::makeFace(int corners, double theta, const D3DXVECTOR3 &start, const D3DXVECTOR3 &normal, const D3DXVECTOR3 &dir, int s) {
  D3DXVECTOR3 p = start;
  D3DXVECTOR3 d = dir;
  double v = 360 / corners;
  Face f;
  for(int i = 0; i < corners; i++) {
//    ::print(p); _tprintf(_T("\n"));
    int pindex;
    if((pindex = findPoint(p)) < 0) {
      m_points.add(p);
      pindex = (int)m_points.size() - 1;
    }
    f.addPoint(pindex);
    p += d;
    d = rotate(d,normal,(float)GRAD2RAD(s*v));
  }
  f.m_normal = normal;
//  f.print();
//  _tprintf(_T("\n"));
  if(findFace(f) < 0) {
    m_faces.add(f);
    if(m_faces.size() > 30)
      throwException(_T("makeface:Too many faces. max 20"));
    p = start;
    d = dir;
    for(int i = 0; i < corners; i++) {
      if(edgeCount(f.m_p[i],f.m_p[(i+1) % corners]) < 2) {
        D3DXVECTOR3 newnormal = rotate(normal,d,(float)GRAD2RAD(s*theta));
        makeFace(corners,theta,p,newnormal,d,-s);
      }
      p += d;
      d = rotate(d,normal,(float)GRAD2RAD(s*v));
    }
  }
}

// vinkler mellem 2 naboflader i de 5 polyedre
#define tetra_theta  109.47122063449
#define hexa_theta   90
#define okta_theta   70.52877936551
/* okta_theta = 180 - tetra_theta */

#define dodeka_theta 63.43494655075848
#define ikosa_theta  41.81031422363637

RegPolyEder::RegPolyEder(int facecount, PrintFormat printFormat)
: m_printFormat(printFormat)
{
  D3DXVECTOR3 start,normal,dir;
  start.x  = start.y  = start.z = 0;
  normal.x = normal.y = 0; normal.z = 1;
  dir.x    = 1; dir.y = 0; dir.z = 0;
  switch(facecount) {
  case 4:
    makeFace(3,tetra_theta,start,normal,dir,1);
    break;
  case 6:
    makeFace(4,hexa_theta,start,normal,dir,1);
    break;
  case 8:
    makeFace(3,okta_theta,start,normal,dir,1);
    break;
  case 12:
    makeFace(5,dodeka_theta,start,normal,dir,1);
    break;
  case 20:
    makeFace(3,ikosa_theta,start,normal,dir,1);
    break;
  default:
    throwException(_T("regpolyeder:Illegal facecount. must be 4,6,8,12 or 20"));
  }
  adjustPoints();
  adjustNormals();
  adjustOrder();
}

static void usage(){
  _ftprintf(stderr, _T("Usage:RegularPolyeder [-ssize] [-fW|D] [-e|-o|-i] [4|6|8|12|20]\n"));
  exit(-1);
}

typedef enum {
  SCALEBY_EDGELENGTH,
  SCALEBY_OUTERRADIUS,
  SCALEBY_INNERRADIUS
} SCALEBY;

int main(int argc, char **argv) {
  char       *cp;
  int         faceCount   = 4;
  double      size        = 1;
  SCALEBY     scaleby     = SCALEBY_EDGELENGTH;
  PrintFormat printFormat = FORMAT_WAVEFRONT;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 's':
        if(sscanf(cp+1,"%le",&size) != 1) usage();
        break;
      case 'f':
        switch (cp[1]) {
        case 'W': printFormat = FORMAT_WAVEFRONT; break;
        case 'D': printFormat = FORMAT_MSDDRAW  ; break;
        default : usage();
        }
        break;
      case 'e':
        scaleby = SCALEBY_EDGELENGTH; continue;
      case 'o':
        scaleby = SCALEBY_OUTERRADIUS; continue;
      case 'i':
        scaleby = SCALEBY_INNERRADIUS; continue;
      default:
        usage();
      }
      break;
    }
  }

  if(*argv) {
    sscanf(*argv, "%d", &faceCount);
  }
  String fname = _T("tetraeder");
  switch(faceCount) {
  case 4: fname = _T("tetraeder" ); break;
  case 6: fname = _T("hexaeder"  ); break;
  case 8: fname = _T("oktaeder"  ); break;
  case 12:fname = _T("dodekaeder"); break;
  case 20:fname = _T("ikosaeder" ); break;
  }

  if (printFormat == FORMAT_WAVEFRONT) {
    fname += _T(".obj");
  } else {
    fname += _T(".x");
  }
  try {
    RegPolyEder polyEder(faceCount, printFormat);
    double scaleFactor;
    switch(scaleby) {
    case SCALEBY_EDGELENGTH :
      scaleFactor = size / polyEder.sideLength();
      break;
    case SCALEBY_OUTERRADIUS:
      scaleFactor = size / polyEder.outerRadius();
      break;
    case SCALEBY_INNERRADIUS:
      scaleFactor = size / polyEder.innerRadius();
      break;
    }
    polyEder.scale(scaleFactor);
    FILE *f = fopen(fname, _T("w"));
    if(f != NULL) {
      polyEder.print(f);
      fclose(f);
    }
    polyEder.print();
  } catch(Exception e) {
    _tprintf(_T("%s\n"), e.what());
    return -1;
  }
  return 0;
}
