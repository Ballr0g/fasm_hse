format pe console 4.0
include 'WIN32AX.INC'
 
.data
 buf db 'London is the capital of Great Britain.'
 bufsize = $ - buf
 byteswritten dd ?

.code
 main:
 invoke  CreateFile, 'firstFile.txt', GENERIC_WRITE, 0, 0, 4, FILE_ATTRIBUTE_NORMAL, 0
 invoke  WriteFile, eax, buf, bufsize, byteswritten, 0
 invoke  ExitProcess, 0
 
.end main