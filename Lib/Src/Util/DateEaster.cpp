#include "pch.h"
#include <Date.h>

// se "Beginning ATL COM programming" s. 78
Date Date::getEaster(int year) { // static
  if(year < 326 || year > 9999) {
    throwException(_T("Easter not defined for year=%d. Year must be in the range [326..9999]"), year);
  }

  short first = year / 100;
  short div19 = year % 19;

  short temp = (first - 15) / 2 + ((first > 26) ? -1 : 0) +
    ((first > 38) ? -1 : 0) + 202 - 11 * div19;

  if(first == 21 || first == 24 || first == 25 ||
     first == 33 || first == 36 || first == 37)
     temp += -1;

  temp %= 30;

  short ta = temp + ((temp == 29) ? -1 : 0) +
           ((temp == 28 && div19 > 10) ? -1 : 0) + 21;

  short tb = (ta - 19) % 7;
  temp = (40 - first) % 4;
  short tc = temp - ((temp > 1) ? -1 : 0) - ((temp == 3) ? -1 : 0);
  temp = year % 100;
  short td = (temp + temp / 4) % 7;
  short te = ((20 - tb - tc - td) % 7) + 1;

  int day = ta + te;
  int month;

  if(day > 61) {
    day -= 61;
    month = 5;
  } else if(day > 31) {
    day -= 31;
    month = 4;
  } else {
    month = 3;
  }
  return Date(day, month, year);
}
