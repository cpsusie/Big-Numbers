#pragma once

#define MAXCODE 0xffff
// = max(Instruction.adr)


typedef enum {
   CODECRTAB                  /*0userTabCreate(data[top])                                      */
  ,CODECRLIK                  /* userTabCreatelike(top-1,top)                                  */
  ,CODEDRTAB                  /* userTabDrop(top)                                              */
  ,CODECRINX                  /* sqlcrindex(m_db,data[top])                                    */
  ,CODEDRINX                  /* userIndexDrop((top)                                           */
  ,CODERETURN                 /* return                                                        */
  ,CODETRBEGIN                /* trbegin()                                                     */
  ,CODETRCOMMIT               /* trcommit()                                                    */
  ,CODETRABORT                /* trabort()                                                     */
  ,CODETUPINIT                /* tuplereg[ins.reg].init(ins.index)                             */
  ,CODETUPINSERT              /*1insert(top,tuplereg[adr])                                     */
  ,CODEPOPTUP                 /* m_tuplereg[ins.reg]->m_fields[ins.index] = m_stack.pop();     */
  ,CODEPUSHTUP                /* m_stack.push(m_tuplereg[ins.reg]->m_fields[ins.index]);       */
  ,CODEPUSHHV                 /* push(hostvar[ins.adr])                                        */
  ,CODEPUSHCONST              /* push(data(ins.adr))                                           */
  ,CODEPUSHADR                /* push(ins.adr)                                                 */
  ,CODEAND                    /* push((top-1) AND top)                                         */
  ,CODEOR                     /* push((top-1) OR  top)                                         */
  ,CODENOT                    /* top = not top                                                 */
  ,CODEADD                    /* push((top-1) + top)                                           */
  ,CODESUB                    /*2push((top-1) - top)                                           */
  ,CODEMULT                   /* push((top-1) * top)                                           */
  ,CODEDIV                    /* push((top-1) / top)                                           */
  ,CODEMOD                    /* push((top-1) % top)                                           */
  ,CODEEXPO                   /* push((top-1) ** top)                                          */
  ,CODENEG                    /* top = -top                                                    */
  ,CODECONCAT                 /* push((top-1) || top)                                          */
  ,CODESUBSTR                 /* push(substr((top-2),(top-1),top))                             */
  ,CODEDATE                   /* push(Date((top-2),(top-1),top))                               */
  ,CODETIME                   /* push(Time((top-2),(top-1),top))                               */
  ,CODETIMESTAMP              /*3push(Timestamp((top-5),(top-4),(top-3),(top-2),(top-1),top))  */
  ,CODECAST                   /* top = cast(CastParameter(adr))top                             */
  ,CODEPUSHNULL               /* push(nullptr)                                                    */
  ,CODEPUSHCURRENTDATE        /* push currentdate                                              */
  ,CODEPUSHCURRENTTIME        /* push currenttime                                              */
  ,CODEPUSHCURRENTTIMESTAMP   /* push currenttimestamp                                         */
  ,CODEOPENCURSOR             /* cursor[ins.reg] = tablcursor.open((TableCursorParam*)(top),tuplereg[0],tuplereg[1]) */
  ,CODEFETCHCURSOR            /* status = cursor[ins.reg].fetch(m_tuplereg[ins.index])?0:1     */
  ,CODESENDTUP                /* pipelines[ins.index].write(m_tuplereg[ins.reg])               */
  ,CODESENDEOF                /* pipelines[ins.adr].writeeof                                   */
  ,CODERECEIVETUP             /*4status = pipelines[ins.index].read(m_tuplereg[ins.reg])?0:1   */
  ,CODENEWOPERATOR            /* start new operator(entrypoint = ins.adr)                      */
  ,CODEINITPIPES              /* pipelines.init(ins.adr)                                       */
  ,CODEEQ                     /* push(top == top-1)                                            */
  ,CODENQ                     /* push(top != top-1)                                            */
  ,CODEGE                     /* push(top >= top-1)                                            */
  ,CODEGT                     /* push(top >  top-1)                                            */
  ,CODELE                     /* push(top <= top-1)                                            */
  ,CODELT                     /* push(top <  top-1)                                            */
  ,CODEISNULL                 /* push(top.isnull)                                              */
  ,CODEISLIKE                 /*5push(m_sqlregex.islike(top-1,top))                            */
  ,CODECMP                    /* if((status = compare(top,top-1)) == -2) pcreg = ins.adr       */
  ,CODECMPTRUE                /* if((status = compare(top,TRUE )) == -2) pcreg = ins.adr       */
  ,CODEJMPONDEFINED           /* if(!top.isNull()) pcreg = ins.adr                             */
  ,CODEJMP                    /* pcreg = ins.adr                                               */
  ,CODEJMPEQ                  /* if(status == 0) pcreg = ins.adr                               */
  ,CODEJMPNQ                  /* if(status != 0) pcreg = ins.adr                               */
  ,CODEJMPGE                  /* if(status >= 0) pcreg = ins.adr                               */
  ,CODEJMPGT                  /* if(status >  0) pcreg = ins.adr                               */
  ,CODEJMPLE                  /* if(status = -1 || status = 0) pcreg = ins.adr                 */
  ,CODEJMPLT                  /*6if(status = -1  pcreg = ins.adr                               */
} VirtualOpCode;

