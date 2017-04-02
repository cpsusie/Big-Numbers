#pragma once

#include <D3DGraphics/D3Scene.h>

#define BOARDHEIGHT 0.8f
#define BOARDSIZE   16
#define HALFSIZE    BOARDSIZE/2
#define GAMECOLOR   RGB(235,235,122)

class BoardFieldObject : public SceneObjectWithMesh {
private:
  static LPDIRECT3DTEXTURE s_unmarkedTexture;
  static LPDIRECT3DTEXTURE s_markTexture;
  Field  m_field;
  bool   m_selected;
  LPDIRECT3DTEXTURE getTexture(bool marked);
  static LPD3DXMESH createMesh(LPDIRECT3DDEVICE device, int row, int col);

public:
  BoardFieldObject(D3Scene &scene, int row, int col);
  D3PosDirUpScale getPDUS() {
    return D3Scene::getOrigo();
  }
  inline void setSelected(bool selected) {
    m_selected = selected;
  }
  void draw();
};

class GameBoardObject : public SceneObjectWithMesh {
private:
  static const float s_vertexPos[];
  static float       s_gridLines[5];
  D3PosDirUpScale   m_pdus;
  LPDIRECT3DTEXTURE m_boardTexture;
  BoardFieldObject *m_fieldObject[ROWCOUNT][COLCOUNT];
  Field             m_currentField;
  static LPD3DXMESH createMesh(LPDIRECT3DDEVICE device);
  friend class BoardFieldObject ;
  void setCurrentSelected(bool selected);
public:
  GameBoardObject(D3Scene &scene);
  ~GameBoardObject();
  D3PosDirUpScale getPDUS() {
    return D3Scene::getOrigo();
  }
  bool hasCurrentField() const {
    return m_currentField.isField();
  }
  void markField(const Field &f);
  void unmarkCurrent();
  const Field getCurrentField() const {
    return m_currentField;
  }
  void draw();
  Field getFieldFromPoint(const CPoint &p) const;
};
