#include "stdafx.h"

#define PI 3.141592653
#define tpi (2 * PI)
#define rads (PI / 180)

double Angle(double Day) {
  return 2 * PI * (int(Day) + 0.5) / 365.25;
}

double range(double x) {
  double b = x / tpi;
  double a = tpi * (b - int(b));
  if(a < 0) a = tpi + a;
  return a;
}

int jday(int y, int m, int d) {
  int J = 367 * y - int(7 * (y + int((m + 9) / 12)) / 4);
  J = J - int(3 * (int((y + int((m - 9) / 7)) / 100) + 1) / 4);
  J = J + int(275 * m / 9) + d + 1721029;
  return J;
}

double Lambda(int jday) {
  double L = range(280.461 * rads + 0.9856474 * rads * jday);
  double g = range(357.528 * rads + 0.9856003 * rads * jday);
  return range(L + 1.915 * rads * sin(g) + 0.02 * rads * sin(2 * g));
}

double oblique(int jday) {
  return 23.439 * rads - 0.0000004 * rads * jday;
}

double delta(int jd) {
  double sdelta = sin(oblique(jd)) * sin(Lambda(jd));
  return atan(sdelta / sqrt(1.0 - sdelta * sdelta));
}

double alpha(int jd) {
  double x = cos(Lambda(jd));
  double y = cos(oblique(jd)) * sin(Lambda(jd));
  double Alpha = atan(y / x);
  // Sign corresponds arctan2()
  if ((x < 0) && (y < 0)) Alpha = Alpha - PI;
  if ((x < 0) && (y > 0)) Alpha = Alpha + PI;
  return Alpha;
}

double timeq(int jd) {
  double L = range(280.461 * rads + 0.9856474 * rads * jd);
  double korj = 720 * (L - alpha(jd)) / PI;
  double Timeq = 1440 - korj;
  if(Timeq > 1400) Timeq = Timeq - 1440;
  return Timeq;
}

double RiseSet(double f0, double tzone, double longit, double dlstime, double timeq) {
  return 12.0 + 12.0 * f0 / PI + tzone - longit / 15.0 + timeq / 60.0 + dlstime;
}

double f0(double declin, double latit) {
  double refract =  PI * 17.0 / 30.0 / 180.0;
  double Sundia = PI * 4.0 / 15.0 / 180.0;
  
  double F0 = 0;
  double x1 = tan(declin + 0.5 * Sundia + refract) * tan(PI * latit / 180.0);
  if(x1 >= 1) F0 = PI;
  if((x1 > -1) && (x1 < 1)) F0 = PI / 2.0 + atan(x1 / sqrt(1.0 - x1 * x1));
  return F0;
}

Date getDate(TCHAR *prompt) {
  for(;;) {
    String s = inputString(_T("%s"), prompt);
    try {
      Date dato(s);
      return dato;
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
    }
  }
}

void dms(double x, int &hour, int &minutes) {
  double rest;
  hour = (int)floor(x);
  rest = x - hour;
  rest *= 100;
  double dec = floor(rest + 0.5);
  rest -= dec;
  dec /= 100;
  dec *= 60;
  minutes = (int)floor(dec);
}

double daylength(const Date &d, double latitude, double longitude, double timezone) {
  int J = d - Date(_T("1.1.2000"));
  int sommertid = 0;

  double K = delta(J);
  double L = timeq(J);
  double B = d.getDayOfYear();
  double C = f0(K,latitude);
  double D = RiseSet(-C,timezone,longitude,sommertid,L);
  double F = RiseSet( C,timezone,longitude,sommertid,L);
  double LEN = F - D;

  int hr_op,mn_op,hr_ned,mn_ned,hr_len,mn_len;
  dms(D,hr_op,mn_op);
  dms(F,hr_ned,mn_ned);
  dms(LEN,hr_len,mn_len);

//  printf("Solopgang:%lf Solnedgang:%lf\n",D,F);
  _tprintf(_T("%s %2d:%02d %2d:%02d %2d:%02d %lf\n")
          ,d.toString().cstr(),hr_op,mn_op,hr_ned,mn_ned,hr_len,mn_len,LEN);

  return LEN;
}

void plot(int year, double latitude, double longitude, double timezone) {
  for(Date d(1,1,year); d.getYear() == year; d += 1) {
    daylength(d,latitude,longitude,timezone);
//    printf("%d %lf\n",D,);
  }
}

static void usage() {
  _ftprintf(stderr,_T("usage:sun [-platitude,longitude] [-ztimezone] [-yyear|-ddate]\n"));
  exit(-1);
}

typedef enum {
  COMMAND_WHOLEYEAR
 ,COMMAND_DATE
} COMMAND;

int _tmain(int argc, TCHAR **argv) {
  Date d;
  int year;
  double latitude  = 56.15; 
  double longitude = 10.22; 
  double timezone  = 1;
  COMMAND cmd = COMMAND_DATE;
  TCHAR *cp;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++;*cp;cp++) {
        switch(*cp) {
        case 'd':
          cp++;
          if(!*cp) usage();
          d = Date(cp);
          cmd = COMMAND_DATE;
          break;
        case 'y':
          if(_stscanf(cp+1,_T("%d"),&year) != 1)
            usage();
          cmd = COMMAND_WHOLEYEAR;
          break;
        case 'p':
          if(_stscanf(cp+1,_T("%lf,%lf"), &latitude, &longitude) != 2)
            usage();
          break;
        case 'z':
          if(_stscanf(cp+1,_T("%lf"), &timezone) != 1) usage();
          break;
        default:
          usage();
        }
        break;
      }
    }

    switch(cmd) {
    case COMMAND_DATE:
      daylength(d,latitude,longitude,timezone);
      break;
    case COMMAND_WHOLEYEAR:
      plot(year,latitude,longitude,timezone);
      break;
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("%s\n"), e.what());  
    return -1;
  }
  return 0;
}
