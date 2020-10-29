#pragma once

#include <NumberInterval.h>
#include "CubeTemplate.h"
#include "Point3D.h"

template<typename PointType, typename SizeType, typename T> class Cube3DTemplate : public CubeTemplate<PointType, SizeType, T, 3> {
public:
  inline Cube3DTemplate() {
  }

  template<typename X, typename Y, typename Z, typename W, typename D, typename H> Cube3DTemplate(const X &x, const Y &y, const Z &z, const W &w, const D &d, const H &h)
    : CubeTemplate(PointType(x,y,z), SizeType(w,d,h))
  {
  }
  template<typename PT, typename ST, typename S> Cube3DTemplate(const CubeTemplate<PT,ST,S, 3> &src)
    : CubeTemplate(src)
  {
  }
  template<typename T1, typename T2> Cube3DTemplate(const PointTemplate<T1, 3> &lbn, const PointTemplate<T2, 3> &rtf)
    : CubeTemplate(lbn, rtf-lbn)
  {
  }
  template<typename P, typename S> Cube3DTemplate(const PointTemplate<P, 3> &p0, const SizeTemplate<S, 3> &size)
    : CubeTemplate(p0, size)
  {
  }

  inline const T                &getX()         const { return p0()[0];                                       }
  inline const T                &getY()         const { return p0()[1];                                       }
  inline const T                &getZ()         const { return p0()[2];                                       }
  inline const T                &getWidth()     const { return size()[0];                                     }
  inline const T                &getDepth()     const { return size()[1];                                     }
  inline const T                &getHeight()    const { return size()[2];                                     }
  inline const T                &getLeft()      const { return getX();                                        }
  inline       T                 getRight()     const { return getLeft()   + getWidth();                      }
  inline const T                &getNear()      const { return getY();                                        }
  inline       T                 getFar()       const { return getNear()   + getDepth();                      }
  inline const T                &getBottom()    const { return getZ();                                        }
  inline       T                 getTop()       const { return getBottom() + getHeight();                     }

  inline       PointType         LBN()          const { return p0();                                          }
  inline       PointType         RBN()          const { return PointType(getRight(), getNear(), getBottom()); }
  inline       PointType         LBF()          const { return PointType(getLeft() , getFar() , getBottom()); }
  inline       PointType         RBF()          const { return PointType(getRight(), getFar() , getBottom()); }
  inline       PointType         LTN()          const { return PointType(getLeft() , getNear(), getTop()   ); }
  inline       PointType         RTN()          const { return PointType(getRight(), getNear(), getTop()   ); }
  inline       PointType         LTF()          const { return PointType(getLeft() , getFar() , getTop()   ); }
  inline       PointType         RTF()          const { return LBN() + size();                                }

  inline       T                 getMinX()      const { return __super::getMin(0);                            }
  inline       T                 getMaxX()      const { return __super::getMax(0);                            }
  inline       T                 getMinY()      const { return __super::getMin(1);                            }
  inline       T                 getMaxY()      const { return __super::getMax(1);                            }
  inline       T                 getMinZ()      const { return __super::getMin(2);                            }
  inline       T                 getMaxZ()      const { return __super::getMax(2);                            }

  inline       NumberInterval<T> getXInterval() const { return getInterval(0);                                }
  inline       NumberInterval<T> getYInterval() const { return getInterval(1);                                }
  inline       NumberInterval<T> getZInterval() const { return getInterval(2);                                }
};

typedef Cube3DTemplate<    FloatPoint3D   , FloatSize3D, float  > FloatCube3D;
typedef Cube3DTemplate<    Point3D        , Size3D     , double > Cube3D;
typedef Cube3DTemplate<    RealPoint3D    , RealSize3D , Real   > RealCube3D;

typedef PointArrayTemplate<FloatPoint3D   , 3                   > FloatPoint3DArray;
typedef PointArrayTemplate<Point3D        , 3                   > Point3DArray;
typedef PointArrayTemplate<RealPoint3D    , 3                   > RealPoint3DArray;

typedef PointRefArrayTemplate<FloatPoint3D, 3                   > FloatPoint3DRefArray;
typedef PointRefArrayTemplate<Point3D     , 3                   > Point3DRefArray;
typedef PointRefArrayTemplate<RealPoint3D , 3                   > RealPoint3DRefArray;
