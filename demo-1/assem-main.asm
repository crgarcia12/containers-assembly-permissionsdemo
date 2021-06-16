

section     .text
global      _start                              ;must be declared for linker (ld)

_start:                                         ;tell linker entry point

    mov     edx,len                             ;message length
    mov     ecx,msg                             ;message to write
    mov     ebx,1                               ;file descriptor (stdout)
    mov     eax,4                               ;system call number (sys_write)
    int     0x80                                ;call kernel

    ; this will try to write the value of al to the port dx. privileged instruction, it will fail
    ; out     dx, al
    mov     eax, ecx
    call    printNumber

    mov     eax,1                               ;system call number (sys_exit)
    int     0x80                                ;call kernel

printCharacter:
    mov             eax, 4                  ; system call #4 = sys_write
    mov             ebx, 1                  ; file descriptor 1 = stdout
    mov             ecx, digit              ; store *address* of digit into ecx
    mov             edx, 16                 ; byte size of 16
    int             80h
    ret

printNumber:
    push rax
    push rdx
    xor edx,edx          ;edx:eax = number
    div dword [const10]  ;eax = quotient, edx = remainder
    test eax,eax         ;Is quotient zero?
    je .l1               ; yes, don't display it
    call printNumber     ;Display the quotient
.l1:
    lea eax,[edx+'0']
    mov [digit], eax
    call printCharacter  ;Display the remainder
    pop rdx
    pop rax
    ret

section     .data

msg         db  'Hello, world!',0xa                 ;our dear string
len         equ $ - msg                             ;length of our dear string
const10     dd 10
digit       dw 0