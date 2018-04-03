.CODE

;void assemblerCode()
assemblerCode PROC
    ret

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
