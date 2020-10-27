#include "stdafx.h"

Tuple::Tuple(UINT size) {
  for(UINT i = 0; i < size; i++)
    add(TupleField());
}

String Tuple::toString() const {
  String result;
  const TCHAR *delim = nullptr;
  for(size_t i = 0; i < size(); i++) {
    if(delim) {
      result += delim;
    } else {
      delim = _T(",");
    }
    result += (*this)[i].toString();
  }
  return result;
}

String Tuple::toString(const TuplePrintDefinition &def) const {
  String result;
  const TCHAR *delim = nullptr;
  for(size_t i = 0; i < size(); i++) {
    if(delim) {
      result += delim;
    } else {
      delim = _T(",");
    }
    String s = (*this)[i].toString();
    if(i >= def.size()) {
      result += s;
    } else {
      UINT w = def[i];
      if(s.length() < w) {
        result += s + spaceString(w - s.length());
      } else {
        result += left(s,w);
      }
    }
  }
  return result;
}

void Tuple::dump(FILE *f) const {
  _ftprintf(f,_T("Tuple:<%s>\n"), toString().cstr());
}

void Tuple::dumpTypes(FILE *f) const {
  _ftprintf(f,_T("tupledef:<"));
  for(size_t i = 0; i < size(); i++) {
    if(i > 0) {
      _ftprintf(f,_T(","));
    }
    _ftprintf(f,_T("%s"),getTypeString((*this)[i].getType()));
  }
  _ftprintf(f,_T(">\n"));
}

TuplePrintDefinition::TuplePrintDefinition(const HostVarDescriptionList &desc) {
  for(size_t i = 0; i < desc.size(); i++) {
    const HostVarDescription &h = desc[i];
    UINT w;
    switch(h.getType()) {
    case DBTYPE_CSTRING :
    case DBTYPE_WSTRING :
    case DBTYPE_VARCHAR :
      w = h.sqllen + 2; // remember " in toString
      break;
    case DBTYPE_CSTRINGN:
    case DBTYPE_WSTRINGN:
    case DBTYPE_VARCHARN:
      w = h.sqllen + 2;
      if(w < 4) w = 4;  // null possible
      break;
    default:
      w = getDbTypePrintWidth(desc[i].getType());
      break;
    }
    add(w);
  }
}

bool operator==(const Tuple &t1, const Tuple &t2) {
  if(t1.size() != t2.size()) {
    return false;
  }
  for(size_t i = 0; i < t1.size(); i++) {
    if(compare(t1[i],t2[i]) != 0) {
      return false;
    }
  }
  return true;
}

bool operator!=(const Tuple &t1, const Tuple &t2) {
  return !(t1 == t2);
}
