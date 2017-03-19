#include "stdafx.h"
#include <ByteMemoryStream.h>

Instruction::Instruction(VirtualOpCode _opcode)
: m_opcode(_opcode) {
  if(opcode() != _opcode) {
    throwSqlError(SQL_FATAL_ERROR,_T("opcode = %d doesn't fit in Instruction"),_opcode);
  }
}

Instruction::Instruction(VirtualOpCode _opcode, UINT _addr)
: m_opcode(_opcode)
, m_addr(  _addr  ) {
  if(opcode() != _opcode || addr() != _addr) {
    throwSqlError(SQL_FATAL_ERROR,_T("opcode,adr = (%d,%d) doesn't fit in Instruction"),_opcode,_addr);
  }
}

Instruction::Instruction(VirtualOpCode _opcode, UINT _reg, UINT _index)
: m_opcode(_opcode)
, m_addr( ((_reg & 3) << 14) | (_index & 0x3fff)) {
  if(opcode() != _opcode || reg() != _reg || index() != _index) {
    throwSqlError(SQL_FATAL_ERROR,_T("opcode,reg,index = (%d,%d,%d) doesn't fit in Instruction"),_opcode,_reg,_index);
  }
}

BYTE Instruction::s_insSizeTable[] = {
  1/*0CODECRTAB           userTabCreate(data[top])                                      */
 ,1/* CODECRLIK           userTabCreatelike(top-1,top)                                  */
 ,1/* CODEDRTAB           userTabDrop(top)                                              */
 ,1/* CODECRINX           sqlCreateIndex(m_db,data[top])                                */
 ,1/* CODEDRINX           userIndexDrop(top)                                            */
 ,1/* CODERETURN          return                                                        */
 ,1/* CODETRBEGIN         trbegin()                                                     */
 ,1/* CODETRCOMMIT        trcommit()                                                    */
 ,1/* CODETRABORT         trabort()                                                     */
 ,3/* CODETUPINIT         tuplereg[ins.reg].init(ins.index)                             */
 ,3/*1CODETUPINSERT       insert(top,tuplereg[adr])                                     */
 ,3/* CODEPOPTUP          m_tuplereg[ins.reg]->m_fields[ins.index] = m_stack.pop();     */
 ,3/* CODEPUSHTUP         m_stack.push(m_tuplereg[ins.reg]->m_fields[ins.index]);       */
 ,3/* CODEPUSHHV          push(hostvar[ins.adr])                                        */
 ,3/* CODEPUSHCONST       push(data(ins.adr))                                           */
 ,3/* CODEPUSHADR         push(ins.adr)                                                 */
 ,1/* CODEAND             push((top-1) AND top)                                         */
 ,1/* CODEOR              push((top-1) OR  top)                                         */
 ,1/* CODENOT             top = not top                                                 */
 ,1/* CODEADD             push((top-1) + top)                                           */
 ,1/*2CODESUB             push((top-1) - top)                                           */
 ,1/* CODEMULT            push((top-1) * top)                                           */
 ,1/* CODEDIV             push((top-1) / top)                                           */
 ,1/* CODEMOD             push((top-1) % top)                                           */
 ,1/* CODEEXPO            push((top-1) ** top)                                          */
 ,1/* CODENEG             top = -top                                                    */
 ,1/* CODECONCAT          push((top-1) || top)                                          */
 ,1/* CODESUBSTR          push(substr((top-2),(top-1),top))                             */
 ,1/* CODEDATE            push(Date((top-2),(top-1),top))                               */
 ,1/* CODETIME            push(Time((top-2),(top-1),top))                              */
 ,1/*3CODETIMESTAMP       push(Timestamp((top-5),(top-4),(top-3),(top-2),(top-1),top))  */
 ,3/* CODECAST            top = cast(CastParameter(adr))top                                 */
 ,1/* CODEPUSHNULL        push(NULL)                                                    */
 ,1/* CODEPUSHCURRENTDATE push currentdate                                              */
 ,1/* CODEPUSHCURRENTTIME push currenttime                                              */
 ,1/* CODEPUSHCURRENTTIMESTAMP push currenttimestamp                                    */
 ,3/* CODEOPENCURSOR      cursor[ins.reg] = tablcursor.open((TableCursorParam*)(top),tuplereg[0],tuplereg[1]) */
 ,3/* CODEFETCHCURSOR     status = cursor[ins.reg].fetch(m_tuplereg[ins.index])?0:1     */
 ,3/* CODESENDTUP         pipelines[ins.index].write(m_tuplereg[ins.reg])               */
 ,3/* CODESENDEOF         pipelines[ins.adr].writeeof                                   */
 ,3/*4CODERECEIVETUP      status = pipelines[ins.index].read(m_tuplereg[ins.reg])?0:1   */
 ,3/* CODENEWOPERATOR     start new operator(entrypoint = ins.adr)                      */
 ,3/* CODEINITPIPES       pipelines.init(ins.adr)                                       */
 ,1/* CODEEQ              push(top == top-1)                                            */
 ,1/* CODENQ              push(top != top-1)                                            */
 ,1/* CODEGE              push(top >= top-1)                                            */
 ,1/* CODEGT              push(top >  top-1)                                            */
 ,1/* CODELE              push(top <= top-1)                                            */
 ,1/* CODELT              push(top <  top-1)                                            */
 ,1/* CODEISNULL          push(top.isnull)                                              */
 ,3/*5CODEISLIKE          push(m_sqlregex.islike(ins.adr,top-1,top))                    */
 ,3/* CODECMP             if((status = compare(top,top-1)) == -2) pcreg = ins.adr       */
 ,3/* CODECMPTRUE         if((status = compare(top,TRUE )) == -2) pcreg = ins.adr       */
 ,3/* CODEJMPONDEFINED    if(!top.isNull()) pcreg = ins.adr                             */
 ,3/* CODEJMP             pcreg = ins.adr                                               */
 ,3/* CODEJMPEQ           if(status == 0) pcreg = ins.adr                               */
 ,3/* CODEJMPNQ           if(status != 0) pcreg = ins.adr                               */
 ,3/* CODEJMPGE           if(status >= 0) pcreg = ins.adr                               */
 ,3/* CODEJMPGT           if(status >  0) pcreg = ins.adr                               */
 ,3/* CODEJMPLE           if(status = -1 || status = 0) pcreg = ins.adr                 */
 ,3/*6CODEJMPLT           if(status = -1  pcreg = ins.adr                               */
};

