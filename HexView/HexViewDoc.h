#pragma once

#include <ByteArray.h>
#include "ModificationMap.h"

class CHexViewDoc : public CDocument, public UpdatableByteContainer {
private:
  String           m_fileName;
  FILE            *m_file;
  struct _stati64  m_stat;
  bool             m_readOnly;
  __int64          m_contentOffset;
  ByteArray        m_content;
  ModificationMap  m_modifiedBytes;
  EditHistory      m_editHistory;
  size_t           m_historyIndex; // Invariant: 0 <= m_historyIndex <= m_editHistory.size()
  bool             m_updateHistory;

  void init();
  void checkIsOpen();
  void checkCanModify();

  void resetStat();
  void resetContent();
  void resetModifications();
  void resetHistory();
  void addToHistory(unsigned __int64 addr, BYTE from, BYTE to);
  void removeLast();

  inline size_t getHistorySize() const {
    return m_editHistory.size();
  }
#ifdef _DEBUG
  void checkInvariant(int line) const;
#define CHECKINVARIANT checkInvariant(__LINE__)
#else
#define CHECKINVARIANT
#endif

protected:
  CHexViewDoc();
  DECLARE_DYNCREATE(CHexViewDoc)

public:

  public:
  void OnOpenDocument(TCHAR *fname);
  virtual void Serialize(CArchive& ar);

  void setReadOnly(bool readOnly);
  bool isReadOnly() const {
    return m_readOnly;
  }

  bool isOpen() const {
    return m_file != NULL;
  }

  const String &getFileName() const {
    return m_fileName;
  }

  String getTitle() const;
  const TCHAR *getModeString() const;

  bool IsModified() const {
    return !m_modifiedBytes.isEmpty();
  }

  void getBytes(unsigned __int64 start, UINT length, ByteArray &dst);
  void putBytes(unsigned __int64 start, ByteArray &src);

  bool setByte( unsigned __int64 addr, BYTE byte);
  BYTE getByte( unsigned __int64 addr);

  void save();
  void saveAs(const String &newName);
  void close();

  unsigned __int64 getSize() const {
    return m_stat.st_size;
  }

  void refresh();

  __int64 undo(); // return the address of the undone action
  __int64 redo(); // do
  bool canUndo() const;
  bool canRedo() const;

	virtual ~CHexViewDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};

