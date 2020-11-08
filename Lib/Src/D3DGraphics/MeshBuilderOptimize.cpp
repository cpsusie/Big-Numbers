#include "pch.h"
#include <TinyBitSet.h>
#include <CompactHashMap.h>
#include "D3ReductionArray.h"
#include <D3DGraphics/MeshBuilder.h>

typedef struct {
  const Vertex        *m_v, *m_n;
  const TextureVertex *m_t;
  UINT  m_faceIndex, m_index;
} TmpVNTI;

static int tmpVertexCmp(const TmpVNTI &tv1, const TmpVNTI &tv2) {
  const Vertex *v1 = tv1.m_v, *v2 = tv2.m_v;
  int c;
  c = sign(v1->x - v2->x); if(c) return c;
  c = sign(v1->y - v2->y); if(c) return c;
  c = sign(v1->z - v2->z); if(c) return c;
  const Vertex *n1 = tv1.m_n, *n2 = tv2.m_n;
  if(n1 != n2) {
    if(!n2) {
      return 1;
    } else if(!n1) {
      return -1;
    } else {
      c = sign(n1->x - n2->x); if(c) return c;
      c = sign(n1->y - n2->y); if(c) return c;
      c = sign(n1->z - n2->z); if(c) return c;
    }
  }
  const TextureVertex *t1 = tv1.m_t, *t2 = tv2.m_t;
  if(t1 != t2) {
    if(!t2) {
      return 1;
    } else if(!t1) {
      return -1;
    } else {
      c = sign(t1->u - t2->u); if(c) return c;
      c = sign(t1->v - t2->v); if(c) return c;
    }
  }
  return 0;
}

inline bool operator==(const TmpVNTI &tv1, const TmpVNTI &tv2) {
  return tmpVertexCmp(tv1, tv2) == 0;
}

MeshBuilder &MeshBuilder::reduceVertexArray() {
  if(m_vertices.size() >= 2) {
    const D3ReductionArray vt(m_vertices);
    const CompactUIntArray &translateTable = vt.getTranslateTable();
    const VertexArray      &reducedArray   = vt.getReducedArray();
    if(reducedArray.size() < m_vertices.size()) {
      m_vertices = reducedArray;
      const UINT fcount = (UINT)m_faceArray.size();
      BitSet     degeneratedFaces(fcount);
      BitSet     usedVertices(    reducedArray.size());
      BitSet64   duplicates;
      for(UINT f = 0; f < fcount; f++) {
        VNTIArray &va           = m_faceArray[f].m_data;
        const UINT vcount       = (UINT)va.size();
        usedVertices.clear();
        duplicates.clear();
        for(UINT v = 0; v < vcount; v++) {
          VertexNormalTextureIndex &vnt = va[v];
          if(vnt.m_vIndex >= 0) {
            const UINT newVinx = translateTable[vnt.m_vIndex];
            if(!usedVertices.contains(newVinx)) {
              vnt.m_vIndex = newVinx;
              usedVertices.add(newVinx);
            } else {
              duplicates.add(v);
            }
          }
        }
        const UINT uniquecount = vcount - duplicates.size();
        if(uniquecount < 3) {
          degeneratedFaces.add(f);
        } else if(uniquecount < vcount) {
          for(Iterator<UINT> it = duplicates.getIterator(); it.hasNext();) {
            va.remove(it.next());
          }
        }
      }
      if(!degeneratedFaces.isEmpty()) {
        const Array<Face> tmp = m_faceArray;
        m_faceArray.clear();
        m_faceArray.setCapacity(tmp.size() - degeneratedFaces.size());
        const size_t n = tmp.size();
        for(size_t f = 0; f < n; f++) {
          if(!degeneratedFaces.contains(f)) {
            m_faceArray.add(tmp[f]);
          }
        }
      }
    }
  }
  return *this;
}

