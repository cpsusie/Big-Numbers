#include "stdafx.h"
#include "GraphicObjects.h"

DECLARE_THISFILE;

static void makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3, int texcoor[4]) { // static
  Face &f2 = mb.addFace();
  f2.addVertexTextureIndex(v0, texcoor[0]);
  f2.addVertexTextureIndex(v3, texcoor[1]);
  f2.addVertexTextureIndex(v2, texcoor[2]);
  f2.addVertexTextureIndex(v1, texcoor[3]);
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

LPDIRECT3DTEXTURE BoardFieldObject::s_unmarkedTexture = NULL;
LPDIRECT3DTEXTURE BoardFieldObject::s_markTexture     = NULL;

BoardFieldObject::BoardFieldObject(D3Scene &scene, int row, int col)
: SceneObjectWithMesh(scene, createMesh(scene.getDevice(), row, col))
{
  m_field.m_row = row;
  m_field.m_col = col;
  m_selected    = false;
  setName(format(_T("%s"), m_field.toString().cstr()));
}

LPD3DXMESH BoardFieldObject::createMesh(LPDIRECT3DDEVICE device, int row, int col) {
  MeshBuilder mb;
  const float *gridLines = GameBoardObject::s_gridLines;
  int vindex[2][2];
  int tindex[2][2];
  for(short r = 0; r < 2; r++) {
    for(short c = 0; c < 2; c++) {
      vindex[r][c] = mb.addVertex(gridLines[row+r],0,gridLines[col+c]);
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

LPDIRECT3DTEXTURE BoardFieldObject::getTexture(bool marked) {
  if(marked) {
    if (s_markTexture == NULL) {
      s_markTexture = loadTextureFromBitmapResource(getDevice(), IDB_MARKEDFIELDBITMAP);
    }
    return s_markTexture;
  } else {
    if (s_unmarkedTexture == NULL) {
      s_unmarkedTexture = loadTextureFromBitmapResource(getDevice(), IDB_UNMARKEDFIELDBITMAP);
    }
    return s_unmarkedTexture;
  }
}

void BoardFieldObject::draw() {
  drawMeshUsingTexture(getDevice(), getMesh(), getTexture(m_selected));
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
      if((EDGE0INDEX <= r) && (r < EDGE4INDEX) && (EDGE0INDEX <= c) && (c < EDGE4INDEX)) {
        continue;
      }
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
//    debugLog(_T("%s\n%s"), __TFUNCTION__, indentString(mb.toString(), 2).cstr());
    throw;
  }
}

GameBoardObject::GameBoardObject(D3Scene &scene) 
: SceneObjectWithMesh(scene, createMesh(scene.getDevice()))
{
//  debugLog(_T("GameBoardObject:\n%s"), indentString(toString(),2).cstr());
  m_boardTexture = loadTextureFromBitmapResource(getDevice(), IDB_BOARDBITMAP      );
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      BoardFieldObject *mfo = new BoardFieldObject(scene, r, c);
      m_fieldObject[r][c] = mfo;
      m_scene.addSceneObject(mfo);
//      debugLog(_T("MarkField:\n%s"), indentString(mfo->toString(),2).cstr());
    }
  }
  for(BYTE attr = 0; attr < ARRAYSIZE(m_brickObject); attr++) {
    m_scene.addSceneObject(m_brickObject[attr] = new BrickObject(m_scene, attr));
  }
  resetBrickPositions(false);
  m_currentField = NOFIELD;
  m_currentBrick = NOBRICK;
}

GameBoardObject::~GameBoardObject() {
  if(m_boardTexture) m_boardTexture->Release();
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      if(m_fieldObject[r][c]) {
        delete m_fieldObject[r][c];
      }
    }
  }
  for(int i = 0; i < ARRAYSIZE(m_brickObject); i++) {
    delete m_brickObject[i];
  }
}

void GameBoardObject::draw() {
  drawMeshUsingTexture(getDevice(), getMesh(), m_boardTexture);
}

#define BRICKZ 0.05f

D3DXVECTOR3 GameBoardObject::getFieldCenter(const Field &f) const {
  assert(f.isValid());
  return D3DXVECTOR3((s_gridLines[f.m_row]+s_gridLines[f.m_row+1])/2
                    ,(s_gridLines[f.m_col]+s_gridLines[f.m_col+1])/2
                    ,BRICKZ
                    );
}

void GameBoardObject::resetBrickPositions(bool colored) {
  for(int i = 0; i < ARRAYSIZE(m_brickObject); i++) {
    BrickObject *b = m_brickObject[i];
    const float x = (float)((i%8)*2-HALFSIZE+0.85);
    if(colored) {
      b->setPos(D3DXVECTOR3(x, (float)((i/8)*(BOARDSIZE-1.5f)-HALFSIZE+0.85),BRICKZ));
    } else {
      b->setPos(D3DXVECTOR3(x, (float)((i/8)*2-HALFSIZE-0.8f)               ,BRICKZ));
    }
  }
}

void GameBoardObject::setBrickOnField(BYTE brick, const Field &f) {
  m_brickObject[brick]->setPos(getFieldCenter(f));
}

void GameBoardObject::markField(const Field &f) {
  if(!f.isValid()) return;
  unmarkCurrentField();
  m_currentField = f;
  setCurrentFieldSelected(true);
}

void GameBoardObject::unmarkCurrentField() {
  if(!hasCurrentField()) return;
  setCurrentFieldSelected(false);
  m_currentField = NOFIELD;
}

void GameBoardObject::setCurrentFieldSelected(bool selected) {
  BoardFieldObject *fld = m_fieldObject[m_currentField.m_row][m_currentField.m_col];
  fld->setSelected(selected);
}

void GameBoardObject::markBrick(char brick) {
  if(!Brick::isValid(brick)) return;
  unmarkCurrentBrick();
  m_currentBrick = brick;
  setCurrentBrickSelected(true);
}

void GameBoardObject::unmarkCurrentBrick() {
  if(hasCurrentBrick()) setCurrentBrickSelected(false);
  m_currentBrick = NOBRICK;
}

void GameBoardObject::setCurrentBrickSelected(bool selected) {
  m_brickObject[m_currentBrick]->setMarked(selected);
}

void GameBoardObject::setBricksVisible(BrickSet set, bool visible) {
  for(Iterator<UINT> it = set.getIterator(); it.hasNext();) {
    m_brickObject[it.next()]->setVisible(visible);
  }
}

int GameBoardObject::getBrickFromPoint(const CPoint &p) const {
  const D3SceneObject *obj = m_scene.getPickedObject(p);
  if((obj == NULL) || (obj == this)) {
    return NOBRICK;
  }
  for(int b = 0; b < FIELDCOUNT; b++) {
    if(m_brickObject[b] == obj) {
      return b;
    }
  }
  return NOBRICK;
}

Field GameBoardObject::getFieldFromPoint(const CPoint &p) const {
  const D3SceneObject *obj = m_scene.getPickedObject(p);
  if(obj == NULL || obj == this) {
    return NOFIELD;
  }
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      if(m_fieldObject[r][c] == obj) {
        return Field(r,c);
      }
    }
  }
  return NOFIELD;
}
