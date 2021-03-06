format PE console
entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable
        ; �������������� ������, ��������� �� ����� �������.
        strInfo1     db 'The Grigorean calendar was first introduced in October 1582.',10,0
        strInfo2     db 'From that point of time, the calendar date of Easter could be determined correctly.',10,0
        strAskYear   db 'Please enter a valid year from 1582 to 2600.',10,0
        strWrongInput db 'Wrong year format. Please try again.',10,0
        strWrongFormat db 'Wrong input format, expected an integer. Terminating process...',10,0
        strReadKey     db 'Press any key to exit...',10,0
        strEasterApril db 'The Easter date for this year is the %dth of April.',10,0
        strEasterMarch db 'The Easter date for this year is the %dth of March.',10,0
        ; ��� ������ �������� ������� ���������� ��� ��������������� ������� scanf
        ; � ��������� ������� ����������� ����� �����, �������� �������������.
        strReadYear  db '%u',0


        ; � ���� ���������� ����� �������� ������ - ���, �������� �������������.
        year         dd ?
        ; � ����������� ���� ���������� ����� ���������� ������������� ������ ���������.
        G            dd ?
        C            dd ?
        X            dd ?
        Z            dd ?
        D            dd ?
        E            dd ?
        N            dd ?


;--------------------------------------------------------------------------

section '.code' code readable executable
start:
        ; ��������� ��� � �������.
        call ReadInput
        ; ��������� ���������� ���������.
        call CalculateEaster

finish:
        ; ������� ��������� �� �������� ����� ������������ ����� ������� �� ���������.
        push strReadKey
        call [printf]
        add esp, 4
        ; ��� ������� ������� �� ������������, ����� ���� �� ��������� ���������.
        call [getch]
        push 0
        call [ExitProcess]

;--------------------------------------------------------------------------
ReadInput:
        ; ������� ���������� ���������� �� �����.
        push strInfo1
        call [printf]
        add esp, 4
        push strInfo2
        call [printf]
        add esp, 4

askForInput:
        ; ����������� ���� � ������������.
        push strAskYear
        call [printf]
        add esp, 4
        ; ��������� ���������������� ����.
        push year
        push strReadYear
        call [scanf]
        add esp, 8

        ; �������� ������������ ������� ������.
        ; scanf ���������� ����� ��������� ��������� ��������.
        ; ������������ �������� scanf �������� ����� � �������� eax.
        ; �� ����, ���� scanf ������� 0, ������ ���� ��� ��������� �����������
        ; (��������, ���� ������� ������, ����� ��������� �����).
        cmp eax, 0
        je wrongFormat

        ; ������ �������� ������� ������. ��� ������ ���� � ���������� [1582, 2600].
        cmp [year], 1582
        jl wrongInput ; ���� �������� ��� < 1582.

checkUpperBound:
        ; ��������� ������� �������.
        cmp [year], 2600
        jg wrongInput ; ���� �������� ��� > 2600, �� �� ��������� ��� ���������� ��������.
        jmp endInput ; ���� �� ������ - ������� �� ������������.

wrongInput:
        ; ������� ��������� �� ������, ���� ��� ��������� ��� ���������� ��������.
        push strWrongInput
        call [printf]
        add esp, 4
        jmp askForInput

wrongFormat:
        ; ���� ���� ��� ��������� �����������, �� ������� ��������� �� ������
        ; � ��������� ���������� ���������.
        push strWrongFormat
        call [printf]
        add esp, 4
        jmp finish

endInput:
        ret

;--------------------------------------------------------------------------
CalculateEaster:

         call CalculateG ; 1-�� ��� ���������.
         call CalculateC ; 2-�� ��� ���������.
         call CalculateX ; 3-�� ��� ���������.
         call CalculateZ ; 4-�� ��� ���������.
         call CalculateD ; 5-�� ��� ���������.
         call CalculateE ; 6-�� ��� ���������.
         call StepSeven  ; 7-�� ��� ���������.
         call CalculateN ; 8-��, 9-��, 10-�� � 11-�� ���� ���������.

endAlg:
        ; ����� �� ������������ �� ���������� ���������� ���������.
        ret

;--------------------------------------------------------------------------
CalculateG:

        mov eax, [year]
        mov ebx, 19
        xor edx, edx
        div ebx ; ����� year �� 19. ������� �� ������� ����� ������ � edx.
        inc edx ; ����������� ������� �� ������� �� 1.
        mov [G], edx ; ���������� ��������� � G.

        ret

