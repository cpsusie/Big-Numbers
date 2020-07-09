#pragma once

#define USE_SNAPMC

#if !defined(USE_SNAPMC)
#include "IsoSurfacePolygonizerStandard.h"
using namespace ISOSURFACE_POLYGONIZER_STANDARD;
#else
#include "IsoSurfacePolygonizerSnapMC.h"
using namespace ISOSURFACE_POLYGONIZER_SNAPMC;
#endif //  USE_SNAPMC
