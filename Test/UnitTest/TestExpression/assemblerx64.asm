.CODE

;void assemblerCode()
assemblerCode PROC
    ret

    fld st(1)
    fld dword ptr [rax]
    fld qword ptr [rax]
    fld tbyte ptr [rax]

    fcom
    fcom st(1)
    fcom st(2)
    fcom dword ptr [rax]
    fcom qword ptr [rax]

    fcomp
    fcomp st(1)
    fcomp st(2)
    fcomp dword ptr [rax]
    fcomp qword ptr [rax]

    fucom
    fucom st(1)
    fucom st(2)

    fucomp
    fucomp st(1)
    fucomp st(2)
;   fucomp dword ptr [rax]
;   fucomp qword ptr [rax]

;    fcomi
;    fcomi st(1)
     fcomi st(0),st(1)
     fcomi st(0),st(2)
;    fcomi st(1),st(0)

;    fucomi
;    fucomi st(1)
    fucomi st(0),st(1)
    fucomi st(0),st(2)

    fdiv
    fdivr
    fdivrp
    fdivp
    nop
    RET
    CMC
    CLC
    STC
    CLI
    STI
    CLD
    STD
    SAHF
    LAHF
    PUSHF
    POPF

    PUSHFQ
    POPFQ

    CBW
    CWDE
    CWD
    CDQ

    CDQE
    CQO
    CLGI
    STGI

    ret
assemblerCode ENDP

END
