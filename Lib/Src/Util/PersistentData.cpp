#include "pch.h"
#include <XmlDoc.h>
#include <FileNameSplitter.h>
#include <PersistentDataTemplate.h>

const TCHAR *_PersistentData::s_defaultName = _T("Untitled");

void _PersistentData::save(const String &fileName) {
  XMLDoc doc;
  putDataToDoc(doc);
  doc.saveToFile(fileName);
  setName(fileName);
}

void _PersistentData::load(const String &fileName) {
  XMLDoc doc;
  doc.loadFromFile(fileName);
  getDataFromDoc(doc);
  setName(fileName);
}

String _PersistentData::getDisplayName() const {
  return FileNameSplitter(getName()).getFileName();
}
