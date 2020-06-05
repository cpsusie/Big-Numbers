#pragma once

class D3SurfaceVertexParameters {
public:
  bool                m_hasNormals;
  bool                m_hasTexture;
  String              m_textureFileName;
  DWORD getFVF() const;
  D3SurfaceVertexParameters();
};
