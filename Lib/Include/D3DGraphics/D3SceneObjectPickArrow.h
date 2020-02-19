#pragma once

#include "D3SceneObjectLineArrow.h"

class D3PickRayArrow : public D3SceneObjectLineArrow {
public:
  D3PickRayArrow(D3Scene &scene, const D3Ray &ray);
};
