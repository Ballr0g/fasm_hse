format PE

entry start

include 'win32a.inc'

section '.data' data readable writable

        defaultStr db 'Default value: %d', 10,0
        leftShiftStr db 'Left byte shift result: %d', 10,0
        rightShiftStr db 'Right byte shift result: %d', 0

        NULL = 0

section '.code' code readable executable

        start:
                ; 30 � ���������� ������� ���������
                mov ecx, 11110b
                push ecx
                push defaultStr
                call [printf]

                mov ecx, 11110b
                ; shl - ��������� ����� ����� �� 1
                shl ecx, 1

                push ecx
                push leftShiftStr
                call [printf] ; �����: 60

                mov ecx, 11110b
                ; shr - ��������� ����� ����� �� 3
                shr ecx, 3

                push ecx
                push rightShiftStr
                call [printf] ; �����: 3

                call [getch]

                push NULL
                call [ExitProcess]

section '.idata' import data readable

        library kernel, 'kernel32.dll',\
                msvcrt, 'msvcrt.dll'

        import kernel,\
               ExitProcess, 'ExitProcess'

        import msvcrt,\
               printf, 'printf',\
               scanf, 'scanf',\
               getch, '_getch'