#pragma once

#include <MFCUtil/AnimationParameters.h>

class D3SurfaceCommonParameters {
public:
  bool                m_calculateNormals;
  bool                m_calculateTexture;
  String              m_textureFileName;
  bool                m_doubleSided;
  AnimationParameters m_animation;

  inline bool isAnimated() const {
    return m_animation.includeTime();
  }
  inline bool calculateTexture() const {
    return m_calculateTexture;
  }
  inline bool hasTextureFileName() const {
    return m_textureFileName.length() != 0;
  }
  inline bool calculateNormals() const {
    return m_calculateNormals;
  }
  DWORD getFVF() const;
  D3SurfaceCommonParameters();
};
