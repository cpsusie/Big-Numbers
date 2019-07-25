#pragma once

#include "ResourceParser.h"

typedef enum {
  LANGUAGE_DIRECTIVE
 ,DIALOG_DEFINITION
 ,MENU_DEFINITION
 ,ACCELERATOR_DEFNITION
 ,STRINGTABLE_DEFINITION
} ResourceType;

typedef enum {
  CTRL_TEXT
 ,CTRL_PUSHBUTTON
 ,CTRL_CHECKBOX
} ControlType;

typedef enum {
  MENU_POPUP
 ,MENU_TEXT
} MenuItemType;

class ResourceFile;

class ResourceDefinition {
private:
  ResourceType        m_type;
  String              m_id;
  const ResourceFile *m_rf;
public:
  ResourceDefinition(ResourceType type, const String &id, const ResourceFile *rf) : m_type(type), m_id(id), m_rf(rf) {
  }
  virtual ~ResourceDefinition() {
  }
  ResourceType getType() const {
    return m_type;
  }
  const String &getId() const {
    return m_id;
  }
  const ResourceFile &getResourceFile() const {
    return *m_rf;
  }
  virtual String toString() const = 0;
  void clear() {
    m_id = EMPTYSTRING;
  }
};

class LanguageDirective : public ResourceDefinition {
protected:
  String m_subLangId;
public:
  LanguageDirective(const ResourceFile *rf);
  LanguageDirective(const SyntaxNode *n, const ResourceFile *rf);
  const String &getSubLangId() const {
    return m_subLangId;
  }
  void clear();
  String toString() const;
};

class ControlDefinition {
private:
  ControlType m_type;
  String      m_id;
  String      m_visibleText;
public:
  ControlDefinition(const SyntaxNode *n);
  ControlType getType() const {
    return m_type;
  }
  const String &getId() const {
    return m_id;
  }
  const String &getVisibleText() const {
    return m_visibleText;
  }
  String toString() const;
};

class DialogDefinition : public ResourceDefinition {
private:
  String                   m_menuId;
  Array<ControlDefinition> m_controls;
  const String &getControlText(const ControlDefinition &ctrl) const;
public:
  DialogDefinition(const SyntaxNode *n, const ResourceFile *rf);
  const String &getMenuId() const {
    return m_menuId;
  }
  bool hasMenu() const {
    return m_menuId.length() != 0;
  }
  bool hasControls() const {
    return m_controls.size() > 0;
  }
//  const Array<ControlDefinition> &getControls() const {
//    return m_controls;
//  }
  StringArray getControlStringWithAltNeedingAccelerator() const;
  StringArray getControlStrings() const;
  String toString() const;
};

#define ACC_VIRTKEY   0x0001
#define ACC_ASCII     0x0002
#define ACC_SHIFT     0x0004
#define ACC_CONTROL   0x0008
#define ACC_ALT       0x0010
#define ACC_NOINVERT  0x0020

class Accelerator {
private:
  String m_command;
  String m_key;
  int    m_asciiKeyValue;
  int    m_flags;
public:
  Accelerator(const SyntaxNode *n);
  String toString() const;
  static String toString(const CompactArray<const Accelerator*> &accArray);
  static String flagsToString(int flags);
  const String &getCommand() const {
    return m_command;
  }
  const String &getKey() const {
    return m_key;
  }
  int getAscii() const {
    return m_asciiKeyValue;
  }
  int getFlags() const {
    return m_flags;
  }
  bool operator==(const Accelerator &a) const;
};

class AcceleratorsDefinition : public ResourceDefinition {
private:
  Array<Accelerator> m_accelerators;
public:
  AcceleratorsDefinition(const SyntaxNode *n, const ResourceFile *rf);

  const Array<Accelerator> &getAccelerators() const {
    return m_accelerators;
  }

  CompactArray<const Accelerator*> findAcceleratorByCommand(const String &command) const;
  String toString() const;
};

class MenuItem {
private:
  MenuItemType m_type;
protected:
  String       m_visibleText;
public:
  MenuItem(MenuItemType type) : m_type(type) {
  }
  virtual ~MenuItem() {
  }
  MenuItemType getType() const {
    return m_type;
  }
  const String &getVisibleText() const {
    return m_visibleText;
  }
  virtual String toString(int level=0) const;
  virtual MenuItem *clone() const = 0;
};

class MenuHotKey {
public:
  String m_key;
  int    m_flags;
  MenuHotKey() {
    m_flags = 0;
  }
  MenuHotKey(const String &s);
  String toString() const;
  bool matchAccelerator(const Accelerator &acc) const;
  bool hasAnyMatchingAccelKeys(const CompactArray<const Accelerator*> &accArray) const;
};

class MenuTextItem : public MenuItem {
private:
  String m_command;
public:
  MenuTextItem(const SyntaxNode *n);
  MenuItem *clone() const { MenuItem *copy = new MenuTextItem(*this); TRACE_NEW(copy); return copy; }
  const String &getCommand() const {
    return m_command;
  }
  static CompactArray<MenuTextItem*> getMenuItemsWithHotKey(const CompactArray<MenuItem*> items);
  MenuHotKey getHotKey() const; // parse string after \t (assume it exist)
  String toString(int level=0) const;
};

