section .data
    fmt db "The result of %d * %d is %d",10,0

section .text
    global main

multiply:
    push ebp
    mov ebp, esp
    sub esp, 4            ; Allocate space for result
    xor eax, eax          ; Clear eax (result)
    mov ebx, [ebp + 8]    ; Load mcand into ebx
    mov ecx, [ebp + 12]   ; Load mplier into ecx

multiply_loop:
    test ecx, 1           ; Check if the least significant bit of mplier is set
    jz skip_addition       ; If not, skip addition
    add eax, ebx           ; Add mcand to result
skip_addition:
    shl ebx, 1            ; Shift mcand left (multiply by 2)
    shr ecx, 1            ; Shift mplier right (divide by 2)
    jnz multiply_loop     ; Continue loop if mplier is not zero

    mov [ebp - 4], eax     ; Store the result in the stack
    mov eax, [ebp - 4]     ; Move the result to eax
    leave
    ret

main:
    push ebp
    mov ebp, esp

    ; Call multiply function
    push dword 10383       ; Push mplier
    push dword 32766       ; Push mcand
    call multiply

    ; Prepare arguments for printf
    push eax               ; Push result
    push dword 10383       ; Push mplier
    push dword 32766       ; Push mcand
    push fmt               ; Push format string
    call printf            ; Call printf function

    add esp, 16            ; Clean up the stack

    mov eax, 0             ; Return 0
    leave
    ret