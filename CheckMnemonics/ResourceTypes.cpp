#include "stdafx.h"
#include "ResourceFile.h"

static String forceToString(const SyntaxNode *n) {
  switch(n->getSymbol()) {
  case NUMBER    : return format(_T("%d"), n->getNumber());
  case IDENTIFIER: return n->getName();
  case STRING    : return n->getString();
  default        : ParserTree::dumpSyntaxTree(n);
                   throwException(_T("forceToString:symbol must be NUMBER,IDENTIFIER or STRING"));
                   return EMPTYSTRING;
  }
}

LanguageDirective::LanguageDirective(const ResourceFile *rf) : ResourceDefinition(LANGUAGE_DIRECTIVE, EMPTYSTRING,rf) {
}

LanguageDirective::LanguageDirective(const SyntaxNode *n, const ResourceFile *rf)
: ResourceDefinition(LANGUAGE_DIRECTIVE, forceToString(n->getChild(0)), rf) {
  m_subLangId = forceToString(n->getChild(1));
}

void LanguageDirective::clear() {
  ResourceDefinition::clear();
  m_subLangId = EMPTYSTRING;
}

String LanguageDirective::toString() const {
  return format(_T("Language(%s,%s)"), getId().cstr(), m_subLangId.cstr());
}

static const TCHAR *controlTypeName(ControlType type) {
  switch(type) {
#define caseType(t) case CTRL_##t: return _T(#t)
  caseType(TEXT      );
  caseType(PUSHBUTTON);
  caseType(CHECKBOX  );
  default: return _T("?");
  }
}

ControlDefinition::ControlDefinition(const SyntaxNode *n) {
  switch(n->getSymbol()) {
  case LTEXT        : m_type = CTRL_TEXT;       break;
  case PUSHBUTTON   :
  case DEFPUSHBUTTON: m_type = CTRL_PUSHBUTTON; break;
  case CONTROL      : m_type = CTRL_CHECKBOX;   break;
  default           : _tprintf(_T("Unknown controltype:%s\n"), ResourceParser::getTables().getSymbolName(n->getSymbol()));
                      ParserTree::dumpSyntaxTree(n);
  }
  m_id          = n->getChild(0)->getName();
  m_visibleText = forceToString(n->getChild(1));
}

String ControlDefinition::toString() const {
  return format(_T("%-10s: %-35s visual text:\"%s\""), controlTypeName(m_type), m_id.cstr(), m_visibleText.cstr());
}

static bool isInterestingControlNode(const SyntaxNode *n) {
  switch(n->getSymbol()) {
  case LTEXT        :
  case PUSHBUTTON   :
  case DEFPUSHBUTTON:
    return true;
  case CONTROL      :
    return n->getChild(2)->getString() == _T("Button");
  }
  return false;
}

DialogDefinition::DialogDefinition(const SyntaxNode *n, const ResourceFile *rf)
: ResourceDefinition(DIALOG_DEFINITION, n->getChild(0)->getName(), rf) {
  if(n->getChildCount() == 3) {
    m_menuId = n->getChild(2)->getChild(0)->getName();
  }
  if(n->getChild(1)->getChildCount() > 0) {
    const NodeArray ctrlNodes = ParserTree::getListFromTree(n->getChild(1)->getChild(0));
    for(size_t i = 0; i < ctrlNodes.size(); i++) {
      const SyntaxNode *ctrlNode = ctrlNodes[i];
      if(isInterestingControlNode(ctrlNode)) {
        m_controls.add(ControlDefinition(ctrlNode));
      }
    }
  }
}

StringArray DialogDefinition::getControlStringWithAltNeedingAccelerator() const {
  StringArray result;
  for(size_t i = 0; i < m_controls.size(); i++) {
    const ControlDefinition &ctrl = m_controls[i];
    switch(ctrl.getType()) {
    case CTRL_TEXT      :
      { const String &visibleText = getControlText(ctrl);
        String t = visibleText;

        t.replace(_T("&&"),EMPTYSTRING);
        intptr_t index = t.find('&');
        if(index >= 0 && index < (int)t.length()-1) {
          result.add(visibleText);
        }
      }
      break;
    case CTRL_PUSHBUTTON:
    case CTRL_CHECKBOX  :
      break; // handles Alt with accelerator
    }
  }
  return result;
}

StringArray DialogDefinition::getControlStrings() const {
  StringArray result;
  for(size_t i = 0; i < m_controls.size(); i++) {
    result.add(getControlText(m_controls[i]));
  }
  return result;
}

