#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObject.h>

class D3SceneObjectIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  D3Scene                      &m_scene;
  CompactArray<D3SceneObject*> &m_a;
  size_t                        m_next;
  intptr_t                      m_current;
  size_t                        m_updateCount;
  const long                    m_mask;

  inline bool checkMask(size_t i) const {
    return (OBJTYPE_MASK(m_a[i]->getType()) & m_mask) != 0;
  }
  void checkUpdateCount() const {
    if(m_updateCount != m_a.getUpdateCount()) {
      concurrentModificationError(s_className);
    }
  }
  size_t first() const {
    size_t i;
    for(i = 0; (i < m_a.size()) && !checkMask(i); i++);
    return i;
  }
public:
  D3SceneObjectIterator(D3Scene &scene, long mask)
    : m_scene(scene)
    , m_a(scene.m_objectArray)
    , m_mask(mask)
  {
    m_current     = -1;
    m_next        = first();
    m_updateCount = m_a.getUpdateCount();
  }
  AbstractIterator *clone() {
    return new D3SceneObjectIterator(*this);
  }
  inline bool hasNext() const {
    return m_next < m_a.size();
  }

  void *next() {
    if(m_next >= m_a.size()) {
      noNextElementError(s_className);
    }
    checkUpdateCount();
    for(m_current = m_next++; (m_next < m_a.size()) && !checkMask(m_next); m_next++);
    return &m_a[m_current];
  }
  void remove() {
    if(m_current < 0) {
      noCurrentElementError(s_className);
    }
    checkUpdateCount();
    m_scene.removeSceneObject(m_current);
    m_current     = -1;
    m_updateCount = m_a.getUpdateCount();
  }
};

DEFINECLASSNAME(D3SceneObjectIterator);

Iterator<D3SceneObject*> D3Scene::getObjectIterator(long mask) const {
  return Iterator<D3SceneObject*>(new D3SceneObjectIterator(*(D3Scene*)this, mask));
}
