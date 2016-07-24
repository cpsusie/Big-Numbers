#include "stdafx.h"
#include "Calculator.h"

BigReal dms(const BigReal &x, const BigReal &f) {
  BigReal h,rest;
  bool negative = false;
  if(x < 0) {
    BigReal xp(-x);
    h = floor(xp);
    rest = xp - h;
    negative = true;
  }
  else {
    h = floor(x);
    rest = x - h;
  }
  rest.multPow10(2);
  BigReal dec = floor(rest);
  rest -= dec;
  dec.multPow10(-2);
  dec = dec * 60;
  BigReal m = floor(dec);
  BigReal s = (dec - m) * 60;
//  cout << "m:" << m << "\n"; cout.flush();
//  cout << "s:" << s << "\n"; cout.flush();
  for(int i = 1; !rest.isZero(); i += 1) {
    rest.multPow10(1);
    BigReal dec = floor(rest);
    rest -= dec;
    dec.multPow10(-i);
//    cout << "dec:" << dec << "\n"; cout.flush();
    s += dec * 36;
//    cout << "s:" << s << "\n"; cout.flush();
//    pause();
  }
  s = round(s,-BigReal::getExpo10(f)-4);
  if(s >= 60) {
    ++m;
    s -= 60;
  }
  if(m >= 60) {
    ++h;
    m -= 60;
  }
  s.multPow10(-4);
  m.multPow10(-2);
//  cout << "m:" << m << "\n"; cout.flush();
//  cout << "s:" << s << "\n"; cout.flush();
  BigReal result(h + m + s); 
  return negative ? -result : result;
}

BigReal inversdms(const BigReal &x, const BigReal &f) {
  BigReal  h,rest;
  bool negative = false;
  if(x < 0) {
    BigReal xp(-x);
    h = floor(xp);
    rest = xp - h;
    negative = true;
  }
  else {
    h = floor(x);
    rest = x - h;
  }
  rest.multPow10(2);
  BigReal  dec = floor(rest);
  rest -= dec;
//  cout << "h   :" << h    << "\n"; cout.flush();
//  cout << "rest:" << rest << "\n"; cout.flush();
//  cout << "dec :" << dec  << "\n"; cout.flush();
  BigReal  m = quot(dec,60,f);
//  cout << "m:" << m << "\n"; cout.flush();
  BigReal  result(h + m); 
//  cout << "result h.mm:" << result << "\n"; cout.flush();
  for(int i = 2; !rest.isZero(); i += 2) {
    rest.multPow10(2);
    BigReal dec = floor(rest);
    rest -= dec;
//  cout << "rest:" << rest << "\n"; cout.flush();
//  cout << "dec :" << dec  << "\n"; cout.flush();
    dec.multPow10(-i);
//    cout << "dec:" << dec << "\n"; cout.flush();
    result += quot(dec,36,f);
//    cout << "result:" << result << "\n"; cout.flush();
  }
  result = round(result, -BigReal::getExpo10(f));
  return negative ? -result : result;
}
