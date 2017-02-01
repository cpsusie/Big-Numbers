#pragma once

#include <HashMap.h>
#include <TreeMap.h>
#include <QueueList.h>
#include <Thread.h>
#include <FileTreeWalker.h>
#include <ByteStream.h>
#include <Date.h>
#include "ProgressDlgThread.h"

class GenreMap : public StringHashMap<int> {
public:
  GenreMap();
  int *get(const String &genre);
};

class MusicDir {
private:
  bool   m_active;
  String m_dir;
public:
  MusicDir(bool active, const String &dir) {
    m_active = active;
    m_dir    = dir;
  }
  const String &getDir() const {
    return m_dir;
  }
  bool isActive() const {
    return m_active;
  }
};

class MusicDirList : public Array<MusicDir> {
private:
  void init(const String &dir);
public:
  MusicDirList() {
  }
  MusicDirList(const String &dir);
  String toString() const;
};

class MediaAttribute {
private:
  const String m_name;
  String       m_value;
  const bool   m_readOnly;
  friend class AttributeArray;
public:
  MediaAttribute(const TCHAR *name, const TCHAR *value, bool readOnly);

  const TCHAR *getName() const {
    return m_name.cstr();
  }
  const TCHAR *getValue() const {
    return m_value.cstr();
  }
  bool isReadOnly() const {
    return m_readOnly;
  }
  void setValue(const String &value);
  ULONG hashCode() const;
  friend bool operator==(const MediaAttribute &a1, const MediaAttribute &a2);
  friend bool operator!=(const MediaAttribute &a1, const MediaAttribute &a2);
};

class AttributeArray : public Array<MediaAttribute> {
private:
  int m_maxNameLength;
  int m_maxValueLength;
  void refreshMaxWidth();

public:
  AttributeArray();

  void add(const MediaAttribute &attr);
  int getMaxNameLength() const {
    return m_maxNameLength;
  }
  int getMaxValueLength() const {
    return m_maxValueLength;
  }
  int findByName(const String &name) const;
  void removeAttribute(const String &name);
};

#define SELECT_READONLY  0x01
#define SELECT_READWRITE 0x02
#define SELECT_EMPTY     0x04

class MediaFile {
private:
  static GenreMap m_genreMap;

  mutable CWMPMedia m_media;
  String            m_sourceURL;
  String            m_title;
  String            m_artist;
  String            m_album;
  String            m_composer;
  String            m_genre;
  short             m_trackNumber;
  double            m_duration;
  bool              m_protected;
  static void read( ByteInputStream  &s,       String &str);
  static void write(ByteOutputStream &s, const String &str);
  void init();
public:
  MediaFile();
  MediaFile(const TCHAR *sourceURL, time_t modified);
  MediaFile(CWMPMedia media);
  const TCHAR *getSourceURL()   const { return m_sourceURL.cstr(); }
  const TCHAR *getTitle()       const { return m_title.cstr();     }
  const TCHAR *getArtist()      const { return m_artist.cstr();    }
  const TCHAR *getAlbum()       const { return m_album.cstr();     }
  const TCHAR *getComposer()    const { return m_composer.cstr();  }
  const TCHAR *getGenre()       const { return m_genre.cstr();     }
  UINT getFileSize()    const;
  int          getTrackNumber() const { return m_trackNumber;      }
  bool         getProtected()   const { return m_protected;        }
  double getDuration() const {
    return m_duration;
  }
  String getDurationString() const;
  time_t m_modified;
  bool   m_hasPlayed;
  void read( ByteInputStream  &s);
  void write(ByteOutputStream &s) const;
  void update(const AttributeArray &attributes);
  void clear();
  bool isDefined() const {
    return m_sourceURL.length() > 0;
  }
  AttributeArray getAttributes(int flags = SELECT_READONLY | SELECT_READWRITE | SELECT_EMPTY) const;
  CWMPMedia getMedia() const;
};

bool operator==(const MediaFile &f1,const MediaFile &f2);

void logError(const TCHAR *format,...);
String makeFileName(const String &ext);
String makeErrorFileName();
String makeLogFileName();

class MediaDatabase : public StringTreeMap<MediaFile> {
private:
  static String getFileName();
  static CWMPPlayer4        *player;
  static CWMPMediaCollection mediaCollection;
  static CWMPPlaylist        playlist; // mediaplayer takes the mediafiles from this list. Only one exist
  bool             m_changed;
  static Semaphore m_gate;

  void load(ByteInputStream  &s);
  void save(ByteOutputStream &s);
  void load();
  void saveAndClear();
public:
  void add(const MediaFile &mf);
  static void update(const MediaFile &mf);