const String &DialogDefinition::getControlText(const ControlDefinition &ctrl) const {
  const String &s = ctrl.getVisibleText();
  if(s.length() == 0 || s[0] != '{') {
    return s;
  } else {
    const ResourceFile &rf = getResourceFile();
    const String *ts = rf.getCurrentSection()->getStringTable().findStringById(ctrl.getId());
    return ts ? *ts : s;
  }
}

String DialogDefinition::toString() const {
  String result = format(_T("Dialog(%s): menu:%s\n"), getId().cstr(), hasMenu()?getMenuId().cstr():_T("-"));
  if(hasControls()) {
    result += _T("  Controls:\n");
    for(size_t i = 0; i < m_controls.size(); i++) {
      const ControlDefinition &ctrl = m_controls[i];
      result += format(_T("    %s\n"), ctrl.toString().cstr());
    }
  }
  return result;
}

static const TCHAR *menuTypeName(MenuItemType type) {
  switch(type) {
  case MENU_POPUP: return _T("popup");
  case MENU_TEXT : return _T("text");
  default        : return _T("?");
  }
}

String MenuItem::toString(int level) const {
  return format(_T("%*.*s%-10s%-30s"), level,level,EMPTYSTRING, format(_T("%s:"), menuTypeName(m_type)).cstr(), format(_T("<%s>"), m_visibleText.cstr()).cstr());
}

MenuTextItem::MenuTextItem(const SyntaxNode *n) : MenuItem(MENU_TEXT) {
  m_visibleText = n->getChild(0)->getString();
  m_command     = forceToString(n->getChild(1));
}

CompactArray<MenuTextItem*> MenuTextItem::getMenuItemsWithHotKey(const CompactArray<MenuItem*> items) { // static
  CompactArray<MenuTextItem*> result;
  for(size_t i = 0; i < items.size(); i++) {
    const MenuItem *item = items[i];
    switch(item->getType()) {
    case MENU_TEXT:
      if(item->getVisibleText().find(_T("\\t")) >= 0) {
        result.add((MenuTextItem*)item);
      }
      break;
    case MENU_POPUP:
      { const CompactArray<MenuTextItem*> subMenu = getMenuItemsWithHotKey(((const PopupMenu*)item)->getItems());
        result.addAll(subMenu);
      }
      break;
    }
  }
  return result;
}

class SynonymMap : public StringHashMap<String> {
public:
  SynonymMap(const TCHAR **initTable);
};

SynonymMap::SynonymMap(const TCHAR **initTable) {
  for(const TCHAR **entry = initTable; *entry; entry++) {
    const String words = *entry;
    Tokenizer tok(words, _T(","));
    if(!tok.hasNext()) continue;
    const String key = tok.next();
    if(!tok.hasNext()) continue;
    const String value = tok.next();
    put(key, value);
//    _tprintf(_T("Synonymer:%-10s -> %s\n"), key.cstr(), value.cstr());
  }
}

MenuHotKey MenuTextItem::getHotKey() const {
  const intptr_t index = getVisibleText().find(_T("\\t"));
  if(index < 0) {
    return MenuHotKey();
  } else {
    return MenuHotKey(substr(getVisibleText(), index+2, getVisibleText().length()));
  }
}

MenuHotKey::MenuHotKey(const String &s) {
  m_flags = 0;
  for(Tokenizer tok(s,_T("+")); tok.hasNext();) {
    String p = trim(tok.next());
    if(p.equalsIgnoreCase(_T("ctrl"))) {
      m_flags |= ACC_CONTROL;
    } else if(p.equalsIgnoreCase(_T("alt"))) {
      m_flags |= ACC_ALT;
    } else if(p.equalsIgnoreCase(_T("shift")) || p.equalsIgnoreCase(_T("shft"))) {
      m_flags |= ACC_SHIFT;
    } else if(!tok.hasNext()) {
      p.replace(_T("&&"),_T("§#")).replace(_T("&"),EMPTYSTRING).replace(_T("§#"),_T("&&")).replace(_T('.'),EMPTYSTRING);
      m_key = p;
    }
  }
  if(m_key.length()) {
    if((m_flags & (ACC_CONTROL|ACC_ALT)) || (s.length() > 1)) {
      m_flags |= ACC_VIRTKEY;
    } else {
      m_flags |= ACC_ASCII;
    }
  }
  if(m_flags) m_flags |= ACC_NOINVERT;
}

