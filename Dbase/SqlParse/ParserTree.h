#pragma once

#include <Date.h>
#include <Scanner.h>
#include "SqlSymbol.h"

class SyntaxNode;

class SyntaxNodeData {
public:
  SyntaxNode *m_node;
  SyntaxNodeData(SyntaxNode *node);
//  SyntaxNodeData(const SyntaxNodeData &src) { m_node = src.m_node; }
};

class SyntaxNode {
private:
  unsigned short  m_token;
  SourcePosition m_pos;
  union {
    double            m_number;
    String           *m_str;       // must be pointer because of union
    Date             *m_date;      // must be pointer because of union
    Time             *m_time;      // must be pointer because of union
    Timestamp        *m_timestamp; // must be pointer because of union
    struct {
      BYTE         m_count;
      SyntaxNode **m_child;
    } m_children;
  } m_attributes;
  union {
    int               m_hostvarindex;
    int               m_likeoperatorindex;
    SyntaxNodeData   *m_data;
  } m_userattributes;
  void init(int token);
public:
  SyntaxNode(const SyntaxNode *src);
  SyntaxNode(const char    *s, int token);
  SyntaxNode(const wchar_t *s, int token);
  SyntaxNode(double number);
  SyntaxNode(const Date      &d);
  SyntaxNode(const Time      &d);
  SyntaxNode(const Timestamp &d);
  SyntaxNode(int token, va_list argptr);
  ~SyntaxNode();
  SyntaxNode &operator=(const SyntaxNode &src); // not defined.  just to be sure we wont call it
  int    token()                          const { return m_token;  }
  SourcePosition pos()                    const { return m_pos;    }
  void setpos(const SourcePosition &pos)        { m_pos = pos;     }
  const TCHAR     *name()                 const;
  const TCHAR     *param()                const;
  const TCHAR     *hostvar()              const;
  const TCHAR     *str()                  const;
  const Date      &getDate()              const;
  const Time      &getTime()              const;
  const Timestamp &getTimestamp()         const;
  double number()                         const;
  SyntaxNode *child(UINT i)               const;
  SyntaxNode *findChild(int searchtoken)  const;
  UINT        childCount()                const;
  void   setHostVarIndex(int hostvarindex);
  int    getHostVarIndex()                const;
  void   setLikeOperatorIndex(int index);
  int    getLikeOperatorIndex()           const;
  void   setData(SyntaxNodeData *p);
  SyntaxNodeData *getData()             const;
};

typedef SyntaxNode *SyntaxNodeP;

typedef CompactArray<SyntaxNode*> SyntaxNodeArray;

class ParserTree {
private:
  SyntaxNodeArray     m_nodetable;
  bool                m_ok;
  String              m_filename;
  SyntaxNode *fetchSyntaxNodeWithChildren(int token, va_list argptr);
public:
  SyntaxNode  *m_root;
  StringArray  m_errmsg;
  ParserTree(TCHAR *filename = EMPTYSTRING);
  ParserTree(const ParserTree &src);            // not defined. just to be sure we wont call it
  ParserTree &operator=(const ParserTree &rhs); // do
  virtual ~ParserTree();
  SyntaxNode *vFetchTokenNode(int token,va_list argptr);
  void         releaseAll();
  void         vAppendError( const TCHAR *format, va_list argptr);
  void         appendError(  const TCHAR *format,...);
  void         vAppendError( const SourcePosition &pos, long sqlcode, const TCHAR *format, va_list argptr);
  SyntaxNode  *fetchStringNode(   const TCHAR *str, int token = STRING);
  SyntaxNode  *fetchNumberNode(   double number       );
  SyntaxNode  *fetchDateNode(     const Date &d       );
  SyntaxNode  *fetchTimeNode(     const Time &d       );
  SyntaxNode  *fetchTimestampNode(const Timestamp &d  );
  SyntaxNode  *fetchTokenNode(    int token, ...      );
  void          listErrors(       FILE *f = stdout    );
  void          listErrors(       const String &fname );
  bool          ok()          const { return m_ok;    }
  const String &getFilename() const { return m_filename; }
  void          setFilename(  const String &filename) { m_filename = filename; }
  void          dumpTree(     FILE *f = stdout) const;
  void          dumpTree(     const String &filename) const;
  void          parse(        const String &sourcefile, const SourcePosition &pos, const String &stmt);
};

void dumpSyntaxTree(const SyntaxNode *node, FILE *f = stdout, int level = 0);
void dumpSyntaxTree(const SyntaxNode *node, const String &filename );
String sprintTree(  const SyntaxNode *n);

class NodeList : public SyntaxNodeArray {
private:
  void appendcommalist(SyntaxNode *n, int token);
  void traversetree(   SyntaxNode *n, int token);
public:
  NodeList();
  NodeList(SyntaxNode *n, int token = COMMA, bool binarytree = false);
  void findNodes(SyntaxNode *n, int token = COMMA, bool binarytree = false);
  const SyntaxNode *genlist(ParserTree &tree, const CompactIntArray &set);
  void dump(FILE *f = stdout, int level = 0) const;
};

const TCHAR *relopstring(int token);
