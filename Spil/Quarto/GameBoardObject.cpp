#include "stdafx.h"
#include <D3DGraphics/MeshBuilder.h>
#include "GameBoardObject.h"

DECLARE_THISFILE;

static D3PosDirUpScale s_pdusOrigo;

static void makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3, int texcoor[4]) { // static
  Face &f2 = mb.addFace();
  f2.addVertexTextureIndex(v0,texcoor[0]);
  f2.addVertexTextureIndex(v3,texcoor[1]);
  f2.addVertexTextureIndex(v2,texcoor[2]);
  f2.addVertexTextureIndex(v1,texcoor[3]);
}

static void drawMeshUsingTexture(LPDIRECT3DDEVICE device, LPD3DXMESH mesh, LPDIRECT3DTEXTURE texture) {
  V(device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
	V(device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
  V(device->SetTexture(     0, texture));
  V(device->SetRenderState(    D3DRS_LIGHTING        , FALSE));
  V(device->SetRenderState(    D3DRS_ALPHABLENDENABLE, FALSE));
  V(device->SetRenderState(    D3DRS_ZENABLE         , TRUE )); 
  V(mesh->DrawSubset(0));
}

const float GameBoardObject::s_vertexPos[] = {
  0.0f     , 0.024408f, 0.048816f, 0.073224f, 0.097632f, 0.12204f 
 ,0.146447f, 0.323223f, 0.5f     , 0.676777f, 0.85355f 
 ,0.87796f , 0.902368f, 0.926776f, 0.951184f, 0.97559f , 1.0f
};

float       GameBoardObject::s_gridLines[5];

#define EDGE0INDEX 6
#define EDGE4INDEX 10

class MarkedFieldObject : public SceneObjectWithMesh {
private:
  static LPDIRECT3DTEXTURE s_markTexture;
  LPDIRECT3DTEXTURE getTexture();
  static LPD3DXMESH createMesh(LPDIRECT3DDEVICE device, int row, int col);
public:
  MarkedFieldObject(D3Scene &scene, int row, int col);
  D3DXMATRIX getWorldMatrix() const {
    return s_pdusOrigo.getWorldMatrix();
  }
  void draw();
};

LPDIRECT3DTEXTURE MarkedFieldObject::s_markTexture = NULL;

MarkedFieldObject::MarkedFieldObject(D3Scene &scene, int row, int col)
: SceneObjectWithMesh(scene, createMesh(scene.getDevice(), row, col))
{
}

LPD3DXMESH MarkedFieldObject::createMesh(LPDIRECT3DDEVICE device, int row, int col) {
  MeshBuilder mb;
  const float *gridLines = GameBoardObject::s_gridLines;
  int vindex[2][2];
  int tindex[2][2];
  for(short r = 0; r < 2; r++) {
    for(short c = 0; c < 2; c++) {
      vindex[r][c] = mb.addVertex(gridLines[row+r],0.035f,gridLines[col+c]);
      tindex[r][c] = mb.addTextureVertex(r,c);
    }
  }
  Face &face = mb.addFace();
  face.addVertexTextureIndex(vindex[0][0], tindex[0][0]);
  face.addVertexTextureIndex(vindex[1][0], tindex[1][0]);
  face.addVertexTextureIndex(vindex[1][1], tindex[1][1]);
  face.addVertexTextureIndex(vindex[0][1], tindex[0][1]);
  return mb.createMesh(device, false);
}

LPDIRECT3DTEXTURE MarkedFieldObject::getTexture() {
  if (s_markTexture == NULL) {
    s_markTexture  = loadTextureFromBitmapResource(getDevice(), IDB_MARKEDFIELDBITMAP);
  }
  return s_markTexture;
}

void MarkedFieldObject::draw() {
  drawMeshUsingTexture(getDevice(), getMesh(), getTexture());
}

LPD3DXMESH GameBoardObject::createMesh(LPDIRECT3DDEVICE device) { // static
  MeshBuilder mb;

  int vindex[BOARDSIZE+1][BOARDSIZE+1];
  int tindex[BOARDSIZE+1][BOARDSIZE+1];
  for(int r = 0; r <= BOARDSIZE; r++) {
    for(int c = 0; c <= BOARDSIZE; c++) {
      vindex[r][c] = mb.addVertex(BOARDSIZE*(s_vertexPos[r]-0.5f),0,BOARDSIZE*(s_vertexPos[c]-0.5f));
      tindex[r][c] = mb.addTextureVertex(s_vertexPos[r],s_vertexPos[c]);
    }
  }
  for(int i = 0; i < ARRAYSIZE(s_gridLines); i++) {
    s_gridLines[i] = BOARDSIZE*(s_vertexPos[i+EDGE0INDEX]-0.5f);
  }
  int textureCorners[4];
  textureCorners[0] = tindex[0        ][0        ];
  textureCorners[1] = tindex[BOARDSIZE][0        ];
  textureCorners[2] = tindex[BOARDSIZE][BOARDSIZE];
  textureCorners[3] = tindex[0        ][BOARDSIZE];

  for(int r = 0; r < BOARDSIZE; r++) {
    for(int c = 0; c < BOARDSIZE; c++) {
      Face &face = mb.addFace();
      face.addVertexTextureIndex(vindex[r  ][c  ], tindex[r  ][c  ]);
      face.addVertexTextureIndex(vindex[r+1][c  ], tindex[r+1][c  ]);
      face.addVertexTextureIndex(vindex[r+1][c+1], tindex[r+1][c+1]);
      face.addVertexTextureIndex(vindex[r  ][c+1], tindex[r  ][c+1]);
    }
  }

  const int  lbn = mb.addVertex(-HALFSIZE,-BOARDHEIGHT,-HALFSIZE); // left  bottom near corner
  const int  lbf = mb.addVertex(-HALFSIZE,-BOARDHEIGHT, HALFSIZE); // left  bottom far  corner
  const int  rbf = mb.addVertex( HALFSIZE,-BOARDHEIGHT, HALFSIZE); // right bottom far  corner
  const int  rbn = mb.addVertex( HALFSIZE,-BOARDHEIGHT,-HALFSIZE); // right bottom near corner
  const int &ltn = vindex[0        ][0        ];                   // left  top    near corner
  const int &ltf = vindex[0        ][BOARDSIZE];                   // left  top    far  corner
  const int &rtf = vindex[BOARDSIZE][BOARDSIZE];                   // right top    far  corner
  const int &rtn = vindex[BOARDSIZE][0        ];                   // right top    near corner

  makeSquareFace(mb,lbn,rbn,rbf,lbf, textureCorners);              // bottom
  makeSquareFace(mb,lbn,lbf,ltf,ltn, textureCorners);              // left side
  makeSquareFace(mb,lbf,rbf,rtf,ltf, textureCorners);              // back side
  makeSquareFace(mb,rbf,rbn,rtn,rtf, textureCorners);              // right side
  makeSquareFace(mb,rbn,lbn,ltn,rtn, textureCorners);              // front side

  try {
    return mb.createMesh(device, false);
  } catch (...) {
    mb.dump();
    throw;
  }
}

GameBoardObject::GameBoardObject(D3Scene &scene) 
: SceneObjectWithMesh(scene, createMesh(scene.getDevice()))
{
  m_boardTexture = loadTextureFromBitmapResource(getDevice(), IDB_BOARDBITMAP      );
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      MarkedFieldObject *mfo = new MarkedFieldObject(scene, r, c);
      m_markFieldObject[r][c] = mfo;
      m_scene.addSceneObject(mfo);
      mfo->setVisible(false);
    }
  }
  m_currentField.m_row = m_currentField.m_col = -1;
}