static const TCHAR *vkSynonyms[] = { // leave no space between words. COMMA (,) is separator here
  _T("esc,escape")
 ,_T("backspace,back")
 ,_T("del,delete")
 ,_T("pg up,prior")
 ,_T("pg down,next")
 ,_T("ins,insert")
 ,NULL
};

#define IGNORE_KEYTYPE         (~(ACC_VIRTKEY | ACC_ASCII))
#define EQUAL_SCANFLAGS(f1,f2) (((f1)&IGNORE_KEYTYPE) == ((f2)&IGNORE_KEYTYPE))

bool MenuHotKey::matchAccelerator(const Accelerator &acc) const {
  if(!EQUAL_SCANFLAGS(m_flags,acc.getFlags())) {
    return false;
  }
  static const SynonymMap synonymTable(vkSynonyms);
  if(m_key.equalsIgnoreCase(acc.getKey())) {
    return true;
  } else if(substr(acc.getKey(),0,3) == _T("VK_")) {
    const String strippedAccKey = substr(acc.getKey(),3, acc.getKey().length());
    if(m_key.equalsIgnoreCase(strippedAccKey)) {
      return true;
    }
    const String *synonym = synonymTable.get(toLowerCase(m_key));
    if(synonym && synonym->equalsIgnoreCase(strippedAccKey)) {
      return true;
    }
  }
  return false;
}

bool MenuHotKey::hasAnyMatchingAccelKeys(const CompactArray<const Accelerator*> &accArray) const {
  for(size_t i = 0; i < accArray.size(); i++) {
    if(matchAccelerator(*accArray[i])) {
      return true;
    }
  }
  return false;
}

String MenuHotKey::toString() const {
  return format(_T("%s+%s"), Accelerator::flagsToString(m_flags).cstr(), m_key.cstr());
}

String MenuTextItem::toString(int level) const {
  return MenuItem::toString(level) + format(_T(" Command:%s"), m_command.cstr());
}

PopupMenu::PopupMenu(const SyntaxNode *n) : MenuItem(MENU_POPUP) {
  m_visibleText = n->getChild(0)->getString();
  if(n->getChildCount() > 0) {
    const NodeArray itemNodes = ParserTree::getListFromTree(n->getChild(1));
    for(size_t i = 0; i < itemNodes.size(); i++) {
      const SyntaxNode *child = itemNodes[i];
      switch(child->getSymbol()) {
      case POPUP    : m_items.add(new PopupMenu(   child)); TRACE_NEW(m_items.last());  break;
      case _MENUITEM: m_items.add(new MenuTextItem(child)); TRACE_NEW(m_items.last()); break;
      }
    }
  }
}

PopupMenu::~PopupMenu() {
  for(size_t i = 0; i < m_items.size(); i++) {
    SAFEDELETE(m_items[i]);
  }
  m_items.clear();
}

PopupMenu::PopupMenu(const PopupMenu &m) : MenuItem(MENU_POPUP) {
  m_visibleText = m.getVisibleText();
  for(size_t i = 0; i < m.m_items.size(); i++) {
    m_items.add(m.m_items[i]->clone());
  }
}

String PopupMenu::toString(int level) const {
  String result = MenuItem::toString(level);
  result += _T("\n");
  if(m_items.size() > 0) {
    for(size_t i = 0; i < m_items.size(); i++) {
      const MenuItem *item = m_items[i];
      result += item->toString(level+2);
      if(item->getType() == MENU_TEXT) {
        result += _T("\n");
      }
    }
  }
  return result;
}

MenuDefinition::MenuDefinition(const MenuDefinition &src)
: ResourceDefinition(MENU_DEFINITION, src.getId(), &src.getResourceFile()) {
  for(size_t i = 0; i < src.m_items.size(); i++) {
    m_items.add(src.m_items[i]->clone());
  }
}

MenuDefinition::MenuDefinition(const SyntaxNode *n, const ResourceFile *rf)
 : ResourceDefinition(MENU_DEFINITION, n->getChild(0)->getName(), rf) {
  const NodeArray nodes = ParserTree::getListFromTree(n->getChild(1));
  for(size_t i = 0; i < nodes.size(); i++) {
    const SyntaxNode *child = nodes[i];
    switch(child->getSymbol()) {
    case POPUP    : m_items.add(new PopupMenu(   child)); TRACE_NEW(m_items.last()); break;
    case _MENUITEM: m_items.add(new MenuTextItem(child)); TRACE_NEW(m_items.last()); break;
    default       : _tprintf(_T("unknown menutype:\n"));
                    ParserTree::dumpSyntaxTree(child);
                    break;
    }
  }

}

