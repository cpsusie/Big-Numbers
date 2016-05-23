#include "stdafx.h"
#include <Array.h>
#include <Math.h>
#include <time.h>
#include <Date.h>
#include "TestDate.h"

static void getDate(int &dd, int &mm, int &yyyy) {
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("Enter date (dd.mm.yyyy):"));
    GETS(line);
    if(_stscanf(line,_T("%d.%d.%d"), &dd,&mm,&yyyy) == 3) {
      return;
    }
  }
}

static void getTime(int &hh, int &mm, int &ss, int &ms) {
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("Enter time (hh:mm:ss.ms):"));
    GETS(line);
    if(_stscanf(line,_T("%d:%d:%d.%d"), &hh,&mm,&ss,&ms) == 4) {
      return;
    }
  }
}

static void getTimestamp(int &dd, int &MM, int &yy, int &hh, int &mm, int &ss, int &ms) {
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("Enter Timestamp (dd.MM.yyyy hh:mm:ss.ms):"));
    GETS(line);
    if(_stscanf(line, _T("%d.%d.%d %d:%d:%d.%d"), &dd,&MM,&yy,&hh,&mm,&ss,&ms) == 7) {
      return;
    }
  }
}


static void interactiveTestWeek() {
  for(;;) {
    int dd,mm,yyyy;
    getDate(dd,mm,yyyy);
    Date d(dd,mm,yyyy);
    _tprintf(_T("uge %d\n"),d.getWeek());
  }
}

static void interactiveTestTimestampAdd() {
  Timestamp t;//("31.12.1999 23:59:59");
  for(;;) {
    try {
      _tprintf(_T("t:%s\n"), t.toString().cstr());
      int c = inputInt(_T("0=second, 1=minute, 2=hour, 3=day, 4=month, 5=year, -1=quit:"));
      if(c == -1) break;
      int count = inputInt(_T("antal:"));
      t.add((TimeComponent)c,count);
    } catch(Exception e) {
      _tprintf(_T("%s\n"), e.what());
    }
  }
}

static void interactiveTestDateIO() {
  for(;;) {
    TCHAR line[100];
    _tprintf(_T("Enter Date:"));
    GETS(line);
    try {
      Date d(line);
      Date d1 = d;
      printdate(d);
      printf("\n");
    } catch(Exception e) {
      _tprintf(_T("error:%s\n"),e.what());
    }
  }
}

static void interactiveTestTimestamp() {
  for(;;) {
    try {
      int dd1,MM1,yy1,hh1,mm1,ss1,ms1;
      int dd2,MM2,yy2,hh2,mm2,ss2,ms2;
      getTimestamp(dd1,MM1,yy1,hh1,mm1,ss1,ms1);
      Timestamp d1(dd1,MM1,yy1,hh1,mm1,ss1,ms1);
      d1.getDMY(dd2,MM2,yy2);
      d1.getHMS(hh2,mm2,ss2,ms2);
      if(dd1 != dd2 || MM1 != MM2 || yy1 != yy2 || hh1 != hh2 || mm1 != mm2 || ss1 != ss2) {
        _tprintf(_T("Error in getHMS or getDMY\n"));
      }
      getTimestamp(dd2,MM2,yy2,hh2,mm2,ss2,ms2);
      Timestamp d2(dd2,MM2,yy2,hh2,mm2,ss2,ms2);
      double dist = d2 - d1;
      _tprintf(_T("Distance:%lf\n"), dist);
      d2 = d1 + (int)dist;
      d2.getDMY(dd2,MM2,yy2);
      d2.getHMS(hh2,mm2,ss2,ms2);
      _tprintf(_T("%d.%d.%d + %lf = %d.%d.%d\n"),hh1,mm1,ss1,dist,hh2,mm2,ss2);
      _tprintf(_T("toString:<%s>\n"),d2.toString().cstr());
      Timestamp ff(d2.toString().cstr());
      _tprintf(_T("toString:<%s>\n"), ff.toString().cstr());
      ff = ff - 3600;
      _tprintf(_T("toString:<%s>\n"), ff.toString().cstr());
    } catch(Exception e) {
      _tprintf(_T("caught Exception:%s\n"), e.what());
    }
  }
}

