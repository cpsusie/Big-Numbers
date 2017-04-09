#pragma once

#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/MeshBuilder.h>

#define BOARDHEIGHT 0.8f
#define BOARDSIZE   16
#define HALFSIZE    (BOARDSIZE/2)
#define GAMECOLOR   D3DCOLOR_XRGB(235,235,122)

class BordObjectWithTexture : public SceneObjectWithMesh {
protected:
  void drawWithTexture(LPDIRECT3DTEXTURE texture);
public:
  BordObjectWithTexture(D3Scene &scene, LPD3DXMESH mesh)
    : SceneObjectWithMesh(scene, mesh)
  {}
  int getMaterialIndex() const {
    return -1;
  }
};

class BoardFieldObject : public BordObjectWithTexture {
private:
#ifdef _DEBUG
  LPDIRECT3DTEXTURE        m_texture[2]; // 0=unmarked,1=marked
  LPDIRECT3DTEXTURE createTexture(bool marked);
  static CFont     *getFont();
#else
  static LPDIRECT3DTEXTURE s_texture[3]; // 0=white,1=black,2=marked
#endif
  D3DXVECTOR3              m_center;
  bool                     m_selected;
  Field                    m_field;
  LPDIRECT3DTEXTURE getTexture(bool marked);
  static LPD3DXMESH createMesh(LPDIRECT3DDEVICE device, int row, int col);
public:
  BoardFieldObject(D3Scene &scene, int row, int col);
  D3PosDirUpScale getPDUS() const {
    return D3Scene::getOrigo();
  }
  inline void setSelected(bool selected) {
    m_selected = selected;
#ifdef _DEBUG
    if(selected) debugLog(_T("Select %s\n"), getName().cstr());
#endif
  }
  inline const D3DXVECTOR3 &getCenter() const {
    return m_center;
  }
  void draw() {
    drawWithTexture(getTexture(m_selected));
  }
#ifdef _DEBUG
  String toString() const;
#endif
};

class BrickObject : public SceneObjectWithMesh {
private:
  const BYTE          m_attr;               
  D3PosDirUpScale     m_pdus;
  D3SceneObject      *m_brickMarker;
  bool                m_marked;
  static LPD3DXMESH   createMesh(LPDIRECT3DDEVICE device, BYTE attr);
public:
  BrickObject(D3Scene &scene, BYTE attr);
  ~BrickObject();
  D3PosDirUpScale getPDUS() const {
    return m_pdus;
  }
  inline void setPos(const D3DXVECTOR3 &pos) {
    m_pdus.setPos(pos);
#ifdef _DEBUG
    debugLog(_T("New position(%s) %s\nPDUS:\n%s")
            ,getName().cstr()
            ,::toString(pos).cstr()
            ,indentString(getPDUS().toString(),2).cstr());
#endif
  }
  inline void setMarked(bool marked) {
    m_marked = marked;
#ifdef _DEBUG
    if (marked) debugLog(_T("Mark %s\n"), toString().cstr());
#endif
  }
  int getMaterialIndex() const {
    return ISBLACK(m_attr) ? 1:2;
  }
  void draw();
#ifdef _DEBUG
  String toString() const;
#endif
};

class GameBoardObject : public BordObjectWithTexture {
private:
  static const float s_vertexXPos[], s_vertexYPos[];
  static float       s_xgridLines[5], s_ygridLines[5];
  LPDIRECT3DTEXTURE  m_boardTexture;
  D3SceneObject     *m_boardSideObject;
  BoardFieldObject  *m_fieldObject[ROWCOUNT][COLCOUNT];
  BrickObject       *m_brickObject[FIELDCOUNT];
  Field              m_currentField;
  char               m_currentBrick;
  static LPD3DXMESH createMesh(LPDIRECT3DDEVICE device);
  void addBrickMaterials();
  friend class BoardFieldObject ;
  void setCurrentFieldSelected(bool selected);
  void setCurrentBrickSelected(bool selected);
public:
  GameBoardObject(D3Scene &scene);
  ~GameBoardObject();
  inline D3PosDirUpScale getPDUS() const {
    return D3Scene::getOrigo();
  }
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
  int   getBrickFromPoint(const CPoint &p) const;
  Field getFieldFromPoint(const CPoint &p) const;
  void draw();
#ifdef _DEBUG
  String toString() const;
#endif
};
