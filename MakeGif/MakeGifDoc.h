#pragma once

#include <Date.h>
#include <PropertyContainer.h>
#include "PixRectArray.h"

class ImageSettings {
public:
  double               m_imageScaleFactor;
  unsigned int         m_colorCount;
  ImageSettings();
};

typedef enum {
  SAVE_TIME
 ,UPDATE_TIME
 ,RAW_IMAGE_LIST
 ,SCALED_IMAGE_LIST
 ,QUANTIZED_IMAGE_LIST
} DocumentProperty;

class CMakeGifDoc : public CDocument, public PropertyContainer, public PropertyChangeListener {
private:
  String               m_name;
  int                  m_fileSize;
  void setDocName(const String &name = EMPTYSTRING);
  Timestamp            m_lastUpdate, m_lastSave;
  GifFileType         *m_gif;
  PixRectArray         m_rawPrArray;
  mutable PixRectArray m_scaledPrArray, m_quantizedPrArray;
  ImageSettings        m_imageSettings;
  void updateTimestamp();
  void setSaveTime(const Timestamp &t);
  void newGif();
protected:
  CMakeGifDoc();
  DECLARE_DYNCREATE(CMakeGifDoc)

public:
  void loadGif(const String &fileName);
  void saveGif(const String &fileName); // return fileSize
  void closeGif();
  bool hasDefaultName() const;
  const String getName() const {
    return m_name;
  }
  inline bool hasGifFile() const {
    return m_gif != NULL;
  }
  const GifFileType *getGifFile() const {
    return m_gif;
  }
  inline int getFileSize() const {
    return m_fileSize;
  }
  bool needSave() const;
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

  void setImageSettings(const ImageSettings &settings);
  const ImageSettings &getImageSettings() const {
    return m_imageSettings;
  }
  const Timestamp &getLastUpdate() {
    return m_lastUpdate;
  }
  void addPixRectArray(const PixRectArray &prArray);
  bool removeAllImages();

  const PixRectArray &getRawPrArray() const {
    return m_rawPrArray;
  }
  const PixRectArray &getScaledPrArray() const;
  const PixRectArray &getQuantizedPrArray() const;

  CSize getGifSize() const;
  int   getImageCount() const {
    return (int)m_rawPrArray.size();
  }

  bool addImagesToGif();
public:

public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

public:
    virtual ~CMakeGifDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
    DECLARE_MESSAGE_MAP()
};