MenuDefinition::~MenuDefinition() {
  for(size_t i = 0; i < m_items.size(); i++) {
    SAFEDELETE(m_items[i]);
  }
  m_items.clear();
}

StringArray MenuDefinition::getTopLevelStrings() const {
  StringArray result;
  for(size_t i = 0; i < m_items.size(); i++) {
    result.add(m_items[i]->getVisibleText());
  }
  return result;
}

void MenuDefinition::compareMenues(const MenuDefinition &menuDef) const {
}

String MenuDefinition::toString() const {
  String result;
  result = format(_T("Menu %s\n"), getId().cstr());
  for(size_t i = 0; i < m_items.size(); i++) {
    result += m_items[i]->toString(2);
  }
  return result;
}

Accelerator::Accelerator(const SyntaxNode *n) {
  m_command       = n->getChild(0)->getName();
  m_flags         = 0;
  m_asciiKeyValue = 0;
  const SyntaxNode *keyNode = n->getChild(1);
  switch(keyNode->getSymbol()) {
  case NUMBER    : m_asciiKeyValue = keyNode->getNumber(); break;
  case IDENTIFIER: m_key           = keyNode->getName();   break;
  case STRING    : m_key           = keyNode->getString(); break;
  }
  const NodeArray modNodes = ParserTree::getListFromTree(n->getChild(2));
  for(size_t i = 0; i < modNodes.size(); i++) {
    const SyntaxNode *child = modNodes[i];
#define addTokenToType(t) case t: m_flags |= ACC_##t; break;
    switch(child->getSymbol()) {
    addTokenToType(VIRTKEY)
    addTokenToType(ASCII)
    addTokenToType(SHIFT)
    addTokenToType(CONTROL)
    addTokenToType(ALT)
    addTokenToType(NOINVERT)
    default: throwException(_T("invalid accelerator-modifier:%s"), ResourceParser::getTables().getSymbolName(child->getSymbol()));
    }
  }
}

bool Accelerator::operator==(const Accelerator &a) const {
  return m_key           == a.m_key
      && m_flags         == a.m_flags
      && m_asciiKeyValue == a.m_asciiKeyValue;
}

String Accelerator::toString() const {
  if(m_flags & ACC_ASCII) {
    const int ascii = m_key.length() ? m_key[0] : m_asciiKeyValue;
    String keyStr = _istprint(ascii) ? format(_T("'%c'"), ascii) : _T("---");
    return format(_T("%-40s %s")
                 ,format(_T("%s+%s ascii(%d)"), flagsToString(m_flags).cstr(), keyStr.cstr(), ascii).cstr()
                 ,m_command.cstr()
                 );
  } else {
    return format(_T("%-40s %s")
                 ,format(_T("%s+%s"), flagsToString(m_flags).cstr(), m_key.cstr()).cstr()
                 ,m_command.cstr()
                 );
  }
}

String Accelerator::toString(const CompactArray<const Accelerator*> &accArray) { // static
  if(accArray.size() == 1) {
    return accArray[0]->toString();
  } else {
    String result;
    for(size_t i = 0; i < accArray.size(); i++) {
      result += format(_T(" (%s)"), accArray[i]->toString().cstr());
    }
    return result;
  }
}

String Accelerator::flagsToString(int flags) { // static
  String result;
#define addIfSet(f) if(flags & ACC_##f) { result += _T(" "); result += _T(#f); }

  addIfSet(VIRTKEY)
  addIfSet(ASCII)
  addIfSet(CONTROL)
  addIfSet(SHIFT)
  addIfSet(ALT)
  addIfSet(NOINVERT)
  return result;
}

AcceleratorsDefinition::AcceleratorsDefinition(const SyntaxNode *n, const ResourceFile *rf)
: ResourceDefinition(ACCELERATOR_DEFNITION, n->getChild(0)->getName(), rf) {
  const NodeArray nodes = ParserTree::getListFromTree(n->getChild(1));
  for(size_t i = 0; i < nodes.size(); i++) {
    m_accelerators.add(Accelerator(nodes[i]));
  }
}

CompactArray<const Accelerator*> AcceleratorsDefinition::findAcceleratorByCommand(const String &command) const {
  CompactArray<const Accelerator*> result;
  for(size_t i = 0; i < m_accelerators.size(); i++) {
    const Accelerator &acc = m_accelerators[i];
    if(acc.getCommand() == command) {
      result.add(&acc);
    }
  }
  return result;
}

