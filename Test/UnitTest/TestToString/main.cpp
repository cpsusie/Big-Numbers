#include "stdafx.h"
#include "TestToString.h"

using namespace std;

int main(int argc, char **argv) {
  try {
    testToString();
  } catch(Exception e) {
    tcout << _T("Exception:") << e.what() << endl;
    return -1;
  }
  return 0;
}