MeshBuilder &MeshBuilder::reduceNormalArray() {
  if(m_normals.size() >= 2) {
    const D3ReductionArray vt(m_normals);
    const CompactUIntArray &translateTable = vt.getTranslateTable();
    const VertexArray      &reducedArray   = vt.getReducedArray();
    if(reducedArray.size() < m_normals.size()) {
      m_normals = reducedArray;
      const size_t fcount = m_faceArray.size();
      for(size_t f = 0; f < fcount; f++) {
        VNTIArray   &va     = m_faceArray[f].m_data;
        const size_t vcount = va.size();
        for(size_t v = 0; v < vcount; v++) {
          VertexNormalTextureIndex &vnt = va[v];
          if(vnt.m_nIndex >= 0) {
            vnt.m_nIndex = translateTable[vnt.m_nIndex];
          }
        }
      }
    }
  }
  return *this;
}

MeshBuilder &MeshBuilder::optimize() {
  reduceVertexArray();
  reduceNormalArray();
  const UINT            faceCount = (UINT)m_faceArray.size();
  CompactArray<TmpVNTI> tmpArray;
  for(UINT f = 0; f < faceCount; f++) {
    const Face      &face  = m_faceArray[f];
    const VNTIArray &a      = face.getIndexArray();
    const UINT       icount = face.getIndexCount();
    for(UINT i = 0; i < icount; i++) {
      const VertexNormalTextureIndex &vnti = a[i];
      tmpArray.add(TmpVNTI());
      TmpVNTI &tmp    = tmpArray.last();
      tmp.m_faceIndex = f;
      tmp.m_index     = i;
      tmp.m_v         = &m_vertices[vnti.m_vIndex];
      tmp.m_n         = (vnti.m_nIndex<0) ? nullptr : &m_normals[           vnti.m_nIndex];
      tmp.m_t         = (vnti.m_tIndex<0) ? nullptr : &m_textureVertexArray[vnti.m_tIndex];
    }
  }
  const size_t n = tmpArray.size();
  if(n > 0) {
    tmpArray.sort(tmpVertexCmp);
    TmpVNTI                        *lastTmp = &tmpArray[0];
    const VertexNormalTextureIndex *lastVn  = &m_faceArray[lastTmp->m_faceIndex].m_data[lastTmp->m_index];
    for(size_t i = 1; i < n; i++) {
      TmpVNTI &tmp = tmpArray[i];
      if(tmp == *lastTmp) {
        VertexNormalTextureIndex &vn = m_faceArray[tmp.m_faceIndex].m_data[tmp.m_index];
        vn.m_vIndex = lastVn->m_vIndex;
        vn.m_nIndex = lastVn->m_nIndex;
        vn.m_tIndex = lastVn->m_tIndex;
      } else {
        lastTmp = &tmp;
        lastVn  = &m_faceArray[tmp.m_faceIndex].m_data[tmp.m_index];
      }
    }
    pruneUnused();
  }
  return *this;
}