UINT VirtualCode::append(const void *data, UINT size) { // size in bytes
  UINT ret = totalSize();
  if(ret + size > sizeof(m_code)) {
    throwSqlError(SQL_TOO_COMPLEX,_T("This code is too complex"));
  }
  memcpy(m_code + m_head.m_totalSize, data, size);
  m_head.m_totalSize += size;
  return ret;
}

UINT VirtualCode::append(const Packer &pack) {
  UINT ret = totalSize();
  if(ret + pack.getRequestedSize() > sizeof(m_code)) {
    throwSqlError(SQL_TOO_COMPLEX,_T("This code is too complex"));
  }
  pack.write(ByteMemoryOutputStream(m_code + m_head.m_totalSize));
  m_head.m_totalSize += (UINT)pack.getRequestedSize();
  return ret;
}

HostVarDescriptionList VirtualCode::getDescription() const {
  Packer p;
  HostVarDescriptionList hl;
  if(hasDescription()) {
    p.read(ByteMemoryInputStream(m_code + m_head.m_dataOffset + m_head.m_hostvarDescriptionList));
    p >> hl;
  }
  return hl;
};

static TupleField retrieveConst(const BYTE *adr);

TupleField VirtualCode::getConst(UINT addr) const {
  return retrieveConst(data(addr));
}


