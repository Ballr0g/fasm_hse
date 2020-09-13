; Консольное приложение
format PE console
; ТОчка входа в программу
entry start

include 'win32a.inc'

section '.data' data readable writeable
        ; Метка с указанием на строку
        ; 0 - ограничитель строки, 10 - перевод
        helloStr db 'Hello world', 0
        ; Указатель-ограничитель вывода
        NULL = 0

section '.code' code readable executable
        start:
                ; Помещение данных в стек
                push helloStr
                call [printf]
                ; Заморозка экрана
                call [getch]
                ; Код выхода из программы перед завершением
                push NULL
                call [ExitProcess]

section '.idata' import data readable
        library kernel, 'kernel32.dll',\
                msvcrt, 'msvcrt.dll'
        import kernel,\
               ExitProcess, 'ExitProcess'
        import msvcrt,\
        printf, 'printf',\
        getch, '_getch'