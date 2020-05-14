#include "stdafx.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include "GraphicObjects.h"

static void makeSquareFace(MeshBuilder &mb, int v0, int v1, int v2, int v3, int texcoor[4]) {
  Face &f2 = mb.addFace();
  f2.addVertexTextureIndex(v0, texcoor[0]);
  f2.addVertexTextureIndex(v1, texcoor[1]);
  f2.addVertexTextureIndex(v2, texcoor[2]);
  f2.addVertexTextureIndex(v3, texcoor[3]);
}

void BoardObjectWithTexture::draw() {
  getDevice().setSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)
             .setSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)
             .setTexture(     0, getTexture())
             .setAlphaBlendEnable(false)
             .setZEnable(         D3DZB_TRUE);
  __super::draw();
}

#define XGRID(i) (((137.0f+((float)(i))/4.0f*750.0f))/1024.0f)
#define YGRID(i) (((139.0f+((float)(i))/4.0f*750.0f))/1024.0f)

const float GameBoardObject::s_vertexXPos[] = {
  0.0f,XGRID(0), XGRID(1), XGRID(2), XGRID(3), XGRID(4),1.0f
};

const float GameBoardObject::s_vertexYPos[] = {
  0.0f,YGRID(0), YGRID(1), YGRID(2), YGRID(3), YGRID(4),1.0f
};

float GameBoardObject::s_xgridLines[5];
float GameBoardObject::s_ygridLines[5];

#define GRIDPOSCOUNT ARRAYSIZE(s_vertexXPos)
#define EDGE0INDEX 1
#define EDGE4INDEX 5

#ifndef _DEBUG
LPDIRECT3DTEXTURE BoardFieldObject::s_texture[3] = {
  NULL
 ,NULL
 ,NULL
};
BYTE BoardFieldObject::s_fieldCount = 0;
void BoardFieldObject::decrFieldCount() {
  if(--s_fieldCount) {
    destroyTexture();
  }
}

#endif // _DEBUG

BoardFieldObject::BoardFieldObject(D3SceneObjectVisual *parent, int row, int col)
: BoardObjectWithTexture(parent, createMesh(parent->getScene(), row, col))
, m_field(row,col)
{
  m_selected    = false;
  m_center      = getBoundingBox().getCenter();

#ifndef _DEBUG
  incrFieldcount();
#else
  m_texture[0] = createTexture(false);
  m_texture[1] = createTexture(true );
  setName(format(_T("Field(%d,%d) Center:%s"), row,col, ::toString(m_center).cstr()));
#endif // _DEBUG
}

BoardFieldObject::~BoardFieldObject() {
#ifndef _DEBUG
  decrFieldCount();
#else
  destroyTexture();
#endif // _DEBUG
}

LPD3DXMESH BoardFieldObject::createMesh(AbstractMeshFactory &amf, int row, int col) {
  MeshBuilder mb;
  const float *xgridLines = GameBoardObject::s_xgridLines;
  const float *ygridLines = GameBoardObject::s_ygridLines;
  int vindex[2][2];
  int tindex[2][2];
  for(short r = 0; r < 2; r++) {
    for(short c = 0; c < 2; c++) {
      vindex[r][c] = mb.addVertex(xgridLines[row+r],ygridLines[col+c],0);
      tindex[r][c] = mb.addTextureVertex(r,c);
    }
  }

  Face &face = mb.addFace();
  face.addVertexTextureIndex(vindex[0][0], tindex[0][0]);
  face.addVertexTextureIndex(vindex[0][1], tindex[0][1]);
  face.addVertexTextureIndex(vindex[1][1], tindex[1][1]);
  face.addVertexTextureIndex(vindex[1][0], tindex[1][0]);
  return mb.createMesh(amf, false);
}

static int fieldResId[] = {
  IDB_WHITEFIELDBITMAP
 ,IDB_BLACKFIELDBITMAP
 ,IDB_GREENFIELDBITMAP
};

#define ISWHITEFIELD(    f       ) (((f).m_row+(f).m_col)&1)
#define GETFIELDRESINDEX(f,marked) ((marked)?2:ISWHITEFIELD(f)?0:1)
#define GETFIELDRESID(   f,marked) fieldResId[GETFIELDRESINDEX(f,marked)]

#ifdef _DEBUG
CFont *BoardFieldObject::getFont() { // static
  static CFont font;
  if(font.m_hObject==NULL) {
    font.CreateFont(22, 22, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                    ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                    ,DEFAULT_PITCH | FF_MODERN
                    ,_T("Courier") );
  }
  return &font;
}

