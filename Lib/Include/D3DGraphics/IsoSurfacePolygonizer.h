#pragma once

#define USE_SNAPMC

#ifndef USE_SNAPMC
#include <D3DGraphics/IsoSurfacePolygonizerStandard.h>
using namespace ISOSURFACE_POLYGONIZER_STANDARD;
#else
#include <D3DGraphics/IsoSurfacePolygonizerSnapMC.h>
using namespace ISOSURFACE_POLYGONIZER_SNAPMC;
#endif //  USE_SNAPMC
