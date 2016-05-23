#include "stdafx.h"
#include <MyUtil.h>
#include "TestPackedArray.h"

int main(int argc, char **argv) {
  try {
    testPackedArray();
  } catch(Exception e) {
    printf("Exception:%s\n", e.what());
    return -1;
  }
  return 0;
}
