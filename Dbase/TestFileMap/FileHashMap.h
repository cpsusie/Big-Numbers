#pragma once

class KeyFileWrapper {
private:
  const String       m_fileName;
  KeyFile           *m_file;
  KeyType            m_key, m_fileKey;
  const unsigned int m_keySize, m_valueSize; // in bytes
  const unsigned int m_keyFieldCount;
  const unsigned int m_valueOffset;
  unsigned __int64   m_size;

  void create(const String &fileName);
  void flush();

public:
  KeyFileWrapper(const String &fileName, int keySize, int valueSize);
  ~KeyFileWrapper();
  void   *get(   const void *key);
  bool    put(   const void *key, const void *value);
  bool    remove(const void *key);
  void    update(const void *key, const void *value);
  void    clear();
  
  unsigned __int64 size() const {
    return m_size;
  }

  KeyFile &getFile() {
    return *m_file;
  }

  void *getValue(KeyType &key) {
    return key.m_data + m_valueOffset;
  }

  const void *getValue(const KeyType &key) const {
    return key.m_data + m_valueOffset;
  }

  unsigned int getKeySize() const {
    return m_keySize;
  }

  unsigned int getValueSize() const {
    return m_valueSize;
  }

  void save(ByteOutputStream &s) const;
  void load(ByteInputStream &s);
};

class FileHashMapKeyIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  KeyFileWrapper &m_keyFile;
  KeyCursor       m_keyCursor;
  KeyType         m_buffer;
  bool            m_hasElement;
public:
  FileHashMapKeyIterator(KeyFileWrapper &keyFile);
  AbstractIterator *clone();
  bool hasNext() const;
  void *next();
  void remove();
};

class FileHashMapEntryIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  class IteratorEntry : public AbstractEntry {
  private:
    const void *m_key;
    void       *m_value;
    friend class FileHashMapEntryIterator;

  public:
    const void *key() const {
      return m_key;
    }

    void *value() { 
      return m_value;
    }

    const void *value() const {
      return m_value;
    }
  };

  KeyFileWrapper *m_keyFile;
  KeyCursor       m_keyCursor;
  KeyType         m_fileKey;
  void           *m_fileValue;
  unsigned int    m_valueSize; // in bytes
  KeyType         m_key;
  IteratorEntry   m_buffer;
  bool            m_hasElement;

  void flush();

public:
  FileHashMapEntryIterator(KeyFileWrapper &keyFile);
  FileHashMapEntryIterator(FileHashMapEntryIterator &src);
  FileHashMapEntryIterator &operator=(FileHashMapEntryIterator &src);
  ~FileHashMapEntryIterator();

  AbstractIterator *clone();
  bool hasNext() const;
  void *next();
  void remove();
};

template <class K, class E> class FileHashMap {
private:
  KeyFileWrapper m_keyFile;

public:
  FileHashMap(const String &fileName) : m_keyFile(fileName, sizeof(K), sizeof(E)) {
  }

  static void destroy(const String &fileName) { // removes the file.
    UNLINK(fileName);
  }

  void setCapacity(unsigned long capacity) {
    // do nothing
  }

  unsigned long getCapacity() const {
    return 0xffffffff;
  }

  bool put(const K &key, const E &value) {
    return m_keyFile.put(&key, &value);
  }

  E *get(const K &key) {
    return (E*)m_keyFile.get(&key);
  }

  const E *get(const K &key) const {
    KeyFileWrapper *keyFile = (KeyFileWrapper*)(&m_keyFile);
    return (const E*)keyFile->get(&key);
  }

  bool remove(const K &key) {
    return m_keyFile.remove(&key);
  }

  void clear() {
    m_keyFile.clear();
  }

  unsigned __int64 size() const {
    return m_keyFile.size();
  }

  Iterator<K> getKeyIterator() {
    return Iterator<K>(new FileHashMapKeyIterator(m_keyFile));
  }

  Iterator<K> getKeyIterator() const {
    KeyFileWrapper *keyFile = (KeyFileWrapper*)(&m_keyFile);
    return Iterator<K>(new FileHashMapKeyIterator(*keyFile));
  }

  Iterator<Entry<K, E> > getEntryIterator() {
    return Iterator<Entry<K, E> >(new FileHashMapEntryIterator(m_keyFile));
  }

  Iterator<Entry<K, E> > getEntryIterator() const {
    KeyFileWrapper *keyFile = (KeyFileWrapper*)(&m_keyFile);
    return Iterator<Entry<K, E> >(new FileHashMapEntryIterator(*keyFile));
  }

  void save(ByteOutputStream &s) const {
    m_keyFile.save(s);
  }

  void load(ByteInputStream &s) {
    m_keyFile.load(s);
  }
};
