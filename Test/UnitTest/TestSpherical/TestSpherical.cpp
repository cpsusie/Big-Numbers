#include "stdafx.h"
#include <Math/Spherical.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestSpherical {		

#include <UnitTestTraits.h>

  TEST_CLASS(TestSpherical)	{
    public:

    TEST_METHOD(SphericalTest) {
      RealPoint3D v;
      for(v[0] = -2; v[0] < 2; v[0] += 0.1f) {
        for(v[1] = -2; v[1] < 2; v[1] += 0.1f) {
          for(v[2] = -2; v[2] < 2; v[2] += 0.1f) {
            const RealSpherical sph(v);
            const RealPoint3D   v1   = sph;
            const RealPoint3D   diff = v - v1;
            const Real          dist = diff.length();
            if(dist >= 1e-5) {
              OUTPUT(_T("dist:%le, v:(%f,%f,%f) -> sph:(%f,%f,%f) -> v1:(%f,%f,%f)")
                    ,dist
                    ,v[0],v[1],v[2]
                    ,sph.r, RAD2GRAD(sph.fi), RAD2GRAD(sph.theta)
                    ,v1[0],v1[1],v1[2]);
            }
            verify(dist < 1e-5);
          }
        }
      }
    }
  };
}
