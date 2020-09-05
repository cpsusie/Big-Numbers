#include <iostream>

using namespace std;

int main(int argc, char **argv) {
  double x = 1.2345678901234568e-10;

  cout.precision(16);
  cout << "dec(16):x=\"" << scientific << x << "\"" << endl;
  cout.precision(13);
  cout << "hex(13):x=\"" << hexfloat << x << "\"" << endl;
  cout.precision(11);
  cout << "hex(11):x=\"" << hexfloat << x << "\"" << endl;

  return 0;
}
