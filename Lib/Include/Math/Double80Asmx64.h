#pragma once

class Double80;

extern "C" {
void   D80FromI16(        Double80 &dst, const short    &x  );
void   D80FromUI16(       Double80 &dst, USHORT          x  );
void   D80FromI32(        Double80 &dst, const int      &x  );
void   D80FromUI32(       Double80 &dst, UINT            x  );
void   D80FromI64(        Double80 &dst, const INT64    &x  );
void   D80FromUI64(       Double80 &dst, UINT64          x  );
void   D80FromFlt(        Double80 &dst, const float    &x  );
void   D80FromDbl(        Double80 &dst, const double   &x  );
int    D80ToI32(          const Double80 &x);
UINT   D80ToUI32(         const Double80 &x);
INT64  D80ToI64(          const Double80 &x);
UINT64 D80ToUI64(         const Double80 &x);
float  D80ToFlt(          const Double80 &x);
double D80ToDbl(          const Double80 &x);
char   D80cmpI16(         const Double80 &x, const short    &y); // return sign(x-y) or 2 if x is nan
char   D80cmpUI16(        const Double80 &x, USHORT          y); // return sign(x-y) or 2 if x is nan
char   D80cmpI32(         const Double80 &x, const int      &y); // return sign(x-y) or 2 if x is nan
char   D80cmpUI32(        const Double80 &x, UINT            y); // return sign(x-y) or 2 if x is nan
char   D80cmpI64(         const Double80 &x, const INT64    &y); // return sign(x-y) or 2 if x is nan
char   D80cmpUI64(        const Double80 &x, UINT64          y); // return sign(x-y) or 2 if x is nan
char   D80cmpFlt(         const Double80 &x, const float    &y); // return sign(x-y) or 2 if x is nan
char   D80cmpDbl(         const Double80 &x, const double   &y); // return sign(x-y) or 2 if x is nan
char   D80cmpD80(         const Double80 &x, const Double80 &y); // return sign(x-y) or 2 if x is nan
BYTE   D80isZero(         const Double80 &x);                    // return 1 if x == 0, 0 if x != 0
void   D80addI16(         Double80 &dst, const short    &x);
void   D80subI16(         Double80 &dst, const short    &x);
void   D80subrI16(        Double80 &dst, const short    &x);
void   D80mulI16(         Double80 &dst, const short    &x);
void   D80divI16(         Double80 &dst, const short    &x);
void   D80divrI16(        Double80 &dst, const short    &x);
void   D80addUI16(        Double80 &dst, USHORT          x);
void   D80subUI16(        Double80 &dst, USHORT          x);
void   D80subrUI16(       Double80 &dst, USHORT          x);
void   D80mulUI16(        Double80 &dst, USHORT          x);
void   D80divUI16(        Double80 &dst, USHORT          x);
void   D80divrUI16(       Double80 &dst, USHORT          x);
void   D80addI32(         Double80 &dst, const int      &x);
void   D80subI32(         Double80 &dst, const int      &x);
void   D80subrI32(        Double80 &dst, const int      &x);
void   D80mulI32(         Double80 &dst, const int      &x);
void   D80divI32(         Double80 &dst, const int      &x);
void   D80divrI32(        Double80 &dst, const int      &x);
void   D80addUI32(        Double80 &dst, UINT            x);
void   D80subUI32(        Double80 &dst, UINT            x);
void   D80subrUI32(       Double80 &dst, UINT            x);
void   D80mulUI32(        Double80 &dst, UINT            x);
void   D80divUI32(        Double80 &dst, UINT            x);
void   D80divrUI32(       Double80 &dst, UINT            x);
void   D80addI64(         Double80 &dst, const INT64    &x);
void   D80subI64(         Double80 &dst, const INT64    &x);
void   D80subrI64(        Double80 &dst, const INT64    &x);
void   D80mulI64(         Double80 &dst, const INT64    &x);
void   D80divI64(         Double80 &dst, const INT64    &x);
void   D80divrI64(        Double80 &dst, const INT64    &x);
void   D80addUI64(        Double80 &dst, UINT64          x);
void   D80subUI64(        Double80 &dst, UINT64          x);
void   D80subrUI64(       Double80 &dst, UINT64          x);
void   D80mulUI64(        Double80 &dst, UINT64          x);
void   D80divUI64(        Double80 &dst, UINT64          x);
void   D80divrUI64(       Double80 &dst, UINT64          x);
void   D80addFlt(         Double80 &dst, const float    &x);
void   D80subFlt(         Double80 &dst, const float    &x);
void   D80subrFlt(        Double80 &dst, const float    &x);
void   D80mulFlt(         Double80 &dst, const float    &x);
void   D80divFlt(         Double80 &dst, const float    &x);
void   D80divrFlt(        Double80 &dst, const float    &x);
void   D80addDbl(         Double80 &dst, const double   &x);
void   D80subDbl(         Double80 &dst, const double   &x);
void   D80subrDbl(        Double80 &dst, const double   &x);
void   D80mulDbl(         Double80 &dst, const double   &x);
void   D80divDbl(         Double80 &dst, const double   &x);
void   D80divrDbl(        Double80 &dst, const double   &x);
void   D80addD80(         Double80 &dst, const Double80 &x);
void   D80subD80(         Double80 &dst, const Double80 &x);
void   D80mulD80(         Double80 &dst, const Double80 &x);
void   D80divD80(         Double80 &dst, const Double80 &x);
void   D80rem(            Double80 &dst, const Double80 &x);
void   D80neg(            Double80 &x);
void   D80inc(            Double80 &x);
void   D80dec(            Double80 &x);
int    D80getExpo10(      const Double80 &x);
void   D80fabs(           Double80 &x);
void   D80sqr(            Double80 &x);
void   D80sqrt(           Double80 &x);
void   D80sin(            Double80 &x);
void   D80cos(            Double80 &x);
void   D80tan(            Double80 &x);
void   D80atan(           Double80 &x);
void   D80atan2(          Double80 &y, const Double80 &x);
// inout is c, out s
void   D80sincos(         Double80 &c, Double80       &s);
void   D80exp(            Double80 &x);
void   D80exp10(          Double80 &x);
void   D80exp2(           Double80 &x);
void   D80log(            Double80 &x);
void   D80log10(          Double80 &x);
void   D80log2(           Double80 &x);
// x = pow(x,y)
void   D80pow(            Double80 &x, const Double80 &y);
// dst = 2^p
void   D80pow2(           Double80 &dst, const int &p);
void   D80floor(          Double80 &x);
void   D80ceil(           Double80 &x);
}