UINT CodeGeneration::appendIns0(VirtualOpCode opcode) {
  const Instruction ins(opcode);
  return m_codesegment.append(&ins,1);
}

UINT CodeGeneration::appendIns1(VirtualOpCode opcode, UINT adr) {
  const Instruction ins(opcode,adr);
  return m_codesegment.append(&ins,3);
}

UINT CodeGeneration::appendIns2(VirtualOpCode opcode, UINT reg, UINT index) {
  const Instruction ins(opcode,reg,index);
  return m_codesegment.append(&ins,3);
}

UINT CodeGeneration::appendConst(const char *s) {
  const char type = DBTYPE_CSTRING;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(s,((UINT)strlen(s)+1)*sizeof(char));
  return adr;
}

UINT CodeGeneration::appendConst(const TCHAR *s) {
  const char type = DBTYPE_WSTRING;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(s,((UINT)_tcslen(s)+1)*sizeof(TCHAR));
  return adr;
}

UINT CodeGeneration::appendConst(char i) {
  const char type = DBTYPE_CHAR;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&i,sizeof(i));
  return adr;
}

UINT CodeGeneration::appendConst(unsigned char i) {
  const char type = DBTYPE_UCHAR;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&i,sizeof(i));
  return adr;
}

UINT CodeGeneration::appendConst(short i) {
  if(validChar(i)) {
    return appendConst((char)i);
  } else if(validUchar(i)) {
    return appendConst((UCHAR)i);
  }
  const char type = DBTYPE_SHORT;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&i,sizeof(i));
  return adr;
}

UINT CodeGeneration::appendConst(USHORT i) {
  if(validChar(i)) {
    return appendConst((char)i);
  } else if(validUchar(i)) {
    return appendConst((UCHAR)i);
  }
  const char type = DBTYPE_USHORT;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&i,sizeof(i));
  return adr;
}

UINT CodeGeneration::appendConst(int i) {
  if(validShort(i)) {
    return appendConst((short)i);
  } else if(validUshort(i)) {
    return appendConst((USHORT)i);
  }
  const char type = DBTYPE_INT;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&i,sizeof(i));
  return adr;
}

UINT CodeGeneration::appendConst(UINT i) {
  if(validShort(i)) {
    return appendConst((short)i);
  } else if(validUshort(i)) {
    return appendConst((USHORT)i);
  }
  const char type = DBTYPE_UINT;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&i,sizeof(i));
  return adr;
}

UINT CodeGeneration::appendConst(long l) {
  if(validInt(l)) {
    return appendConst((int)l);
  } else if(validUint(l)) {
    return appendConst((UINT)l);
  }
  const char type = DBTYPE_LONG;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&l,sizeof(l));
  return adr;
}

UINT CodeGeneration::appendConst(ULONG l) {
  if(validInt(l)) {
    return appendConst((int)l);
  } else if(validUint(l)) {
    return appendConst((UINT)l);
  }
  const char type = DBTYPE_ULONG;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&l,sizeof(l));
  return adr;
}

UINT CodeGeneration::appendConst(float d) {
  if(validLong(d)) {
    return appendConst((long)d);
  } else if(validUlong(d)) {
    return appendConst((ULONG)d);
  }
  const char type = DBTYPE_FLOAT;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&d,sizeof(d));
  return adr;
}

UINT CodeGeneration::appendConst(double d) {
  if(validFloat(d))  {
    return appendConst((float)d);
  }
  const char type = DBTYPE_DOUBLE;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&d,sizeof(d));
  return adr;
}

UINT CodeGeneration::appendConst(Date d) {
  const char type = DBTYPE_DATE;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&d,sizeof(d));
  return adr;
}

UINT CodeGeneration::appendConst(Time d) {
  const char type = DBTYPE_TIME;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&d,sizeof(d));
  return adr;
}

UINT CodeGeneration::appendConst(Timestamp d) {
  const char type = DBTYPE_TIMESTAMP;
  const UINT adr  = m_datasegment.append(&type,sizeof(type));
  m_datasegment.append(&d,sizeof(d));
  return adr;
}

