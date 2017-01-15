#pragma once

#include "IsoMesher.h"

namespace ThreeD {

class IsoMesherMC : public IsoMesher {
private:
  static int s_edgeTable[256];
  static int s_triTable[256][16];

protected:

  typedef Mesh::MeshPoint MeshPoint;

  struct Row {
    D3DXVECTOR3 *points;
    float       *densities;
  };

  int m_xsize;
  int m_zsize;

  /* Compute a row (y-slice) of grid points */
  bool computeRow(const D3DXVECTOR3 &vRow, Row *row);

  /* Perform marching cubes on two adjacent voxel slices */
  bool marchCubes(Row *rows[2]);

  /* Generate triangles within a voxel */
  void generateFaces(Point corners[8], int index);

public:
  IsoMesherMC(Isosurface &iso);
  Mesh *createMesh();

};

} // namespace ThreeD