String AcceleratorsDefinition::toString() const {
  String result = format(_T("Acceleartos(%s):\n"), getId().cstr());
  for(size_t i = 0; i < m_accelerators.size(); i++) {
    const Accelerator &acc = m_accelerators[i];
    result += format(_T("  %s\n"), acc.toString().cstr());
  }
  return result;
}

StringId::StringId(const SyntaxNode *n) {
  m_id   = n->getChild(0)->getName();
  m_text = n->getChild(1)->getString();
}

String StringId::toString() const {
  return format(_T("%-35s text:<%s>"), m_id.cstr(), m_text.cstr());
}

StringTableDefinition::StringTableDefinition(const ResourceFile *rf)
: ResourceDefinition(STRINGTABLE_DEFINITION, _T("no Id"), rf) {
}

StringTableDefinition::StringTableDefinition(const SyntaxNode *n, const ResourceFile *rf)
: ResourceDefinition(STRINGTABLE_DEFINITION, _T("no Id"), rf) {
  const NodeArray nodes = ParserTree::getListFromTree(n->getChild(0));
  for(size_t i = 0; i < nodes.size(); i++) {
    m_strings.add(StringId(nodes[i]));
  }
}

void StringTableDefinition::append(const StringTableDefinition &src) {
  m_strings.addAll(src.m_strings);
}

static int stringIdCmp(const StringId &id1, const StringId &id2) {
  return _tcscmp(id1.getId().cstr(), id2.getId().cstr());
}

void StringTableDefinition::sort() {
  m_strings.sort(stringIdCmp);
}

const String *StringTableDefinition::findStringById(const String &id) const {
  intptr_t index = m_strings.binarySearch(StringId(id), stringIdCmp);
  return (index >= 0) ? &m_strings[index].getText() : NULL;
}

String StringTableDefinition::toString() const {
  String result = _T("StringTable:\n");
  for(size_t i = 0; i < m_strings.size(); i++) {
    result += m_strings[i].toString(); result += _T("\n");
  }
  return result;
}

void LanguageSection::clear() {
  m_language.clear();
  m_dialogs.clear();
  m_menues.clear();
  m_acceleratorDefinitions.clear();
  m_stringTable.clear();
}

void LanguageSection::fixup() {
  m_stringTable.sort();
}

void LanguageSection::compareSections(const LanguageSection &ls) const {
  for(size_t i = 0; i < m_menues.size(); i++) {
    const MenuDefinition &mn1 = m_menues[i];
    const MenuDefinition *mn2 = ls.findMenu(mn1.getId());
    if(mn2 == NULL) {
      continue;
    }
    mn1.compareMenues(*mn2);
  }
}

bool LanguageSection::isEmpty() const {
  return m_dialogs.isEmpty()
      && m_menues.isEmpty()
      && m_acceleratorDefinitions.isEmpty()
      && m_stringTable.isEmpty();
}

const MenuDefinition *LanguageSection::findMenu(const String &id) const {
  for(size_t i = 0; i < m_menues.size(); i++) {
    const MenuDefinition &menu = m_menues[i];
    if(menu.getId() == id) {
      return &menu;
    }
  }
  return NULL;
}

class MenuItemAcceleratorPair {
private:
  const MenuTextItem &m_menuItem;
  const Accelerator  &m_accel;
public:
  MenuItemAcceleratorPair(const MenuTextItem &menuItem, const Accelerator &accel)
    : m_menuItem(menuItem)
    , m_accel(accel) {
  }
  const MenuTextItem &getMenuItem() const {
    return m_menuItem;
  }
  const Accelerator &getAccelerator() const {
    return m_accel;
  }
};

static const MenuTextItem *findMenuItemByCommand(const CompactArray<MenuTextItem*> &menuItems, const String &command) {
  for(size_t i = 0; i < menuItems.size(); i++) {
    const MenuTextItem *item = menuItems[i];
    if(item->getCommand() == command) {
      return item;
    }
  }
  return NULL;
}

static Array<MenuItemAcceleratorPair> findMatchingCommands(const AcceleratorsDefinition &accel, const CompactArray<MenuTextItem*> &menuItems) {
  Array<MenuItemAcceleratorPair> result;
  const Array<Accelerator> &accelerators = accel.getAccelerators();
  for(size_t i = 0; i < accelerators.size(); i++) {
    const Accelerator  &acc  = accelerators[i];
    const MenuTextItem *item = findMenuItemByCommand(menuItems, acc.getCommand());
    if(item) {
      result.add(MenuItemAcceleratorPair(*item, acc));
    }
  }
  return result;
}

