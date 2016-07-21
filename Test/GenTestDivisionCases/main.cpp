#include <MyUtil.h>
#include <Math/int128.h>

String toDecString(const _uint128 &x) {
  TCHAR s[200];
  _ui128tot(x, s, 10);
  return s;
}

int main(int argc, char **argv) {
  FILE *f = f = FOPEN(_T("\\temp\\testDivision.txt"), _T("w"));

  for (int i = 0; i < 20; i++) {
    _uint128 x(randInt64(0x1000000000000000), randInt64());
    _uint128 y(randInt64(0x0000001000000000), randInt64());
    if (i < 10) y.s2.i[1] = 0;
    _uint128 Q = x / y;
    _uint128 R = x % y;
    _ftprintf(f, _T("  TEST_METHOD(TestDivison%02d) {\n"), i);
    _ftprintf(f, _T("    const _uint128 x(        %#18I64x,%#018I64x); // %40s\n"), x.s2.i[1], x.s2.i[0], toDecString(x).cstr());
    _ftprintf(f, _T("    const _uint128 y(        %#18I64x,%#018I64x); // %40s\n"), y.s2.i[1], y.s2.i[0], toDecString(y).cstr());
    _ftprintf(f, _T("    const _uint128 expectedQ(%#18I64x,%#018I64x); // %40s\n"), Q.s2.i[1], Q.s2.i[0], toDecString(Q).cstr());
    _ftprintf(f, _T("    const _uint128 expectedR(%#18I64x,%#018I64x); // %40s\n"), R.s2.i[1], R.s2.i[0], toDecString(R).cstr());
    _ftprintf(f, _T("    const _uint128 Q = x / y;\n"));
    _ftprintf(f, _T("    const _uint128 R = x %% y;\n"));
    _ftprintf(f, _T("    verify(Q == expectedQ);\n"));
    _ftprintf(f, _T("    verify(R == expectedR);\n"));
    _ftprintf(f, _T("  }\n\n"));
  }
  fclose(f);
  return 0;
}