void CodeGeneration::fixins1(UINT pos, UINT adr) {
  Instruction ins;
  memcpy(&ins,m_codesegment.getCode() + pos, sizeof(ins));
  Instruction newins(ins.opcode(),adr);
  memcpy(m_codesegment.m_code + pos, &newins, newins.size());
}

void CodeGeneration::fixins2(UINT pos, UINT reg, UINT index ) {
  Instruction ins;
  memcpy(&ins,m_codesegment.m_code + pos, sizeof(ins));
  Instruction newins(ins.opcode(),reg,index);
  memcpy(m_codesegment.m_code + pos, &newins, newins.size());
}

void CodeGeneration::appendDesc( const HostVarDescriptionList &desc) {
  Packer p;
  p << desc;
  m_datasegment.m_head.m_hostvarDescriptionList = m_datasegment.append(p);
}

void CodeGeneration::appendDataToCode() {
  m_codesegment.m_head.m_dataOffset = m_codesegment.append(m_datasegment.m_code
                                                          ,m_datasegment.totalSize());
  m_codesegment.m_head.m_hostvarDescriptionList = m_datasegment.m_head.m_hostvarDescriptionList;
}

void CodeGeneration::splitCodeAndData(const VirtualCode &loadedcode) {
  m_codesegment.m_head.m_totalSize = loadedcode.m_head.m_dataOffset;
  m_datasegment.m_head.m_totalSize = loadedcode.m_head.m_totalSize - loadedcode.m_head.m_dataOffset;
  memcpy(m_codesegment.m_code
        ,loadedcode.m_code
        ,m_codesegment.m_head.m_totalSize);
  memcpy(m_datasegment.m_code
        ,loadedcode.m_code+loadedcode.m_head.m_dataOffset
        ,m_datasegment.m_head.m_totalSize);

  m_datasegment.m_head.m_hostvarDescriptionList = m_codesegment.m_head.m_hostvarDescriptionList;
}

void CodeGeneration::dump( FILE *f ) const {
  m_codesegment.dump(f);
//  datasegment.dump(f);
}

static TupleField retrieveConst(const BYTE *addr) {
  const DbFieldType type = (DbFieldType)(*(addr++));
  TupleField        f;
  switch(type) {
  case DBTYPE_CHAR     : f = *(char*     )addr; break;
  case DBTYPE_UCHAR    : f = *(BYTE*     )addr; break;
  case DBTYPE_SHORT    : f = *(short*    )addr; break;
  case DBTYPE_USHORT   : f = *(USHORT*   )addr; break;
  case DBTYPE_INT      : f = *(int*      )addr; break;
  case DBTYPE_UINT     : f = *(UINT*     )addr; break;
  case DBTYPE_LONG     : f = *(long*     )addr; break;
  case DBTYPE_ULONG    : f = *(ULONG*    )addr; break;
  case DBTYPE_FLOAT    : f = *(float*    )addr; break;
  case DBTYPE_DOUBLE   : f = *(double*   )addr; break;
  case DBTYPE_CSTRING  : f =  (char*     )addr; break;
  case DBTYPE_WSTRING  : f =  (wchar_t*  )addr; break;
  case DBTYPE_DATE     : f = *(Date*     )addr; break;
  case DBTYPE_TIME     : f = *(Time*     )addr; break;
  case DBTYPE_TIMESTAMP: f = *(Timestamp*)addr; break;
  default              : throwSqlError(SQL_FATAL_ERROR,_T("Unknown type in retrieveConst:%d"),type);
  }
  return f;
}

Packer &operator<<(Packer &p, const CastParameter &v ) {
  p << v.m_type
    << v.m_len;
  return p;
}

Packer &operator>>(Packer &p,       CastParameter &v ) {
  p >> v.m_type
    >> v.m_len;
  return p;
}