class PopupMenu : public MenuItem {
private:
  CompactArray<MenuItem*> m_items;
public:
  PopupMenu(const SyntaxNode *n);
  PopupMenu(const PopupMenu &m);
  PopupMenu &operator=(const PopupMenu &m); // not defined
  ~PopupMenu();
  const CompactArray<MenuItem*> &getItems() const {
    return m_items;
  }
  String toString(int level=0) const;
  MenuItem *clone() const {
    MenuItem *copy = new PopupMenu(*this); TRACE_NEW(copy); return copy;
  }
};

class MenuDefinition : public ResourceDefinition {
private:
  CompactArray<MenuItem*> m_items;
public:
  MenuDefinition(const SyntaxNode *n, const ResourceFile *rf);
  MenuDefinition(const MenuDefinition &src);
  MenuDefinition &operator=(const MenuDefinition &src); // not defined
 ~MenuDefinition();
  const CompactArray<MenuItem*> &getItems() const {
    return m_items;
  }
  StringArray getTopLevelStrings() const;
  CompactArray<MenuTextItem*> getMenuItemsWithHotKey() const {
    return MenuTextItem::getMenuItemsWithHotKey(m_items);
  };
  void compareMenues(const MenuDefinition &mn) const;
  String toString() const;
};

class StringId {
private:
  String m_id;
  String m_text;
public:
  StringId(const SyntaxNode *n);
  StringId(const String &id) {
    m_id = id;
  }
  String toString() const;
  const String getId() const {
    return m_id;
  }
  const String &getText() const {
    return m_text;
  }
};

class StringTableDefinition : ResourceDefinition {
private:
  Array<StringId> m_strings;
public:
  StringTableDefinition(const ResourceFile *rf);
  StringTableDefinition(const SyntaxNode *n, const ResourceFile *rf);
  String toString() const;
  void append(const StringTableDefinition &src);
  bool isEmpty() const {
    return m_strings.isEmpty();
  }
  void clear() {
    m_strings.clear();
  }
  void sort();
  const String *findStringById(const String &id) const;
  const Array<StringId> &getAllStrings() const {
    return m_strings;
  }
};

class LanguageSection {
private:
  const ResourceFile           *m_rf;
  LanguageDirective             m_language;
  Array<DialogDefinition>       m_dialogs;
  Array<MenuDefinition>         m_menues;
  Array<AcceleratorsDefinition> m_acceleratorDefinitions;
  StringTableDefinition         m_stringTable;
  void fixup();
  const MenuDefinition *findMenu(const String &id) const;
  const AcceleratorsDefinition *findMatchingAccelerator(const DialogDefinition &dialog, const MenuDefinition *menu) const;
  void checkHasAllMenuAndStrings(const StringHashSet &menuIds, const StringHashSet &stringIds) const;
  String toString() const;
  friend class ResourceFile;
public:
  LanguageSection(const ResourceFile *rf) : m_rf(rf), m_language(rf), m_stringTable(rf) {
  }
  bool isEmpty() const;
  void clear();
  const LanguageDirective &getLanguage() const {
    return m_language;
  }
  const Array<DialogDefinition>       &getDialogs() const {
    return m_dialogs;
  }
  const Array<MenuDefinition>         &getMenues() const {
    return m_menues;
  }
  const Array<AcceleratorsDefinition> &getAcceleratorDefinitions() const {
    return m_acceleratorDefinitions;
  }
  const StringTableDefinition         &getStringTable() const {
    return m_stringTable;
  }
  void compareSections(const LanguageSection &ls) const;
};

class ResourceFile {
private:
  String                         m_fileName;
  mutable bool                   m_ok;
  mutable StringArray            m_errors;
  CompactArray<LanguageSection*> m_sections;       // all sections defined in the file
  mutable const LanguageSection *m_currentSection; // section being analyzed
  LanguageSection *newSection();
  void checkHasSection(LanguageSection *&section, const SyntaxNode *n);
  void pruneEmptySection();
  void analyzeCurrentSection() const;
  void analyzeDialog(const DialogDefinition &dialog) const;
  void analyzeMenu(const String &id, const String &popupTet, const CompactArray<MenuItem*> &itemArray) const;
  void analyzeAccelerator(const AcceleratorsDefinition &accel) const;

  BitSet getAltLetterSet(const StringArray &a, const String &container) const;
  const MenuDefinition *findMenu(const String &id) const; // try lookup menu in all sections
  const AcceleratorsDefinition *findMatchingAccelerator(const DialogDefinition &dialog, const MenuDefinition *menu) const;
  StringHashSet getAllMenuIds() const;
  StringHashSet getAllStringIds() const;
  friend class LanguageSection;
public:
  ResourceFile(const String &fileName);
  ResourceFile(const ResourceFile &src);      // not defined
  ResourceFile &operator=(ResourceFile &src); // not defined
  ~ResourceFile();
  const String &getFileName() const {
    return m_fileName;
  }
  const CompactArray<LanguageSection*> &getSections() const {
    return m_sections;
  }
  const LanguageSection *getCurrentSection() const {
    return m_currentSection;
  }
  bool isOk() const {
    return m_ok;
  }
  bool isEmpty() const {
    return m_sections.isEmpty();
  }
  void analyze() const;
  void verror(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) const;
  void error(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const;
  void listErrors(FILE *f = stdout) const;
  String toString() const;
};
