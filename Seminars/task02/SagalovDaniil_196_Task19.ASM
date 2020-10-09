; Сагалов Даниил БПИ-196
; Вариант 19

; Сформировать массив B из элементов А
; с заменой нулевых элементов, предшествующих первому отрицательному, единицей

format PE console
entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        strVecSize   db 'Please enter size of vector A. ', 0
        strIncorSize db 'Incorrect size of vector.', 10, 0
        strVecElemI  db '[%d]? ', 0
        strScanInt   db '%d', 0
        strVecElemOut  db 'B[%d] = %d', 10, 0

        vec_size     dd 0
        i            dd ?
        index        dd -1
        value        dd 1
        tmp          dd ?
        tmpB         dd ?
        tmpStack     dd ?
        vec          rd 100
        vecB         rd 100

;--------------------------------------------------------------------------

section '.code' code readable executable
start:
        call VectorInput
        call FindIndex
        call ChangeVector
        call VectorOutB
finish:
        call [getch]

        push 0
        call [ExitProcess]

;--------------------------------------------------------------------------

VectorInput:

        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [vec_size]
        cmp eax, 0
        jg  getVector

        push strIncorSize
        call [printf]
        add esp, 4
        jmp finish

getVector:
        xor ecx, ecx
        mov ebx, vec

getVecLoop:
        mov [tmp], ebx

        cmp ecx, [vec_size]
        jge endInputVector

        mov [i], ecx

        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
endInputVector:
        ret

;--------------------------------------------------------------------------
FindIndex:
        mov [tmpStack], esp
        xor ecx, ecx
        mov eax, vec

CheckVecLoop:
        mov [tmp], eax
        cmp ecx, [vec_size]
        je endCheck
        mov [i], ecx


        mov eax, [tmp]
        mov ebx, [eax]

        cmp ebx, 0
        jl FoundNegative

        mov ecx, [i]
        inc ecx
        mov eax, [tmp]
        add eax, 4
        xor ebx, ebx
        jmp CheckVecLoop

FoundNegative:
        mov [index], ecx
        jmp endCheck

endCheck:
        mov esp, [tmpStack]
        ret

;--------------------------------------------------------------------------

ChangeVector:
        mov [tmpStack], esp
        xor ecx, ecx
        mov eax, vec
        mov ebx, vecB

ChangeVecLoop:
        mov [tmp], eax
        mov [tmpB], ebx
        cmp ecx, [vec_size]
        je endChange
        mov [i], ecx

        mov eax, [tmp]
        mov edx, [eax]

        cmp ecx, [index]
        jl CheckIfZero
        jmp NoChange

CheckIfZero:
        cmp edx, 0
        je IsZero
        jmp NoChange

IsZero:
        mov eax, [tmp]
        mov ebx, [tmpB]
        mov edx, [value]
        mov [ebx], edx

        mov ecx, [i]
        inc ecx
        add eax, 4
        add ebx, 4

        jmp ChangeVecLoop

NoChange:
        mov eax, [tmp]
        mov ebx, [tmpB]
        mov [ebx], edx

        mov ecx, [i]
        inc ecx
        add eax, 4
        add ebx, 4

        jmp ChangeVecLoop

endChange:
        mov esp, [tmpStack]
        ret

;-------------------------------------------------------------------

VectorOutB:
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, vecB
putVecLoopB:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVectorB
        mov [i], ecx

        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoopB
endOutputVectorB:
        mov esp, [tmpStack]
        ret

;---------------------------------------------------------
                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'


; Тесты программы:

; Тест #1
; Ввод:
; -10
; Вывод:
; Incorrect size of vector.

; Тест #2
; Ввод:
; 10
; 0
; 0
; 1
; 10
; 12
; 46
; -1
; 0
; 1
; 22
; Вывод:
; B[0] = 1
; B[1] = 1
; B[2] = 1
; B[3] = 10
; B[4] = 12
; B[5] = 46
; B[6] = -1
; B[7] = 0
; B[8] = 1
; B[9] = 22

; Тест #3
; Ввод:
; 5
; 0
; 1
; 0
; -1
; 0
; Вывод:
; B[0] = 1
; B[1] = 1
; B[2] = 1
; B[3] = -1
; B[4] = 0

; Тест #4
; Ввод:
; 3
; 3
; 2
; 1
; Вывод:
; B[0] = 3
; B[1] = 2
; B[2] = 1