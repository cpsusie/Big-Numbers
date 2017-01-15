#pragma once

#include "IsoMesher.h"

namespace ThreeD {

/* IsoMesher, Dual Contour */
class IsoMesherDC : public IsoMesher {
private:
  static int s_edgeTable[256];

protected:

  typedef Mesh::MeshPoint MeshPoint;

  struct Cube {
    int        index;
    MeshPoint *meshPoint;
#ifdef USE_MATERIAL
    Material   mat;
#endif
  };

  struct Row {
    D3DXVECTOR3 v;
    D3DXVECTOR3 *points;
    float       *densities;
    Cube        *cubes;
  };

  int m_xsize;
  int m_zsize;

  /* Compute a row (y-slice) of grid points */
  bool computePoints(Row *row);

  /* Compute a row (y-slice) of cubes (voxels) */
  bool computeCubes(Row *rows[2]);

  /* Generate a new (QEF-minimizing) vertex */
  void generateVertex(Cube *cube, Point corners[8]);

  /* Generate a quad for voxels sharing an edge */
  bool generateQuads(Row *rows[2]);

public:
  IsoMesherDC(Isosurface &iso);
  Mesh *createMesh();
};

} // namespace ThreeD
