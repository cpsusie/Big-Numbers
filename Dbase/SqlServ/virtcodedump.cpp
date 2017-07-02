#include "stdafx.h"
#include <ByteMemoryStream.h>

String VirtualCode::toString() const {
  String result;
  Instruction ins;
//  hexdump((void*)m_code,20,stdout);
  for(const BYTE *p = m_code; p < m_code + m_head.m_dataOffset; p += ins.size()) {
    memcpy(&ins,p,sizeof(ins));
    VirtualOpCode opcode = ins.opcode();
    result += format(_T("%4u:"),(UINT)(p - m_code));
    switch(opcode) {

#define casepr0(op)               case CODE##op: result += format(_T("%-20s\n"),                      _T(#op)                                             ); break;
#define casepr1(op)               case CODE##op: result += format(_T("%-20s adr  = %5d\n")           ,_T(#op),ins.addr()                                  ); break;
#define casepr2(op)               case CODE##op: result += format(_T("%-20s reg  = %1d index = %d\n"),_T(#op),ins.reg(),ins.index()                       ); break;
#define caseprtype(op)            case CODE##op: result += format(_T("%-20s type = %s\n")            ,_T(#op),getTypeString[ins.addr()]                   ); break;
#define caseprconst(op)           case CODE##op: result += format(_T("%-20s adr  = %5d <%s>")        ,_T(#op),ins.addr(), constToString(ins.addr()).cstr()); break;

    casepr0(CRTAB          )  /*0userTabCreate(data[top])                                      */
    casepr0(CRLIK          )  /* userTabCreatelike(top,top-1)                                  */
    casepr0(DRTAB          )  /* userTabDrop(top)                                              */
    casepr0(CRINX          )  /* sqlcrindex(m_db,data[top])                                    */
    casepr0(DRINX          )  /* userIndexDrop(top)                                            */
    casepr0(RETURN         )  /* return                                                        */
    casepr0(TRBEGIN        )  /* trbegin()                                                     */
    casepr0(TRCOMMIT       )  /* trcommit()                                                    */
    casepr0(TRABORT        )  /* trabort()                                                     */
    casepr2(TUPINIT        )  /* tuplereg[ins.reg].init(ins.index)                             */
    casepr2(TUPINSERT      )  /*1insert(top,tuplereg[ins.reg])                                 */
    casepr2(POPTUP         )  /* m_tuplereg[ins.reg]->m_fields[ins.index] = m_stack.pop()      */
    casepr2(PUSHTUP        )  /* m_stack.push(m_tuplereg[ins.reg]->m_fields[ins.index])        */
    casepr1(PUSHHV         )  /* push(hostvar[ins.addr])                                       */
    caseprconst(PUSHCONST  )  /* push(data(ins.addr))                                          */

    case CODEPUSHADR:         /* push(ins.addr)                                                */
      result += format(_T("%-20s adr  = %5d\n"),_T("PUSHADR"),ins.addr());
      { Instruction nextins;
        memcpy(&nextins,p+ins.size(),sizeof(nextins));
        switch(nextins.opcode()) {
        case CODEOPENCURSOR:
          { Packer pack;
            pack.read(ByteMemoryInputStream(data(ins.addr())));
            TableCursorParam param;
            pack >> param;
            result += param.toString();
            break;
          }
        case CODECRTAB:
          { Packer pack;
            pack.read(ByteMemoryInputStream(data(ins.addr())));
            TableDefinition tableDef;
            IndexDefinition indexDef;
            pack >> tableDef >> indexDef;
            result += tableDef.toString();
            result += indexDef.toString();
            break;
          }
        case CODECRINX:
          { Packer pack;
            pack.read(ByteMemoryInputStream(data(ins.addr())));
            CreateIndexData crinx;
            pack >> crinx;
            result += crinx.toString();
            break;
          }
        default:
          break;
        }
      }
      break;
    casepr0(AND            )  /* push((top-1) AND top)                                         */
    casepr0(OR             )  /* push((top-1) OR  top)                                         */
    casepr0(NOT            )  /* top = not top                                                 */
    casepr0(ADD            )  /* push((top-1) + top)                                           */
    casepr0(SUB            )  /*2push((top-1) - top)                                           */
    casepr0(MULT           )  /* push((top-1) * top)                                           */
    casepr0(DIV            )  /* push((top-1) / top)                                           */
    casepr0(MOD            )  /* push((top-1) % top)                                           */
    casepr0(EXPO           )  /* push((top-1) ** top)                                          */
    casepr0(NEG            )  /* top = -top                                                    */
    casepr0(CONCAT         )  /* push((top-1) || top)                                          */
    casepr0(SUBSTR         )  /* push(substr((top-2),(top-1),top))                             */
    casepr0(DATE           )  /* push(Date((top-2),(top-1),top))                               */
    casepr0(TIME           )  /* push(Time((top-2),(top-1),top))                               */
    casepr0(TIMESTAMP      )  /*3push(Timestamp((top-5),(top-4),(top-3),(top-2),(top-1),top))  */
    casepr1(CAST           )  /* top = cast(castparam(adr))top                                 */
    casepr0(PUSHNULL       )  /* push(NULL)                                                    */
    casepr0(PUSHCURRENTDATE)  /* push currentdate                                              */
    casepr0(PUSHCURRENTTIME)  /* push currenttime                                              */
    casepr0(PUSHCURRENTTIMESTAMP) /* push currenttimestamp                                     */
    casepr2(OPENCURSOR     )  /* cursor[inx.reg] = tablcursor.open((TableCursorParam*)(top),tuplereg[0],tuplereg[1]) */
    casepr2(FETCHCURSOR    )  /* cursor[inx.reg].fetch(m_tuplereg[ins.index]);                 */
    casepr2(SENDTUP        )  /* pipelines[ins.index].write(m_tuplereg[ins.reg])               */
    casepr1(SENDEOF        )  /* pipelines[ins.addr].writeeof                                   */
    casepr2(RECEIVETUP     )  /*4pipelines[ins.index].read(m_tuplereg[ins.reg])                */
    casepr1(NEWOPERATOR    )  /* start new operator(entrypoint = ins.addr)                      */
    casepr1(INITPIPES      )  /* pipelines.init(ins.addr)                                       */
    casepr0(EQ             )  /* push(top == top-1)                                            */
    casepr0(NQ             )  /* push(top != top-1)                                            */
    casepr0(GE             )  /* push(top >= top-1)                                            */
    casepr0(GT             )  /* push(top >  top-1)                                            */
    casepr0(LE             )  /* push(top <= top-1)                                            */
    casepr0(LT             )  /* push(top <  top-1)                                            */
    casepr0(ISNULL         )  /* push(top.isnull)                                              */
    casepr1(ISLIKE         )  /*5push(m_sqlregex[ins.addr].islike(top-1,top))                   */
    casepr1(CMP            )  /* if((status = compare(top,top-1)) == -2) pcreg = ins.addr       */
    casepr1(CMPTRUE        )  /* if((status = compare(top,TRUE )) == -2) pcreg = ins.addr       */
    casepr1(JMPONDEFINED   )  /* if(!top.isNull()) pcreg = ins.addr                             */
    casepr1(JMP            )  /* pcreg = ins.addr                                               */
    casepr1(JMPEQ          )  /* if(status == 0) pcreg = ins.addr                               */
    casepr1(JMPNQ          )  /* if(status != 0) pcreg = ins.addr                               */
    casepr1(JMPGE          )  /* if(status >= 0) pcreg = ins.addr                               */
    casepr1(JMPGT          )  /* if(status >  0) pcreg = ins.addr                               */
    casepr1(JMPLE          )  /* if(status = -1 || status = 0) pcreg = ins.addr                 */
    casepr1(JMPLT          )  /*6if(status = -1  pcreg = ins.addr                               */
    default:
      result += format(_T("Unknown opcode:%ld adr = %5d\n"),opcode,ins.addr());
      break;
    }
  }
  if(hasDescription()) {
    result += getDescription().toString();
  }
  result += format(_T("Total size:%d\n"),totalSize());
  return result;
}
