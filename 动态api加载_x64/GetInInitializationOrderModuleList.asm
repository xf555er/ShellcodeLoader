.CODE
    GetInInitializationOrderModuleList PROC
    mov rax,gs:[60h] ; PEB，注意，这里不能写0x60
    mov rax,[rax+18h] ; PEB_LDR_DATA
    mov rax,[rax+30h] ; InInitializationOrderModuleList
    ret ; 这里不能写retn
    GetInInitializationOrderModuleList ENDP
END