LPDIRECT3DTEXTURE BoardFieldObject::createTexture(bool marked) {
  CBitmap bm;
  const int resId = GETFIELDRESID(m_field,marked);
  bm.LoadBitmap(GETFIELDRESID(m_field,marked));
  CSize sz = getBitmapSize(bm);
  sz.cx/= 2; sz.cy/=2;
  HDC screenDC = getScreenDC();
  HDC bmDC     = CreateCompatibleDC(screenDC);
  DeleteDC(screenDC);
  SelectObject(bmDC, bm);
  textOutTransparentBackground(bmDC
                              ,CPoint(sz.cx-30, sz.cy-6)
                              ,m_field.toString()
                              ,*getFont()
                              ,ISWHITEFIELD(m_field)?BLACK:WHITE);
  DeleteDC(bmDC);
  return getScene().getTextureFromBitmap(bm);
}
#endif

LPDIRECT3DTEXTURE BoardFieldObject::getTexture(bool marked) {
#ifndef _DEBUG
  const int index = GETFIELDRESINDEX(m_field,marked);
  if(s_texture[index] == NULL) {
    s_texture[index] = getScene().loadTextureFromBitmapResource(GETFIELDRESID(m_field,marked));
  }
  return s_texture[index];
#else
  return m_texture[marked?1:0];
#endif // _DEBUG
}

void BoardFieldObject::destroyTexture() {
#ifndef _DEBUG
  for(UINT i = 0; i < ARRAYSIZE(s_texture); i++) {
    SAFERELEASE(s_texture[i]);
  }
#else
  for(UINT i = 0; i < ARRAYSIZE(m_texture); i++) {
    SAFERELEASE(m_texture[i])
  }
#endif // _DEBUG
}

#ifdef _DEBUG
String BoardFieldObject::toString() const {
  return getName() + _T("\n")
       + indentString(format(_T("World:\n%s")
                            ,indentString(D3World(*this).toString(),2).cstr()
                            )
                     ,2
                     );
}
#endif

class BoardSideObject : public BoardObjectWithTexture {
private:
  LPDIRECT3DTEXTURE m_boardSideTexture;
  static LPD3DXMESH createMesh(AbstractMeshFactory &amf);
public:
  BoardSideObject(D3SceneObjectVisual *parent);
  ~BoardSideObject();

  LPDIRECT3DTEXTURE getTexture() {
    return m_boardSideTexture;
  }
};

BoardSideObject::BoardSideObject(D3SceneObjectVisual *parent)
: BoardObjectWithTexture(parent, createMesh(parent->getScene()))
{
  m_boardSideTexture = getScene().loadTextureFromBitmapResource(IDB_BOARDSIDEBITMAP);
}

BoardSideObject::~BoardSideObject() {
  SAFERELEASE(m_boardSideTexture);
}

LPD3DXMESH BoardSideObject::createMesh(AbstractMeshFactory &amf) { // static
  MeshBuilder mb;

  const int  ltn = mb.addVertex(-HALFSIZE,-HALFSIZE, 0          ); // left  top    near corner
  const int  ltf = mb.addVertex(-HALFSIZE, HALFSIZE, 0          ); // left  top    far  corner
  const int  rtf = mb.addVertex( HALFSIZE, HALFSIZE, 0          ); // right top    far  corner
  const int  rtn = mb.addVertex( HALFSIZE,-HALFSIZE, 0          ); // right top    near corner
  const int  lbn = mb.addVertex(-HALFSIZE,-HALFSIZE,-BOARDHEIGHT); // left  bottom near corner
  const int  lbf = mb.addVertex(-HALFSIZE, HALFSIZE,-BOARDHEIGHT); // left  bottom far  corner
  const int  rbf = mb.addVertex( HALFSIZE, HALFSIZE,-BOARDHEIGHT); // right bottom far  corner
  const int  rbn = mb.addVertex( HALFSIZE,-HALFSIZE,-BOARDHEIGHT); // right bottom near corner
  const int  tx00 = mb.addTextureVertex(0,0);
  const int  tx01 = mb.addTextureVertex(0,1);
  const int  tx10 = mb.addTextureVertex(1,0);
  const int  tx11 = mb.addTextureVertex(1,1);

  int textureCorners[4];
  textureCorners[0] = tx00;
  textureCorners[1] = tx10;
  textureCorners[2] = tx11;
  textureCorners[3] = tx01;

  makeSquareFace(mb,lbn,rbn,rbf,lbf, textureCorners);              // bottom
  makeSquareFace(mb,lbn,lbf,ltf,ltn, textureCorners);              // left side
  makeSquareFace(mb,lbf,rbf,rtf,ltf, textureCorners);              // back side
  makeSquareFace(mb,rbf,rbn,rtn,rtf, textureCorners);              // right side
  makeSquareFace(mb,rbn,lbn,ltn,rtn, textureCorners);              // front side

  try {
    return mb.createMesh(amf, false);
  } catch (...) {
//    debugLog(_T("%s\n%s"), __TFUNCTION__, indentString(mb.toString(), 2).cstr());
    throw;
  }
}

