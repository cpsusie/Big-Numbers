#pragma once

#include "AnimatedImage.h"

class CGifCtrl : public CStatic {
private:
    DECLARECLASSNAME;
    AnimatedImage m_image;
    bool          m_isPainted : 1;
    bool          m_suspended : 1; // set to true when window becomes invisible. so thread can be restarted when it becomes visible again
    int           m_paintedIndex;  // only valid if m_isPainted = true. -1 for all
    void suspend();
    void resume();
public:
    CGifCtrl();

public:
protected:
#ifdef _DEBUG
    void trace(const char *format, ...);
#endif
public:
    void substituteControl(CWnd *wnd, int id);
    virtual ~CGifCtrl();

    void load(const String &fileName);
    void loadFromResource(int resId, const String &typeName);
    void loadFromGifFile(const GifFileType *gifFile);
    void unload();
    void play(bool force = false);
    void stop();
    void paintFrame(UINT index);
    void paintAllFrames();
    void hide();
    inline bool isPlaying() const {
      return m_image.isPlaying();
    }
    inline bool isLoaded() const {
      return m_image.isLoaded();
    }
    inline bool isPainted() const {
      return m_isPainted;
    }
    inline bool isSuspended() const {
      return m_suspended;
    }
    inline const CSize &getImageSize() const {
      return m_image.getSize();
    }
    inline int getFrameCount() const {
      return m_image.getFrameCount();
    }
protected:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);

  DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
