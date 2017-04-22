#include "stdafx.h"
#include <Tokenizer.h>
#include <ByteFile.h>
#include <CompressFilter.h>
#include "ProgressDlgThread.h"
#include "MediaDatabase.h"

#define HEADERSTRING "mediahash"
#define CURRENTVERSION 3

class Header {
public:
  char  m_headerString[10];
  int   m_version;
  UINT  m_count;
  Header();
  Header(int version, UINT count);
};

Header::Header(int version, UINT count) {
  strcpy(m_headerString,HEADERSTRING);
  m_version = version;
  m_count   = count;
}

Header::Header() {
  memset(m_headerString,0,sizeof(m_headerString));
  m_version = 0;
  m_count   = 0;
}

String makeFileName(const String &ext) {
  return FileNameSplitter(getModuleFileName()).setExtension(ext).getFullPath();
}

String makeErrorFileName() {
  return makeFileName(_T("merr"));
}

String makeLogFileName() {
  return makeFileName(_T("mlog"));
}

String MediaDatabase::getFileName() { // static
  return makeFileName(_T("dat"));
}

void MediaDatabase::load(ByteInputStream &s) {
  m_changed = false;
  clear();

  Header header;
  s.getBytesForced((BYTE*)&header, sizeof(header));
  if(strcmp(header.m_headerString,HEADERSTRING) != 0 || (header.m_version != CURRENTVERSION)) {
    return;
  }

  for(UINT i = 0; i < header.m_count; i++) {
    MediaFile mf;
    mf.read(s);
    add(mf);
  }
  m_changed = false;
}

void MediaDatabase::save(ByteOutputStream &s) {
  Header header(CURRENTVERSION, (UINT)size());
  s.putBytes((BYTE*)&header, sizeof(header));
  for(Iterator<MediaFile> it = values().getIterator(); it.hasNext(); ) {
    it.next().write(s);
  }
}

void MediaDatabase::load() {
  try {
    load(DecompressFilter(ByteInputFile(getFileName())));
  } catch(Exception e) {
    // ignore
  }
}

void MediaDatabase::saveAndClear() {
  if(!m_changed) {
    return;
  }
  save(CompressFilter(ByteOutputFile(getFileName())));
  clear();
}

void MediaDatabase::add(const MediaFile &mf) {
  remove(toLowerCase(mf.getSourceURL()));
  if(put(toLowerCase(mf.getSourceURL()),mf)) {
    m_changed = true;
  }
}

MediaFile *MediaDatabase::search(const TCHAR *fname) {
  return get(toLowerCase(fname));
}

void MediaDatabase::wait() {
  m_gate.wait();
}

void MediaDatabase::notify() {
  m_gate.signal();
}

static MediaDatabase mediaDatabase;
Semaphore            MediaDatabase::m_gate;
CWMPPlayer4         *MediaDatabase::player          = NULL;
CWMPMediaCollection  MediaDatabase::mediaCollection = NULL;
CWMPPlaylist         MediaDatabase::playlist        = NULL;

void MediaDatabase::open(CWMPPlayer4 &player) { // static
  wait();
  try {
    MediaDatabase::player = &player;
    mediaDatabase.load();
    notify();
  } catch(...) {
    notify();
    throw;
  }
}

void MediaDatabase::close() {
  wait();
  try {
    mediaDatabase.saveAndClear();

    if(playlist != NULL) {
      playlist.clear();
      playlist = NULL;
    }
    if(mediaCollection != NULL) {
      mediaCollection = NULL;
    }
    player = NULL;
    notify();
  } catch(...) {
    notify();
    throw;
  }
}

CWMPPlayer4 &MediaDatabase::getPlayer() { // static
  if(player == NULL) {
    throwException(_T("MediaDatabase not initialized"));
  }
  return *player;
}

CWMPMediaCollection &MediaDatabase::getMediaCollection() { // static
  if(mediaCollection == NULL) {
    mediaCollection = getPlayer().GetMediaCollection();
  }
  return mediaCollection;
}

static const TCHAR *queueName = _T("partymakerqueue");

CWMPPlaylist &MediaDatabase::getPlaylist() { // static
  if(playlist == NULL) {
    CWMPPlayer4           &player       = MediaDatabase::getPlayer();
    CWMPPlaylistCollection plCollection = player.GetPlaylistCollection();
    CWMPPlaylistArray      plArray      = plCollection.getByName(queueName);
    if(plArray.GetCount() > 0) {
      playlist = plArray.Item(0);
      playlist.clear();
    } else {
      playlist = plCollection.newPlaylist(queueName);
    }
    player.SetCurrentPlaylist(playlist);
  }
  return playlist;
}