LPD3DXMESH GameBoardObject::createMesh(AbstractMeshFactory &amf) { // static
  MeshBuilder mb;

  int vindex[GRIDPOSCOUNT][GRIDPOSCOUNT];
  int tindex[GRIDPOSCOUNT][GRIDPOSCOUNT];
  for(int r = 0; r < GRIDPOSCOUNT; r++) {
    for(int c = 0; c < GRIDPOSCOUNT; c++) {
      vindex[r][c] = mb.addVertex(BOARDSIZE*(s_vertexXPos[r]-0.5f),BOARDSIZE*(s_vertexYPos[c]-0.5f),0);
      tindex[r][c] = mb.addTextureVertex(s_vertexXPos[r],s_vertexYPos[c]);
    }
  }
  for(int i = 0; i < ARRAYSIZE(s_xgridLines); i++) {
    s_xgridLines[i] = BOARDSIZE*(s_vertexXPos[i+EDGE0INDEX]-0.5f);
    s_ygridLines[i] = BOARDSIZE*(s_vertexYPos[i+EDGE0INDEX]-0.5f);
  }

  for(int r = 0; r < GRIDPOSCOUNT-1; r++) {
    for(int c = 0; c < GRIDPOSCOUNT-1; c++) {
      if((EDGE0INDEX <= r) && (r < EDGE4INDEX) && (EDGE0INDEX <= c) && (c < EDGE4INDEX)) {
        continue;
      }
      Face &face = mb.addFace();
      face.addVertexTextureIndex(vindex[r  ][c  ], tindex[r  ][c  ]);
      face.addVertexTextureIndex(vindex[r  ][c+1], tindex[r  ][c+1]);
      face.addVertexTextureIndex(vindex[r+1][c+1], tindex[r+1][c+1]);
      face.addVertexTextureIndex(vindex[r+1][c  ], tindex[r+1][c  ]);
    }
  }

  try {
    return mb.createMesh(amf, false);
  } catch (...) {
//    debugLog(_T("%s\n%s"), __TFUNCTION__, indentString(mb.toString(), 2).cstr());
    throw;
  }
}

int GameBoardObject::s_markerMaterialId = -1;

GameBoardObject::GameBoardObject(D3Scene &scene)
: BoardObjectWithTexture(scene, createMesh(scene))
{
  setName(_T("Board"));
  m_boardTexture     = scene.loadTextureFromBitmapResource(IDB_BOARDBITMAP);
  D3SceneObjectVisual *so = new BoardSideObject(this); TRACE_NEW(so);
  addChild(so);

  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      BoardFieldObject *f = new BoardFieldObject(this, r, c); TRACE_NEW(f);
      m_fieldObject[r][c] = f;
      scene.addVisual(f); // to make it pickable
//    addChild(f);        // NOT this....coz draw by camera::draw, as its a visual added to scene
                          // draw by GameboardObject::draw()
    }
  }

  addBrickMaterials();
  s_markerMaterialId = getScene().addMaterialWithColor(D3D_WHITE);
  for(BYTE attr = 0; attr < ARRAYSIZE(m_brickObject); attr++) {
    BrickObject *b = new BrickObject(this, attr); TRACE_NEW(b);
    m_brickObject[attr] = b;
    scene.addVisual(b); // to make if pickable
//  addChild(b); // NOT this...same reason asBoardFieldObject
  }
  resetBrickPositions(false);
  getBrickPositions(m_brickPositions);
  m_currentField = NOFIELD;
  m_currentBrick = NOBRICK;
}

GameBoardObject::~GameBoardObject() {
  getScene().removeAllVisuals();
  SAFERELEASE(m_boardTexture);
  destroyFieldObjets();
  removeBrickMaterials();
  destroyBrickObjects();
  getScene().removeMaterial(s_markerMaterialId); s_markerMaterialId = -1;

}

void GameBoardObject::destroyFieldObjets() {
  for(int r = 0; r < ROWCOUNT; r++) {
    for(int c = 0; c < COLCOUNT; c++) {
      SAFEDELETE(m_fieldObject[r][c]);
    }
  }
}
void GameBoardObject::destroyBrickObjects() {
  for(BYTE attr = 0; attr < ARRAYSIZE(m_brickObject); attr++) {
    SAFEDELETE(m_brickObject[attr]);
  }
}

