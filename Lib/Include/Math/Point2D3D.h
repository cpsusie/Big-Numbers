#pragma once

#include "Point2D.h"
#include "Point3D.h"

typedef FunctionTemplate<Point2D, Point3D> FunctionR2R3;
typedef FunctionTemplate<Point3D, Point2D> FunctionR3R2;