static void interactiveTestTime() {
  for(;;) {
    try {
      int hh1,mm1,ss1,ms1;
      int hh2,mm2,ss2,ms2;
      getTime(hh1,mm1,ss1,ms1);
      Time d1(hh1,mm1,ss1,ms1);
      d1.getHMS(hh2,mm2,ss2,ms2);
      if(hh1 != hh2 || mm1 != mm2 || ss1 != ss2 || ms1 != ms2) {
        _tprintf(_T("Error in  getHMS\n"));
      }
      getTime(hh2,mm2,ss2,ms2);
      Time d2(hh2,mm2,ss2,ms2);
      int dist = d2 - d1;
      _tprintf(_T("Distance:%d\n"), dist);
      d2 = d1 + dist;
      d2.getHMS(hh2,mm2,ss2,ms2);
      _tprintf(_T("%d.%d.%d.%d + %d = %d.%d.%d.%d\n"),hh1,mm1,ss1,ms1,dist,hh2,mm2,ss2,ms2);
      _tprintf(_T("toString:<%s>\n"), d2.toString().cstr());
      Time ff(d2.toString().cstr());
      _tprintf(_T("tostr:<%s>\n"), ff.toString().cstr());
      ff = ff - 3600;
      _tprintf(_T("tostr:<%s>\n"), ff.toString().cstr());
    } catch(Exception e) {
      _tprintf(_T("Caught Exception:%s\n"), e.what());
    }
  }
}

static void interactiveTestDate() {
  for(;;) {
    try {
      int dd1,mm1,yyyy1;
      int dd2,mm2,yyyy2;
      getDate(dd1,mm1,yyyy1);
      Date d1(dd1,mm1,yyyy1);
      d1.getDMY(dd2,mm2,yyyy2);
      if(dd1 != dd2 || mm1 != mm2 || yyyy1 != yyyy2) {
        _tprintf(_T("Error in getmdy\n"));
      }
      getDate(dd2,mm2,yyyy2);
      Date d2(dd2,mm2,yyyy2);
      int dist = d2 - d1;
      _tprintf(_T("Distance:%d\n"),dist);
      d2 = d1 + dist;
      d2.getDMY(dd2,mm2,yyyy2);
      _tprintf(_T("%d.%d.%d + %d = %d.%d.%d\n"), dd1,mm1,yyyy1,dist,dd2,mm2,yyyy2);
      _tprintf(_T("%s %s\n"), d1.toString().cstr(),Date::daynames[d1.getWeekDay()]);
      _tprintf(_T("%s %s\n"), d2.toString().cstr(),Date::daynames[d2.getWeekDay()]);
    } catch(Exception e) {
      _tprintf(_T("Caught Exception:%s\n"), e.what());
    }
  }
}


static void testmktime() {
  struct tm tm,*tmp;
  time_t now;
  time(&now);
  tmp = localtime(&now);
  tm = *tmp;
  tm.tm_mday++;


  time_t tt = mktime(&tm);
  tmp = localtime(&tt);
}


static void usage() {
  fprintf(stderr,"Usage testdate [-xwistd]\n"
                 "  -x:testsuite\n"
                 "  -w:interactive test of weeknumbers\n"
                 "  -i:interactive test of dateio\n"
                 "  -s:interactive test of Timestamp\n"
                 "  -t:interactive test of Time\n"
                 "  -d:interactive test of Date\n"
                 "  -a:interactive test of add\n"
         );

  exit(-1);
}

int main(int argc, char **argv) { 

  testDate();
  return 0;

//  testmktime();


  char *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++)
    for(cp++;*cp; cp++) {
      switch(*cp) {
      case 'x':
        break;
      case 'w':
        interactiveTestWeek();
        break;
      case 'i':
        interactiveTestDateIO();
        break;
      case 's':
        interactiveTestTimestamp();
        break;
      case 't':
        interactiveTestTime();
        break;
      case 'd':
        interactiveTestDate();
        break;
      case 'a':
        interactiveTestTimestampAdd();
        break;
      default:
        usage();
      }
    }

  return 0;
}
