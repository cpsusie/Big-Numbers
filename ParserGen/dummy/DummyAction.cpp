/* DO NOT EDIT THIS FILE - it is machine generated */

#line 32 "C:\\Mytools2015\\ParserGen\\dummy\\Dummy.y"
#include "stdafx.h"
#include "dummyparser.h"
#line 20 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
#line 23 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
#pragma warning(disable : 4060) // warning C4060: switch statement contains no 'case' or 'default' labels

int DummyParser::reduceAction(unsigned int prod) {
  switch(prod) {
  case 1: /* S -> a S */
#line 39 "C:\\Mytools2015\\ParserGen\\dummy\\Dummy.y"
    { printf("reduce by S -> a S\n"); }
#line 27 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 2: /* S -> S a */
#line 40 "C:\\Mytools2015\\ParserGen\\dummy\\Dummy.y"
    { printf("reduce by S -> S a\n"); }
#line 27 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 3: /* S -> b */
#line 41 "C:\\Mytools2015\\ParserGen\\dummy\\Dummy.y"
    { printf("reduce by S -> b\n");   }
#line 27 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  case 4: /* S -> a */
#line 42 "C:\\Mytools2015\\ParserGen\\dummy\\Dummy.y"
    { printf("reduce by S -> a\n");   }
#line 27 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
    break;
  }
#line 30 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
  return 0;
}

#line 47 "C:\\Mytools2015\\ParserGen\\dummy\\Dummy.y"
#line 33 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"
