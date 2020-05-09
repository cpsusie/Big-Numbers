#pragma once

#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectWithMesh.h>
#include <D3DGraphics/MeshBuilder.h>

#define BOARDHEIGHT 0.8f
#define BOARDSIZE   16
#define HALFSIZE    (BOARDSIZE/2)
#define GAMECOLOR   D3DCOLOR_XRGB(235,235,122)

class GameBoardObject;

class BoardObjectWithTexture : public D3SceneObjectWithMesh {
protected:
  virtual LPDIRECT3DTEXTURE getTexture() = NULL;
public:
  BoardObjectWithTexture(D3Scene &scene, LPD3DXMESH mesh) : D3SceneObjectWithMesh(scene, mesh) {
  }
  BoardObjectWithTexture(D3SceneObjectVisual *parent, LPD3DXMESH mesh) : D3SceneObjectWithMesh(parent, mesh) {
  }
  ~BoardObjectWithTexture() {
  }
  bool getLightingEnable() const {
    return false;
  }
  D3DXMATRIX &getWorld() {
    D3SceneObjectVisual *parent = getParent();
    return parent ? getParent()->getWorld() : __super::getWorld();
  }

  void draw();
};

class BoardFieldObject : public BoardObjectWithTexture {
private:
#ifdef _DEBUG
  LPDIRECT3DTEXTURE m_texture[2]; // 0=unmarked,1=marked
  LPDIRECT3DTEXTURE createTexture(bool marked);
  static CFont     *getFont();
#else
  static LPDIRECT3DTEXTURE s_texture[3]; // 0=white,1=black,2=marked
  static BYTE              s_fieldCount;
  void incrFieldcount() {
    s_fieldCount++;
  }
  void decrFieldCount();
#endif
  void                     destroyTexture();
  D3DXVECTOR3              m_center;
  bool                     m_selected;
  Field                    m_field;

  LPDIRECT3DTEXTURE getTexture(bool marked);
  static LPD3DXMESH createMesh(AbstractMeshFactory &amf, int row, int col);
public:
  BoardFieldObject(D3SceneObjectVisual *parent, int row, int col);
  ~BoardFieldObject();
  inline void setSelected(bool selected) {
    m_selected = selected;
#ifdef _DEBUG
    if(selected) debugLog(_T("Select %s\n"), getName().cstr());
#endif
  }
  inline const D3DXVECTOR3 &getCenter() const {
    return m_center;
  }
  LPDIRECT3DTEXTURE getTexture() {
    return getTexture(m_selected);
  }

#ifdef _DEBUG
  String toString() const;
#endif
};

class BrickObject : public D3SceneObjectWithMesh {
private:
  const BYTE           m_attr;
  D3SceneObjectVisual *m_brickMarker;
  D3DXVECTOR3          m_pos; // relative to m_parent (board)
  bool                 m_marked;
  bool                 m_visible;
  static LPD3DXMESH    createMesh(AbstractMeshFactory &amf, BYTE attr);
  GameBoardObject     &getBoard() const;
public:
  BrickObject(GameBoardObject *board, BYTE attr);
  ~BrickObject();

  void setPos(const D3DXVECTOR3 &pos) {
    m_pos = pos;
  }
  inline const D3DXVECTOR3 &getPos() const {
    return m_pos;
  }
  inline void setMarked(bool marked) {
    m_marked = marked;
#ifdef _DEBUG
    if(marked) debugLog(_T("Mark %s\n"), toString().cstr());
#endif
  }
  bool isVisible() const {
    return m_visible;
  }
  inline void setVisible(bool visible) {
    m_visible = visible;
  }
  int getMaterialId() const;
  D3DXMATRIX &getWorld();
  void draw();
#ifdef _DEBUG
  String toString() const;
#endif
};

typedef enum {
  GB_CURRENTFIELD   // Field
 ,GB_CURRENTBRICK   // char
 ,GB_BRICKPOSITIONS // BrickPositions
} GameBordProperties;

class BrickPositions {
public:
  D3DXVECTOR3 m_pos[FIELDCOUNT];
};

inline bool operator==(const BrickPositions &b1, const BrickPositions &b2) {
  return memcmp(&b1, &b2, sizeof(BrickPositions)) == 0;
}
inline bool operator!=(const BrickPositions &b1, const BrickPositions &b2) {
  return !(b1 == b2);
}

class GameBoardObject : public BoardObjectWithTexture, public PropertyContainer {
private:
  static const float   s_vertexXPos[], s_vertexYPos[];
  static float         s_xgridLines[5], s_ygridLines[5];
  static int           s_markerMaterialId;
  LPDIRECT3DTEXTURE    m_boardTexture;
  D3SceneObjectVisual *m_boardSideObject;
  BoardFieldObject    *m_fieldObject[ROWCOUNT][COLCOUNT];
  BrickObject         *m_brickObject[FIELDCOUNT];
  Field                m_currentField;
  UINT                 m_brickMaterialId[2];
  char                 m_currentBrick;
  BrickPositions       m_brickPositions;
  static LPD3DXMESH createMesh(AbstractMeshFactory &amf);
  void addBrickMaterials();
  void removeBrickMaterials();
  void destroyFieldObjets();
  void destroyBrickObjects();
  friend class BoardFieldObject;
  void setCurrentFieldSelected(bool selected);
  void setCurrentBrickSelected(bool selected);
  void getBrickPositions(BrickPositions &bp) const;
  void notifyIfBrickPosisionChanged();
public:
  GameBoardObject(D3Scene &scene);
  ~GameBoardObject();
  inline bool hasCurrentField() const {
    return m_currentField.isField();
  }
  inline bool hasCurrentBrick() const {
    return m_currentBrick != NOBRICK;
  }
  inline D3DXVECTOR3 getFieldCenter(const Field &f) const {
    return m_fieldObject[f.m_row][f.m_col]->getCenter();
  }
  void resetBrickPositions(bool colored);
  void setBrickOnField(BYTE brick, const Field &f);
  void markField(const Field &f);
  void unmarkCurrentField();
  void markBrick(char brick);
  void unmarkCurrentBrick();
  void setBricksVisible(BrickSet set, bool visible);
  inline const Field getCurrentField() const {
    return m_currentField;
  }
  inline char getCurrentBrick() const {
    return m_currentBrick;
  }
  int   getBrickFromPoint(const CPoint &p, const D3Camera *camera) const;
  Field getFieldFromPoint(const CPoint &p, const D3Camera *camera) const;
  LPDIRECT3DTEXTURE getTexture() {
    return m_boardTexture;
  }
  UINT getBrickMaterialId(bool black);
  static inline UINT getMarkerMaterialId() {
    return s_markerMaterialId;
  }
  void draw();
#ifdef _DEBUG
  String toString() const;
#endif
};
