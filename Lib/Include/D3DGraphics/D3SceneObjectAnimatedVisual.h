#pragma once

#include "AbstractAnimator.h"
#include "D3SceneObjectVisual.h"
#include "MeshArray.h"

class D3SceneObjectAnimatedVisual : public D3SceneObjectVisual, public AbstractAnimator {
private:
  D3DFILLMODE   m_fillMode;
  D3DSHADEMODE  m_shadeMode;
  UINT          m_lastRenderedIndex;
  FastSemaphore m_lock;
  void init(const MeshArray &meshArray);

  // return (m_lastRenderedIndex < childCount())
  //      ?  getChild(m_lastRenderedIndex)
  //      : (getCurrentIndex()    < childCount()) ? getChild(getCurrentIndex()) : NULL
  D3SceneObjectVisual *getCurrentChild() const;
public:
  D3SceneObjectAnimatedVisual(D3Scene             &scene , const MeshArray &meshArray, const String &name = _T("Animated Visual"));
  D3SceneObjectAnimatedVisual(D3SceneObjectVisual *parent, const MeshArray &meshArray, const String &name = _T("Animated Visual"));
  ~D3SceneObjectAnimatedVisual();

  void                   handleTimeout(Timer &timer)                override;
  SceneObjectType        getType()                            const override {
    return SOTYPE_ANIMATEDOBJECT;
  }

  // synchronized
  // if(getCurrentIndex() < getChildCount()) {
  //   getChild(getCurrentIndex())->draw(); m_lastRenderedIndex = index;
  // }
  void                   draw()                                     override;
  // Return currentChild ? currentChild->getMesh() : NULL
  LPD3DXMESH             getMesh()                            const override;
  // Return currentChild ? currentChild->getVertexBuffer() : NULL
  LPDIRECT3DVERTEXBUFFER getVertexBuffer()                    const override;
  // Return currentChild ? currentChild->getIndexBuffer() : NULL
  LPDIRECT3DINDEXBUFFER  getIndexBuffer()                     const override;
  // Return currentChild ? currentChild->isNormalsVisible() : false
  bool                   isNormalsVisible()                   const override;
  // synchronized
  // foreach child child->setNormalsVisible(visible);
  void                   setNormalsVisible(bool visible)            override;
  bool                   hasFillMode()                        const override {
    return true;
  }
  bool                   hasShadeMode()                       const override {
    return true;
  }
  void                   setFillMode(D3DFILLMODE  fillMode)         override {
    m_fillMode = fillMode;
  }
  D3DFILLMODE             getFillMode()                       const override {
    return m_fillMode;
  }
  void                    setShadeMode(D3DSHADEMODE shadeMode)      override {
    m_shadeMode = shadeMode;
  }
  D3DSHADEMODE            getShadeMode()                      const override {
    return m_shadeMode;
  }
};
