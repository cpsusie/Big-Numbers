#include "stdafx.h"
#include <Math.h>

class rectcoor {
public:
  double x,y,z;
};

class polarcoor {
public:
  double r,fi,theta;
};

class map2d {
public:
  double E,N;
};

map2d mercator(const polarcoor &p) {
  /*
  L = latitude in radians (positive north)
  Lo = longitude in radians (positive east)
  E = easting (meters)
  N = northing (meters)
  r = radius of the sphere (meters)
  E = r Lo
  N = r ln [ tan (pi/4 + L/2) ]
  */
  double L  = p.fi;
  double Lo = p.theta;
  map2d res;
  res.E = p.r * Lo;
  double tmp = tan (M_PI/4 + L /2);
  res.N = sign(tmp) * p.r * log (fabs(tmp));
  return res;
}

polarcoor rect2polar(const rectcoor &c) {
  polarcoor res;
  double x2 = sqr(c.x);
  double y2 = sqr(c.y);
  double z2 = sqr(c.z);
  res.r = sqrt(x2 + y2 + z2);
  if(x2 == 0 && y2 == 0) {
    if(c.z < 0)
      res.fi = M_PI;
    else
      res.fi  = 0;
    res.theta = 0;
  }
  else {
    res.fi = acos(c.z / res.r) ;
    res.theta = acos(c.x / sqrt(x2+y2));
    if(c.y < 0)
      res.theta = 2 * M_PI - res.theta;
  }
  return res;
}

rectcoor polar2rect(const polarcoor &p) {
  rectcoor res;
  double sf = sin(p.fi);
  double cf = cos(p.fi);
  double st = sin(p.theta);
  double ct = cos(p.theta);
  res.x = p.r * sf * ct;
  res.y = p.r * sf * st;
  res.z = p.r * cf;
  return res;
}

bool readcurve(FILE *f) {
  int n,i,type;
  rectcoor c;
  polarcoor p;
  map2d m;
  int dx,dy,dz;
  char line[200];
  int ch;
  while((ch = fgetc(f)) != EOF)
    if(ch == '/') break;
  if(feof(f)) return true;
  fgets(line,sizeof(line),f);
//  printf("%s",line);
  if(fscanf(f,"%d %d",&n,&type) != 2) return false;
  if(fscanf(f,"%le %le %le",&c.x,&c.z,&c.y) != 3) return false;
  for(i = 1; i < n; i++) {
    p = rect2polar(c);
    p.fi = M_PI/2 - p.fi;
    m = mercator(p);
    printf("%lf %lf\n",-m.E,m.N);
    if(fscanf(f,"%d %d %d",&dx,&dz,&dy) != 3) 
      return false;
    c.x += dx; c.y += dy; c.z += dz;
  }
  return true;
}

static void testrectpolar() {
  for(;;) {
    rectcoor rc;
    polarcoor pc;

    String line = inputString(_T("Indtast x,y,z:"));
    if(_stscanf(line.cstr(),_T("%le %le %le"), &rc.x, &rc.y, &rc.z) != 3) {
      printf("Fejl");
      continue;
    }
    pc = rect2polar(rc);
    printf("(r,fi,theta):(%lf %lf %lf)\n",pc.r,pc.fi/M_PI*180,pc.theta/M_PI*180);
    rc = polar2rect(pc);
    printf("(x,y,z):(%lf %lf %lf)\n",rc.x,rc.y,rc.z);
  }
}

int main(int argc, char* argv[]) {
  FILE *f = fopen("mapdata.dat","r");
  while(!feof(f)) {
    if(!readcurve(f)) {
      printf("Error reading input\n");
      exit(-1);
    }
  }
  return 0;
}