CWMPPlaylist MediaDatabase::createPlayList(const String &name) { // static
  if(name.equalsIgnoreCase(queueName)) {
    throwException(_T("Cannot use name of queue (=%s) as name for playlist"), queueName);
  }
  CWMPPlayer4           &player       = MediaDatabase::getPlayer();
  CWMPPlaylistCollection plCollection = player.GetPlaylistCollection();
  CWMPPlaylistArray      plArray      = plCollection.getByName(name.cstr());
  CWMPPlaylist           result;
  if(plArray.GetCount() > 0) {
    result = plArray.Item(0);
    result.clear();
  } else {
    result = plCollection.newPlaylist(name.cstr());
  }
  return result;
}

void MediaDatabase::update(const MediaFile &mf) { // static
  MediaFile *cf = mediaDatabase.search(mf.getSourceURL());
  if(cf == NULL) {
    return;
  }
  *cf = mf;
  mediaDatabase.m_changed = true;
}

ComparatorCriteria::ComparatorCriteria() {
  m_compareField = CMP_FNAME;
  m_headerIndex  = -1;
  m_asc          = true;
}

bool operator==(const ComparatorCriteria &c1, const ComparatorCriteria &c2) {
  return (c1.m_headerIndex == c2.m_headerIndex) && (c1.m_asc == c2.m_asc);
}

bool operator!=(const ComparatorCriteria &c1, const ComparatorCriteria &c2) {
  return !(c1 == c2);
}

MediaComparator::MediaComparator(CompareField field, bool asc ) {
  m_sortCrit.m_compareField = field;
  m_sortCrit.m_asc          = asc;
  switch(field) {
  case CMP_TITLE : m_sortCrit.m_headerIndex =  0; break;
  case CMP_ARTIST: m_sortCrit.m_headerIndex =  1; break;
  case CMP_ALBUM : m_sortCrit.m_headerIndex =  2; break;
  default        : m_sortCrit.m_headerIndex = -1; break;
  }
}

static const CompareField compareField[] = { CMP_TITLE, CMP_ARTIST, CMP_ALBUM };

void MediaComparator::setHeaderIndex(int index, bool asc) {
  if(index < 0 || index >= ARRAYSIZE(compareField)) {
    throwException(_T("setHeaderIndex:index=%d. Must be [0..%d]"), index, ARRAYSIZE(compareField)-1);
  }
  m_sortCrit.m_compareField = compareField[index];
  m_sortCrit.m_headerIndex  = index;
  m_sortCrit.m_asc          = asc;
}

const TCHAR *MediaComparator::getCompareString(const MediaFile &mf) const {
  switch(m_sortCrit.m_compareField) {
  case CMP_ARTIST: return mf.getArtist();
  case CMP_TITLE : return mf.getTitle();
  case CMP_ALBUM : return mf.getAlbum();
  case CMP_FNAME : return mf.getSourceURL();
  }
  return mf.getArtist();
}

int MediaComparator::compare(const MediaFile &mf1, const MediaFile &mf2) {
  int a = 0;
  switch(m_sortCrit.m_compareField) {
  case CMP_FNAME  :
    break;

  case CMP_TITLE  :
    a = streaicmp(   mf1.getTitle() ,mf2.getTitle() );
    break;

  case CMP_ALBUM  :
    a = streaicmp(   mf1.getAlbum() ,mf2.getAlbum() );
    if(a) break;
    a = mf1.getTrackNumber() - mf2.getTrackNumber();
    if(a) break;;
    a = streaicmp(   mf1.getArtist(),mf2.getArtist());
    if(a) break;
    a = streaicmp(   mf1.getTitle() ,mf2.getTitle() );
    break;

  case CMP_ARTIST :
  default         :
    a = streaicmp(   mf1.getArtist(),mf2.getArtist());
    if(a) break;
    a = streaicmp(   mf1.getAlbum() ,mf2.getAlbum() );
    if(a) break;
    a = mf1.getTrackNumber() - mf2.getTrackNumber();
    if(a) break;
    a = streaicmp(   mf1.getTitle() ,mf2.getTitle() );
    break;
  }
  if(a == 0) {
    a = _tcsicmp(mf1.getSourceURL() ,mf2.getSourceURL() );
  }

  return m_sortCrit.m_asc ? a : -a;
}

