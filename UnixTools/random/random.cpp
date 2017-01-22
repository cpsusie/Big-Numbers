// random.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <MyUtil.h>
#include <Array.h>
#include <String.h>
#include <math.h>
#include <Random.h>

//#define TESTRANDOM // define this to test random-integers

#ifdef TESTRANDOM

class IntegerStat {
public:
  int *m_count;
  int m_total;
  double m_low,m_high;
  IntegerStat(double low,double high);
  ~IntegerStat();
  int operator+=(double v);
};

IntegerStat::IntegerStat(double low,double high) { 
  m_total = 0;
  m_low   = low;
  m_high  = high;
  int n = (int)(m_high-m_low+1);
  m_count = new int[n];
  memset(m_count,0,sizeof(m_count[0])*n); 
}

int IntegerStat::operator+=(double v) {
  if(v < m_low || v > m_high) {
    printf("value %le out of range [%le;%le]\n",v,m_low,m_high);
    return 0;
  }
  int index = (int)(v-m_low);
  m_count[index]++;
  return m_total++;
}

IntegerStat::~IntegerStat() {
  int n = (int)(m_high-m_low+1);
  int sum = 0;
  for(int i = 0; i < n; i++) {
    printf("%3d ",m_count[i]);
    sum += m_count[i];
  }
  printf("\ntotal:%d, sum:%d\n",m_total,sum);
  delete[] m_count;
}

int main(int argc, char **argv) {
#define LOW  1
#define HIGH 20
  IntegerStat *statistic = new IntegerStat(LOW,HIGH);
  Random rnd(177);

  double start = getProcessTime();
  double sum = 0;
  for(int i = 0; i < 50000000; i++)
    sum += rnd.nextDouble();
  double slut = getProcessTime();
  printf("sum:%lf. getProcessTime:%lf\n",sum,(slut - start) / 1000000);
  exit(0);
  for(i = 0; i < 10000; i++) {
    double v = floor(rnd.nextDouble(LOW,HIGH+1));
    *statistic += v;
  }

  delete statistic;
  return 0;
}

#else

typedef enum {
  UNIFORM
 ,INTEGER
 ,GAUSS
} RandomType;

class RandomParameters {
public:
  RandomType m_type;
  double m_p1,m_p2;

  RandomParameters(RandomType type, double p1, double p2);
  double getRandom(Random &rnd);
};

RandomParameters::RandomParameters(RandomType type, double p1, double p2) {
  m_type = type;
  m_p1   = p1;
  m_p2   = p2;
}

double RandomParameters::getRandom(Random &rnd) {
  switch(m_type) {
  case UNIFORM: return rnd.nextDouble(m_p1,m_p2);
  case INTEGER: return floor(rnd.nextDouble(m_p1,m_p2+1));
  case GAUSS  : return rnd.nextGaussian(m_p1,m_p2);
  default     : throwException(_T("Illegal type (=%d)"), m_type);
                return 0;
  }
}

static void printRandom(Random &rnd, bool integer,double low, double high) {
  if(integer) {
    _tprintf(_T("%.0lf"), floor(rnd.nextDouble(low,high+1)));
  } else {
    _tprintf(_T("%le"), rnd.nextDouble(low,high));
  }
}

static void printRandomLine(Random &rnd, Array<RandomParameters> &param) {
  size_t dim = param.size();
  TCHAR *delim = _T("");
  for(size_t i = 0; i < dim; i++, delim = _T(" ")) {
    _tprintf(_T("%s%lg"), delim, param[i].getRandom(rnd));
  }
  _tprintf(_T("\n"));
}

static void usage() {
  _ftprintf(stderr,_T("Usage:random {[-iu[low,high]]|[-g[m[,v]]]}* [-ncount] [-ddimension] [-s[seed]]\n"));
  exit(-1);
}

int main(int argc, char **argv) { 
  char *cp;
  int n = 1;
  bool setseed = false;
  UINT64 seed = 1;
  RandomParameters u(UNIFORM,0,1);
  Array<RandomParameters> param;
  int dim,i;
  double p1,p2;

  for(argv++;*argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'n':
        if(sscanf(cp+1,"%d",&n) != 1)
          usage();
        if(n < 1) usage();
        break;
      case 'd':
        { if(sscanf(cp+1,"%d",&dim) != 1) {
            usage();
          }
          if(dim < 1) {
            usage();
          }
          RandomParameters &copy = (param.size() == 0) ? u : param[param.size()-1];
          for(i = 0; i < dim; i++) {
            param.add(copy);
          }
          break;
        }
      case 's':
        if(sscanf(cp+1,"%I64u", &seed) != 1) {
          seed = getRandomSeed();
        }
        break;
      case 'i':
        if(sscanf(cp+1,"%le,%le",&p1,&p2) != 2) {
          p1 = 0;
          p2 = 1;
        }
        if(p1 >= p2) {
          usage();
        }
        if(p1 != floor(p1)) {
          _ftprintf(stderr, _T("low must be integer (=%lf)\n"), p1);
          exit(-1);
        }
        if(p2 != floor(p2)) {
          _ftprintf(stderr, _T("high must be integer (=%lf)\n"), p2);
          exit(-1);
        }
        param.add(RandomParameters(INTEGER,p1,p2));
        break;
      case 'u':
        if(sscanf(cp+1,"%le,%le",&p1,&p2) != 2) {
          p1 = 0;
          p2 = 1;
        }
        if(p1 >= p2) {
          usage();
        }
        param.add(RandomParameters(UNIFORM,p1,p2));
        break;
      case 'g':
        if(sscanf(cp+1,"%le,%le",&p1,&p2) == 2) {
          if(p2 <= 0) {
            _ftprintf(stderr, _T("negative varians not allowed\n"));
            usage();
          }
        } else if(sscanf(cp+1,"%le",&p1) == 1) {
          p2 = 1;
        } else {
          p1 = 0;
          p2 = 1;
        }
        param.add(RandomParameters(GAUSS,p1,p2));
        break;
      default:
        usage();
      }
      break;
    }
  }

  if(param.size() == 0) {
    param.add(u);
  }

  Random rnd(seed);
  for(i = 0; i < n; i++) {
    printRandomLine(rnd,param);
  }
  return 0;
}

#endif
