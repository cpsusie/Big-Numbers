.CODE

;void assemblerCode()
assemblerCode PROC
    ret

;    pushf
;    popf
;    pushfd // not allowed in 64 mode
;    popfd
;    pushfq
;    popfq

    cbw
    cwde
    cwd
    cdq
    cdqe
    cqo

    clgi
    stgi

    ret
assemblerCode ENDP

END
