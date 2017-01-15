#pragma once

//#define USE_MATERIAL

#pragma warning(disable: 4786)  // name truncated to 255 chars

#include "misc.h"
#include "BoundingBox.h"
#include "Color.h"
#include "Mesh.h"
#include "MeshFace.h"
#include "Isosurface.h"
#include "CsgIsosurface.h"
#include "IsoMesher.h"

#ifdef _DEBUG
//#pragma message("link with MTDebug/util.lib")
#pragma comment(lib, "c:/MyTools/D3FunctionPlotter/MarchingCube/Debug/MarchingCube.lib")
#else
//#pragma message("link with MTRelease/util.lib")
#pragma comment(lib, "c:/MyTools/D3FunctionPlotter/MarchingCube/Release/MarchingCube.lib")
#endif