#define MATERIAL_DIFFUSE_BLACK D3DCOLOR_XRGB(66,56,196)
#define MATERIAL_DIFFUSE_WHITE D3DCOLOR_XRGB(216,50,50)

void GameBoardObject::addBrickMaterials() {
  for(int i = 0; i < 2; i++) {
    D3Material mat = getScene().getMaterial(0);
    mat.Diffuse  = colorToColorValue(i ? MATERIAL_DIFFUSE_BLACK : MATERIAL_DIFFUSE_WHITE);
    mat.Specular = colorToColorValue(D3D_WHITE);
    mat.Ambient  = colorToColorValue(D3D_BLACK);
    mat.Emissive = mat.Ambient;
    mat.Power    = 200.0f;
    m_brickMaterialId[i] = getScene().addMaterial(mat);
  }
}

void GameBoardObject::removeBrickMaterials() {
  for(int i = 0; i < ARRAYSIZE(m_brickMaterialId); i++) {
    getScene().removeMaterial(m_brickMaterialId[i]);
  }
}

UINT GameBoardObject::getBrickMaterialId(bool black) {
  return m_brickMaterialId[ordinal(black)];
}

#define BRICKZ 0.05f

void GameBoardObject::getBrickPositions(BrickPositions &bp) const {
  for(int i = 0; i < FIELDCOUNT; i++) {
    const BrickObject *b = m_brickObject[i];
    bp.m_pos[i] = b->getPos();
  }
}

void GameBoardObject::notifyIfBrickPosisionChanged() {
  BrickPositions newPositions;
  getBrickPositions(newPositions);
  setProperty(GB_BRICKPOSITIONS, m_brickPositions, newPositions);
}

void GameBoardObject::resetBrickPositions(bool colored) {
  for(int i = 0; i < ARRAYSIZE(m_brickObject); i++) {
    BrickObject *b = m_brickObject[i];
    const float x = (float)((i%8)*2-HALFSIZE+0.85);
    const float y = colored
                  ? (float)((i/8)*(BOARDSIZE-1.5f)-HALFSIZE+0.85)
                  : (float)((i/8)*2-HALFSIZE-0.8f);
    b->setPos(D3DXVECTOR3(x, y, BRICKZ));
  }
  notifyIfBrickPosisionChanged();
}

void GameBoardObject::setBrickOnField(BYTE brick, const Field &f) {
  m_brickObject[brick]->setPos(getFieldCenter(f));
  notifyIfBrickPosisionChanged();
}

void GameBoardObject::markField(const Field &f) {
  if(!f.isValid()) return;
  unmarkCurrentField();
  setProperty(GB_CURRENTFIELD,m_currentField,f);
  setCurrentFieldSelected(true);
}

void GameBoardObject::unmarkCurrentField() {
  if(!hasCurrentField()) return;
  setCurrentFieldSelected(false);
  setProperty(GB_CURRENTFIELD,m_currentField,NOFIELD);
}

void GameBoardObject::setCurrentFieldSelected(bool selected) {
  BoardFieldObject *fld = m_fieldObject[m_currentField.m_row][m_currentField.m_col];
  fld->setSelected(selected);
}

void GameBoardObject::markBrick(char brick) {
  if(!Brick::isValid(brick)) return;
  unmarkCurrentBrick();
  setProperty(GB_CURRENTBRICK,m_currentBrick,brick);
  setCurrentBrickSelected(true);
}

void GameBoardObject::unmarkCurrentBrick() {
  if(hasCurrentBrick()) setCurrentBrickSelected(false);
  setProperty(GB_CURRENTBRICK,m_currentBrick,NOBRICK);
}

void GameBoardObject::setCurrentBrickSelected(bool selected) {
  m_brickObject[m_currentBrick]->setMarked(selected);
}

void GameBoardObject::setBricksVisible(BrickSet set, bool visible) {
  for(Iterator<UINT> it = set.getIterator(); it.hasNext();) {
    m_brickObject[it.next()]->setVisible(visible);
  }
}

int GameBoardObject::getBrickFromPoint(const CPoint &p, const D3Camera *camera) const {
  const D3SceneObjectVisual *obj = camera->getPickedVisual(p);
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

Field GameBoardObject::getFieldFromPoint(const CPoint &p, const D3Camera *camera) const {
  const D3SceneObjectVisual *obj = camera->getPickedVisual(p);
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

#ifdef _DEBUG
String GameBoardObject::toString() const {
  return format(_T("%s BoundingBox:%s")
                ,getName().cstr()
                ,::getBoundingBox(getMesh()).toString().cstr()
               );
}
#endif
