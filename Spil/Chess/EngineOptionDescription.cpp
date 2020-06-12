#include "stdafx.h"

#if !defined(TABLEBASE_BUILDER)

#include "ExternEngine.h"

const EngineOptionDescription EngineOptionDescription::debugOption(_T("name Debug type check default false"),999);

EngineOptionDescription::EngineOptionDescription(String line, USHORT index) : m_index(index) {
  DEFINEMETHODNAME;
  m_min = m_max = m_defaultInt = 0;
  m_defaultBool = false;
  String defaultStr;
  bool typeDefined = false;

  intptr_t nameIndex = line.find(_T("name"));
  if(nameIndex < 0) {
    throwInvalidArgumentException(method, _T("No name for option. line:<%s>"), line.cstr());
  }
  line = substr(line, nameIndex+5, line.length());
  for(Tokenizer tok1(line, _T(" ")); tok1.hasNext();) {
    const String word = tok1.next();
    if(word == _T("type") || word == _T("default") || word == _T("min") || word == _T("max") || word == _T("var")) {
      intptr_t nextField = line.find(word);
      m_name = trim(substr(line, 0, nextField - 1));
      line = substr(line, nextField, line.length());
      break;
    }
  }

  for(Tokenizer tok(line, _T(" ")); tok.hasNext();) {
    const String field = tok.next();
    if(field == _T("type")) {
      const String typeStr = tok.next();
      if(typeStr == _T("check")) {
        m_type = OptionTypeCheckbox;
      } else if(typeStr == _T("spin")) {
        m_type = OptionTypeSpin;
      } else if(typeStr == _T("combo")) {
        m_type = OptionTypeCombo;
      } else if(typeStr == _T("button")) {
        m_type = OptionTypeButton;
      } else if(typeStr == _T("string")) {
        m_type = OptionTypeString;
      } else {
        throwInvalidArgumentException(method, _T("typeStr=<%s>"), typeStr.cstr());
      }
      typeDefined = true;
    } else if(field == _T("min")) {
      m_min = tok.getInt();
    } else if(field == _T("max")) {
      m_max = tok.getInt();
    } else if(field == _T("default")) {
      if(m_type == OptionTypeString) {
        defaultStr = tok.getRemaining();
        break;
      } else {
        defaultStr = tok.next();
      }
    } else if(field == _T("var")) {
      m_comboValues.add(tok.next());
    } else {
      throwInvalidArgumentException(method, _T("field:<%s>"), field.cstr());
    }
  }
  if(!typeDefined) {
    throwInvalidArgumentException(method, _T("Type not defined. line:<%s>"), line.cstr());
  }
  switch(m_type) {
  case OptionTypeCheckbox :
    if(defaultStr == _T("false")) {
      m_defaultBool = false;
    } else if(defaultStr == _T("true")) {
      m_defaultBool = true;
    } else {
      throwInvalidArgumentException(method, _T("Invalid default value for check option %s:<%s>")
                                   ,m_name.cstr(), defaultStr.cstr());
    }
    break;
  case OptionTypeSpin     :
    if(m_min > m_max) {
      throwInvalidArgumentException(method, _T("Invalid range for spin option %s. min=%d, max=%d")
                                   ,m_name.cstr(), m_min, m_max);
    }
    if(_stscanf(defaultStr.cstr(), _T("%d"), &m_defaultInt) != 1) {
      throwInvalidArgumentException(method, _T("Invalid default value for spin option %s:<%s>")
                                   ,m_name.cstr(), defaultStr.cstr());
    }
    if(m_defaultInt < m_min || m_max < m_defaultInt) {
      throwInvalidArgumentException(method, _T("Invalid default value for spin option %s:%d. Range:[%d;%d]")
                                   ,m_name.cstr(), m_defaultInt, m_min,m_max);
    }
    break;
  case OptionTypeButton   :
    break;
  case OptionTypeCombo    :
    if(m_comboValues.size() == 0) {
      throwInvalidArgumentException(method, _T("No values defined for combo option %s. line:<%s>"), m_name.cstr(), line.cstr());
    }
    // NB continue case
  case OptionTypeString   :
    m_defaultString = defaultStr;
    break;
  }
}

int EngineOptionDescription::getValueIndex(const String &str) const {
  for(UINT i = 0; i < m_comboValues.size(); i++) {
    if(m_comboValues[i] == str) {
      return i;
    }
  }
  return -1;
}