void logError(const TCHAR *format,...) {
  FILE *f = fopen(makeErrorFileName(), _T("a"));
  if(f == NULL) {
    return;
  }

  _ftprintf(f, _T("%s"), Timestamp().toString().cstr());

  va_list argptr;
  va_start(argptr,format);
  _vftprintf(f, format, argptr);
  va_end(argptr);

  _ftprintf(f,_T("\n"));
  fclose(f);
}

MediaArray::MediaArray(const MediaArray &src) {
  src.wait();
  addAll(src);
  src.notify();
}

MediaArray::MediaArray(const MediaFile &mf) {
  add(mf);
}

MediaArray &MediaArray::operator=(const MediaArray &src) {
  if(this == &src) {
    return *this;
  }
  wait();
  Array<MediaFile>::clear();
  src.wait();
  addAll(src);
  src.notify();
  notify();
  return *this;
}

void MediaArray::clear() {
  wait();
  Array<MediaFile>::clear();
  notify();
}

double MediaArray::getDuration() const {
  wait();
  double result = 0;
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    result += (*this)[i].getDuration();
  }
  notify();
  return result;
}

int MediaArray::findBySourceURL(const String &sourceURL) const {
  wait();
  intptr_t found = -1;
  const TCHAR *s = sourceURL.cstr();
  for(size_t i = 0; i < size(); i++) {
    if(_tcsicmp((*this)[i].getSourceURL(), s) == 0) {
      found = i;
      break;
    }
  }
  notify();
  return (int)found;
}

const MediaFile *MediaArray::selectRandom() const {
  const MediaFile *mf = NULL;
  wait();
  if(size() > 0) {
    for(int k = 0; k < 20; k++) {
      mf = &select();
      if(!mf->m_hasPlayed) {
        break;
      }
    }
  }
  notify();
  return mf;
}

class AttributeSet : public HashSet<MediaAttribute> {
public:
  AttributeSet();
  AttributeSet(const Collection<MediaAttribute> &c);
  AttributeSet &operator*=(const AttributeSet &s);
  operator AttributeArray() const;
};

static ULONG attributeHash(const MediaAttribute &a) {
  return a.hashCode();
}

static int attributeCmp(const MediaAttribute &a1, const MediaAttribute &a2) {
  return (a1 == a2) ? 0 : 1;
}

static int attributeNameCmp(const MediaAttribute &a1, const MediaAttribute &a2) {
  return _tcsicmp(a1.getName(), a2.getName());
}

AttributeSet::AttributeSet() : HashSet<MediaAttribute>(attributeHash, attributeCmp) {
}

AttributeSet::AttributeSet(const Collection<MediaAttribute> &c) : HashSet<MediaAttribute>(attributeHash, attributeCmp) {
  addAll(c);
}

AttributeSet &AttributeSet::operator*=(const AttributeSet &s) {
  *this = *this * s;
  return *this;
}

AttributeSet::operator AttributeArray() const {
  AttributeArray a;
  a.addAll(*this);
  a.sort(attributeNameCmp);
  return a;
}

AttributeArray MediaArray::getAttributes(int flags) const {
  if(size() == 0) {
    return AttributeArray();
  }
  wait();
  AttributeSet aset = (*this)[0].getAttributes(flags);
  for(size_t i = 1; i < size(); i++) {
    aset *= (*this)[i].getAttributes(flags);
  }
  notify();
  return aset;
}

ScannerThread::ScannerThread() {
  setDeamon(true);
}

void ScannerThread::startScan(const String &dir, bool recurse, bool &busy, LoadableMediaArray *mediaArray) {
  m_dirList    = dir;
  m_recurse    = recurse;
  m_busy       = &busy;
  m_mediaArray = mediaArray;
  resume();
}

UINT ScannerThread::run() {
  for(;;) {
    doScan();
    *m_busy = false;
    suspend();
  }
}

void ScannerThread::doScan() {
  try {
    m_dirList = m_mediaArray->getDir();

    TCHAR *extensions[] = { _T("*.mp3"),_T("*.wma"), _T("*.wav"), NULL };

    for(size_t i = 0; i < m_dirList.size(); i++) {
      if(m_dirList[i].isActive()) {
        if(m_recurse) {
          FileTreeWalker::walkFileTree(m_dirList[i].getDir(), *m_mediaArray, extensions);
        } else {
          FileTreeWalker::walkDir(m_dirList[i].getDir(), *m_mediaArray, extensions);
        }
      }
      if(m_mediaArray->isCancelled()) {
        break;
      }
    }

    if(!m_mediaArray->isCancelled()) {
      m_mediaArray->stopProgressThread();
    }
  } catch(Exception e) {
    logError(_T("%s"), e.what());
  }
}

