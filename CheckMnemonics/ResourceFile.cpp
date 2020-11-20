#include "stdafx.h"
#include <FileNameSplitter.h>
#include "ResourceFile.h"

//#define DUMPTREE
ResourceFile::ResourceFile(const String &fileName) {
  try {
    m_ok = true;
    m_fileName = FileNameSplitter(fileName).getAbsolutePath();
    LexFileStream    stream(fileName);
    ParserTree tree(fileName);
    if(!stream.ok()) {
      tree.addError(_T("%s"), _tcserror(errno));
    } else {
      ResourceLex      lex(&stream);
      lex.setLineNumber(0);
      ResourceParser   parser(tree, &lex);
      lex.setParser(&parser);
      parser.parse();

      const NodeArray resourceNodes = ParserTree::getListFromTree(tree.getRoot());
      if(!resourceNodes.isEmpty()) {
        LanguageSection *currentSection = newSection();
        for(size_t i = 0; i < resourceNodes.size(); i++) {
          const SyntaxNode *n = resourceNodes[i];
          switch(n->getSymbol()) {
          case languageDirective:
            { if(!currentSection->isEmpty()) {
                currentSection = newSection();
              } else {
                currentSection->clear();
              }
              currentSection->m_language = LanguageDirective(n, this);
            }
            break;
          case dialogDefinition      : currentSection->m_dialogs.add(DialogDefinition(n,this));                      break;
          case menuDefinition        : currentSection->m_menues.add(MenuDefinition(n, this));                        break;
          case acceleratorsDefinition: currentSection->m_acceleratorDefinitions.add(AcceleratorsDefinition(n,this)); break;
          case stringTableDefinition:  currentSection->m_stringTable.append(StringTableDefinition(n,this));          break;
          default:
            tree.addError(_T("Unknown resourceNode:symbol:%s\n"), ResourceParser::getTables().getSymbolName(n->getSymbol()).cstr());
            break;
          }
        }
      }
    }
    m_ok &= tree.isOk();
    if(!isOk()) {
      m_errors.addAll(tree.getErrors());
    }
#if defined(DUMPTREE)
    tree.dumpTree(_T("c:\\temp\\checkMnemonicsTree.txt"));
#endif
    if(isOk()) {
      pruneEmptySection();
    }
    for(size_t i = 0; i < m_sections.size(); i++) {
      m_sections[i]->fixup();
    }
  } catch(Exception e) {
    error(_T("Exception:%s"), e.what());
    m_ok = false;
  }
}

LanguageSection *ResourceFile::newSection() {
  LanguageSection *result = new LanguageSection(this); TRACE_NEW(result);
  m_sections.add(result);
  return result;
}

ResourceFile::~ResourceFile() {
  for(size_t i = 0; i < m_sections.size(); i++) {
    SAFEDELETE(m_sections[i]);
  }
  m_sections.clear();
}

void ResourceFile::pruneEmptySection() {
  for(intptr_t i = m_sections.size()-1; i >= 0; i--) {
    if(m_sections[i]->isEmpty()) {
      SAFEDELETE(m_sections[i]);
      m_sections.remove(i);
    }
  }
}

void ResourceFile::analyze() const {
  const size_t n = m_sections.size();
  for(size_t i = 0; i < n; i++) {
    m_currentSection = m_sections[i];
    analyzeCurrentSection();
  }
  const StringHashSet allMenuIds   = getAllMenuIds();
  const StringHashSet allStringIds = getAllStringIds();
  for(size_t i = 0; i < n; i++) {
    const LanguageSection &ls = *m_sections[i];
    ls.checkHasAllMenuAndStrings(allMenuIds, allStringIds);
    for(size_t j = i+1; j < n; j++) {
      ls.compareSections(*m_sections[j]);
    }
  }
}

void ResourceFile::analyzeCurrentSection() const {
  for(size_t i = 0; i < m_currentSection->m_menues.size(); i++) {
    const MenuDefinition &menu = m_currentSection->m_menues[i];
    analyzeMenu(menu.getId(), EMPTYSTRING, menu.getItems());
  }
  for(size_t i = 0; i < m_currentSection->m_acceleratorDefinitions.size(); i++) {
    analyzeAccelerator(m_currentSection->m_acceleratorDefinitions[i]);
  }
  for(size_t i = 0; i < m_currentSection->m_dialogs.size(); i++) {
    analyzeDialog(m_currentSection->m_dialogs[i]);
  }
}

