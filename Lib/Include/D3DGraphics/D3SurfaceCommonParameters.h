#pragma once

#include <MFCUtil/AnimationParameters.h>

class D3SurfaceCommonParameters {
public:
  bool                m_hasNormals;
  bool                m_hasTexture;
  String              m_textureFileName;
  bool                m_doubleSided;
  AnimationParameters m_animation;

  inline bool isAnimated() const {
    return m_animation.includeTime();
  }
  inline bool hasTexture() const {
    return m_hasTexture;
  }
  inline bool hasNormals() const {
    return m_hasNormals;
  }
  DWORD getFVF() const;
  D3SurfaceCommonParameters();
};