LoadableMediaArray::LoadableMediaArray() : m_mediaCollection(MediaDatabase::getMediaCollection()) {
  m_progressThread = NULL;
  m_busy           = false;
  m_cancelled      = false;
  m_dir            = EMPTYSTRING;
  initFileName();
}

void LoadableMediaArray::initFileName() {
  memset(m_currentFileName,0,sizeof(m_currentFileName));
}

bool LoadableMediaArray::add(const MediaFile &mf) {
  try {
    if(mf.getProtected()) {
      debugLog(_T("rm \"%s\"\n"), mf.getSourceURL());
      return true;
    }
    wait();
    MediaArray::add(mf);
  } catch(...) {
    notify();
    throw;
  }
  notify();
  return true;
}

void LoadableMediaArray::handleFileName(const TCHAR *name, DirListEntry &info) {
  MediaDatabase::wait();
  try {
    _tcscpy(m_currentFileName,name);
    MediaFile *mfp = mediaDatabase.search(name);
    if(mfp != NULL && (mfp->m_modified == info.time_write)) {
      add(*mfp);
    } else {
      try {
        MediaFile mf(name, info.time_write);
        mediaDatabase.add(mf);
        add(mf);
      } catch(...) {
        logError(_T("Error reading %s"),name);
      }
    }
    MediaDatabase::notify();
  } catch(...) {
    MediaDatabase::notify();
    throw;
  }
}