static String findFirstStringUsingAltLetter(const StringArray &a, TCHAR ch) {
  ch = _totupper(ch);
  for(size_t i = 0; i < a.size(); i++) {
    String t = a[i];
    t.replace(_T("&&"), EMPTYSTRING);
    intptr_t index = t.find('&');
    if(index >= 0 && index < (int)t.length()-1) {
      const TCHAR c = _totupper(t[index+1]);
      if(c == ch) {
        return a[i];
      }
    }
  }
  return format(_T("Internal error:Alt+%c not found in stringArray"), ch);
}

BitSet ResourceFile::getAltLetterSet(const StringArray &a, const String &container) const {
  BitSet altSet(256);
  for(size_t i = 0; i < a.size(); i++) {
    String t = a[i];
    t.replace(_T("&&"), EMPTYSTRING);
    intptr_t index = t.find('&');
    if(index >= 0 && index < (int)t.length()-1) {
      const TCHAR ch = _totupper(t[index+1]);
      if(altSet.contains(ch)) {
        error(_T("%s:Ambiguous use of Alt+%c in \"%s\" and \"%s\"")
              ,container.cstr()
              ,ch
              ,a[i].cstr()
              ,findFirstStringUsingAltLetter(a, ch).cstr());
      }
      altSet.add(ch);
    }
  }
  return altSet;
}

void ResourceFile::analyzeDialog(const DialogDefinition &dialog) const {
  const MenuDefinition *menu = nullptr;
  if(dialog.hasMenu()) {
    menu = m_currentSection->findMenu(dialog.getMenuId());
    if(menu == nullptr) {
      menu = findMenu(dialog.getMenuId());
      if(menu == nullptr) {
        error(_T("Dialog %s:Menu %s not defined")
             ,dialog.getId().cstr()
             ,dialog.getMenuId().cstr());
      }
    }
  }
  const StringArray controlStrings    = dialog.getControlStrings();
  const BitSet      controlAltLetters = getAltLetterSet(controlStrings, format(_T("Dialog %s"), dialog.getId().cstr()));
  if(menu) {
    const StringArray menuStrings       = menu->getTopLevelStrings();
    const BitSet      menuAltLetters    = getAltLetterSet(menuStrings, format(_T("Menu %s"), menu->getId().cstr()));
          BitSet intersection = menuAltLetters & controlAltLetters;
    if(!intersection.isEmpty()) {
      for(auto it = intersection.getIterator(); it.hasNext();) {
        TCHAR ch = (TCHAR)it.next();
        const String menuText    = findFirstStringUsingAltLetter(menuStrings   , ch);
        const String controlText = findFirstStringUsingAltLetter(controlStrings, ch);
        error(_T("Dialog %s:Ambiguous use of Alt+%c in menu text \"%s\" and control text \"%s\"")
             ,dialog.getId().cstr()
             ,ch
             ,menuText.cstr()
             ,controlText.cstr());
      }
    }
  }

  if(menu) {
    const AcceleratorsDefinition *accel = m_currentSection->findMatchingAccelerator(dialog, menu);
    if(accel == nullptr) {
      accel = findMatchingAccelerator(dialog, menu);
    }
    if(accel) {
      const CompactArray<MenuTextItem*> menuItems = menu->getMenuItemsWithHotKey();
      for(size_t i = 0; i < menuItems.size(); i++) {
        const MenuTextItem *menuItem = menuItems[i];
        CompactArray<const Accelerator*> accArray = accel->findAcceleratorByCommand(menuItem->getCommand());
        if(accArray.size() == 0) {
          error(_T("Dialog %s:Menu item \"%s\" in menu %s has no elements in accelerator %s")
               ,dialog.getId().cstr()
               ,menuItem->getVisibleText().cstr()
               ,menu->getId().cstr()
               ,accel->getId().cstr());
        } else if(!menuItem->getHotKey().hasAnyMatchingAccelKeys(accArray)) {
          error(_T("Dialog %s:Menu tekst \"%s\" in menu %s does not match %s %s")
                ,dialog.getId().cstr()
                ,menuItem->getVisibleText().cstr()
                ,menu->getId().cstr()
                ,(accArray.size()==1)?_T("accelerator ") : _T("any of the accelerators")
                ,Accelerator::toString(accArray).cstr()
                );
/*
          _tprintf(_T("menuHotKey:%-40s accel:%s\n"), menuItem->getHotKey().toString().cstr(), accArray[0]->toString().cstr());
          if(accArray.size() > 1) {
            for(size_t i = 1; i < accArray.size(); i++) {
              _tprintf(_T("%-52saccel:%s\n"), EMPTYSTRING, accArray[i]->toString().cstr());
            }
          }
*/
        }
      }
    }
  }
}