class VirtualCodeHeader {
public:
  SqlApiBindProgramId m_programid;
  UINT                m_totalSize;
  UINT                m_dataOffset;
  int                 m_hostvarDescriptionList;
  String toString() const;
};

#pragma pack(push,2)

class Instruction {
private:
  static BYTE s_insSizeTable[];
  UINT m_opcode : 8;
  UINT m_addr   : 16;
  UINT m_filler : 8;
public:
  inline VirtualOpCode opcode() const {
    return (VirtualOpCode)m_opcode;
  }
  inline UINT addr() const {
    return m_addr;
  }
  inline UINT reg()  const {
    return (addr() >> 14) & 3;
  }
  inline UINT index() const {
    return addr() & 0x3fff;
  }
  inline UINT size() const {
    return s_insSizeTable[opcode()];
  }
  Instruction(VirtualOpCode _opcode);
  Instruction(VirtualOpCode _opcode, UINT _adr);
  Instruction(VirtualOpCode _opcode, UINT _reg, UINT _index);
  Instruction() {}
};

#pragma pack(pop)

class VirtualCode {
private:
  VirtualCodeHeader m_head;
  BYTE              m_code[MAXCODE];
  inline String constToString(UINT addr) const {
    return getConst(addr).toString();
  }
public:
  friend class CodeGeneration;
  inline VirtualCode() {
    m_head.m_totalSize = m_head.m_dataOffset = 0;
    m_head.m_hostvarDescriptionList = -1;
  }
  UINT append(const void *data, UINT size);
  UINT append(const Packer &pack);
  inline UINT totalSize()                   const { return m_head.m_totalSize; }
  inline const BYTE *getCode(int index = 0) const { return m_code + index;     }
  inline void setProgramId(const SqlApiBindProgramId &programid) { m_head.m_programid  = programid; }
  const SqlApiBindProgramId getProgramId()  const { return m_head.m_programid; }
  inline const BYTE *data(int adr)          const { return m_code + m_head.m_dataOffset + adr; }
  HostVarDescriptionList getDescription()   const;
  inline bool hasDescription()              const { return m_head.m_hostvarDescriptionList >= 0; }
  TupleField getConst(UINT addr)            const;
  String toString() const;
  inline void dump(FILE *f = stdout) const {
    _ftprintf(f, _T("%s"), toString().cstr());
  }
};

class CodeGeneration {
private:
  VirtualCode m_codesegment;
  VirtualCode m_datasegment;
public:
  UINT appendData(const void *data, UINT size) {
    return m_datasegment.append(data,size);
  }
  UINT appendData(const Packer &pack) {
    return m_datasegment.append(pack);
  }
  UINT appendIns0(VirtualOpCode opcode);
  UINT appendIns1(VirtualOpCode opcode, UINT adr);
  UINT appendIns2(VirtualOpCode opcode, UINT reg, UINT index);
  UINT appendConst(const char    *s);
  UINT appendConst(const TCHAR   *s);
  UINT appendConst(char           i);
  UINT appendConst(UCHAR          i);
  UINT appendConst(short          i);
  UINT appendConst(USHORT         i);
  UINT appendConst(int            i);
  UINT appendConst(UINT           i);
  UINT appendConst(long           i);
  UINT appendConst(ULONG          i);
  UINT appendConst(float          d);
  UINT appendConst(double         d);
  UINT appendConst(Date           d);
  UINT appendConst(Time           d);
  UINT appendConst(Timestamp      d);
  void         fixins1( UINT pos, UINT adr);
  void         fixins2( UINT pos, UINT reg, UINT index );
  inline int   currentCodeSize() const { return m_codesegment.totalSize(); }
  void         appendDesc( const HostVarDescriptionList &desc);
  void         appendDataToCode();
  void         splitCodeAndData(const VirtualCode &loadedcode);
  inline const VirtualCode &getCode() const { return m_codesegment; }
  String       toString() const;
  void         dump(FILE *f = stdout) const;
};

class CastParameter {
public:
  int          m_type;
  int          m_len;
  CastParameter(DbFieldType type, int len) { m_type = type;           m_len = len; }
  CastParameter()                          { m_type = DBTYPE_UNKNOWN; m_len = 0;   }
  inline DbFieldType getType() const { return (DbFieldType)m_type; }
  String toString() const;
};

Packer &operator<<(Packer &p, const CastParameter &v );
Packer &operator>>(Packer &p,       CastParameter &v );

TupleField sqlcast(const TupleField &v, const CastParameter &p);
