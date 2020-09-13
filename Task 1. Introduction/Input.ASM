format PE console

entry start

include 'win32a.inc'

section '.data' data readable writable

        helloStr db 'Hello world', 0
        stringFormatter db '%s', 0
        digitFormatter db '%d', 0

        userName rd 2
        personAge rd 1

        introMessage db 'What is your name? ', 0
        ageMessage db 'What is your age? ', 0
        ; ќдновременно добавл€ем переход на новую строку
        ; и признак конца строки

        ; !!! Ќа семинаре: узнать, почему без 0 он продолжает
        ; добавл€ть данные стека в качестве параметров функции !!!
        helloMessage db 'Hello, %s, my %d years old friend!', 10,0
        adressMessage db 'The adress of your name is: %d', 0

        NULL = 0

section '.code' code readable writable

        start:
                push introMessage
                call [printf]

                ; ѕараметры передаютс€ в обратном
                ; пор€дке - так устроен стек.
                push userName
                push stringFormatter
                call [scanf]

                push ageMessage
                call [printf]

                push personAge
                push digitFormatter
                call [scanf]

                ; ƒобавл€ем квадратные скобки, т. к.
                ; используетс€ значение, а не адрес.
                push [personAge]
                push userName
                push helloMessage
                call [printf]

                ; lea - помещаем адрес переменной в регистр
                lea eax, [userName]
                push eax
                push adressMessage
                call [printf]

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
               getch, '_getch',\
               scanf, 'scanf'