void LoadableMediaArray::rescan(const String &dir, bool recurse) {
  m_busy = true;
  clear();
  initFileName();
  m_dir             = dir;
  m_cancelled       = false;
  m_progressThread  = (CProgressDlgThread*)AfxBeginThread(RUNTIME_CLASS(CProgressDlgThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
  m_progressThread->m_mediaArray = this;

  m_progressThread->ResumeThread();
  m_scannerThread.startScan(m_dir, recurse, m_busy, this);
}

void LoadableMediaArray::stopProgressThread() {
  m_progressThread->PostThreadMessage(WM_QUIT,0,0);
}

MediaQueue::MediaQueue() {
  m_duration           = 0;
  m_lastPlayedQueueId  = -1;
  m_transactionCounter =  0;
  m_lastSync = m_lastUpdate = 0;
}

void MediaQueue::put(const MediaFileWithQueueId &mf) {
  m_duration += mf.getDuration();
  QueueList<MediaFileWithQueueId>::put(mf);
  m_lastUpdate = m_transactionCounter++;
}

bool MediaQueue::contains(const MediaFile &mf) const {
  for(size_t i = 0; i < size(); i++) {
    if((*this)[i] == mf) {
      return true;
    }
  }
  return false;
}

MediaFileWithQueueId MediaQueue::get() {
  MediaFileWithQueueId mf = QueueList<MediaFileWithQueueId>::get();
  m_duration -= mf.getDuration();
  m_lastUpdate = m_transactionCounter++;
  return mf;
}

double MediaQueue::getDuration() const {
  return m_duration;
}

void MediaQueue::clear() {
  QueueList<MediaFileWithQueueId>::clear();
  m_duration = 0;
  m_lastUpdate = m_transactionCounter++;
}

MediaFileWithQueueId MediaQueue::get(int index) {
  Array<MediaFileWithQueueId> tmp;
  while(!isEmpty()) {
    tmp.add(get());
  }
  MediaFileWithQueueId mf = tmp[index];
  tmp.removeIndex(index);
  for(size_t i = 0; i < tmp.size(); i++) {
    put(tmp[i]);
  }
  m_lastUpdate = m_transactionCounter++;
  return mf;
}

const MediaFileWithQueueId &MediaQueue::peek(size_t index) const {
  return (*this)[index];
}

void MediaQueue::move(int from, int to) {
  Array<MediaFileWithQueueId> tmp;
  if((from < 0) || (from >= (int)size()) || (to < 0) || (to > (int)size()) || (to == from)) {
    return;
  }
  while(!isEmpty()) {
    tmp.add(get());
  }
  MediaFileWithQueueId mf = tmp[from];
  tmp.removeIndex(from);
  if(to < from) {
    tmp.add(to,mf);
  } else if(to >= (int)tmp.size()) {
    tmp.add(mf);
  } else {
    tmp.add(to,mf);
  }
  for(size_t i = 0; i < tmp.size(); i++) {
    put(tmp[i]);
  }
  m_lastUpdate = m_transactionCounter++;
}

bool MediaQueue::syncPlayList(bool shiftToNext) {
  MediaDatabase::wait();
  try {
    CWMPMedia currentMedia = MediaDatabase::getPlayer().GetCurrentMedia();
    int firstPendingIndex = 0;
    CWMPPlaylist &playlist = MediaDatabase::getPlaylist();

    if(currentMedia != NULL) {
      m_lastPlayedURL = currentMedia.GetSourceURL();
      const int pn = playlist.GetCount();
      if(pn > 0) { // should always be the case
        CWMPMedia firstMedia = playlist.GetItem(0);
        if(m_lastPlayedURL == firstMedia.GetSourceURL()) {
          firstPendingIndex = 1;
        } else {
          bool found = false;
          int i;
          for(i = 1; i < pn; i++) {
            if(playlist.GetItem(i).GetSourceURL() == m_lastPlayedURL) {
              found = true;
              break;
            }
          }
          if(found) {
            for(i--; i>=0; i--) {
              playlist.removeItem(playlist.GetItem(i));
            }
            firstPendingIndex = 1;
            m_lastUpdate = m_transactionCounter++;
          }
        }
      }
      if(!isEmpty()) {
        const MediaFileWithQueueId &firstQueueElement = peek();
        if((_tcsicmp(firstQueueElement.getSourceURL(),m_lastPlayedURL) == 0)) {
  //         && (firstQueueElement.getQueueId() == m_lastPlayedQueueId))
          get();
        }
      }
      if(shiftToNext) {
        firstPendingIndex = 0;
        m_lastUpdate = m_transactionCounter++;
      }
    } else if((m_lastPlayedURL.GetLength() > 0) && (playlist.GetCount() > 0)) {
      CWMPMedia firstMedia = playlist.GetItem(0);
      if(m_lastPlayedURL == firstMedia.GetSourceURL()) {
        playlist.removeItem(firstMedia);
        firstPendingIndex = 1;
      }
    }

    bool result = false;
    if(m_lastSync <= m_lastUpdate) {
      if(firstPendingIndex == 0) {
        playlist.clear();
      } else {
        for(int i = playlist.GetCount()-1; i >= firstPendingIndex; i--) {
          playlist.removeItem(playlist.GetItem(i));
        }
      }
      for(size_t i = 0; i < size(); i++) {
        playlist.appendItem(peek(i).getMedia());
      }
      m_lastSync = m_transactionCounter++;
      result = true;
    }

    MediaDatabase::notify();

    return result;

  } catch(...) {
    MediaDatabase::notify();
    throw;
  }
}

void MediaQueue::updateLastPlayedQueueId() {
  for(size_t i = 0; i < size(); i++) {
    const MediaFileWithQueueId &qe = peek(i);
    if(_tcsicmp(qe.getSourceURL(),m_lastPlayedURL) == 0) {
      m_lastPlayedQueueId = qe.getQueueId();
      break;
    }
  }
}

void MusicDirList::init(const String &dir) {
  clear();
  for(Tokenizer tok(dir,_T(";")); tok.hasNext(); ) {
    String s = tok.next();
    bool active = true;
    String dir;
    if(s.equalsIgnoreCase(_T("(N)"))) {
      active = false;
      dir = tok.hasNext() ? tok.next() : EMPTYSTRING;
    } else if(s.equalsIgnoreCase(_T("(Y)"))) {
      active = true;
      dir = tok.hasNext() ? tok.next() : EMPTYSTRING;
    } else {
      dir = s;
    }
    if(s.length() > 0) {
      add(MusicDir(active,dir));
    }
  }
}

MusicDirList::MusicDirList(const String &dir) {
  init(dir);
}

String MusicDirList::toString() const {
  String result;
  for(size_t i = 0; i < size(); i++) {
    const MusicDir &d = (*this)[i];
    if(i > 0) {
      result += _T(";");
    }
    result += d.isActive()?_T("(Y)"):_T("(N)");
    result += _T(";");
    result += d.getDir();
  }
  return result;
}
