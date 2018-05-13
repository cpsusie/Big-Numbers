#pragma once

#include <MyUtil.h>
#include <Scanner.h>
#include "ResourceSymbol.h"

class SyntaxNode;

class SyntaxNodeHandler {
public:
  virtual void handleNode(const SyntaxNode *n) = 0;
};

class SyntaxNode {
private:
  int            m_symbol;
  SourcePosition m_pos;
  Exception createException(const String &attribute) const;
public:
  SyntaxNode(int symbol);
  SyntaxNode(const SourcePosition &pos, int symbol);
  int                               getSymbol()        const   { return m_symbol;                         }
  const SourcePosition             &getPos()           const   { return m_pos;                            }
  String                            getSymbolName()    const;
  virtual SyntaxNode     *getLeftChild()     const             { return getChild(0);                      }
  virtual SyntaxNode     *getRightChild()    const             { return getChild(1);                      }
  virtual SyntaxNode     *getChild(unsigned int i)     const   { throw createException(_T("child"));      }
  virtual int                       getChildCount()    const   { throw createException(_T("childCount")); }
  virtual const String             &getName()          const   { throw createException(_T("name"));       }
  virtual const String             &getString()        const   { throw createException(_T("string"));     }
  virtual int                       getNumber()        const   { throw createException(_T("number"));     }
  virtual bool                      isName()           const   { return m_symbol == IDENTIFIER;           }
  virtual bool                      isNumber()         const   { return m_symbol == NUMBER;               }
  virtual bool                      isString()         const   { return m_symbol == STRING;               }
  virtual SyntaxNode     *clone() const = 0;
  int     getNodeCount() const;
  virtual String toString() const = 0;
  virtual ~SyntaxNode() {}
};

class NumberSyntaxNode : public SyntaxNode {
private:
  int m_value;
public:
  NumberSyntaxNode(const SourcePosition &pos, int value) : SyntaxNode(pos, NUMBER) {
    m_value = value;
  }

  NumberSyntaxNode(int value) : SyntaxNode(NUMBER) {
    m_value = value;
  }

  int getNumber() const {
    return m_value;
  }

  SyntaxNode  *clone() const {
    return new NumberSyntaxNode(getPos(), m_value);
  }

  String toString() const {
    return ::toString(m_value);
  }
};

class NameSyntaxNode : public SyntaxNode {
private:
  String m_name;
public:

  NameSyntaxNode(const String &name) : SyntaxNode(IDENTIFIER) {
    m_name = name;
  }

  NameSyntaxNode(const SourcePosition &pos, const String &name) : SyntaxNode(pos, IDENTIFIER)  {
    m_name = name;
  }

  const String &getName() const {
    return m_name;
  }

  SyntaxNode *clone() const {
    return new NameSyntaxNode(getPos(), m_name);
  }

  String toString() const {
    return m_name;
  }
};

class StringSyntaxNode : public SyntaxNode {
private:
  String m_str;
public:

  StringSyntaxNode(const String &str) : SyntaxNode(STRING) {
    m_str = str;
  }

  StringSyntaxNode(const SourcePosition &pos, const String &str) : SyntaxNode(pos, STRING)  {
    m_str = str;
  }

  const String &getString() const {
    return m_str;
  }

  SyntaxNode *clone() const {
    return new StringSyntaxNode(getPos(), m_str);
  }

  String toString() const {
    return m_str;
  }
};

class ResourceNodeTree : public SyntaxNode {
private:
  unsigned short m_childCount;
  SyntaxNode   **m_child;
public:
  ResourceNodeTree(const SourcePosition &pos, int symbol, va_list argptr);
  ResourceNodeTree(const ResourceNodeTree *src);
  ~ResourceNodeTree();

  SyntaxNode *getChild(unsigned int i) const;

  int getChildCount() const {
    return m_childCount;
  }

  SyntaxNode *clone() const;
  String toString() const;
};