void MeshBuilder::pruneUnused() {
  const UINT vCount = getVertexCount(), nCount = getNormalCount(), tCount = getTextureCount();
  BitSet unusedVertices(vCount+1), unusedNormals( nCount+1), unusedTextures(tCount+1);
  if(vCount > 0) unusedVertices.add(0, vCount-1);
  if(nCount > 0) unusedNormals.add( 0, nCount-1);
  if(tCount > 0) unusedTextures.add(0, tCount-1);
  const UINT faceCount = (UINT)m_faceArray.size();
  for(UINT f = 0; f < faceCount; f++) {
    const Face      &face    = m_faceArray[f];
    const VNTIArray &vnArray = face.getIndexArray();
    for(const VertexNormalTextureIndex vnt : vnArray) {
      unusedVertices.remove(vnt.m_vIndex);
      if(vnt.m_nIndex >= 0) unusedNormals.remove( vnt.m_nIndex);
      if(vnt.m_tIndex >= 0) unusedTextures.remove(vnt.m_tIndex);
    }
  }
  const UINT unusedVCount = (UINT)unusedVertices.size(), unusedNCount = (UINT)unusedNormals.size(), unusedTCount = (UINT)unusedTextures.size();

#define TR_VERTEX  0x01
#define TR_NORMAL  0x02
#define TR_TEXTURE 0x04
  const BYTE translateFlag = (unusedVCount?TR_VERTEX:0) | (unusedNCount?TR_NORMAL:0) | (unusedTCount?TR_TEXTURE:0);

  if(translateFlag) {
    CompactIntArray vTranslate, nTranslate, tTranslate;

    if(translateFlag&TR_VERTEX) {
      vTranslate.setCapacity(vCount);
      UINT lastIndex = 0;
      for(UINT i = 0; i < vCount; i++) {
        if(unusedVertices.contains(i)) {
          vTranslate.add(-1);
        } else {
          vTranslate.add(lastIndex++);
        }
      }
    }

    if(translateFlag&TR_NORMAL) {
      nTranslate.setCapacity(nCount);
      UINT lastIndex = 0;
      for(UINT i = 0; i < nCount; i++) {
        if(unusedNormals.contains(i)) {
          nTranslate.add(-1);
        } else {
          nTranslate.add(lastIndex++);
        }
      }
    }

    if(translateFlag&TR_TEXTURE) {
      tTranslate.setCapacity(tCount);
      UINT lastIndex = 0;
      for(UINT i = 0; i < tCount; i++) {
        if(unusedTextures.contains(i)) {
          tTranslate.add(-1);
        } else {
          tTranslate.add(lastIndex++);
        }
      }
    }

    for(UINT f = 0; f < faceCount; f++) {
      Face &face = m_faceArray[f];
      VNTIArray &vnArray = face.m_data;
      const UINT n       = face.getIndexCount();
      if(n) {
        for(VertexNormalTextureIndex *vp = &vnArray.first(), *endp = vp + n; vp < endp; vp++) {
          if(translateFlag & TR_VERTEX) {
            vp->m_vIndex = vTranslate[vp->m_vIndex];
          }
          if((translateFlag & TR_NORMAL) && (vp->m_nIndex >= 0)) {
            vp->m_nIndex = nTranslate[vp->m_nIndex];
          }
          if((translateFlag & TR_TEXTURE) && (vp->m_tIndex >= 0)) {
            vp->m_tIndex = tTranslate[vp->m_tIndex];
          }
        }
      }
    }
    vTranslate.clear();
    nTranslate.clear();
    tTranslate.clear();

    if(translateFlag & TR_VERTEX) {
      const VertexArray tmp = m_vertices;
      m_vertices.clear(vCount - unusedVCount);
      for(UINT i = 0; i < vCount; i++) {
        if(!unusedVertices.contains(i)) {
          m_vertices.add(tmp[i]);
        }
      }
    }

    if(translateFlag & TR_NORMAL) {
      const VertexArray tmp = m_normals;
      m_normals.clear(nCount - unusedNCount);
      for(UINT i = 0; i < nCount; i++) {
        if(!unusedNormals.contains(i)) {
          m_normals.add(tmp[i]);
        }
      }
    }

    if(translateFlag & TR_TEXTURE) {
      const TextureVertexArray tmp = m_textureVertexArray;
      m_textureVertexArray.clear(tCount - unusedTCount);
      for(UINT i = 0; i < tCount; i++) {
        if(!unusedTextures.contains(i)) {
          m_textureVertexArray.add(tmp[i]);
        }
      }
    }
  }
//#define DUMP_PRUNECOUNT
#if defined(DUMP_PRUNECOUNT)
  debugLog(_T("Pruned %s vertices, %s normals, %s texturevertices\n")
          ,format1000(unusedVCount).cstr()
          ,format1000(unusedNCount).cstr()
          ,format1000(unusedTCount).cstr()
          );
#endif // DUMP_PRUNECOUNT
}