;--------------------------------------------------------------------------
CalculateC:

        mov eax, [year]
        mov ebx, 100
        xor edx, edx
        div ebx ; ����� �������� � ���������� year �� 100.
        inc eax ; ���������� 1.
        mov [C], eax ; ��������� ����� ���� � ���������� C.

        ret

;--------------------------------------------------------------------------
CalculateX:

        mov eax, [C]
        mov ebx, 3
        mul ebx ; ������ � eax ����� ������ 3*C.
        mov ebx, 4
        xor edx, edx
        div ebx ; ����� 3*C �� 4. ����� ����� ����� � eax.
        sub eax, 12 ; eax -= 12.
        mov [X], eax ; ��������� ��������� � ���������� X.

        ret

;--------------------------------------------------------------------------
CalculateZ:

        mov eax, [C]
        mov ebx, 8
        mul ebx ; ������ � eax ����� ������ 8*C.
        add eax, 5
        mov ebx, 25
        xor edx, edx
        div ebx ; (8� + 5)/25
        sub eax, 5 ; �� ����� ����� �� ������� �������� 5.
        mov [Z], eax

        ret

;--------------------------------------------------------------------------
CalculateD:

        mov eax, [year]
        mov ebx, 5
        mul ebx ; ������ � eax ����� ������ 5*Y.
        mov ebx, 4
        xor edx, edx
        div ebx ; 5Y/4. ����� ����� �� ������� ����� � eax.
        sub eax, [X] ; �������� X.
        sub eax, 10 ; �������� 10.
        mov [D], eax ; ��������� ���������� � ���������� D.

        ret

;--------------------------------------------------------------------------
CalculateE:

        mov eax, [G]
        mov ebx, 11
        mul ebx ; � eax ����� ������ 11G.
        add eax, 20 ; 11G + 20
        add eax, [Z] ; 11G + 20 + Z
        sub eax, [X] ; 11G + 20 + Z - X
        mov ebx, 30
        xor edx, edx
        div ebx ; ��, ��� � eax, ����� �� 30 � ���� �������.
        add edx, 30 ; � ������� �� ������� ���������� 30.
        mov eax, edx
        mov ebx, 30
        xor edx, edx
        div ebx ; ��������� ��� ����� �� 30 � ���� �������.
        mov [E], edx ; ���������� ��� � ���������� E.

        ret

;--------------------------------------------------------------------------
StepSeven:

        cmp [E], 24
        je incrementE ; ���� E == 24, �� ����� ��������� E �� 1.
        cmp [E], 25
        je checkG ; ���� E == 25, �� ����� ��������� ������ �������.
        jmp finishStepSeven ; ����� - ������� �� ������������.

checkG:
        cmp [G], 11 ; ���������� G � 11.
        jb incrementE ; ���� G > 11 � E == 25, �� ����������� E �� 1.
        jmp finishStepSeven ; ����� - ������� �� ������������.

incrementE:
        add [E], 1 ; ����������� E �� 1 ��� �������������.

finishStepSeven:
        ret

;--------------------------------------------------------------------------
CalculateN:

stepEight:
        ; 8-�� ��� ���������.
        mov eax, 44
        mov ebx, [E]
        sub eax, ebx ; �������� E �� 44.
        mov [N], eax ; ��������� ��������� � N.

stepNine:
        ; 9-�� ��� ���������
        cmp [N], 21
        jl addThirty ; ���� N < 21, �� ��������� � ������ �����.
        jmp stepTen ; ����� ����� ��������� �� ��������� ��� ���������.

addThirty:
        add [N], 30 ; ����������� N �� 30.

stepTen:
        ; 10-�� ��� ���������
        add [N], 7 ; N += 7.
        mov eax, [D] ; ���������� �������� ���������� D � ������� eax.
        add eax, [N] ; D += N.
        mov ebx, 7
        xor edx, edx
        div ebx ; ����� D + N �� 7 � ���� �������.
        sub [N], edx ; �������� ������� �� ������� �� N + 7.

stepEleven:
        ; 11-�� ��� ���������
        cmp [N], 31 ; ���������� N � 31.
        jg isApril ; ���� N > 31, �� ���� ����� ����� � ������.
        jmp isMarch ; ����� - � �����.

isApril:
        sub [N], 31 ; N -= 31.
        push [N]
        push strEasterApril
        call [printf] ; ������� ���������� �� �����.
        add esp, 8
        jmp endAlgorithm ; ������� �� ������������.

isMarch:
        push [N]
        push strEasterMarch
        call [printf] ; ������� ���������� �� �����.
        add esp, 8

endAlgorithm:
        ret

;-------------------------------Import section--------------------------
                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess'

  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'