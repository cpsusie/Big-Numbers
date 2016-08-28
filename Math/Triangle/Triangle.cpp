// Triangle.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Math/Triangle/Triangle.h>

int main(int argc, char **argv) {
  try {
    return triangleMain(argc, argv);
  }
  catch (Exception e) {
    _ftprintf(stderr, _T("Exception:%s\n"), e.what());
    return -1;
  }
}