static Array<const Accelerator*> findMatchingAltKeys(const AcceleratorsDefinition &accel, const BitSet &altSet) {
  Array<const Accelerator*> result;
  const Array<Accelerator> &accelerators = accel.getAccelerators();
  for(size_t i = 0; i < accelerators.size(); i++) {
    const Accelerator &acc = accelerators[i];
    const String &key = acc.getKey();
    if(key.length() == 0) {
      continue;
    }
    if((acc.getFlags() == (ACC_VIRTKEY | ACC_ALT | ACC_NOINVERT)) && altSet.contains(key[0])) {
      result.add(&acc);
    }
  }
  return result;
}

const AcceleratorsDefinition *LanguageSection::findMatchingAccelerator(const DialogDefinition &dialog, const MenuDefinition *menu) const {
  if(menu) {
    const CompactArray<MenuTextItem*> menuHotKeys = menu->getMenuItemsWithHotKey();
    intptr_t                          bestCount       = 0;
    const AcceleratorsDefinition     *bestAccelerator = NULL;
    for(size_t i = 0; i < m_acceleratorDefinitions.size(); i++) {
      const AcceleratorsDefinition &accel        = m_acceleratorDefinitions[i];

      const Array<MenuItemAcceleratorPair> pairArray = findMatchingCommands(accel, menuHotKeys);
      if((int)pairArray.size() > bestCount) {
        bestCount       = pairArray.size();
        bestAccelerator = &accel;
      }
    }
    if(bestAccelerator != NULL) {
      return bestAccelerator;
    }
  }
  const StringArray ctrlStrings = dialog.getControlStringWithAltNeedingAccelerator();

  const BitSet altSet = m_rf->getAltLetterSet(ctrlStrings, format(_T("Dialog %s"), dialog.getId().cstr()));
  if(!altSet.isEmpty()) {
    double                        bestPct         = 0;
    const AcceleratorsDefinition *bestAccelerator = NULL;
    for(size_t i = 0; i < m_acceleratorDefinitions.size(); i++) {
      const AcceleratorsDefinition    &accel   = m_acceleratorDefinitions[i];
      const Array<const Accelerator*> accArray = findMatchingAltKeys(accel, altSet);
      String accelId      = accel.getId();
      size_t altSetSize   = altSet.size();
      size_t accArraySize = accArray.size();
      size_t accelSize    = accel.getAccelerators().size();
      const double pct = PERCENT(accArray.size(), accel.getAccelerators().size());
      if(pct > bestPct) {
        bestPct = pct;
        bestAccelerator = &accel;
      }
    }
    if(bestAccelerator) {
      return bestAccelerator;
    }
  }
  return NULL;
}


void LanguageSection::checkHasAllMenuAndStrings(const StringHashSet &menuIds, const StringHashSet &stringIds) const {
  for(Iterator<String> it = menuIds.getIterator(); it.hasNext();) {
    const String &id = it.next();
    if(findMenu(id) == NULL) {
      m_rf->error(_T("Menu %s not defined for language %s"), id.cstr(), getLanguage().getId().cstr());
    }
  }
  for(Iterator<String> it = stringIds.getIterator(); it.hasNext();) {
    const String &id = it.next();
    if(m_stringTable.findStringById(id) == NULL) {
      m_rf->error(_T("StringId %s not defined for language %s"), id.cstr(), getLanguage().getId().cstr());
    }
  }
}

String LanguageSection::toString() const {
  String result = m_language.toString() + _T("\n");
  if(m_dialogs.size()) {
    result += _T("Dialogs\n");
    for(size_t i = 0; i < m_dialogs.size(); i++) {
      result += m_dialogs[i].toString(); result += _T("\n");
    }
  }

  if(m_menues.size()) {
    result += _T("Menues\n");
    for(size_t i = 0; i < m_menues.size(); i++) {
      result += m_menues[i].toString(); result += _T("\n");
    }
  }

  if(m_acceleratorDefinitions.size()) {
    result += _T("Accelerator tables\n");
    for(size_t i = 0; i < m_acceleratorDefinitions.size(); i++) {
      result += m_acceleratorDefinitions[i].toString(); result += _T("\n");
    }
  }
  if(!m_stringTable.isEmpty()) {
    result += m_stringTable.toString();
  }
  return result;
}

