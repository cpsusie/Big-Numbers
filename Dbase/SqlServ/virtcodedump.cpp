#include "stdafx.h"
#include <ByteMemoryStream.h>

void VirtualCode::dumpconst(FILE *f, unsigned int addr) const {
  TupleField v;
  v = getConst(addr);
  v.dump(f);
  fprintf(f,"\n");
}

void VirtualCode::dump(FILE *f) const {
  Instruction ins;
//  hexdump((void*)m_code,20,stdout);
  for(const unsigned char *p = m_code; p < m_code + m_head.m_dataOffset; p += ins.size()) {
    memcpy(&ins,p,sizeof(ins));
    VirtualOpCode opcode = ins.opcode();
    fprintf(f,"%4d:",p - m_code);
    switch(opcode) {

#define casepr0(op)               case CODE##op: fprintf(f,"%-20s\n",                      #op                                  ); break;
#define casepr1(op)               case CODE##op: fprintf(f,"%-20s adr  = %5d\n"           ,#op,ins.adr()                        ); break;
#define casepr2(op)               case CODE##op: fprintf(f,"%-20s reg  = %1d index = %d\n",#op,ins.reg(),ins.index()            ); break;
#define caseprtype(op)            case CODE##op: fprintf(f,"%-20s type = %s\n"            ,#op,getTypeString[ins.adr()]          ); break;
#define caseprconst(op)           case CODE##op: fprintf(f,"%-20s adr  = %5d "            ,#op,ins.adr()); dumpconst(f,ins.adr()); break;

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
    casepr1(PUSHHV         )  /* push(hostvar[ins.adr])                                        */
    caseprconst(PUSHCONST  )  /* push(data(ins.adr))                                           */
   
    case CODEPUSHADR:         /* push(ins.adr)                                                 */
      fprintf(f,"%-20s adr  = %5d\n","PUSHADR",ins.adr());
      { Instruction nextins;
        memcpy(&nextins,p+ins.size(),sizeof(nextins));
        switch(nextins.opcode()) {
        case CODEOPENCURSOR:
          { Packer pack;
            pack.read(ByteMemoryInputStream(data(ins.adr())));
            TableCursorParam param;
            pack >> param;
            param.dump(f);
            break;
          }
        case CODECRTAB:
          { Packer pack;
            pack.read(ByteMemoryInputStream(data(ins.adr())));
            TableDefinition tableDef;
            IndexDefinition indexDef;
            pack >> tableDef >> indexDef;
            tableDef.dump(f);
            indexDef.dump(f);
            break;
          }
        case CODECRINX:
          { Packer pack;
            pack.read(ByteMemoryInputStream(data(ins.adr())));
            CreateIndexData crinx;
            pack >> crinx;
            crinx.dump(f);
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
    casepr1(SENDEOF        )  /* pipelines[ins.adr].writeeof                                   */
    casepr2(RECEIVETUP     )  /*4pipelines[ins.index].read(m_tuplereg[ins.reg])                */
    casepr1(NEWOPERATOR    )  /* start new operator(entrypoint = ins.adr)                      */
    casepr1(INITPIPES      )  /* pipelines.init(ins.adr)                                       */
    casepr0(EQ             )  /* push(top == top-1)                                            */
    casepr0(NQ             )  /* push(top != top-1)                                            */
    casepr0(GE             )  /* push(top >= top-1)                                            */
    casepr0(GT             )  /* push(top >  top-1)                                            */
    casepr0(LE             )  /* push(top <= top-1)                                            */
    casepr0(LT             )  /* push(top <  top-1)                                            */
    casepr0(ISNULL         )  /* push(top.isnull)                                              */
    casepr1(ISLIKE         )  /*5push(m_sqlregex[ins.adr].islike(top-1,top))                   */
    casepr1(CMP            )  /* if((status = compare(top,top-1)) == -2) pcreg = ins.adr       */
    casepr1(CMPTRUE        )  /* if((status = compare(top,TRUE )) == -2) pcreg = ins.adr       */
    casepr1(JMPONDEFINED   )  /* if(!top.isNull()) pcreg = ins.adr                             */
    casepr1(JMP            )  /* pcreg = ins.adr                                               */
    casepr1(JMPEQ          )  /* if(status == 0) pcreg = ins.adr                               */
    casepr1(JMPNQ          )  /* if(status != 0) pcreg = ins.adr                               */
    casepr1(JMPGE          )  /* if(status >= 0) pcreg = ins.adr                               */
    casepr1(JMPGT          )  /* if(status >  0) pcreg = ins.adr                               */
    casepr1(JMPLE          )  /* if(status = -1 || status = 0) pcreg = ins.adr                 */
    casepr1(JMPLT          )  /*6if(status = -1  pcreg = ins.adr                               */
    default:
      fprintf(f,"Unknown opcode:%ld adr = %5d\n",opcode,ins.adr());
      break;
    }
  }
  if(hasDescription()) {
    HostVarDescriptionList hl = getDescription();
    hl.dump(f);
  }
  fprintf(f,"Total size:%d\n",totalSize());
}