void ResourceFile::analyzeMenu(const String &id, const String &popupText, const CompactArray<MenuItem*> &itemArray) const {
  StringArray menuStrings;
  for(size_t i = 0; i < itemArray.size(); i++) {
    const MenuItem *item = itemArray[i];
    menuStrings.add(item->getVisibleText());
  }
  getAltLetterSet(menuStrings, format(_T("Menu %s"), id.cstr()));
  for(size_t i = 0; i < itemArray.size(); i++) {
    const MenuItem *item = itemArray[i];
    if(item->getType() == MENU_POPUP) {
      analyzeMenu(id, item->getVisibleText(), ((const PopupMenu*)item)->getItems());
    }
  }
}

const MenuDefinition *ResourceFile::findMenu(const String &id) const {
  for(size_t i = 0; i < m_sections.size(); i++) {
    const MenuDefinition *menu = m_sections[i]->findMenu(id);
    if(menu) {
      return menu;
    }
  }
  return nullptr;
}

StringHashSet ResourceFile::getAllMenuIds() const {
  StringHashSet result;
  for(size_t i = 0; i < m_sections.size(); i++) {
    const LanguageSection &ls = *m_sections[i];
    for(size_t j = 0; j < ls.m_menues.size(); j++) {
      result.add(ls.m_menues[j].getId());
    }
  }
  return result;
}

StringHashSet ResourceFile::getAllStringIds() const {
  StringHashSet result;
  for(size_t i = 0; i < m_sections.size(); i++) {
    const Array<StringId> &sl = m_sections[i]->m_stringTable.getAllStrings();
    for(size_t j = 0; j < sl.size(); j++) {
      result.add(sl[j].getId());
    }
  }
  return result;
}

const AcceleratorsDefinition *ResourceFile::findMatchingAccelerator(const DialogDefinition &dialog, const MenuDefinition *menu) const {
  for(size_t i = 0; i < m_sections.size(); i++) {
    const AcceleratorsDefinition *accel = m_sections[i]->findMatchingAccelerator(dialog, menu);
    if(accel) {
      return accel;
    }
  }
  return nullptr;
}

void ResourceFile::analyzeAccelerator(const AcceleratorsDefinition &accel) const {
  const Array<Accelerator> &accArray = accel.getAccelerators();
  const size_t n = accArray.size();
  for(size_t i = 0; i < n; i++) {
    const Accelerator &acc1 = accArray[i];
    for(size_t j = i+1; j < n; j++) {
      const Accelerator &acc2 = accArray[j];
      if(acc2 == acc1) {
        error(_T("Accelerators %s:Accelerator %s equals %s"), accel.getId().cstr(), acc1.toString().cstr(), acc2.toString().cstr());
      }
    }
  }
}

String ResourceFile::toString() const {
  String result = format(_T("Resourcefile %s\n"), m_fileName.cstr());
  if(m_sections.size()) {
    for(size_t i = 0; i < m_sections.size(); i++) {
      result += m_sections[i]->toString();
    }
  }
  result += spaceString(80, '_');
  return result;
}

void ResourceFile::error(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
  va_list argptr;
  va_start(argptr, format);
  verror(format, argptr);
  va_end(argptr);
}

void ResourceFile::verror(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) const {
  m_errors.add(_T(":")+vformat(format, argptr));
  m_ok = false;
}

void ResourceFile::listErrors(FILE *f) const {
  for(size_t i = 0; i < m_errors.size();i++) {
    _ftprintf(f,_T("%s%s\n"), m_fileName.cstr(), m_errors[i].cstr());
  }
  fflush(f);
}