  static CWMPPlayer4         &getPlayer();
  static CWMPMediaCollection &getMediaCollection();
  static CWMPPlaylist        &getPlaylist();
  static CWMPPlaylist        createPlayList(const String &name);

  static void open(CWMPPlayer4 &player);
  static void close();
  static void wait();
  static void notify();
  MediaFile *search(const TCHAR *fname);
};

class MediaArray : public Array<MediaFile> {
private:
  mutable Semaphore m_lock;
public:
  MediaArray() {
  }
  MediaArray(const MediaArray &src);
  MediaArray &operator=(const MediaArray &src);
  explicit MediaArray(const MediaFile &mf);
  int findBySourceURL(const String &sourceURL) const;
  const MediaFile *selectRandom() const;
  AttributeArray getAttributes(int flags = SELECT_READONLY | SELECT_READWRITE | SELECT_EMPTY) const;
  void wait() const {
    m_lock.wait();
  }
  void notify() const {
    m_lock.signal();
  }
  void clear();
  double getDuration() const;
};

class LoadableMediaArray;

class ScannerThread : public Thread {
private:
  MusicDirList        m_dirList;
  bool                m_recurse;
  bool               *m_busy;
  LoadableMediaArray *m_mediaArray;
  void doScan();
public:
  ScannerThread();
  void startScan(const String &dir, bool recurse, bool &busy, LoadableMediaArray *mediaArray);
  UINT run();
};

class LoadableMediaArray : public MediaArray, public FileNameHandler {
private:
  CWMPMediaCollection &m_mediaCollection;
  CProgressDlgThread  *m_progressThread;
  ScannerThread        m_scannerThread;
  bool                 m_cancelled;
  bool                 m_busy;
  String               m_dir;
  TCHAR                m_currentFileName[MAX_PATH];
  void initFileName();
public:
  LoadableMediaArray();
  LoadableMediaArray(const LoadableMediaArray &src); // not defined
  bool add(const MediaFile &f);
  void rescan(const String &dir, bool recurse = true);
  void handleFileName(const TCHAR *name, DirListEntry &info);

  const TCHAR *getCurrentFileName() const {
    return m_currentFileName;
  }

  void cancelScan() {
    m_cancelled = true;
  }

  bool isCancelled() const {
    return m_cancelled;
  }

  bool isBusy() const {
    return m_busy;
  }
  void stopProgressThread();
  const String &getDir() const {
    return m_dir;
  }
};

typedef enum {
  CMP_ARTIST,
  CMP_TITLE,
  CMP_ALBUM,
  CMP_FNAME
} CompareField;

class ComparatorCriteria {
public:
  CompareField m_compareField;
  int          m_headerIndex;
  bool         m_asc;
  ComparatorCriteria();
};

bool operator==(const ComparatorCriteria &c1, const ComparatorCriteria &c2);
bool operator!=(const ComparatorCriteria &c1, const ComparatorCriteria &c2);

class MediaComparator : public Comparator<MediaFile> {
private:
  ComparatorCriteria m_sortCrit;
public:
  int compare(const MediaFile &mf1, const MediaFile &mf2);

  MediaComparator(CompareField field = CMP_ARTIST, bool asc = true);

  void setHeaderIndex(int index, bool asc = true);
  inline int getHeaderIndex() const {
    return m_sortCrit.m_headerIndex;
  }
  inline bool getAscending() const {
    return m_sortCrit.m_asc;
  }
  const ComparatorCriteria &getComparatorCriteria() const {
    return m_sortCrit;
  }
  const TCHAR *getCompareString(const MediaFile &mf) const;

  AbstractComparator *clone() const {
    return new MediaComparator(m_sortCrit.m_compareField, m_sortCrit.m_asc);
  }
};

class MediaFileWithQueueId : public MediaFile {
private:
  const int m_queueId;
  static int idCounter;
public:
  MediaFileWithQueueId(const MediaFile &mf) : MediaFile(mf), m_queueId(idCounter++) {
  }
  int getQueueId() const {
    return m_queueId;
  }
};

class MediaQueue : public QueueList<MediaFileWithQueueId> {
private:
  CString      m_lastPlayedURL;
  int          m_lastPlayedQueueId;
  double       m_duration;
  UINT         m_transactionCounter;
  UINT         m_lastSync, m_lastUpdate;
  void updateLastPlayedQueueId();
public:
  MediaQueue();
  void put(const MediaFileWithQueueId &mf);
  bool contains(const MediaFile &mf) const;
  MediaFileWithQueueId get();
  MediaFileWithQueueId get(int index);
  const MediaFileWithQueueId &peek(size_t index = 0) const;
  double getDuration() const;
  bool syncPlayList(bool shiftToNext);
  void clear();
  void move(int from, int to);
};
