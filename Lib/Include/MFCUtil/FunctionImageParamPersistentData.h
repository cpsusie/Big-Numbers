#pragma once

#include <PersistentDataTemplate.h>
#include <Math/MathLib.h>

class FunctionImageParamPersistentData : public PersistentDataTemplate<FunctionDomainRangeDimension> {
};

#define TYPE_ISOCURVE              FunctionDomainRangeDimension(2, 1)
#define TYPE_FUNCTIONR2R1SURFACE   FunctionDomainRangeDimension(2, 1)
#define TYPE_ISOSURFACE            FunctionDomainRangeDimension(3, 1)
#define TYPE_PARAMETRICR1R3SURFACE FunctionDomainRangeDimension(1, 3)
#define TYPE_PARAMETRICR2R3SURFACE FunctionDomainRangeDimension(2, 3)