String EngineOptionDescription::getLabelName() const {
  String result = m_name;
  return result.replace('_',' ');
}

String EngineOptionDescription::toString(int nameLength) const {
  switch(m_type) {
  case OptionTypeCheckbox: return format(_T("%-*s Checkbox default:%s")                 , nameLength, m_name.cstr(), boolToStr(m_defaultBool));
  case OptionTypeSpin    : return format(_T("%-*s Spin     default:%6d  range:[%d..%d]"), nameLength, m_name.cstr(), m_defaultInt, m_min, m_max);
  case OptionTypeCombo   : return format(_T("%-*s Combobox default:<%s> values:[%s]")   , nameLength, m_name.cstr(), m_defaultString.cstr(), m_comboValues.toString().cstr());
  case OptionTypeButton  : return format(_T("%-*s Button")                              , nameLength, m_name.cstr());
  case OptionTypeString  : return format(_T("%-*s String   default:<%s>")               , nameLength, m_name.cstr(), m_defaultString.cstr());
  }
  return format(_T("%s:Unknown type:%d"), m_name.cstr(), m_type);
}

String EngineOptionDescription::getDefaultAsString() const {
  switch(m_type) {
  case OptionTypeCheckbox: return getDefaultBool() ? _T("Checked") : _T("Unchecked");
  case OptionTypeSpin    : return format(_T("%d"), getDefaultInt());
  case OptionTypeCombo   :
  case OptionTypeString  : return getDefaultString();
  case OptionTypeButton  :
  default                : return EMPTYSTRING;
  }
}

const EngineOptionDescription *EngineOptionDescriptionArray::findOptionByName(const String &optionName) const {
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionDescription &option = (*this)[i];
    if(option.getName() == optionName) {
      return &option;
    }
  }
  return NULL;
}

EngineOptionValueArray EngineOptionDescriptionArray::createDefaultValueArray(const String &engineName) const {
  EngineOptionValueArray result(engineName);
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionDescription &desc = (*this)[i];
    switch(desc.getType()) {
    case OptionTypeCheckbox: result.setValue(desc.getName(), desc.getDefaultBool());   break;
    case OptionTypeSpin    : result.setValue(desc.getName(), desc.getDefaultInt());    break;
    case OptionTypeCombo   :
    case OptionTypeString  : result.setValue(desc.getName(), desc.getDefaultString()); break;
    }
  }
  return result;
}

EngineOptionDescriptionArray &EngineOptionDescriptionArray::removeOptionsByType(EngineOptionType type) { // return *this
  for(size_t i = size(); i--;) {
    if((*this)[i].getType() == type) {
      removeIndex(i);
    }
  }
  return *this;
}

EngineOptionValueArray EngineOptionDescriptionArray::pruneDefaults(const EngineOptionValueArray &valueArray) const {
  EngineOptionValueArray result(valueArray);
  for(size_t i = result.size(); i--;) {
    const EngineOptionValue       &v      = result[i];
    const EngineOptionDescription *option = findOptionByName(v.getName());
    bool prune = false;
    if(option == NULL) {
      prune = true;
    } else {
      switch(option->getType()) {
      case OptionTypeSpin    :
        if(v.getIntValue() == option->getDefaultInt()) {
          prune = true;
        }
        break;
      case OptionTypeCombo   :
      case OptionTypeString  :
        if(v.getStringValue() == option->getDefaultString()) {
          prune = true;
        }
        break;
      case OptionTypeCheckbox:
        if(v.getBoolValue() == option->getDefaultBool()) {
          prune = true;
        }
        break;
      default:
        prune = true;
        break;
      }
    }
    if(prune) {
      result.removeIndex(i);
    }
  }
  return result;
}

String EngineOptionDescriptionArray::toString() const {
  const int nl = findMaxNameLength();
  String result;
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionDescription &option = (*this)[i];
    result += format(_T("%s\n"), option.toString(nl).cstr());
  }
  return result;
}

int EngineOptionDescriptionArray::findMaxNameLength() const {
  int result = 0;
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionDescription &option = (*this)[i];
    const int l = (int)option.getName().length();
    if(l > result) result = l;
  }
  return result;
}

#endif // TABLEBASE_BUILDER