GameBoardObject::~GameBoardObject() {
  if(m_boardTexture) m_boardTexture->Release();
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      if(m_markFieldObject[r][c]) {
        delete m_markFieldObject[r][c];
      }
    }
  }
}

D3DXMATRIX GameBoardObject::getWorldMatrix() const {
  return s_pdusOrigo.getWorldMatrix();
}

void GameBoardObject::draw() {
  drawMeshUsingTexture(getDevice(), getMesh(), m_boardTexture);
}

void GameBoardObject::markField(const Field &f) {
  if(!f.isValid()) return;
  unmarkCurrent();
  m_currentField = f;
  setCurrentVisible(true);
}

void GameBoardObject::unmarkCurrent() {
  if(!hasCurrentField()) return;
  setCurrentVisible(false);
  m_currentField.m_row = m_currentField.m_col = -1;
}

void GameBoardObject::setCurrentVisible(bool visible) {
  m_markFieldObject[m_currentField.m_row][m_currentField.m_col]->setVisible(visible);
}

Field GameBoardObject::getFieldFromPoint(const CPoint &p) const {
  D3DXVECTOR3  hitPoint;
  D3SceneObject *obj = m_scene.getPickedObject(p, PICK_ALL, &hitPoint);
  if(obj != this) {
    return NOFIELD;
  }
  for (int r = 0; r < ROWCOUNT; r++) {
    if((s_gridLines[r] <= hitPoint.x) && (hitPoint.x <= s_gridLines[r+1])) {
      for(int c = 0; c < COLCOUNT; c++) {
        if((s_gridLines[c] <= hitPoint.y) && (hitPoint.y <= s_gridLines[c+1])) {
          Field result;
          result.m_row = r;
          result.m_col = c;
          return result;
        }
      }
    }
  }
  return NOFIELD;
}
