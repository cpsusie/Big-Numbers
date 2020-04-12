#include "pch.h"
#include <HashMap.h>
#include <D3DGraphics/MeshBuilder.h>

typedef enum {
  TAG_GEOMETRIC_VERTEX
 ,TAG_TEXTURE_VERTEX
 ,TAG_NORMAL
 ,TAG_POINT
 ,TAG_LINE
 ,TAG_FACE
 ,TAG_GROUP
 ,TAG_SMOOTHINGGROUP
 ,TAG_MATERIAL
 ,TAG_MATERIALLIB
} ObjTag;

typedef StrHashMap<ObjTag> HashMapType;

typedef struct {
  const ObjTag m_tag;
  const TCHAR *m_str;
} ObjKeyword;

static ObjKeyword keywordTable[] = {
  TAG_GEOMETRIC_VERTEX  , _T("v")
 ,TAG_TEXTURE_VERTEX    , _T("vt")
 ,TAG_NORMAL            , _T("vn")
 ,TAG_POINT             , _T("p")
 ,TAG_LINE              , _T("l")
 ,TAG_FACE              , _T("f")
 ,TAG_GROUP             , _T("g")
 ,TAG_SMOOTHINGGROUP    , _T("s")
 ,TAG_MATERIAL          , _T("usemtl")
 ,TAG_MATERIALLIB       , _T("mtllib")
};

class ObjKeyWordMap : public HashMapType {
public:
  ObjKeyWordMap() : HashMapType(23) {
    for(int i = 0; i < ARRAYSIZE(keywordTable); i++) {
      put(keywordTable[i].m_str,keywordTable[i].m_tag);
    }
  }
};

static ObjKeyWordMap keywords;

void MeshBuilder::parseWavefrontObjFile(FILE *f) {
  TCHAR line[1000];
  UINT lineCount = 0;
  while(_fgetts(line, ARRAYSIZE(line), f)) {
    lineCount++;
    strTrim(line);
    switch(line[0]) {
    case _T('#'): // comment
    case 0      : // empty line
      continue;
    }
    Tokenizer tok(line, _T(" "));
    const ObjTag *tag = keywords.get(tok.next().cstr());
    if(tag == NULL) {
      throwException(_T("Illegal input in line %d:%s"), lineCount, line);
    }
    switch(*tag) {
    case TAG_GEOMETRIC_VERTEX  :
      { Vertex v;
        if(_stscanf(tok.getRemaining().cstr(), _T("%f %f %f"), &v.x, &v.y, &v.z) != 3) {
          throwException(_T("Expected 3 coordinates for vertex in line %d:%s"), lineCount, line);
        }
        addVertex(v);
      }
      break;
    case TAG_TEXTURE_VERTEX    :
      { TextureVertex vt;
        if(_stscanf(tok.getRemaining().cstr(), _T("%f %f"), &vt.u, &vt.v) != 2) {
          throwException(_T("Expected 2 coordinates for texturevertex in line %d:%s"), lineCount, line);
        }
        addTextureVertex(vt);
      }
      break;
    case TAG_NORMAL            :
      { Vertex n;
        if(_stscanf(tok.getRemaining().cstr(), _T("%f %f %f"), &n.x, &n.y, &n.z) != 3) {
          throwException(_T("Expected 3 coordinates for normal in line %d:%s"), lineCount, line);
        }
        addNormal(n);
      }
      break;
    case TAG_POINT             :
    case TAG_LINE              :
      break;
    case TAG_FACE              :
      { Face &f = addFace();
        while(tok.hasNext()) {
          const String s = tok.next();
          int v,t,n;
          if(_stscanf(s.cstr(), _T("%d/%d/%d"), &v, &t, &n) == 3) {
            if(v < 0) adjustNegativeVertexIndex( v); else v--;
            if(t < 0) adjustNegativeTextureIndex(t); else t--;
            if(n < 0) adjustNegativeNormalIndex( n); else n--;
            f.addVertexNormalTextureIndex(v,n,t);
          } else if(_stscanf(s.cstr(), _T("%d//%d"), &v, &n) == 2) {
            if(v < 0) adjustNegativeVertexIndex( v); else v--;
            if(n < 0) adjustNegativeNormalIndex( n); else n--;
            f.addVertexNormalIndex(v,n);
          } else if(_stscanf(s.cstr(), _T("%d/%d"), &v, &t) == 2) {
            if(v < 0) adjustNegativeVertexIndex( v); else v--;
            if(t < 0) adjustNegativeTextureIndex(t); else t--;
            f.addVertexTextureIndex(v,t);
          } else {
            throwException(_T("Invalid format for vertexData in line %d:<%s>"), lineCount, s.cstr());
          }
        }
        f.invertOrientation();
      }
      break;
    case TAG_GROUP             :
      break;

    case TAG_SMOOTHINGGROUP    :
      break;
    case TAG_MATERIAL          :
      break;
    case TAG_MATERIALLIB       :
      break;
    default                    :
      throwException(_T("Unknown tag in line %d:%s"), lineCount, line);
    }
  }
}

LPD3DXMESH createMeshFromObjFile(AbstractMeshFactory &amf, const String &fileName, bool doubleSided) {
  FILE *f = NULL;
  try {
    f = FOPEN(fileName, _T("r"));
    MeshBuilder mb;
    mb.parseWavefrontObjFile(f);
    fclose(f);
    return mb.createMesh(amf, false);
  } catch(...) {
    if(f) {
      fclose(f);
    }
    throw;
  }
}
